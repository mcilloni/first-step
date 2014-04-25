#include "importer.h"
#include "parse.h"

#include <list/pool.h>
#include <syms/types.h>
#include <utils/utils.h>

#include <stdlib.h>

struct parser* parser_new(const char *fileName, struct importer *importer) {
  Pool *pool = NULL;

  if (!importer) {
    pool = pool_new();
    importer = importer_new(pool);
  } else {
    pool = importer_getPool(importer);
  }

  struct parser *ret = calloc(1, sizeof(struct parser));
  *ret = (struct parser) { 
    .filename = str_clone(fileName), 
    .types = pool,
    .importer = importer,
    .freeTypes = !pool,
    .lastLineno = 1
  };

  return ret;
}

void parser_close(struct parser *parser) {
  if (parser->freeTypes) {
    pool_release(parser->types, (void (*)(void*)) type_free);
  }

  free(parser->filename);
  free(parser);
}
