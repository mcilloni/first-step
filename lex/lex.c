#include "lex.h"
#include "../utils/utils.h"
#include "../utils/env.h"

#include <ctype.h>
#include <errno.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

struct lexer* lexer_open(const char *path) {
  FILE *file = fopen(path, "r");

  if (!file) {
    env.fail("Cannot open file %s: %s", path, strerror(errno));
  }

  return lexer_fromFile(file);
}

struct lexer* lexer_fromFile(FILE *file) {
  struct lexer *lex = calloc(1,sizeof(struct lexer)); 
  
  if (!file) {
    env.fail("NULL file given for lexer");
  }

  lex->file = file;
  env_setLine(line_read(lex->file, &lex->errcode));
  
  if (lexer_error(lex)) {
    env.fail("Error while reading file %s: %s", strerror(errno));
  }

  lex->peek = *(env.line.val);
  ++env.line.position;
  
  lex->newline = false;

  return lex;
  
}
void lexer_close(struct lexer *lex) {
  
  if (lex->file) {
    fclose(lex->file);
  }

  line_free(env.line);
  free(lex);
  
}

void lexer_discardLine(struct lexer *lex) {
      env_setLine(line_read(lex->file, &lex->errcode));

      if (!lexer_error(lex)) {
        lex->peek = *env.line.val;
        ++env.line.position;
      }
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
    ++env.line.position;

    lex->newline = (env.line.len + 1) == env.line.position;     

    if (!lex->newline) {
      lex->peek = env.line.val[env.line.position - 1];     
    } else {
      lexer_discardLine(lex);
      if (lexer_error(lex)) {
        return false;
      }
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
    env.error("Token too long");
    return false;
  }

  data[i] = 0;

  return true;   

}

#define MAX_TOKEN_LENGTH 1024 //fucking C messing VLAs with constant arrays

struct token* token_get(struct lexer *lex) {

  if (lexer_error(lex) || lexer_eof(lex)) {
    return NULL;
  }

  char data[MAX_TOKEN_LENGTH];

  if (lex->saved) {
    strcpy(data, lex->saved);
    free(lex->saved);
    lex->saved = NULL;
  } else {
    if (!stok(lex, (char*) data, MAX_TOKEN_LENGTH)) {
      return NULL;
    }
  }

  struct token *res = calloc(1, sizeof(struct token));

  // \n
  if (!strcmp("\n", data)) {
    res->type = LEX_NEWLINE;
    return res;
  }

  // =
  if (!strcmp("=", data)) {
    res->type = LEX_ASSIGN;
    return res;
  }

  // ==
  if (!strcmp("==", data)) {
    res->type = LEX_EQUAL;
    return res;
  }

  // !=
  if (!strcmp("!=", data)) {
    res->type = LEX_DIFFERENT;
    return res;
  }

  // >
  if (!strcmp(">", data)) {
    res->type = LEX_MAJOR;
    return res;
  }

  // <
  if (!strcmp("<", data)) {
    res->type = LEX_MINOR;
    return res;
  }

  // *
  if (!strcmp("*", data)) {
    res->type = LEX_TIMES;
    return res;
  }

  // +
  if (!strcmp("+", data)) {
    res->type = LEX_PLUS;
    return res;
  }
  
  // -
  if (!strcmp("-", data)) {
    res->type = LEX_MINUS;
    return res;
  }

  // ++
  if (!strcmp("++", data)) {
    res->type = LEX_INC;
    return res;
  }

  // --
  if (!strcmp("--", data)) {
    res->type = LEX_DEC;
    return res;
  }

  // !
  if (!strcmp("!", data)) {
    res->type = LEX_NOT;
    return res;
  }

  // entry
  if (!strcmp("entry", data)) {
    res->type = LEX_ENTRY;
    return res;
  }

  // /entry
  if (!strcmp("/entry", data)) {
    res->type = LEX_ENDENTRY;
    return res;
  }

  // if
  if (!strcmp("if", data)) {
    res->type = LEX_IF;
    return res;
  }

  // /if
  if (!strcmp("/if", data)) {
    res->type = LEX_ENDIF;
    return res;
  }

  // /
  if (*data == '/') {
    res->type = LEX_DIV;
    if (strlen(data) > 1) {
      lex->saved = str_clone(data + 1);
    }
    return res;
  }

  if (!strcmp("var", data)) {
    res->type = LEX_VAR;
    return res;
  }

  // number
  if (isStrNum(data)) {
    res->type = LEX_NUMBER;
    res->value = atoll(data);
    return res;
  }

  //generic id
  res->type = LEX_ID;
  res->value = (uintptr_t) str_clone(data);
  
  return res;
}

void token_free(struct token *tok) {

  if (tok->type == LEX_ID) {
    free( (void*) tok->value);
  }

  free(tok);

}

const char* token_str(struct token *tok) {
  switch (tok->type) {
  case LEX_NONE:
    return "NONE";
  case LEX_ASSIGN:
    return "=";
  case LEX_DEC:
    return "--";
  case LEX_DIFFERENT:
    return "!=";
  case LEX_DIV:
    return "/ (Division)";
  case LEX_ENDENTRY:
    return "/entry";
  case LEX_ENDIF:
    return "/if";
  case LEX_ENTRY:
    return "entry";
  case LEX_EQUAL:
    return "==";
  case LEX_ID:
    return "an identifier";
  case LEX_IF:
    return "if";
  case LEX_INC:
    return "++";
  case LEX_MAJOR:
    return ">";
  case LEX_MINOR:
    return "<";
  case LEX_MINUS:
    return "-";
  case LEX_NEWLINE:
    return "a new line";
  case LEX_NOT:
    return "!";
  case LEX_NUMBER:
    return "a number";
  case LEX_PLUS:
    return "+";
  case LEX_TIMES:
    return "*";
  case LEX_VAR:
    return "var";
  default:
    return "unknown";
  }

  return "";
}

bool lexer_eof(struct lexer *lex) {
  return lex->errcode == FILEEND;
}

bool lexer_error(struct lexer *lex) {
  return lex->errcode == ERROR;
}

int8_t token_comparePriority(struct token *tok1, struct token *tok2) {
  uint8_t p1 = token_getPriority(tok1);
  uint8_t p2 = token_getPriority(tok2);

  return (p1 > p2) - (p1 < p2);
}

int8_t token_getPriority(struct token *tok) {
  switch(tok->type) {
  case LEX_ASSIGN:
    return 1;
 /* 
  case LEX_OR:
    return 2;

  case LEX_AND:
    return 3;
*/
  case LEX_DIFFERENT:
  case LEX_EQUAL:
    return 4;

  case LEX_MAJOR:
  case LEX_MINOR:
    return 5;

  case LEX_PLUS:
    return 6;

  case LEX_DIV:
  case LEX_TIMES:
    return 7;

  case LEX_MINUS:
  case LEX_NOT:
    return 8;

  case LEX_INC:
  case LEX_DEC:
    return 9;

  default:
    return -1;
  }
}

enum optype token_getOpType(struct token *tok) {
  switch (tok->type) {
  case LEX_DEC:
  case LEX_INC:
  case LEX_MINUS:
  case LEX_NOT:
    return OPTYPE_UNARY;

  case LEX_ASSIGN:
  case LEX_DIFFERENT:
  case LEX_DIV:
  case LEX_EQUAL:
  case LEX_MAJOR:
  case LEX_MINOR:
  case LEX_PLUS:
  case LEX_TIMES:
    return OPTYPE_BINARY;

  default:
    return OPTYPE_NOTOP;
  }
}

