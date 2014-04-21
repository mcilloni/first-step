#if !defined(_PARSE_H)
#define _PARSE_H

#include "ptree.h"

#include <lex/lex.h>
#include <list/pool.h>

#include <stdint.h>

typedef bool (*bodyender)(struct token*);

struct parser {
  struct lexer *lex;

  bool firstTok;
  struct token *precTok;
  struct token *curTok;
  struct token *nextTok;
  uintmax_t lastLineno;
  char *filename;

  Pool *types;
};

struct token* parser_getTok(struct parser *prs);

struct parser* parser_new(const char *filename);
struct pnode* parser_parse(struct parser *parser, FILE *file);
void parser_close(struct parser *parser);

#endif
