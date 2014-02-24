#include "lex.h"
#include "utils.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(void) {
  struct lexer *lex = lexer_open("base.helm");

  if (lex->errcode) {
    puts(lex->error);
  }

  do {
    
    struct token tok = gettok(lex);
    
    fputs(represent(tok.type), stdout);

    switch (tok.type) {
    case ID:
      printf(": %s\n", (char*) tok.value);
      break;

    case NUMBER:
      printf(": %ld\n", tok.value);
      break;

    default:
      putchar('\n');
      break;
    }

    freetok(tok);
    
  } while (!lex->errcode);

  if (lex->errcode && lex->errcode != FILEEND) {
    perror(lex->error);
  }

  lexer_close(lex);

  return EXIT_SUCCESS;
}
