/*
 *  This file is part of First Step.
 *  
 *  First Step is free software: you can redistribute it and/or modify it under the terms of the GNU Lesser General Public License as published by the Free Software 
 *  Foundation, either version 3 of the License, or (at your option) any later version. 
 *
 *  First Step is distributed in the hope that it will be useful, but 
 *  WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with First Step.  If not, see <http://www.gnu.org/licenses/>
 *
 *  Copyright (C) Marco Cilloni <marco.cilloni@yahoo.com> 2014
 *
 */

#include "ptree.h"

#include <lex/lex.h>
#include <utils/env.h>
#include <utils/utils.h>

#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

  if (pnode_getType(pnode, id)) {
    env.fail("Identifier %s is already a type identifier");
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

Symbols* pnode_getFuncParams(struct pnode *pnode) {
  if (!pnode_isFunc(pnode)) {
    return NULL;
  }

  return ((struct pfunc*) pnode)->params;
}

Aliases* pnode_getAliases(struct pnode *pnode) {
  
  if (!pnode_isScope(pnode)) {
    return NULL;
  }

  return ((struct pscope*) pnode)->aliases;

}

Symbols* pnode_getSyms(struct pnode *pnode) {
  
  if (!pnode_isScope(pnode)) {
    return NULL;
  }

  return ((struct pscope*) pnode)->symbols;

}

void pnode_alias(struct pnode *pnode, const char *name, struct type *type) {
  if (!pnode) {
    env.fail("Broken tree: found NULL root");
  }  

  if (pnode_isScope(pnode)) {
    struct pscope *pscope = (struct pscope*) pnode;

    

    if (type_getBuiltin(name)) {
      env.fail("Cannot alias: %s is a builtin type", name);
    }

    struct type *oldType = aliases_alias(pscope->aliases, name, type);

    if (oldType) {
      char buf[4096];
      env.fail("Alias %s already assigned to type %s", type_str(oldType, buf, 4096));
    }

  } else {
    pnode_alias(pnode->root, name, type);
  }
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
  if (!(ret = aliases_get(pscope->aliases, name))) {
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

void pnode_verifyNodesAreCompatible(Pool *pool, struct pnode *root, struct pnode *assign, struct pnode *assigned) {

  struct type *first = pnode_evalType(pool, assign, root);
  struct type *second = pnode_evalType(pool, assigned, root);

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

struct type* pnode_evalType(Pool *pool, struct pnode *pnode, struct pnode *scope) {

  if (pnode->id == PR_SIZE) {
    return type_getBuiltin("uint64");
  }

  if (!scope) {
    scope = pnode;
  }

  if (pnode == expr_empty) {
    return type_none;
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
  case PR_CALL: {
    ret = ((struct ftype*) pnode_evalType(pool, *leaves_get(pnode->leaves, 0), scope))->ret;
    break;
  }

  case PR_CAST: {
    ret = (struct type*) pexpr->value;
    break;
  }

  case PR_STRING: {
    ret = type_makePtr(pool, type_getBuiltin("uint8"));
    break;
  }

  case PR_BINOP: {
  
    struct type *firstType = pnode_evalType(pool, *leaves_get(pnode->leaves, 0), scope);
    struct pnode *second = *leaves_get(pnode->leaves, 1);

    if (pexpr->value == LEX_ASSIGN) {
      ret = firstType; //only the destination type matters
    } else {
      if (token_isBooleanOp(pexpr->value)) {
        ret = type_getBuiltin("bool");
      } else {
        ret = type_evalLarger(firstType, pnode_evalType(pool, second, scope));
      }
    }

    break;
  }

  case PR_ID: {
    ret = pnode_symbolType(scope, (const char*) pexpr->value);
    break;
  }
                
  case PR_UNOP: {
    ret = pnode_evalType(pool, *leaves_get(pnode->leaves, 0), scope);

    switch((enum token_type) pnode_getval(pnode)) {
    case LEX_PTR:
      ret = type_makePtr(pool, ret);
      break;

    case LEX_VAL:
      ret = ((struct ptype*) ret)->val;
      break;

    default:
      break;
    }   

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

struct type* pnode_isRootAndIdIsFunc(struct pnode *pnode, const char *id) {
  if (pnode->id == PR_PROGRAM) {
    Leaves *leaves = pnode->leaves;
    struct pnode *leaf;
    size_t len = array_len(leaves);

    for (size_t i = 0; i < len; ++i) {
      leaf = *leaves_get(leaves, i);
      if (pnode_isFunc(leaf)) {
        struct pfunc *pfunc = (struct pfunc*) leaf;

        if (!strcmp(pfunc->name, id)) {
          return (struct type*) pfunc->ftype;
        }
      }
    }
  }

  return NULL;
}

struct type* pnode_symbolType(struct pnode *pnode, const char *id) {

  struct symbol *bt;

  if ((bt = symbol_getBuiltin(id))) {
    return bt->type;
  }

  if (!pnode) {
    return NULL;
  }

  struct type *type = NULL;

  if((type = pnode_isRootAndIdIsFunc(pnode, id))) {
    return type;
  }

  struct symbol *sym = NULL; 

  bool isFunc = pnode_isFunc(pnode);

  if (isFunc) {
    struct pfunc *pfunc = (struct pfunc*) pnode;
  
    if (!strcmp(pfunc->name, id)) {
      return (struct type*) pfunc->ftype;
    }
  }

  if (!pnode_isScope(pnode) || !(sym = symbols_get(((struct pscope*) pnode)->symbols, id))) {
    if (!isFunc || !(sym = symbols_get(((struct pfunc*) pnode)->params, id))) {
      return pnode_symbolType(pnode->root, id);
    }
  }

  return sym->type;

}

struct type* pnode_funcReturnType(struct pnode *pnode) {
  
  if (!pnode) {
    return NULL;
  }

  if (pnode_isFunc(pnode)) {
    struct pfunc *pfunc = (struct pfunc*) pnode;

    return pfunc->ftype->ret;
  }

  return pnode_funcReturnType(pnode->root);
}

bool nonterminals_isConstNum(enum nonterminals id) {
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
  case PR_BINOP:
  case PR_CALL:
  case PR_CAST:
  case PR_ID:
  case PR_NUMBER:
  case PR_SIZE:
  case PR_STRING:
  case PR_STRUCTID:
  case PR_UNOP:
    return true;

  default:
    return false;
  }
}

bool pnode_isConstNum(struct pnode *pnode) {
  return nonterminals_isConstNum(pnode->id); 
}

bool pnode_isScope(struct pnode *pnode) {
  return nonterminals_isScope(pnode->id);
}

bool pnode_isInCurrentScope(struct pnode *pnode, const char *id) {

  if (symbol_getBuiltin(id)) {
    return true;
  }

  if (!pnode) {
    return false;
  }

  if (pnode_isFunc(pnode) && symbols_defined(((struct pfunc*) pnode)->params, id)) {
    return true;
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

    struct pscope *pscope = (struct pscope*) ret;
    pscope->symbols = symbols_new();
    pscope->aliases = aliases_new();
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

struct ftype* type_mkFunFromRetSyms(Pool *pool, struct type *ret, Symbols *params) {
  size_t len = params ? params->size : 0U;
  Array *arp = array_new(len);

  if (len) {

    MapIter *iter = mapiter_start(params);
    Pair *pair;
    
    while ((pair = mapiter_next(iter))) {
      array_append(arp, ((struct symbol*) pair->value)->type);
    }

  }
  return (struct ftype*) type_makeFuncType(pool, ret, arp);
}

struct pnode* pnode_newfunc(Pool *pool, enum nonterminals id, const char *name, struct type *ret, Symbols *params) {
  struct pfunc *retVal = (struct pfunc*) pnode_new(id);

  if (pnode_isFunc(&(retVal->node))) {
    *retVal = (struct pfunc) {retVal->node, type_mkFunFromRetSyms(pool, ret, params), str_clone(name), params};
  }

  return (struct pnode*) retVal;
}

struct pnode* pnode_newval(enum nonterminals id, uintmax_t val) {
  struct pexpr *ret = (struct pexpr*) pnode_new(id);
  
  if (pnode_isValue(&(ret->node))) {
    ret->value = val;
  }
  
  return (struct pnode*) ret;
}

void pnode_free(struct pnode *pnode) {

  if (!pnode || pnode == expr_empty) {
    return;
  }

  if (pnode_isScope(pnode)) {
    struct pscope *pscope = (struct pscope*) pnode;
    symbols_free(pscope->symbols);
    aliases_free(pscope->aliases);
  } 

  if (pnode_isFunc(pnode)) {
    struct pfunc *pfunc = (struct pfunc*) pnode;

    free((void*) pfunc->name);
    symbols_free(pfunc->params);
  }

  if (pnode->id == PR_ID || pnode->id == PR_STRING) {
    free((void*)((struct pexpr*) pnode)->value);
  }
  array_freeContents(pnode->leaves, (void (*) (void*)) pnode_free);
  array_free(pnode->leaves);

  free(pnode);
}

void pnode_dump(Pool *pool, struct pnode *val, uint64_t depth) {
  if (!val) {
    return;
  }

  for(uint64_t i = 0; i < depth; ++i) {
    fputs("| ", stdout);
  }

  fputs(nt_str(val->id), stdout);

  switch(val->id) {
  case PR_CAST: {
    char buf[4096];
    printf(" to type %s", type_str((struct type*) pnode_getval(val), buf, 4096));
    break;
  }
  case PR_NUMBER:
    printf(": %" PRIdMAX, (intmax_t) pnode_getval(val));
    break;
  case PR_ID:
  case PR_STRUCTID: {
    char buf[2048];
    printf(": %s, type %s", (const char*) pnode_getval(val), type_str(pnode_evalType(pool, val, NULL), buf, 2048));
    break;
  }
  case PR_SIZE: {
    char buf[4096];
    printf(" of type %s", type_str((struct type*) pnode_getval(val), buf, 4096));
    break;
  }
  case PR_STRING:
    printf(": \"%s\"", (const char*) pnode_getval(val));
    break;
  case PR_BINOP:
  case PR_UNOP:
    printf(": %s", tokentype_str((enum token_type) pnode_getval(val)));
    break;
  default:
    break;
  }
  
  if (pnode_isFunc(val)) {
    struct pfunc *pfunc = (struct pfunc*) val;
    char buf[2048];    
    printf(" %s of type %s\n", pfunc->name, type_str((struct type*) pfunc->ftype, buf, 2048));

    symbols_dump(pfunc->params, "Parameters:", depth + 1); 
  } else { 
    putchar('\n');
  }

  if (pnode_isScope(val)) {
    struct pscope *pscope = (struct pscope*) val;

    symbols_dump(pscope->symbols, "Symbols:", depth + 1);
    aliases_dump(pscope->aliases, "Aliases:", depth + 1);
  }

  size_t len = array_len(val->leaves);

  for(size_t i = 0; i < len; ++i) {
    pnode_dump(pool, *leaves_get(val->leaves, i), depth + 1);
  }
}

void ptree_dump(struct pnode *root) {
  Pool *tmpPool = pool_new();
  pnode_dump(tmpPool, root, 0);
  
  pool_release(tmpPool, (void (*)(void*)) type_free);
}

