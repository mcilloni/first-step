#if !defined(LEX_H)
#define LEX_H

#include "../utils/errors.h"
#include "../utils/lines.h"

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

enum token_type {
  LEX_NONE = 0,
  LEX_ASSIGN,
  LEX_DEC,
  LEX_DIFFERENT,
  LEX_DIV,
  LEX_ENDENTRY,
  LEX_ENDIF,
  LEX_ENTRY,
  LEX_EQUAL,
  LEX_ID,
  LEX_IF,
  LEX_INC,
  LEX_MAJOR,
  LEX_MINOR,
  LEX_MINUS,
  LEX_NEWLINE,
  LEX_NOT,
  LEX_NUMBER,
  LEX_PLUS,
  LEX_TIMES,
  LEX_VAR
};

static const char *const lex_errors[] = {
  "All good",
  "Error"
};

struct lexer {
  FILE *file;
  enum errors errcode;
  const char *error;

  struct line current;
  char peek;
  bool newline;
};

struct token {
  enum token_type type;
  intmax_t value;
};

struct lexer* lexer_open(const char *path);
void lexer_close(struct lexer *lex);
struct token gettok(struct lexer *lex);
void freetok(struct token tok);

const char* represent(enum token_type tok);

#endif
