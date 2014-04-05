#if !defined(_PARSE_H)
#define _PARSE_H

#include "ptree.h"

#include "../lex/lex.h"

typedef bool (*bodyender)(struct token*);

struct token* token_getOrDie(struct lexer *lex);

struct pnode* parse(FILE *file);

#endif
