#if !defined(_PTREE_H)
#define _PTREE_H

#include "nonterminals.h"
#include "pnleaves.h"
#include <syms/symbols.h>
#include <syms/types.h>

#include <lex/lex.h>

#include <list/pool.h>

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
  struct ftype *ftype;

  const char *name;
  Symbols *params;
};

struct pscope {                                                                                         
  struct pnode node;                                                                                                               
  Symbols *symbols;                                                                                        
  Aliases *aliases;
};       

extern struct pnode *expr_empty;

void pnode_addLeaf(struct pnode *pnode, struct pnode *leaf);
bool pnode_addSymbol(struct pnode *pnode, const char *id, struct type *type, enum symbols_resp *resp);
bool pnode_declSymbol(struct pnode *pnode, const char *id, struct type *type, enum symbols_resp *resp);
struct type* pnode_evalType(Pool *pool, struct pnode *pnode, struct pnode *scope);
void pnode_free(struct pnode *pnode);
void pnode_alias(struct pnode *pnode, const char *name, struct type *type);
Aliases* pnode_getAliases(struct pnode *pnode);
Symbols* pnode_getSyms(struct pnode *pnode);
Symbols* pnode_getFuncParams(struct pnode *pnode);
struct type* pnode_getType(struct pnode *pnode, const char *name);
uintmax_t pnode_getval(struct pnode *pnode);
bool pnode_isConstNum(struct pnode *pnode);
bool pnode_isFunc(struct pnode *pnode);
bool pnode_isScope(struct pnode *pnode);
bool pnode_isValue(struct pnode *pnode);
bool pnode_isInCurrentScope(struct pnode *pnode, const char *id);
struct type* pnode_symbolType(struct pnode *pnode, const char *id);
struct pnode* pnode_new(enum nonterminals id);
struct pnode* pnode_newfunc(Pool *pool, enum nonterminals id, const char *name, struct type *ret, Symbols *params);
struct pnode* pnode_newval(enum nonterminals id, uintmax_t val);
void pnode_verifyNodesAreCompatible(Pool *pool, struct pnode *root, struct pnode *assign, struct pnode *assigned);
struct type* pnode_funcReturnType(struct pnode *pnode);

void ptree_dump(struct pnode *root);

#endif

