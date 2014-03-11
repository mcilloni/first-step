#include "lex.h"
#include "../utils/utils.h"
#include "../utils/env.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(void) {
  struct lexer *lex = lexer_open("../base.helm");

  if (lex->errcode) {
    env.fail("Cannot init lexer, errcode=%d", lex->errcode);
  }

  struct token *tok;

  while((tok = token_get(lex))) {
    
    fputs(token_str(tok), stdout);

    switch (tok->type) {
    case LEX_ID:
      printf(": %s\n", (char*) tok->value);
      break;

    case LEX_NUMBER:
      printf(": %ld\n", tok->value);
      break;

    default:
      putchar('\n');
      break;
    }

    token_free(tok);
    
  } 

  int ret = EXIT_SUCCESS;

  if (lexer_error(lex)) {
    env.error("Errors during lexing of file");
    ret = EXIT_FAILURE;
  }

  lexer_close(lex);

  return ret;
}
