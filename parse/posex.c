#include "../lex/lex.h"
#include "../list/list.h"
#include "../utils/utils.h"
#include "../utils/env.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern size_t expr_findLowPriorityOp(List *expr);
extern void expr_fixMinus(List *expr);

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
  char *toLex = "-5-6-7-8";

  printf("Expr: %s\n", toLex);

  struct lexer *lex = lexer_fromFile(fmemopen(toLex, strlen(toLex), "r"));

  if (lex->errcode) {
    env.fail("Cannot init lexer, errcode=%d", lex->errcode);
  }

  List *list = list_new();

  struct token *tok;

  while((tok = token_get(lex))) {
    list_append(list, tok);        
  } 

  int ret = EXIT_SUCCESS;

  if (lexer_error(lex)) {
    env.error("Errors during lexing of file");
    ret = EXIT_FAILURE;
  }

  printlist(list);

  size_t pos = expr_findLowPriorityOp(list);

  printf("Lowest rightest low priority op: %lu (%s)\n", pos, token_str(*list_get(list, pos)));

  puts(token_str((struct token*) *list_get(list, pos)));

  expr_fixMinus(list);

  printlist(list);

  lexer_close(lex);

  list_freeAll(list, (void (*)(void*)) token_free);

  return ret;
}
