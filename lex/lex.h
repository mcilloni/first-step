#if !defined(_LEX_H)
#define _LEX_H

#include "../utils/errors.h"
#include "../utils/lines.h"

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

enum token_type {
  LEX_NONE = 0,
  LEX_AND,
  LEX_APOS,
  LEX_ASSIGN,
  LEX_CPAR,
  LEX_COMMA,
  LEX_DEC,
  LEX_DECL,
  LEX_DIFFERENT,
  LEX_DIV,
  LEX_ENDENTRY,
  LEX_ENDFUNC,
  LEX_ENDIF,
  LEX_ENTRY,
  LEX_EQUAL,
  LEX_FUNC,
  LEX_ID,
  LEX_IF,
  LEX_INC,
  LEX_MAJOR,
  LEX_MINOR,
  LEX_MINUS,
  LEX_NEWLINE,
  LEX_NOT,
  LEX_NUMBER,
  LEX_OPAR,
  LEX_OR,
  LEX_PLUS,
  LEX_POW,
  LEX_PTR,
  LEX_RETURN,
  LEX_STRING,
  LEX_STRUCT,
  LEX_TIMES,
  LEX_VAL,
  LEX_VAR
};

static const char *const lex_errors[] = {
  "All good",
  "Error"
};

struct token {
  enum token_type type;
  uintmax_t value;
};

struct lexer {
  FILE *file;
  enum errors errcode;

  char peek;
  bool newline;

  bool inString;
  char *saved;
};

enum optype {
  OPTYPE_NOTOP = 0,
  OPTYPE_BINARY,
  OPTYPE_UNARY
};

struct token* token_get(struct lexer *lex);
void token_free(struct token* tok);

void lexer_close(struct lexer *lex);
void lexer_discardLine(struct lexer *lex);
bool lexer_eof(struct lexer *lex);
bool lexer_error(struct lexer *lex);
struct lexer* lexer_open(const char *path);
struct lexer* lexer_fromFile(FILE *file);

int8_t token_comparePriority(struct token *tok1, struct token *tok2);
int8_t token_getPriority(struct token *tok);
enum optype token_getOpType(struct token *tok);
const char* token_str(struct token *tok);

const char* tokentype_str(enum token_type type);

#endif
