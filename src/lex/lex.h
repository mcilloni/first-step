#if !defined(_LEX_H)
#define _LEX_H

#include <list/pool.h>

#include <utils/errors.h>
#include <utils/lines.h>

#include <cstdio>
#include <cstdint>


enum token_type {
  LEX_NONE = 0,
  LEX_ALIAS,
  LEX_AMPER,
  LEX_AND,
  LEX_APOS,
  LEX_ASSIGN,
  LEX_BREAK,
  LEX_CAST,
  LEX_CBRAC,
  LEX_COLON,
  LEX_COMMA,
  LEX_CONTINUE,
  LEX_CPAR,
  LEX_DEC,
  LEX_DECL,
  LEX_DIFFERENT,
  LEX_DIV,
  LEX_ELSE,
  LEX_ENDENTRY,
  LEX_ENDFUNC,
  LEX_ENDIF,
  LEX_ENDWHILE,
  LEX_ENTRY,
  LEX_EQUAL,
  LEX_FATARROW,
  LEX_FUNC,
  LEX_ID,
  LEX_IF,
  LEX_IMPORT,
  LEX_INC,
  LEX_MAJEQ,
  LEX_MAJOR,
  LEX_MINEQ,
  LEX_MINOR,
  LEX_MINUS,
  LEX_MOD,
  LEX_MODULE,
  LEX_MUT,
  LEX_NEWLINE,
  LEX_NOT,
  LEX_NUMBER,
  LEX_OBRAC,
  LEX_OPAR,
  LEX_OR,
  LEX_PIPE,
  LEX_PLUS,
  LEX_POW,
  LEX_PTR,
  LEX_RETURN,
  LEX_SEPARATOR,
  LEX_SIZE,
  LEX_STRING,
  LEX_STRUCT,
  LEX_TIMES,
  LEX_VAL,
  LEX_WHILE,
  LEX_XOR
};

static const char *const lex_errors[] = {
  "All good",
  "Error"
};

struct token {
  enum token_type type;
  uintmax_t value;
  uintmax_t lineno;
};

struct lexer {
  struct filereader *fr;
  enum errors errcode;

  struct line *line;

  char peek;
  bool newline;

  bool inString;
  bool closeFile;
  char *saved;

  Pool *tokens;
};

enum optype {
  OPTYPE_NOTOP = 0,
  OPTYPE_TERNARY,
  OPTYPE_BINARY,
  OPTYPE_UNARY
};

struct token* token_get(struct lexer *lex);

void lexer_close(struct lexer *lex);
void lexer_discardLine(struct lexer *lex);
bool lexer_eof(struct lexer *lex);
bool lexer_error(struct lexer *lex);
struct lexer* lexer_open(const char *path);
struct lexer* lexer_fromFile(FILE *file);

int8_t token_comparePriority(struct token *tok1, struct token *tok2);
int8_t tokentype_getPriority(enum token_type type);
int8_t token_getPriority(struct token *tok);
enum optype token_getOpType(struct token *tok);
bool token_isBooleanOp(enum token_type type);
const char* token_str(struct token *tok);

const char* tokentype_str(enum token_type type);

#endif
