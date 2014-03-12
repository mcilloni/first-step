#include "ptree.h"

#include <stdio.h>
#include <stdlib.h>

int main(void) {
  struct pnode *st = pnode_new(PR_PROGRAM);
  pnode_addLeaf(st, pnode_newval(PR_NUMBER, 33));
  struct pnode *en = pnode_new(PR_ENTRY);
  pnode_addLeaf(st, en);

  struct pnode *plus = pnode_newval(PR_UNOP, LEX_PLUS); 
  pnode_addLeaf(en, plus);

  enum symbols_resp resp;

  if (pnode_addSymbol(plus, "potato", "int8", &resp)) {
    puts((resp == SYM_ADDED) ? "Added" : "Exists");
  }
  
  if (pnode_addSymbol(st, "motato", "int16", &resp)) {
    puts((resp == SYM_ADDED) ? "Added" : "Exists");
  }

  if (pnode_addSymbol(plus, "potato", "int8", &resp)) {
    puts((resp == SYM_ADDED) ? "Added" : "Exists");
  }

  puts(pnode_symbolType(plus, "motato")->name);

  puts(pnode_isInCurrentScope(plus, "motato") ? "Yup" : "Nope");

  pnode_free(st);

  return EXIT_SUCCESS;
}
