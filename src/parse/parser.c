#include "parse.h"

#include <list/pool.h>
#include <syms/types.h>

#include <stdlib.h>

struct parser* parser_new(void) {
  struct parser *ret = calloc(1, sizeof(struct parser));
  ret->types = pool_new();

  return ret;
}

void parser_close(struct parser *parser) {
  pool_release(parser->types, (void (*)(void*)) type_free);

  free(parser);
}
