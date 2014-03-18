#include "ptree.h"
#include "types.h"

#include "../utils/env.h"

#include <stdlib.h>

struct pscope {                                                                                         
  struct pnode node;                                                                                                               
  Symbols *symbols;                                                                                        
};       

struct pexpr {
  struct pnode node;
  uintmax_t value;
  struct type *type;
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

  res = symbols_register(((struct pscope*) pnode)->symbols, id, type);
  
  if (resp) {
    *resp = res;
  }

  return true;
}

uintmax_t pnode_getval(struct pnode *pnode) {
  if (!pnode_isValue(pnode)) {
    return 0;
  }

  return ((struct pexpr*) pnode)->value;
}

void pnode_verifyNodesAreCompatible(struct pnode *root, struct pnode *assign, struct pnode *assigned) {

  struct type *first = pnode_evalType(assign, root);
  struct type *second = pnode_evalType(assigned, root);

  switch(type_areCompatible(first, second)) {
  case TYPECOMP_NO:
    env.fail("Cannot assign an expression of type %s to a location of type %s", second->name, first->name);
    break;
  case TYPECOMP_SMALLER:
    env.warning("Coercing an expression of type %s to smaller type %s", second->name, first->name);
    break;
  default:
    break;
  }

}

struct type* pnode_evalType(struct pnode *pnode, struct pnode *scope) {

  if (!scope) {
    scope = pnode;
  }

  if (!(pnode && pnode_isValue(pnode))) {
    return NULL;
  }

  struct pexpr *pexpr = (struct pexpr*) pnode;
  struct type *ret = NULL;

  if (pexpr->type) {
    return pexpr->type; 
  } 

  switch (pnode->id){ 
  case PR_BINOP: {
  
    struct type *firstType = pnode_evalType(*leaves_get(pnode->leaves, 0), scope);
    struct pnode *second = *leaves_get(pnode->leaves, 1);

    if (pexpr->value == LEX_ASSIGN) {
      ret = firstType; //only the destination type matters
    } else {
      ret = type_evalLarger(firstType, pnode_evalType(second, scope));
    }

    break;
  }

  case PR_ID: {
    ret = pnode_symbolType(scope, (const char*) pexpr->value);
    break;
  }
                
  case PR_UNOP: {
    ret = pnode_evalType(*leaves_get(pnode->leaves, 0), scope);
    break;
  }

  case PR_NUMBER: {
    ret = type_evalNumberType(pexpr->value);
    break;
  }
 
  default: 
    ret = NULL;
  }

  pexpr->type = ret;

  return ret;

}

bool pnode_getValue(struct pnode *pnode, uintmax_t *val) {

  if (!pnode_isValue(pnode)) {
    return false;
  }
  
  *val = ((struct pexpr*) pnode)->value;

  return true;

}

struct type* pnode_symbolType(struct pnode *pnode, const char *id) {

  if (!pnode) {
    return NULL;
  }

  const char *type = NULL;

  if (!pnode_isScope(pnode) || !(type = symbols_getType(((struct pscope*) pnode)->symbols, id))) {
    return pnode_symbolType(pnode->root, id);
  }

  return type_getTypeDef(NULL, type);

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
    if (value) {
      //so the type should be NULL
      ret = calloc(1, sizeof(struct pexpr));
    } else {
      ret = malloc(sizeof(struct pnode));
    }
  }

  ret->id = id;
  ret->root = NULL;
  ret->leaves = array_new(2);

  return ret;
}

struct pnode* pnode_newval(enum nonterminals id, uintmax_t val) {
  struct pexpr *ret = (struct pexpr*) pnode_new(id);
  
  if (pnode_isValue(&(ret->node))) {
    ret->value = val;
  }
  
  return (struct pnode*) ret;
}

void pnode_free(struct pnode *pnode) {
  if (pnode_isScope(pnode)) {
    symbols_free(((struct pscope*) pnode)->symbols);
  } 

  if (pnode->id == PR_ID) {
    free((void*)((struct pexpr*) pnode)->value);
  }
  array_freeContents(pnode->leaves, (void (*) (void*)) pnode_free);
  array_free(pnode->leaves);

  free(pnode);
}


