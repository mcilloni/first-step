#if !defined(_CGEN_H)
#define _CGEN_H

#include "../parse/ptree.h"

#include <stdio.h>

void cgen(struct pnode *tree, FILE *out);

#endif
