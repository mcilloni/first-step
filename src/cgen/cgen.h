#if !defined(_CGEN_H)
#define _CGEN_H

#include "../parse/ptree.h"

#include <cstdio>

void cgen(const char *filename, struct pnode *tree, FILE *out);

#endif
