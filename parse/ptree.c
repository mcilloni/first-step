#include "ptree.h"

#include <jemalloc/jemalloc.h>

struct pscope {                                                                                         
  struct pnode node;                                                                                                               
  Symbols *symbols;                                                                                        
};       

void pnode_addLeaf(struct pnode *pnode, struct pnode *leaf) {
  leaf->root = pnode;
  array_append(pnode->leaves, leaf);
}

bool pnode_addSymbol(struct pnode *pnode, const char *id, const char *type, enum symbols_resp *resp) {
  if (!pnode) {
    return false;
  }

  enum symbols_resp res;

  if (!pnode_isScope(pnode)) {
    bool ret = pnode_addSymbol(pnode->root, id, type, &res);
    
    if (resp) {
      *resp = res;
    }

    return ret;
  }

  *resp = symbols_register(((struct pscope*) pnode)->symbols, id, type);

  return true;
}

const char* pnode_symbolType(struct pnode *pnode, const char *id) {

  if (!pnode) {
    return NULL;
  }

  const char *type = NULL;

  if (!pnode_isScope(pnode) || !(type = symbols_getType(((struct pscope*) pnode)->symbols, id))) {
    return pnode_symbolType(pnode->root, id);
  }

  return type;

}

bool nonterminals_isConst(enum nonterminals id) {
  switch (id) {
  case PR_NUMBER:
    return true;
  default:
    return false;
  }
}
 
bool nonterminals_isScope(enum nonterminals id) {
  switch (id) {
  case PR_ENTRY:
  case PR_IF:
  case PR_PROGRAM:
    return true;

  default:
    return false;
  }
}

bool nonterminals_isValue(enum nonterminals id) {
  switch (id) {
  case PR_ID:
  case PR_UNOP:
  case PR_BINOP:
  case PR_NUMBER:
    return true;

  default:
    return false;
  }
}

bool pnode_isConst(struct pnode *pnode) {
  return nonterminals_isConst(pnode->id); 
}

bool pnode_isScope(struct pnode *pnode) {
  return nonterminals_isScope(pnode->id);
}

bool pnode_isInCurrentScope(struct pnode *pnode, const char *id) {
  if (!pnode) {
    return false;
  }

  if (pnode_isScope(pnode)) {
    return symbols_defined(((struct pscope*) pnode)->symbols, id);
  }

  return pnode_isInCurrentScope(pnode->root, id);
}

bool pnode_isValue(struct pnode *pnode) {
  return nonterminals_isValue(pnode->id);
}

struct pnode* pnode_new(enum nonterminals id) {

  struct pnode *ret;

  bool value = nonterminals_isValue(id);

  if (nonterminals_isScope(id)) {
    ret = malloc(sizeof(struct pscope));
    ((struct pscope*) ret)->symbols = symbols_new();
  } else { 
    ret = malloc(sizeof(struct pnode));
  }

  ret->id = id;
  ret->root = NULL;
  ret->leaves = value ? NULL : array_new(2);

  return ret;
}

struct pnode* pnode_newval(enum nonterminals id, uintptr_t val) {
  struct pnode *ret = pnode_new(id);
  
  if (pnode_isValue(ret)) {
    ret->leaves = (void*) val;
  }
  
  return ret;
}

void pnode_free(struct pnode *pnode) {
  if (pnode_isScope(pnode)) {
    symbols_free(((struct pscope*) pnode)->symbols);
  } 

  if (!pnode_isValue(pnode) || (pnode->id == PR_ID)) {
    if (pnode->id != PR_ID) {
      array_freeContents(pnode->leaves, (void (*) (void*)) pnode_free);
    }

    array_free(pnode->leaves);
  }

  free(pnode);
}


