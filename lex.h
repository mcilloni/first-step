#if !defined(LEX_H)
#define LEX_H

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

enum token_type {
  NONE = 0,
  ASSIGN,
  DIV,
  ENDENTRY,
  ENDIF,
  ENTRY,
  EQUAL,
  ID,
  IF,
  INC,
  MAJOR,
  MINOR,
  MINUS,
  NEWLINE,
  NOT,
  NUMBER,
  PLUS,
  TIMES,
  VAR
};

enum lex_errc {
  NOERR = 0,
  ERROR = 1,
  FILEEND = 2 //Expand me
};

static const char *const lex_errors[] = {
  "All good",
  "Error"
};

struct lexer {
  FILE *file;
  enum lex_errc errcode;
  const char *error;
  
  bool consider;
};

struct token {
  enum token_type type;
  intmax_t value;
};

struct lexer* lexer_open(const char *path);
void lexer_close(struct lexer *lex);
struct token gettok(struct lexer *lex);
void freetok(struct token tok);

#endif
