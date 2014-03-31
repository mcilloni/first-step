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

#include "../list/list.h"
#include "../lex/lex.h"
#include "../utils/env.h"

#include <errno.h>
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>

extern struct pnode* expr(struct pnode*, struct lexer*);
extern struct token* token_getOrDie(struct lexer*);
extern struct token *nextTok;

void printlist(List *list) {
  bool inside = false;
  fputs("[ ", stdout);
  size_t len = list_len(list);
  struct token *tok;

  for (uint8_t i = 0; i < len; ++i) {
    tok = *list_get(list, i);

    if (inside) {
      fputs(", ",stdout);
    } else {
      inside = true;
    }

    fputs(token_str(tok), stdout);

    switch (tok->type) {
    case LEX_ID:
      printf(": %s", (char*) tok->value);
      break;

    case LEX_NUMBER:
      printf(": %" PRIuMAX, tok->value);
      break;

    default:
      break;
    }
  }
  puts(" ]");
}                      

void printtree(struct pnode *root) {
  if (!root) {
    return;
  }

  struct token tok;
  uintptr_t val = pnode_getval(root);

  tok.type = (enum token_type) val;

  switch (root->id) {
  case PR_BINOP:
    if (array_len(root->leaves) != 2) {
      env.fail("Unacceptable len: %zu", array_len(root->leaves));
    }
    
    printtree(*leaves_get(root->leaves, 0));
    printf(" %s ", token_str(&tok));
    printtree(*leaves_get(root->leaves, 1));
    break;

  case PR_UNOP:
    if (array_len(root->leaves) != 1) {
      env.fail("Unacceptable len: %zu", array_len(root->leaves));
    }
    
    printf("%s", token_str(&tok));
    printtree(*leaves_get(root->leaves, 0));
    break;
  case PR_ID:
    fputs((char*) val, stdout);
    break;
  default:
    printf("%" PRIdPTR, (intptr_t) val);
    break;

  }
  
}

int main(int argc, const char *argv[]) {
  char *toLex = "a + 3 - 7\na";

  fputs("Expr: ", stdout);

  fwrite(toLex, sizeof(*toLex), 9, stdout);

  putchar('\n');

  struct lexer *lex = lexer_fromFile(fmemopen(toLex, strlen(toLex), "r"));                                                                 

  if (lex->errcode) {
    env.fail("Cannot init lexer, errcode=%d", lex->errcode);
  }

  if (lexer_error(lex)) {
    env.fail("Errors during lexing of file");
  }

  struct pnode *root = pnode_new(PR_PROGRAM);
  pnode_addSymbol(root, "a", type_getBuiltin("int8"), NULL);
  struct pnode *res = expr(root, lex);
  pnode_addLeaf(root, res);

  printtree(res);

  putchar('\n');

  token_free(nextTok);
  lexer_close(lex);
  pnode_free(root);

  return EXIT_SUCCESS;
}
