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

// This whole source is pretty inefficient and also pretty dumb; it should be rewritten entirely because it is not optimal and has a few bugs, but still, it works.
// I'm not concerned about this because I intend this to be just an experimental, bootstrap compiler, so I don't care about performances, or ugliness, or anything.

#include "parse.h"

#include <list/list.h>
#include <lex/lex.h>
#include <utils/env.h>
#include <utils/utils.h>

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

extern struct token *nextTok;

struct pnode enVd = {PR_EXPR, NULL, NULL};

struct pnode *expr_empty = &enVd;

struct pnode* expr_treeize(struct pnode *root, List *expr);

enum operator_assoc {
  ASSOC_NOTOP,
  ASSOC_LEFT,
  ASSOC_RIGHT
};

uintmax_t uintpow(uintmax_t base, uintmax_t exp) {

  if (!exp) {
    return 1U;
  }

  switch (base) {
  case 1U:
    return 1U;
  case 0U:
    return 0U;
  default: {
    uintmax_t ret = 1U;
    for (uintmax_t i = 1; i <= exp; ++i) {
      ret *= base;
    }
    return ret;
  }
  }

}

void expr_callCheckAliases(struct pnode *call) {
  Leaves *lv = call->leaves;

  size_t len = array_len(lv);

  if (!len) {
    env.fail("No function to call");
  }

  struct pnode *id = *leaves_get(lv, 0);
  char *name = (char*) pnode_getval(id);

  struct type *type = pnode_symbolType(call, name);

  if (!type) {
    env.fail("Cannot call undefined id %s", name);
  }

  if (type_isPtr(type)) {
    type = ((struct ptype*) type)->val;
  }

  if (!type_isFunc(type)) {
    env.fail("Cannot call not function type id %s", name);
  }

  struct ftype *ftype = (struct ftype*) type;
  size_t pLen = array_len(ftype->params);

  if ((len - 1) != pLen) {
    char buf[2048];
    type_str(type, buf, 2048);
    env.fail("Cannot call function %s of type %s: wrong number of arguments (%zu), expected %zu", name, buf, len-1, pLen);
  }

  struct type *typeA, *typeB;

  for (size_t i = 0; i < pLen; ++i) {
    
    typeA = (struct type*) *array_get(ftype->params, i);
    typeB = pnode_evalType((struct pnode*) *leaves_get(lv, i + 1), NULL);

    if (!type_areCompatible(typeA, typeB)) {
      char buf[2048], cuf[2048];
      type_str(typeB, cuf, 2048);
      type_str(typeA, buf, 2048);
      env.fail("Cannot assign %s to parameter %zu of type %s of function id %s", cuf, i, buf, name);
    }

  }
}

struct pnode* expr_evalBinary(struct token *tok, struct pnode *left, struct pnode *right) {
  struct pnode *ret = NULL; 
  intmax_t lval = pnode_getval(left);
  intmax_t rval = pnode_getval(right);

  switch (tok->type) {
  case LEX_PLUS: 
    ret = pnode_newval(PR_NUMBER, lval + rval);
    break;
  case LEX_TIMES:
    ret = pnode_newval(PR_NUMBER, lval * rval);
    break;
  case LEX_POW:
    ret = pnode_newval(PR_NUMBER, uintpow(lval, rval));
    break;
  case LEX_DIV:
    ret = pnode_newval(PR_NUMBER, lval / rval);
    break;
  case LEX_EQUAL:
    ret = pnode_newval(PR_NUMBER, lval == rval);
    break;
  case LEX_DIFFERENT:
    ret = pnode_newval(PR_NUMBER, lval != rval);
    break;
  case LEX_MAJOR:
    ret = pnode_newval(PR_NUMBER, lval > rval);
    break;
  case LEX_MINOR:
    ret = pnode_newval(PR_NUMBER, lval < rval);
    break;
  case LEX_AND:
    ret = pnode_newval(PR_NUMBER, lval && rval);
    break;
  case LEX_OR:
    ret = pnode_newval(PR_NUMBER, lval || rval);
    break;
  default:
    env.fail("A wrong token finished into constant evaluation: %s", token_str(tok));
    break;
  }

  return ret;
}

struct pnode* expr_evalUnary(struct token *tok, struct pnode *operand) {
  struct pnode *ret = NULL;

  int64_t value = pnode_getval(operand);
  bool sup = false;

  switch (tok->type) {
  case LEX_NOT:
    ret = pnode_newval(PR_NUMBER, !value);
    break;
  case LEX_INC:
  case LEX_DEC:
    env.fail("Cannot apply %s to a constant value", token_str(tok));
    break;
  case LEX_MINUS:
    ret = pnode_newval(PR_NUMBER, -value);
    break;
  case LEX_PTR:
    sup = true;
  case LEX_VAL:
    env.fail("Cannot %s a constant value", (sup) ? "reference" : "dereference");
    break;
  default:
    env.fail("A wrong token finished into constant evaluation: %s", token_str(tok));
    break;
  }

  return ret;
}

size_t expr_findReverseLPOp(List *expr) {
  size_t len = list_len(expr);
  size_t pos = 0;
  int8_t posPri = INT8_MAX, tmp;  

  int16_t par = 0;

  struct token *tok;

  for (int64_t i = 0; i < len; ++i) {
    tok = (struct token*) *list_get(expr, i);

    switch (tok->type) {
    case LEX_OPAR:
      ++par;
      break;
    case LEX_CPAR:
      if (!par) {
        env.fail("Unmatched parentheses");
      }
      --par;
      break;
    default:
      if (!par) {
        tmp = token_getPriority(tok);
        if (tmp > 0) {
          if (tmp < posPri) {
            pos = i;
            posPri = tmp;
          }
        }
      }
      break;
    }
  }

  if (par) {
    env.fail("Unmatched parentheses in expression");
  }

  return pos;
}

enum operator_assoc expr_getOpAssociation(struct token *tok);

size_t expr_findLowPriorityOp(List *expr) {
  size_t beg = list_len(expr) - 1;
  size_t pos = 0;
  int8_t posPri = INT8_MAX, tmp;  

  int16_t par = 0;

  struct token *tok, *posTok = NULL;

  for (int64_t i = beg; i > -1; --i) {
    tok = (struct token*) *list_get(expr, i);

    switch (tok->type) {
    case LEX_CPAR:
      ++par;
      break;
    case LEX_OPAR:
      if (!par) {
        env.fail("Unmatched parentheses");
      }
      --par;
      break;
    default:
      if (!par) {
        tmp = token_getPriority(tok);
        if (tmp > 0) {
          if (tmp < posPri) {
            pos = i;
            posPri = tmp;
            posTok = tok;
          }
        }
      }
      break;
    }
  }

  if (par) {
    env.fail("Unmatched parentheses in expression");
  }

  //Inefficient, but simple and I just don't care about it right now
  if (posTok && token_getOpType(posTok) == OPTYPE_BINARY && expr_getOpAssociation(posTok) == ASSOC_RIGHT) {
    pos = expr_findReverseLPOp(expr);
  }

  return pos;
}

void expr_fixMinus(List *expr) {
  size_t len = list_len(expr);
  len = (!len) ? len : (len  - 1);

  struct token *current, *aux;

  int64_t par = 0;

  //skip the last one
  for (size_t i = 0;  i < len; ++i) {
    current = *list_get(expr, i);

    switch(current->type) {
    case LEX_MINUS: {
      if (!i || par) {
        break;
      }

      aux = *list_get(expr, i - 1);
      enum optype ot = token_getOpType(aux);

      if (!ot) {
        aux = calloc(1, sizeof(struct token));
        aux->type = LEX_PLUS;
        list_add(expr, i, aux);
        ++len;
        ++i;
      }
      break;
    }
    case LEX_CPAR:
      --par;
      break;
    case LEX_OPAR:
      ++par;
      break;
    default:
      break;
    }
  } 
}

void expr_fixParentheses(List **expr) {
  size_t len = list_len(*expr);

  struct token *first = (struct token*) *list_get(*expr, 0LU);
  struct token *last = (struct token*) *list_get(*expr, len - 1);

  if ((first->type == LEX_OPAR) && (last->type == LEX_CPAR)) {
    *expr = list_extract(*expr, 1LU, len - 2);
    token_free(first);
    token_free(last);
  }

}

enum operator_assoc expr_getOpAssociation(struct token *tok) {
  switch(token_getOpType(tok)) {
  case OPTYPE_BINARY: {
    switch(tok->type) {
    case LEX_ASSIGN:
      return ASSOC_RIGHT;
    default:
      return ASSOC_LEFT;
    }
  }
  case OPTYPE_UNARY: {
    switch(tok->type) {
    case LEX_MINUS:
    case LEX_NOT:
    case LEX_PTR:
    case LEX_VAL:
      return ASSOC_RIGHT;
    default:
      return ASSOC_LEFT;
    }
  }
  default:
    return ASSOC_NOTOP;
  }
}

struct pnode* expr_handleSingle(struct pnode *root, List *expr) {
    struct token *tok = *list_get(expr, 0);
    struct pnode *ret = NULL;

    switch (tok->type) {
    case LEX_ID: {

      const char *id = str_clone((const char*) tok->value);
      
      if (!pnode_symbolType(root, id)) {
        env.fail("Symbol %s not defined", id);
      }

      ret = pnode_newval(PR_ID, (uintptr_t) id);
      break;
    }
    case LEX_NUMBER:
      ret = pnode_newval(PR_NUMBER, tok->value);
      break;
    case LEX_STRING:
      ret = pnode_newval(PR_STRING, (uintptr_t) str_clone((const char*) tok->value));
      break;
    default:
      env.fail("Unexpected token found, got %s, expected a constant or identifier", token_str(tok));
      break;
    }

    return ret;
}

bool expr_isBinOpCompatible(struct pnode *root, struct token *tok, struct pnode *left, struct pnode *right) {
  
  switch (tok->type) {
  case LEX_AND:
  case LEX_DIFFERENT:
  case LEX_DIV:
  case LEX_EQUAL:
  case LEX_MAJOR:
  case LEX_MINOR:
  case LEX_OR:
  case LEX_PLUS:
  case LEX_POW:
  case LEX_TIMES: {
    struct type *tleft = pnode_evalType(left, root);
    struct type *tright = pnode_evalType(right, root);
    return ((tleft->kind == TYPE_NUMERIC) && (tright->kind == TYPE_NUMERIC)) || ((tleft->kind == TYPE_PTR) && (tright->kind == TYPE_PTR));
  }
  default:
    env.fail("Token %s mistakenly entered in a wrong path", token_str(tok));
    return false;
  }

}

bool expr_isUnOpCompatible(struct pnode *root, struct token *tok, struct pnode *operand) {
  
  switch (tok->type) {
  case LEX_DEC:
  case LEX_INC:
  case LEX_MINUS:
  case LEX_NOT:
    return pnode_evalType(operand, root)->kind != TYPE_FUNC;
  case LEX_PTR:
    return true;
  case LEX_VAL:
    return pnode_evalType(operand, root)->kind == TYPE_PTR;
  default:
    env.fail("Token %s mistakenly entered in a wrong path", token_str(tok));
    return false;
  }
  
}

bool expr_nextIsEnd(struct token *tok) {

  if (!tok) {
    env.fail("Unexpected end of file in expression body");
  }

  env.debug("nextTok@%p: %s", nextTok, token_str(nextTok));

  switch (tok->type) {
  case LEX_NEWLINE:
    return true;

  default:
    return false;
  }
}

enum nonterminals expr_ntFromTokVal(struct token *tok) {

  switch (token_getOpType(tok)) {
  case OPTYPE_BINARY:
    return PR_BINOP;

  case OPTYPE_UNARY:
    return PR_UNOP;
  default:
    env.fail("I have no idea of what I am doing");
    return PR_PROGRAM;
  }
  
}

size_t expr_findComma(List *expr) {
  size_t len = list_len(expr);

  uint16_t par = 0;

  struct token *tok;

  for (size_t i = 0; i < len; ++i) {
    tok = (struct token*) *list_get(expr, i);

    switch (tok->type) {
    case LEX_CPAR:
      if (!par) {
        env.fail("Unmatched parentheses");
      }
      --par;
      break;
    case LEX_OPAR:
      ++par;
      break;
    case LEX_COMMA:  
      if (!par) {
        return i;
      }
      break;
    default:
      break;
    }
  }

  if (par) {
    env.fail("Unmatched parentheses in expression");
  }

  return 0;
}

struct pnode* expr_handleCall(struct pnode *root, List *expr) {
  
  size_t len = list_len(expr);

  if (len < 3) {
    return false;
  }

  struct token *first = (struct token*) *list_get(expr, 0);
  struct token *second = (struct token*) *list_get(expr, 1);
  struct token *last = (struct token*) *list_get(expr, len - 1);

  if (first->type == LEX_ID && second->type == LEX_OPAR && last->type == LEX_CPAR) {
    struct pnode *ret = pnode_newval(PR_CALL, 0U);

    ret->root = root;

    struct pnode *id = expr_handleSingle(ret, list_extract(expr, 0, 1));

    id->root = ret;

    pnode_addLeaf(ret, id);

    expr_fixParentheses(&expr);

    if (len) {
      List *tmp;
      size_t comma;
      
      while((comma = expr_findComma(expr))) {
        tmp = list_extract(expr, 0, comma);
        pnode_addLeaf(ret, expr_treeize(ret, tmp));
        token_free(*list_get(expr, 0));
        expr = list_extract(expr, 1, -1);
      }

      pnode_addLeaf(ret, expr_treeize(ret, expr));
    }

    expr_callCheckAliases(ret);

    return ret;
  }

  return NULL;
}

struct pnode* expr_handleStructNode(struct pnode *root, struct pnode *left, struct token *rtok) {

  if (rtok->type != LEX_ID) {
    env.fail("Cannot extract non-identifier '%s' from structure", token_str(rtok));
  }

  char *member = (char*) rtok->value;

  struct pnode *right = pnode_newval(PR_STRUCTID, (uintmax_t) str_clone(member));

  struct type *ltype = pnode_evalType(left, root), *otype = NULL;

  if (type_isPtr(ltype)) {
    otype = ltype;
    ltype = ((struct ptype*) ltype)->val;
  }

  if (!type_isStruct(ltype)) {
    char buf[4096];
    type_str(ltype, buf, 4096);
    env.fail("Cannot extract member %s from non-struct type %s", member, buf);
  }

  struct stype *stype = (struct stype*) ltype;

  struct symbol *sym = symbols_get(stype->symbols, member);

  if (!sym) {
    char buf[4096];
    type_str(ltype, buf, 4096);
    env.fail("No member called %s in struct declared as %s", member, buf);
  }

  struct pexpr *pexpr = (struct pexpr*) pnode_newval(PR_BINOP, LEX_APOS);

  pexpr->type = type_secptr(sym->type);
  ((struct pexpr*) right)->type = type_secptr(sym->type);
 
  struct pnode *ret = (struct pnode*) pexpr;

  pnode_addLeaf(ret, left);
  pnode_addLeaf(ret, right);

  type_free(otype);
  return (struct pnode*) pexpr;
}

bool expr_isValidAssign(struct pnode *node) {
  if (node->id == PR_ID) {
    return true;
  }

  if (node->id != PR_UNOP) {
    return false;
  }

  enum token_type tt = (enum token_type) pnode_getval(node);

  return tt == LEX_PTR || tt == LEX_VAL;
}

struct pnode* expr_treeize(struct pnode *root, List *expr) {

  expr_fixParentheses(&expr);
  expr_fixMinus(expr);

  if (!list_len(expr)) {
    return expr_empty;    
  }

  struct pnode *ret = NULL;

  if ((ret = expr_handleCall(root, expr))) {
    return ret;
  }

  if (list_len(expr) == 1) {
    ret = expr_handleSingle(root, expr);
  } else {

    size_t pos = expr_findLowPriorityOp(expr);  

    //handle binary operator. Split expr in two and get subexpressions.
  
    struct token *tok = *list_get(expr, pos);
    switch (token_getOpType(tok)) {
    case OPTYPE_BINARY: {

      if (!pos) {
        env.fail("Binary operator %s found at expression start", token_str(tok));
      }

      struct pnode *left = expr_treeize(root, list_extract(expr, 0, pos));

      size_t posAfter = expr_findLowPriorityOp(expr);

      if (tok->type == LEX_APOS) {
        List *tmp = list_extract(expr, posAfter + 1, 1);
        ret = expr_handleStructNode(root, left, (struct token*) *list_get(tmp, 0));
        list_freeAll(tmp, (void (*)(void*)) token_free);
        break;
      }

      struct pnode *right = expr_treeize(root, list_extract(expr, posAfter + 1, -1));

      bool assign = tok->type == LEX_ASSIGN;

      if (pnode_isConst(left) && pnode_isConst(right) && !assign) {

        ret = expr_evalBinary(tok, left, right);
        pnode_free(left);
        pnode_free(right);
        break;

      }

      ret = pnode_newval(expr_ntFromTokVal(tok), (uintmax_t) tok->type);

      pnode_addLeaf(ret, left);
      pnode_addLeaf(ret, right);

      if (assign) {
        pnode_verifyNodesAreCompatible(root, left, right);
      } else {
        if (!expr_isBinOpCompatible(root, tok, left, right)) {
          struct type *ltype = pnode_evalType(left, root);
          struct type *rtype = pnode_evalType(right, root);
          env.fail("Cannot apply operator %s to types %s and %s", token_str(tok), ltype->name, rtype->name);
        }
      }
      break;
    } 

    case OPTYPE_UNARY: {

      struct pnode *operand;

      if (expr_getOpAssociation(tok) == ASSOC_LEFT) {
        if (!pos) {
          env.fail("Operator %s cannot bind to anything on its left", token_str(tok));
        }

        operand = expr_treeize(root, list_extract(expr, 0, pos));
      } else {
        if (pos + 1 == list_len(expr)) {
          env.fail("Operator %s cannot bind to anything on its right", token_str(tok));
        }

        operand = expr_treeize(root, list_extract(expr, pos + 1, -1));
      }


      if (pnode_isConst(operand)) {
        ret = expr_evalUnary(tok, operand);
        pnode_free(operand);
        break;
      }

      ret = pnode_newval(expr_ntFromTokVal(tok), (uintmax_t) tok->type);
      pnode_addLeaf(ret, operand);

      if (!expr_isUnOpCompatible(root, tok, operand)) {
        env.fail("Operator %s cannot be applied to an incompatible expression", token_str(tok));
      }

      break;
    } 

    default:{
      char buf[4096];
      if (tok->type == LEX_ID) {
        snprintf(buf, 4095, "an identifier ('%s')", (char*) tok->value);
      } else {
        strncpy(buf, token_str(tok), 4095);
      }
      env.fail("Misplaced token: %s", buf);
      break;
    }
    }
  }

  //this actually frees only the original list
  list_freeAll(expr, (void (*)(void*)) token_free);

  return ret;

}

struct pnode* expr(struct pnode *root, struct lexer *lex, bodyender be) {

  if (!be) {
    be = (bodyender) expr_nextIsEnd;
  }

  List *list = list_new();

  struct token *tok;
  do {
    tok = token_getOrDie(lex);
    list_append(list, tok);
  } while(!be(nextTok));

  if (!list_len(list)) {
    env.fail("Empty expression body");
  }

  struct pnode *ret = expr_treeize(root, list);

  return ret;
}

