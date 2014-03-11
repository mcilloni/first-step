#if !defined(_LEX_H)
#define _LEX_H

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

  char peek;
  bool newline;
};

struct token {
  enum token_type type;
  uintptr_t value;
};

struct token* token_get(struct lexer *lex);
void token_free(struct token* tok);

void lexer_close(struct lexer *lex);
void lexer_discardLine(struct lexer *lex);
bool lexer_eof(struct lexer *lex);
bool lexer_error(struct lexer *lex);
struct lexer* lexer_open(const char *path);

const char* token_str(struct token *tok);

#endif
