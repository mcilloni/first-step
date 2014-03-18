#include "symbols.h"
#include "../utils/utils.h"

#include <stddef.h>

Symbols* (*symbols_new) (void) = strmap_new;
bool (*symbols_defined)(Symbols *symt, const char *id) = (bool (*)(Symbols*,const char*)) map_contains;
void (*symbols_free)(Symbols *symt) = (void (*) (Symbols*)) map_free;

const char* symbols_getType(Symbols *symt, const char *id) {
  char *ret;

  if (map_get(symt, id, (void**) &ret)) {
    return ret;
  }

  return NULL;
}

enum symbols_resp symbols_register(Symbols *symt, const char *id, const char *type) {
  if (symbols_defined(symt, (void*) id)) {
    return SYM_EXISTS;
  }

  map_put(symt, (void*) str_clone(id), (void*) str_clone(type), FREE_KEY | FREE_VALUE);
  return SYM_ADDED;
}

