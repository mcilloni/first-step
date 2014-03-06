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

  if (lex->newline) {
    lex->newline = false;
    *data = '\n';
    *(data + 1) = 0;
    return true;
  }

  for (i = 0; i < max && !lex->newline;) {
    
    ch = lex->peek;
    ++lex->current.position;

    lex->newline = (lex->current.len + 1) == lex->current.position;     

    if (!lex->newline) {
      lex->peek = lex->current.val[lex->current.position - 1];     
    } else {
      line_free(lex->current);
      lex->current = line_read(lex->file, &lex->errcode);

      if (lex->errcode == ERROR) {
        lex->error = "Error, who knows why";
        return false;
      }

      lex->peek = *lex->current.val;
      ++lex->current.position;
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

  struct token res = {LEX_NONE, 0};

  if (!stok(lex, (char*) data, MAX_TOKEN_LENGTH)) {
    return res;
  }

  // \n
  if (!strcmp("\n", data)) {
    res.type = LEX_NEWLINE;
    return res;
  }

  // /
  if (!strcmp("/", data)) {
    res.type = LEX_DIV;
    return res;
  }

  // =

  if (!strcmp("=", data)) {
    res.type = LEX_ASSIGN;
    return res;
  }

  // ==
  if (!strcmp("==", data)) {
    res.type = LEX_EQUAL;
    return res;
  }

  // !=
  if (!strcmp("!=", data)) {
    res.type = LEX_DIFFERENT;
    return res;
  }

  // >
  if (!strcmp(">", data)) {
    res.type = LEX_MAJOR;
    return res;
  }

  // <
  if (!strcmp("<", data)) {
    res.type = LEX_MINOR;
    return res;
  }

  // *
  if (!strcmp("*", data)) {
    res.type = LEX_TIMES;
    return res;
  }

  // +
  if (!strcmp("+", data)) {
    res.type = LEX_PLUS;
    return res;
  }
  
  // -
  if (!strcmp("-", data)) {
    res.type = LEX_MINUS;
    return res;
  }

  // ++
  if (!strcmp("++", data)) {
    res.type = LEX_INC;
    return res;
  }

  // --
  if (!strcmp("--", data)) {
    res.type = LEX_DEC;
    return res;
  }

  // !
  if (!strcmp("!", data)) {
    res.type = LEX_NOT;
    return res;
  }

  // entry
  if (!strcmp("entry", data)) {
    res.type = LEX_ENTRY;
    return res;
  }

  // /entry
  if (!strcmp("/entry", data)) {
    res.type = LEX_ENDENTRY;
    return res;
  }

  // if
  if (!strcmp("if", data)) {
    res.type = LEX_IF;
    return res;
  }

  // /if
  if (!strcmp("/if", data)) {
    res.type = LEX_ENDIF;
    return res;
  }

  if (!strcmp("var", data)) {
    res.type = LEX_VAR;
    return res;
  }

  // number
  if (isStrNum(data)) {
    res.type = LEX_NUMBER;
    res.value = atoll(data);
    return res;
  }

  //generic id
  res.type = LEX_ID;
  res.value = (intmax_t) strdup(data);
  
  return res;
}

void freetok(struct token tok) {

  if (tok.type == LEX_ID) {
    free( (void*) tok.value);
  }

}

const char* represent(enum token_type tok) {
  switch (tok) {
  case LEX_NONE:
    return "NONE";
  case LEX_ASSIGN:
    return "ASSIGN";
  case LEX_DEC:
    return "DEC";
  case LEX_DIFFERENT:
    return "DIFFERENT";
  case LEX_DIV:
    return "DIV";
  case LEX_ENDENTRY:
    return "ENDENTRY";
  case LEX_ENDIF:
    return "ENDIF";
  case LEX_ENTRY:
    return "ENTRY";
  case LEX_EQUAL:
    return "EQUAL";
  case LEX_ID:
    return "ID";
  case LEX_IF:
    return "IF";
  case LEX_INC:
    return "INC";
  case LEX_MAJOR:
    return "MAJOR";
  case LEX_MINOR:
    return "MINOR";
  case LEX_MINUS:
    return "MINUS";
  case LEX_NEWLINE:
    return "NEWLINE";
  case LEX_NOT:
    return "NOT";
  case LEX_NUMBER:
    return "NUMBER";
  case LEX_PLUS:
    return "PLUS";
  case LEX_TIMES:
    return "TIMES";
  case LEX_VAR:
    return "VAR";
  default:
    return "UNKNOWN";
  }

  return "";
}

