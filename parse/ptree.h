#if !defined(_PTREE_H)
#define _PTREE_H

#include "nonterminals.h"
#include "symbols.h"
#include "pnleaves.h"
#include "../lex/lex.h"

struct pnode {
  enum nonterminals id;
  struct pnode *root;
  Leaves *leaves;
};

void pnode_addLeaf(struct pnode *pnode, struct pnode *leaf);
bool pnode_addSymbol(struct pnode *pnode, const char *id, const char *type, enum symbols_resp *resp);
bool pnode_isConst(struct pnode *pnode);
bool pnode_isScope(struct pnode *pnode);
bool pnode_isValue(struct pnode *pnode);
bool pnode_isInCurrentScope(struct pnode *pnode, const char *id);
const char* pnode_symbolType(struct pnode *pnode, const char *id);
struct pnode* pnode_new(enum nonterminals id);
struct pnode* pnode_newval(enum nonterminals id, uintptr_t val);
void pnode_free(struct pnode *pnode);

#endif

