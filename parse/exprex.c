#include "ptree.h"

#include "../list/list.h"
#include "../lex/lex.h"
#include "../utils/env.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern struct pnode* expr(struct pnode*, struct lexer*);
extern struct token* token_getOrDie(struct lexer*);

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
      printf(": %ld", tok->value);
      break;

    default:
      break;
    }
  }
  puts(" ]");
}                      

int main(void) {
  char *toLex = "-5-6-7-8\na";

  env_setDebug(true);

  printf("Expr: %s\n", toLex);

  struct lexer *lex = lexer_fromFile(fmemopen(toLex, strlen(toLex), "r"));

  if (lex->errcode) {
    env.fail("Cannot init lexer, errcode=%d", lex->errcode);
  }

  int ret = EXIT_SUCCESS;

  if (lexer_error(lex)) {
    env.error("Errors during lexing of file");
    ret = EXIT_FAILURE;
  }

  struct pnode *res = expr(NULL, lex);

  lexer_close(lex);
  pnode_free(res);

  return ret;
}
