#include "parse.h"

#include "../array/array.h"
#include "../lex/lex.h"
#include "../utils/env.h"
#include "../utils/utils.h"

#include <stdbool.h>

extern struct token *next;

enum operator_assoc {
  ASSOC_LEFT,
  ASSOC_RIGHT
};

bool expr_nextIsEnd(void) {
  if (!next) {
    env.fail("Unexpected end of file in expression body");
  }

  switch (next->type) {
  case LEX_NEWLINE:
    return true;

  default:
    return false;
  }
}

struct pnode* expr_evalBinary(struct token *tok, struct pnode *left, struct pnode *right) {
  return NULL;
}

struct pnode* expr_evalUnary(struct token *tok, struct pnode *operand) {
  return NULL;
}

void expr_fixMinusConst(Array *expr) {

}

size_t expr_findLowPriorityOp(Array *expr) {
  return 0;
}

enum operator_assoc expr_getOpAssociation(struct token *tok) {
  return ASSOC_LEFT;
}

bool expr_isBinaryOp(struct token *tok) {
  return false;
}

bool expr_isOpCompatible(struct token *tok, struct pnode *operand) {
  return false;
}

bool expr_isUnaryOp(struct token *tok) {
  return false;
}

enum nonterminals expr_ntFromTokVal(enum token_type type) {
  return PR_PROGRAM;
}

struct pnode* expr_treeize(Array *expr) {

  expr_fixMinusConst(expr);

  struct pnode *ret = NULL;

  if (array_len(expr) == 1) {
    struct token *tok = *array_get(expr, 0);

    switch (tok->type) {
    case LEX_ID:
      ret = pnode_newval(PR_ID, (uintptr_t) str_clone((const char*) tok->value));
      break;
    case LEX_NUMBER:
      ret = pnode_newval(PR_NUMBER, tok->value);
      break;
    default:
      env.fail("Unexpected token found, got %s, expected a costant or identifier", token_str(tok));
    }
  } else {

    size_t pos = expr_findLowPriorityOp(expr);  

    //handle binary operator. Split expr in two and get subexpressions.
  
    struct token *tok = *array_get(expr, pos);
    if (expr_isBinaryOp(tok)) {

      struct pnode *left = expr_treeize(array_slice(expr, 0, pos));
      struct pnode *right = expr_treeize(array_slice(expr, pos, -1));
  
      if (pnode_isConst(left) && pnode_isConst(right)) {

        ret = expr_evalBinary(tok, left, right);
        pnode_free(left);
        pnode_free(right);

      } else {

        ret = pnode_new(expr_ntFromTokVal(tok->type));
        pnode_addLeaf(ret, left);
        pnode_addLeaf(ret, right);

      }
    } else {
      if (expr_isUnaryOp(tok)) {

        struct pnode *operand;

        if (expr_getOpAssociation(tok) == ASSOC_LEFT) {
          if (!pos) {
            env.fail("Operator %s cannot bind to anything on its left", token_str(tok));
          }

          operand = expr_treeize(array_slice(expr, 0, pos));
        } else {
          if (pos + 1 == array_len(expr)) {
            env.fail("Operator %s cannot bind to anything on its right", token_str(tok));
          }

          operand = expr_treeize(array_slice(expr, pos, -1));
        }

        if (!expr_isOpCompatible(tok, operand)) {
          env.fail("Operator %s cannot be applied to an incompatible expression", token_str(tok));
        }

        if (pnode_isConst(operand)) {
          ret = expr_evalUnary(tok, operand);
          pnode_free(operand);
        } else {

          ret = pnode_new(expr_ntFromTokVal(tok->type));
          pnode_addLeaf(ret, operand);

        }

      } else {
        env.fail("Misplaced token: %s", token_str(tok));
      }
    }
  }

  //this actually frees only the original array
  array_freeContents(expr, (freefunc) token_free);

  array_free(expr);

  return ret;

}

struct pnode* expr(struct pnode *root, struct lexer *lex) {
  Array *arr = array_new(10);

  while(!expr_nextIsEnd()) {
    array_append(arr, token_getOrDie(lex));
  } 

  if (!array_len(arr)) {
    env.fail("Empty expression body");
  }

  return expr_treeize(arr);

}

