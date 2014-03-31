#include "parse.h"

#include "../list/list.h"

#include <sys/time.h>
#include <inttypes.h>
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
      printf(": %" PRIuMAX, tok->value);
      break;

    default:
      break;
    }
  }
  puts(" ]");
}                      

int main(void) {

  struct timeval t;
  struct timezone tzp;
  gettimeofday(&t, &tzp);

  uintmax_t t1 = t.tv_sec*1000000LU + t.tv_usec;

  struct pnode *ret = parse("../base.helm");

  gettimeofday(&t, &tzp);

  ptree_dump(ret);

  printf("\nParsing took %" PRIuMAX " µs\n", (t.tv_sec*1000000LU + t.tv_usec) - t1);

  return EXIT_SUCCESS;
}
