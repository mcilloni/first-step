#include "parse.h"

#include "../list/list.h"

#include <stdlib.h>

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

  struct pnode *ret = parse("../base.helm");

  ptree_dump(ret);

  return EXIT_SUCCESS;
}
