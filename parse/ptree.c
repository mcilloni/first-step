#include "ptree.h"
#include "types.h"

#include "../lex/lex.h"
#include "../utils/env.h"

#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>

void pnode_addLeaf(struct pnode *pnode, struct pnode *leaf) {
  leaf->root = pnode;
  array_append(pnode->leaves, leaf);
}

bool pnode_addSymbolReal(struct pnode *pnode, const char *id, struct type *type, enum symbols_resp *resp, bool decl) {
  if (!pnode) {
    return false;
  }

  enum symbols_resp res;

  if (!pnode_isScope(pnode)) {
    bool ret = pnode_addSymbolReal(pnode->root, id, type, &res, decl);
    
    if (resp) {
      *resp = res;
    }

    return ret;
  }

  res = symbols_register(((struct pscope*) pnode)->symbols, id, type, decl);
  
  if (resp) {
    *resp = res;
  }

  return true;
}

bool pnode_addSymbol(struct pnode *pnode, const char *id, struct type *type, enum symbols_resp *resp) {
  return pnode_addSymbolReal(pnode, id, type, resp, false);
}

bool pnode_declSymbol(struct pnode *pnode, const char *id, struct type *type, enum symbols_resp *resp) {
  return pnode_addSymbolReal(pnode, id, type, resp, true);
}

Array* pnode_getFuncParams(struct pnode *pnode) {
  if (!pnode_isFunc(pnode)) {
    return NULL;
  }

  return ((struct pfunc*) pnode)->params;
}

Symbols* pnode_getSyms(struct pnode *pnode) {
  
  if (!pnode_isScope(pnode)) {
    return NULL;
  }

  return ((struct pscope*) pnode)->symbols;

}

struct type* pnode_getType(struct pnode *pnode, const char *name) {

  struct type *ret;

  if ((ret = type_getBuiltin(name))) {
      return ret;
  }

  if (!pnode) {
    return NULL;
  }

  if (!(pnode_isScope(pnode))) {
    return pnode_getType(pnode->root, name);
  }

  struct pscope *pscope = (struct pscope*) pnode;
  if (!(ret = types_get(pscope->types, name))) {
    return pnode_getType(pnode->root, name); 
  }

  return ret;
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

  struct symbol *sym = NULL;

  if (!pnode_isScope(pnode) || !(sym = symbols_get(((struct pscope*) pnode)->symbols, id))) {
    return pnode_symbolType(pnode->root, id);
  }

  return sym->type;

}

bool nonterminals_isConst(enum nonterminals id) {
  switch (id) {
  case PR_NUMBER:
    return true;
  default:
    return false;
  }
}

bool nonterminals_isFunc(enum nonterminals id) {
  switch (id) {
  case PR_ENTRY:
  case PR_FUNC:
    return true;
  default:
    return false;
  }
}

bool pnode_isFunc(struct pnode *pnode) { 
  return nonterminals_isFunc(pnode->id);
}

bool nonterminals_isScope(enum nonterminals id) {
  switch (id) {
  case PR_BODY:
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
    if (nonterminals_isFunc(id)) {
      ret = malloc(sizeof(struct pfunc));
    } else {
      if (value) {
        //so the type should be NULL
        ret = calloc(1, sizeof(struct pexpr));
      } else {
        ret = malloc(sizeof(struct pnode));
      }
    }
  }

  ret->id = id;
  ret->root = NULL;
  ret->leaves = array_new(2);

  return ret;
}

struct pnode* pnode_newfunc(enum nonterminals id, Array *params) {
  struct pfunc *ret = (struct pfunc*) pnode_new(id);

  if (pnode_isFunc(&(ret->node))) {
    ret->params = params;
  }

  return (struct pnode*) ret;
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
    struct pscope *pscope = (struct pscope*) pnode;
    symbols_free(pscope->symbols);
    types_free(pscope->types);
  } 

  if (pnode_isFunc(pnode)) {
    Array *arr = ((struct pfunc*) pnode)->params;
    array_freeContents(arr, (void (*)(void*)) type_free);
    array_free(arr);
  }

  if (pnode->id == PR_ID) {
    free((void*)((struct pexpr*) pnode)->value);
  }
  array_freeContents(pnode->leaves, (void (*) (void*)) pnode_free);
  array_free(pnode->leaves);

  free(pnode);
}

void pnode_dump(struct pnode *val, uint64_t depth) {
  if (!val) {
    return;
  }

  for(uint64_t i = 0; i < depth; ++i) {
    fputs("| ", stdout);
  }

  fputs(nt_str(val->id), stdout);

  switch(val->id) {
  case PR_NUMBER:
    printf(": %" PRIdMAX, (intmax_t) pnode_getval(val));
    break;
  case PR_ID: {
    char buf[2048];
    printf(": %s, type %s", (const char*) pnode_getval(val), type_str(pnode_evalType(val, NULL), buf, 2048));
    break;
  }
  case PR_BINOP:
  case PR_UNOP:
    printf(": %s", tokentype_str((enum token_type) pnode_getval(val)));
    break;
  default:
    break;
  }
  putchar('\n');

  if (pnode_isScope(val)) {
    struct pscope *pscope = (struct pscope*) val;

    symbols_dump(pscope->symbols, depth + 1);
  }

  size_t len = array_len(val->leaves);

  for(size_t i = 0; i < len; ++i) {
    pnode_dump(*leaves_get(val->leaves, i), depth + 1);
  }
}

void ptree_dump(struct pnode *root) {

  pnode_dump(root, 0);
}

