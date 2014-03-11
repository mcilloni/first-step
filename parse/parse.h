#if !defined(_PARSE_H)
#define _PARSE_H

#include "ptree.h"

#include "../lex/lex.h"

struct token* token_getOrDie(struct lexer *lex);

struct pnode* parse(const char *filename);

#endif
