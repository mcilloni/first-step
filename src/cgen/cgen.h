#if !defined(_CGEN_H)
#define _CGEN_H

#include "../parse/ptree.h"

#include <stdio.h>

void cgen(const char *filename, struct pnode *tree, FILE *out);

#endif
