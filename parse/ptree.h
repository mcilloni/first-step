#if !defined(_PTREE_H)
#define _PTREE_H

#include "nonterminals.h"
#include "pnleaves.h"
#include "symbols.h"
#include "types.h"

#include "../lex/lex.h"

struct pnode {
  enum nonterminals id;
  struct pnode *root;
  Leaves *leaves;
};

struct pexpr {
  struct pnode node;
  uintmax_t value;
  struct type *type;
};

struct pfunc {
  struct pnode node;
  Array *params;
};

struct pscope {                                                                                         
  struct pnode node;                                                                                                               
  Symbols *symbols;                                                                                        
  Types *types;
};       

void pnode_addLeaf(struct pnode *pnode, struct pnode *leaf);
bool pnode_addSymbol(struct pnode *pnode, const char *id, struct type *type, enum symbols_resp *resp);
bool pnode_declSymbol(struct pnode *pnode, const char *id, struct type *type, enum symbols_resp *resp);
struct type* pnode_evalType(struct pnode *pnode, struct pnode *scope);
void pnode_free(struct pnode *pnode);
Symbols* pnode_getSyms(struct pnode *pnode);
Array* pnode_getFuncParams(struct pnode *pnode);
struct type* pnode_getType(struct pnode *pnode, const char *name);
uintmax_t pnode_getval(struct pnode *pnode);
bool pnode_isConst(struct pnode *pnode);
bool pnode_isFunc(struct pnode *pnode);
bool pnode_isScope(struct pnode *pnode);
bool pnode_isValue(struct pnode *pnode);
bool pnode_isInCurrentScope(struct pnode *pnode, const char *id);
struct type* pnode_symbolType(struct pnode *pnode, const char *id);
struct pnode* pnode_new(enum nonterminals id);
struct pnode* pnode_newfunc(enum nonterminals id, Array *params);
struct pnode* pnode_newval(enum nonterminals id, uintmax_t val);
void pnode_verifyNodesAreCompatible(struct pnode *root, struct pnode *assign, struct pnode *assigned);

void ptree_dump(struct pnode *root);

#endif

