/*
 *  This file is part of First Step.
 *  
 *  First Step is free software: you can redistribute it and/or modify it under the terms of the GNU Lesser General Public License as published by the Free Software 
 *  Foundation, either version 3 of the License, or (at your option) any later version. 
 *
 *  First Step is distributed in the hope that it will be useful, but 
 *  WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with First Step.  If not, see <http://www.gnu.org/licenses/>
 *
 *  Copyright (C) Marco Cilloni <marco.cilloni@yahoo.com> 2014
 *
 */

#include "lex.h"

#include <utils/utils.h>
#include <utils/env.h>

#include <syms/types.h>

#include <ctype.h>
#include <errno.h>
#include <inttypes.h>
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
  lex->tokens = pool_new();
  env_setLine(line_read(lex->file, &lex->errcode));
  
  if (lexer_error(lex)) {
    env.fail("Error while reading file %s: %s", strerror(errno));
  }

  lex->peek = *(env.line->val);
  ++env.line->position;
  
  lex->newline = false;

  return lex;
  
}

void token_free(struct token *tok) {

  if (tok) {

    switch (tok->type) {

    case LEX_CAST:
      type_free((struct type*) tok->value);
      break;

    case LEX_ID:
    case LEX_STRING:
      free( (void*) tok->value);
      break;

    default:
      break;

    }

    free(tok);
  }

}

void lexer_close(struct lexer *lex) {
  
  if (lex->file) {
    fclose(lex->file);
  }

  pool_release(lex->tokens, (void (*)(void *)) token_free);

  line_free(env.line);
  free(lex);
  
}

void lexer_discardLine(struct lexer *lex) {
      env_setLine(line_read(lex->file, &lex->errcode));

      if (!lexer_error(lex)) {
        lex->peek = *env.line->val;
        ++env.line->position;
      }
}

bool ch_isPar(char c) {
  return ( c == '(') || ( c == ')');
}

bool ch_id(char c) {
  return isalnum(c) || c == '_';
}

bool ch_sym(char c) {
  return c != '_' && ispunct(c);
}

bool stok(struct lexer *lex, char *data, size_t max) {

  char ch;
  size_t i;

  do {
    if (lex->newline) {
      lex->newline = false;
      *data = '\n';
      *(data + 1) = 0;
      return true;
    }

    for (i = 0; i < max && !lex->newline;) {
      
      ch = lex->peek;
      ++env.line->position;

      lex->newline = (env.line->len + 1) == env.line->position;     

      if (!lex->newline) {
        lex->peek = env.line->val[env.line->position - 1];     
      } else {
        lexer_discardLine(lex);
        if (lexer_error(lex)) {
          return false;
        }
      }

      if (ch == '"') {
        if (lex->inString) {
          break;
        }

        if (i) {
          break;
        } else {
          lex->inString = true;
          continue;
        }
        
      }
      
      if(!lex->inString) {

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

        if ((ch_isPar(lex->peek) || (ch_id(ch) && ch_sym(lex->peek)) || (ch_sym(ch) && ch_id(lex->peek)))) {
          break;
        }

        if (lex->peek == '"') {
          break;
        }

      } else {
        data[i] = ch;
        ++i;
      }
    }
  } while (!i);

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
  uintmax_t lineno = env.line->lineno;

  if (lex->saved) {
    strcpy(data, lex->saved);
    free(lex->saved);
    lex->saved = NULL;
  } else {
    if (!stok(lex, (char*) data, MAX_TOKEN_LENGTH)) {
      return NULL;
    }
  }

  struct token *res = pool_zalloc(lex->tokens, sizeof(struct token));
  
  res->lineno = lineno;

  if (lex->inString) {
    *res = (struct token) { LEX_STRING, (uintmax_t) str_clone(data) };
    lex->inString = false;
    return res;
  }

  // \n
  if (!strcmp("\n", data)) {
    res->type = LEX_NEWLINE;
    return res;
  }

  // '
  if (!strcmp("'", data)) {
    res->type = LEX_APOS;
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

  // and
  if (!strcmp("and", data)) {
    res->type = LEX_AND;
    return res;
  }

  //or
  if (!strcmp("or", data)) {
    res->type = LEX_OR;
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

  // ^
  if (!strcmp("^", data)) {
    res->type = LEX_POW;
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

  // )
  if (!strcmp(")", data)) {
    res->type = LEX_CPAR;
    return res;
  }

  // (
  if (!strcmp("(", data)) {
    res->type = LEX_OPAR;
    return res;
  }

  // ,
  if (!strcmp(",", data)) {
    res->type = LEX_COMMA;
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

  // alias
  if (!strcmp("alias", data)) {
    res->type = LEX_ALIAS;
    return res;
  }

  // struct
  if (!strcmp("struct", data)) {
    res->type = LEX_STRUCT;
    return res;
  }

  // cast
  if (!strcmp("cast", data)) {
    res->type = LEX_CAST;
    return res;
  }

  // func
  if (!strcmp("func", data)) {
    res->type = LEX_FUNC;
    return res;
  }

  // /func
  if (!strcmp("/func", data)) {
    res->type = LEX_ENDFUNC;
    return res;
  }

  // if
  if (!strcmp("if", data)) {
    res->type = LEX_IF;
    return res;
  }

  // else
  if (!strcmp("else", data)) {
    res->type = LEX_ELSE;
    return res;
  }

  // /if
  if (!strcmp("/if", data)) {
    res->type = LEX_ENDIF;
    return res;
  }

  // while
  if (!strcmp("while", data)) {
    res->type = LEX_WHILE;
    return res;
  }

  // /while
  if (!strcmp("/while", data)) {
    res->type = LEX_ENDWHILE;
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

  // return
  if (!strcmp("return", data)) {
    res->type = LEX_RETURN;
    return res;
  }

  // var
  if (!strcmp("var", data)) {
    res->type = LEX_VAR;
    return res;
  }

  // decl
  if (!strcmp("decl", data)) {
    res->type = LEX_DECL;
    return res;
  }

  // number
  if (isStrNum(data)) {
    res->type = LEX_NUMBER;
    res->value = strtoumax(data, NULL, 10);
    return res;
  }

  //ptr
  if (!strcmp("ptr", data)) {
    res->type = LEX_PTR;
    return res;
  }

  //val
  if (!strcmp("val", data)) {
    res->type = LEX_VAL;
    return res;
  }
  
  //generic id
  res->type = LEX_ID;
  res->value = (uintptr_t) str_clone(data);

  return res;
}

const char* token_str(struct token *tok) {
  return tokentype_str(tok->type);
}

const char* tokentype_str(enum token_type type) {
  switch (type) {
  case LEX_NONE:
    return "NONE";
  case LEX_ALIAS:
    return "alias";
  case LEX_APOS:
    return "'";
  case LEX_ASSIGN:
    return "=";
  case LEX_CAST:
    return "cast";
  case LEX_COMMA:
    return ",";
  case LEX_CPAR:
    return ")";
  case LEX_DEC:
    return "--";
  case LEX_DECL:
    return "decl";
  case LEX_DIFFERENT:
    return "!=";
  case LEX_DIV:
    return "/";
  case LEX_ELSE:
    return "else";
  case LEX_ENDENTRY:
    return "/entry";
  case LEX_ENDFUNC:
    return "/func";
  case LEX_ENDIF:
    return "/if";
  case LEX_ENTRY:
    return "entry";
  case LEX_EQUAL:
    return "==";
  case LEX_FUNC:
    return "func";
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
  case LEX_OPAR:
    return "(";
  case LEX_PTR:
    return "ptr";
  case LEX_PLUS:
    return "+";
  case LEX_RETURN:
    return "return";
  case LEX_STRING:
    return "string";
  case LEX_STRUCT:
    return "struct";
  case LEX_TIMES:
    return "*";
  case LEX_VAL:
    return "val";
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

  case LEX_OR:
    return 2;

  case LEX_AND:
    return 3;

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

  case LEX_POW:
    return 8;

  case LEX_MINUS:
  case LEX_NOT:
    return 9;

  case LEX_CAST:
  case LEX_PTR:
  case LEX_VAL:
    return 10;

  case LEX_APOS:
  case LEX_INC:
  case LEX_DEC:
    return 11;
  default:
    return -1;
  }
}

enum optype token_getOpType(struct token *tok) {
  switch (tok->type) {
  case LEX_CAST:
  case LEX_DEC:
  case LEX_INC:
  case LEX_MINUS:
  case LEX_NOT:
  case LEX_PTR:
  case LEX_VAL:
    return OPTYPE_UNARY;

  case LEX_AND:
  case LEX_APOS:
  case LEX_ASSIGN:
  case LEX_DIFFERENT:
  case LEX_DIV:
  case LEX_EQUAL:
  case LEX_MAJOR:
  case LEX_MINOR:
  case LEX_OR:
  case LEX_PLUS:
  case LEX_POW:
  case LEX_TIMES:
    return OPTYPE_BINARY;

  default:
    return OPTYPE_NOTOP;
  }
}

bool token_isBooleanOp(enum token_type type) {
  switch (type) {
  case LEX_AND:
  case LEX_DIFFERENT:
  case LEX_EQUAL:
  case LEX_MAJOR:
  case LEX_MINOR:
  case LEX_NOT:
  case LEX_OR:
    return true;
  default:
    return false;
  }
}

