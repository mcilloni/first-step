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

#include <cinttypes>
#include <cstdio>
#include <cstdlib>
#include <cstring>

void pnode_addLeaf(struct pnode *pnode, struct pnode *leaf) {
  leaf->root = pnode;
  array_append(pnode->leaves, leaf);
}

bool pnode_addSymbolReal(struct pnode *pnode, const char *id, struct type *type, struct pnode *init, enum symbols_resp *resp, bool decl) {
  if (!pnode) {
    return false;
  }

  enum symbols_resp res;

  if (!pnode_isScope(pnode)) {
    bool ret = pnode_addSymbolReal(pnode->root, id, type, init, &res, decl);

    if (resp) {
      *resp = res;
    }

    return ret;
  }

  if (pnode_getType(pnode, id)) {
    env.fail("Identifier %s is already a type identifier");
  }

  res = symbols_registerWithOpt(((struct pscope*) pnode)->symbols, id, type, init, (void (*)(void*)) pnode_free, decl);

  if (resp) {
    *resp = res;
  }

  return true;
}

bool pnode_addSymbol(struct pnode *pnode, const char *id, struct type *type, enum symbols_resp *resp) {
  return pnode_addSymbolReal(pnode, id, type, nullptr, resp, false);
}

bool pnode_addSymbolAndInit(struct pnode *pnode, const char *id, struct type *type, struct pnode *init, enum symbols_resp *resp) {
  return pnode_addSymbolReal(pnode, id, type, init, resp, false);
}

bool pnode_declSymbol(struct pnode *pnode, const char *id, struct type *type, enum symbols_resp *resp) {
  return pnode_addSymbolReal(pnode, id, type, nullptr, resp, true);
}

Symbols* pnode_getFuncParams(struct pnode *pnode) {
  if (!pnode_isFunc(pnode)) {
    return nullptr;
  }

  return ((struct pfunc*) pnode)->params;
}

Aliases* pnode_getAliases(struct pnode *pnode) {

  if (!pnode_isScope(pnode)) {
    return nullptr;
  }

  return ((struct pscope*) pnode)->aliases;

}

Symbols* pnode_getSyms(struct pnode *pnode) {

  if (!pnode_isScope(pnode)) {
    return nullptr;
  }

  return ((struct pscope*) pnode)->symbols;

}

void pnode_alias(struct pnode *pnode, const char *name, struct type *type) {
  if (!pnode) {
    env.fail("Broken tree: found nullptr root");
  }

  if (pnode_isScope(pnode)) {
    struct pscope *pscope = (struct pscope*) pnode;



    if (type_getBuiltin(name)) {
      env.fail("Cannot alias: %s is a builtin type", name);
    }

    struct type *oldType = static_cast<struct type*>(aliases_alias(pscope->aliases, name, type));

    if (oldType && oldType->kind != TYPE_ALIAS) {
      char buf[4096];
      env.fail("Alias %s already assigned to type %s", type_str(oldType, buf, 4096));
    }

  } else {
    pnode_alias(pnode->root, name, type);
  }
}

struct type* pnode_isRootAndIdIsFunc(struct pnode *pnode, const char *id) {
  if (pnode->id == PR_ROOT) {
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

  return nullptr;
}

struct type* pnode_findExportedSym(struct proot *root, const char *name) {
  struct pscope *pscope = &root->node;
  struct pnode *pnode = &pscope->node;
  struct symbol *sym;

  if ((sym = symbols_get(pscope->symbols, name))) {
    return sym->type;
  }

  return pnode_isRootAndIdIsFunc(pnode, name);
}

struct type* pnode_extractFromModule(struct pnode *pnode, const char *module, const char *name) {
  if (!pnode) {
    env.fail("Malformed tree");
  }

  if (!pnode_isRoot(pnode)) {
    return pnode_extractFromModule(pnode->root, module, name);
  }

  struct proot *proot = (struct proot*) pnode;
  struct pnode *modtree = nullptr;

  if (!(modtree = imports_get(proot->imports, module))) {
    env.fail("No module called %s has been imported", module);
  }

  return pnode_findExportedSym((struct proot*) modtree, name);
}

struct type* pnode_getModuleAlias(struct pnode *pnode, const char *module, const char *name) {
  if (!pnode) {
    env.fail("Malformed tree");
  }

  if (!pnode_isRoot(pnode)) {
    return pnode_getModuleAlias(pnode->root, module, name);
  }

  struct proot *proot = (struct proot*) pnode;
  struct pnode *modtree = nullptr;

  if (!(modtree = imports_get(proot->imports, module))) {
    env.fail("No module called %s has been imported", module);
  }

  return aliases_get(((struct pscope*) modtree)->aliases, name);
}

struct type* pnode_getType(struct pnode *pnode, const char *name) {

  struct type *ret;

  if ((ret = type_getBuiltin(name))) {
      return ret;
  }

  if (!pnode) {
    return nullptr;
  }

  if (!(pnode_isScope(pnode))) {
    return pnode_getType(pnode->root, name);
  }

  struct pscope *pscope = (struct pscope*) pnode;
  if (!(ret = aliases_get(pscope->aliases, name))) {
    return pnode_getType(pnode->root, name);
  }

  if (ret->kind == TYPE_STRUCT || ret->kind == TYPE_FUNC) {
    ret->name = str_clone(name); //this is an aliased type, so let's give them the aliased name
  }

  return ret;
}

uintmax_t pnode_getval(struct pnode *pnode) {
  if (!pnode_isValue(pnode)) {
    return 0;
  }

  return ((struct pexpr*) pnode)->value;
}

struct type* pnode_fixAlias(Pool *pool, struct pnode *root, struct type *type) {
  if (type_isPtr(type)) {
    struct ptype *ptype = (struct ptype*) type;

    if (type_isPtr(ptype->val)) {
      return type_makePtr(pool, pnode_fixAlias(pool, root, ptype->val));
    }

    if (type_isAlias(ptype->val)) {
      return type_makePtr(pool, aliases_get(ptype->val->discover, ptype->val->name));
    }
  }

  return type;
}

void pnode_verifyNodesAreCompatible(Pool *pool, struct pnode *root, struct pnode *assign, struct pnode *assigned) {

  struct type *first = pnode_fixAlias(pool, root, pnode_evalType(pool, assign, root));

  struct type *second = pnode_fixAlias(pool, root, pnode_evalType(pool, assigned, root));

  switch(type_areCompatible(first, second)) {
  case TYPECOMP_NO: {
    char buf[4096];
    char cuf[4096];

    env.fail("Cannot assign an expression of type %s to a location of type %s", type_str(second, buf, 4096), type_str(first, cuf, 4096));
    break;
  }

  case TYPECOMP_SMALLER:
    env.warning("Coercing an expression of type %s to smaller type %s", second->name, first->name);
    break;
  default:
    break;
  }

}

struct type* pnode_evalType(Pool *pool, struct pnode *pnode, struct pnode *scope) {

  if (pnode->id == PR_SIZE) {
    return type_getBuiltin("uintptr");
  }

  if (!scope) {
    scope = pnode;
  }

  if (pnode == expr_empty) {
    return type_none;
  }

  if (!(pnode && pnode_isValue(pnode))) {
    return nullptr;
  }

  struct pexpr *pexpr = (struct pexpr*) pnode;
  struct type *ret = nullptr;

  if (pexpr->type) {
    return pexpr->type;
  }

  switch (pnode->id){
  case PR_ACCESS: {
    ret = ((struct ptype*) pnode_evalType(pool, *leaves_get(pnode->leaves, 0), scope))->val;
    break;
  }

  case PR_CALL: {
    struct type *toCall = pnode_evalType(pool, *leaves_get(pnode->leaves, 0), scope);
    struct ftype *ftype = (struct ftype*) toCall;

    if (type_isPtr(toCall)) {
      ftype = (struct ftype*) ((struct ptype*) toCall)->val;
    }

    ret = ftype->ret;
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
      if (token_isBooleanOp(token_type((pexpr->value)))) {
        ret = type_getBuiltin("bool");
      } else {
        if (type_isPtr(firstType)) {
          ret = firstType;
          break;
        }

        struct type *secondType = pnode_evalType(pool, second, scope);

        if (type_isPtr(secondType)) {
          ret = secondType;
          break;
        }

        ret = type_evalLarger(firstType, secondType);
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
    ret = nullptr;
  }

  if (ret->kind == TYPE_ALIAS) {
    ret = pnode_getType(scope, ret->name);
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
  struct symbol *bt;

  if ((bt = symbol_getBuiltin(id))) {
    return bt->type;
  }

  if (!pnode) {
    return nullptr;
  }

  struct type *type = nullptr;

  if((type = pnode_isRootAndIdIsFunc(pnode, id))) {
    return type;
  }

  struct symbol *sym = nullptr;

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

struct symbol* pnode_matchSymbolForDeclaration(struct pnode *pnode, const char *name) {
  struct symbol *sym;

  if ((sym = symbol_getBuiltin(name))) {
    return sym;
  }

  if (!pnode) {
    return nullptr;
  }

  if (pnode_isScope(pnode)) {
    struct pscope *pscope = (struct pscope*) pnode;
    if ((sym = symbols_get(pscope->symbols, name))) {
      return sym;
    }
  }

  return pnode_matchSymbolForDeclaration(pnode->root, name);

}

struct type* pnode_funcReturnType(struct pnode *pnode) {

  if (!pnode) {
    return nullptr;
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
  case PR_ROOT:
    return true;

  default:
    return false;
  }
}

bool nonterminals_isValue(enum nonterminals id) {
  switch (id) {
  case PR_ACCESS:
  case PR_BINOP:
  case PR_CALL:
  case PR_CAST:
  case PR_DECLARATION:
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

bool nonterminals_isRoot(enum nonterminals id) {
  return id == PR_ROOT;
}

bool pnode_isRoot(struct pnode *pnode) {
  return nonterminals_isRoot(pnode->id);
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
    if (nonterminals_isRoot(id)) {
      ret = reinterpret_cast<pnode*>(new proot());
    } else {
      ret = reinterpret_cast<pnode*>(new pscope());
    }

    struct pscope *pscope = (struct pscope*) ret;
    pscope->symbols = symbols_new();
    pscope->aliases = aliases_new();
  } else {
    if (nonterminals_isFunc(id)) {
      ret = reinterpret_cast<pnode*>(new pfunc());
    } else {
      if (value) {
        //so the type should be nullptr
        ret = reinterpret_cast<pnode*>(new pexpr());
      } else {
        ret = new pnode();
      }
    }
  }

  ret->id = id;
  ret->root = nullptr;
  ret->leaves = array_new(2);

  return ret;
}

struct pnode* pnode_newfunc(Pool *pool, enum nonterminals id, const char *name, struct type *ret, Symbols *params) {
  struct pfunc *retVal = reinterpret_cast<struct pfunc*>(pnode_new(id));

  if (pnode_isFunc(&(retVal->node))) {
    retVal->ftype = reinterpret_cast<struct ftype*>(type_makeFuncType(pool, ret, list_shallowCopy(params)));
    retVal->name = str_clone(name);
    retVal->params = params;
  }

  return (struct pnode*) retVal;
}

struct pnode* pnode_newroot(const char *filename, const char *module, Imports *imports) {
  struct proot *ret = (struct proot*) pnode_new(PR_ROOT);

  ret->filename = str_clone(filename);
  ret->imports = imports;
  ret->module = str_clone(module);

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

  if (!pnode || pnode == expr_empty) {
    return;
  }

  if (pnode_isScope(pnode)) {
    if (pnode_isRoot(pnode)) {
      struct proot *proot = (struct proot*) pnode;
      free(proot->filename);
      free(proot->module);
    }

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

  delete pnode;
}

void pnode_dump(Pool *pool, struct pnode *val, uint64_t depth) {
  if (!val) {
    return;
  }

  if (val == expr_empty) {
    return;
  }

  for(uint64_t i = 0; i < depth; ++i) {
    fputs("| ", stdout);
  }

  if (val->id == PR_BINOP && ((enum token_type) pnode_getval(val)) == LEX_COLON) {
    struct type *type = pnode_evalType(pool, val, nullptr);
    struct pnode *module = *leaves_get(val->leaves, 0);
    struct pnode *id = *leaves_get(val->leaves, 1);
    char buf[4096];

    printf("Identifier: %s:%s, type %s\n", (char*) pnode_getval(module), (char*) pnode_getval(id), type_str(type, buf, 4096));

    return;
  }

  fputs(nt_str(val->id), stdout);
  if (val->startLine && val->endLine) {
    if (val->startLine == val->endLine) {
      printf(" (line %zu)", val->startLine);
    } else {
      printf(" (line %zu - %zu)", val->startLine, val->endLine);
    }
  }

  switch(val->id) {
  case PR_CAST: {
    char buf[4096];
    printf(" to type %s", type_str((struct type*) pnode_getval(val), buf, 4096));
    break;
  }
  case PR_DECLARATION: {
    char *id = (char*) pnode_getval(val), buf[4096];
    struct symbol *sym = pnode_matchSymbolForDeclaration(val, id);
    printf(": %s %s %s", (sym->decl) ? "decl" : "var", id, type_str(sym->type, buf, 4096));
    if (sym->optData) {
      putchar('\n');
      pnode_dump(pool, (struct pnode*) sym->optData, depth + 1);
      for(uint64_t i = 0; i < depth + 1; ++i) {
        fputs("| ", stdout);
      }
    }
    break;
  }
  case PR_NUMBER:
    printf(": %" PRIdMAX, (intmax_t) pnode_getval(val));
    break;
  case PR_ID:
  case PR_STRUCTID: {
    char buf[2048];
    printf(": %s, type %s", (const char*) pnode_getval(val), type_str(pnode_evalType(pool, val, nullptr), buf, 2048));
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
  case PR_UNOP: {
    printf(": %s", tokentype_str((enum token_type) pnode_getval(val)));
    break;
  }
  default:
    break;
  }

  if (pnode_isFunc(val)) {
    struct pfunc *pfunc = (struct pfunc*) val;
    char buf[2048];
    printf(" %s of type %s\n", pfunc->name, type_str((struct type*) pfunc->ftype, buf, 2048));

    symbols_dump(pfunc->params, pool, "Parameters:", (void (*)(Pool *, void *, uint64_t)) pnode_dump, depth + 1);
  } else {
    putchar('\n');
  }

  if (pnode_isScope(val)) {
    if (pnode_isRoot(val)) {
      struct proot *proot = (struct proot*) val;

      printf("(module %s)\n", strlen(proot->module) ? proot->module : "<main>");
    }
    struct pscope *pscope = (struct pscope*) val;

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
