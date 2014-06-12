#include "importer.h"
#include "parse.h"

#include <list/pool.h>
#include <syms/types.h>
#include <utils/utils.h>

#include <cstdlib>

struct parser* parser_new(const char *fileName, struct importer *importer) {
  Pool *pool = nullptr;

  if (!importer) {
    pool = pool_new();
    importer = importer_new(pool);
  } else {
    pool = importer_getPool(importer);
  }

  struct parser *ret = new parser();
  ret->filename = str_clone(fileName), 
  ret->types = pool,
  ret->importer = importer,
  ret->freeTypes = !pool,
  ret->lastLineno = 1;

  return ret;
}

void parser_close(struct parser *parser) {
  if (parser->freeTypes) {
    pool_release(parser->types, (void (*)(void*)) type_free);
  }

  free(parser->filename);
  delete parser;
}
