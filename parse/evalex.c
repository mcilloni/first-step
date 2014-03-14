#include "ptree.h"

#include "../lex/lex.h"

#include <stdio.h>
#include <stdlib.h>

extern struct pnode* expr_evalBinary(struct token*,struct pnode*,struct pnode*);
extern struct pnode* expr_evalUnary(struct token*,struct pnode*);

int main(void) {
  struct pnode *val1 = pnode_newval(PR_NUMBER, 33);
  struct pnode *val2 = pnode_newval(PR_NUMBER, 22);

  struct token tok = {LEX_PLUS};

  struct pnode *res = expr_evalBinary(&tok, val1, val2);
  printf("%lu\n", pnode_getval(res));
  pnode_free(res);
  
  tok.type = LEX_MINUS;

  res = expr_evalUnary(&tok, val1);
  printf("%ld\n", (intptr_t) pnode_getval(res));
  pnode_free(res);

  tok.type = LEX_NOT;

  res = expr_evalUnary(&tok, val1);
  printf("%ld\n", (intptr_t) pnode_getval(res));
  pnode_free(res);
  
  tok.type = LEX_MAJOR;

  res = expr_evalBinary(&tok, val1, val2);
  printf("%ld\n", (intptr_t) pnode_getval(res));
  pnode_free(res);
  
  pnode_free(val1);
  pnode_free(val2);

  return EXIT_SUCCESS;
}

