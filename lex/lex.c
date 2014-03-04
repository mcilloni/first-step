#include "lex.h"
#include "../utils/utils.h"

#include <ctype.h>
#include <errno.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include <jemalloc/jemalloc.h>


static inline struct lexer* err(struct lexer *lex, const char *error) {
  lex->errcode = ERROR;
  lex->error = error;
  return lex;
}

struct lexer* lexer_open(const char *path) {
  struct lexer *lex = calloc(1,sizeof(struct lexer)); 
  
  lex->file = fopen(path, "r");
  if (!lex->file) {
    return err(lex, "Cannot open file");
  }

  lex->current = line_read(lex->file, &lex->errcode);
  
  if (lex->errcode == ERROR) {
    lex->error = "Error, who knows why";
  }

  lex->peek = *(lex->current.val);
  ++lex->current.position;
  
  lex->newline = false;

  return lex;
  
}

void lexer_close(struct lexer *lex) {
  
  if (lex->file) {
    fclose(lex->file);
  }

  line_free(lex->current);
  free(lex);
  
}

bool stok(struct lexer *lex, char *data, size_t max) {

  char ch;
  size_t i;
  bool eol = false;

  if (lex->newline) {
    lex->newline = false;
    *data = '\n';
    *(data + 1) = 0;
    return true;
  }

  for (i = 0; i < max && !eol;) {
    
    ch = lex->peek;
    ++lex->current.position;

    if ((lex->current.len + 1) == lex->current.position) {
      line_free(lex->current);
      lex->current = line_read(lex->file, &lex->errcode);

      if (lex->errcode == ERROR) {
        lex->error = "Error, who knows why";
        return false;
      }

      lex->peek = *lex->current.val;
      ++lex->current.position;
      lex->newline = eol = true;
    } else {
      lex->peek = lex->current.val[lex->current.position - 1];      
    }

    if (isblank(ch)) {
      if(i) {
        break;
      } else {
        continue;
      }
    }

    data[i] = ch;
    ++i;

    if (ch == '/' && (i == 1)) {
      continue;
    }

    if ((isalnum(ch) && ispunct(lex->peek)) || (ispunct(ch) && isalnum(lex->peek))) {
      break;
    }

  }

  if (i == max) {
    lex->errcode = ERROR;
    lex->error = "Token too long";
    return false;
  }

  data[i] = 0;

  return true;   

}

#define MAX_TOKEN_LENGTH 1024 //fucking C messing VLAs with constant arrays

struct token gettok(struct lexer *lex) {

  const char data[MAX_TOKEN_LENGTH];

  struct token res = {NONE, 0};

  if (!stok(lex, (char*) data, MAX_TOKEN_LENGTH)) {
    return res;
  }

  // \n
  if (!strcmp("\n", data)) {
    res.type = NEWLINE;
    return res;
  }

  // /
  if (!strcmp("/", data)) {
    res.type = DIV;
    return res;
  }

  // =

  if (!strcmp("=", data)) {
    res.type = ASSIGN;
    return res;
  }

  // ==
  if (!strcmp("==", data)) {
    res.type = EQUAL;
    return res;
  }

  // !=
  if (!strcmp("!=", data)) {
    res.type = DIFFERENT;
    return res;
  }

  // >
  if (!strcmp(">", data)) {
    res.type = MAJOR;
    return res;
  }

  // <
  if (!strcmp("<", data)) {
    res.type = MINOR;
    return res;
  }

  // *
  if (!strcmp("*", data)) {
    res.type = TIMES;
    return res;
  }

  // +
  if (!strcmp("+", data)) {
    res.type = PLUS;
    return res;
  }
  
  // -
  if (!strcmp("-", data)) {
    res.type = MINUS;
    return res;
  }

  // ++
  if (!strcmp("++", data)) {
    res.type = INC;
    return res;
  }

  // !
  if (!strcmp("!", data)) {
    res.type = NOT;
    return res;
  }

  // entry
  if (!strcmp("entry", data)) {
    res.type = ENTRY;
    return res;
  }

  // /entry
  if (!strcmp("/entry", data)) {
    res.type = ENDENTRY;
    return res;
  }

  // if
  if (!strcmp("if", data)) {
    res.type = IF;
    return res;
  }

  // /if
  if (!strcmp("/if", data)) {
    res.type = ENDIF;
    return res;
  }

  if (!strcmp("var", data)) {
    res.type = VAR;
    return res;
  }

  // number
  if (isStrNum(data)) {
    res.type = NUMBER;
    res.value = atoll(data);
    return res;
  }

  //generic id
  res.type = ID;
  res.value = (intmax_t) strdup(data);
  
  return res;
}

void freetok(struct token tok) {

  if (tok.type == ID) {
    free( (void*) tok.value);
  }

}

const char* represent(enum token_type tok) {
  switch (tok) {
  case NONE:
    return "NONE";
  case ASSIGN:
    return "ASSIGN";
  case DIFFERENT:
    return "DIFFERENT";
  case DIV:
    return "DIV";
  case ENDENTRY:
    return "ENDENTRY";
  case ENDIF:
    return "ENDIF";
  case ENTRY:
    return "ENTRY";
  case EQUAL:
    return "EQUAL";
  case ID:
    return "ID";
  case IF:
    return "IF";
  case INC:
    return "INC";
  case MAJOR:
    return "MAJOR";
  case MINOR:
    return "MINOR";
  case MINUS:
    return "MINUS";
  case NEWLINE:
    return "NEWLINE";
  case NOT:
    return "NOT";
  case NUMBER:
    return "NUMBER";
  case PLUS:
    return "PLUS";
  case TIMES:
    return "TIMES";
  case VAR:
    return "VAR";
  default:
    return "UNKNOWN";
  }

  return "";
}

