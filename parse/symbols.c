#include "symbols.h"
#include "../utils/utils.h"

Symbols* (*symbols_new) (void) = strmap_new;
bool (*symbols_defined)(Symbols *symt, const char *id) = (bool (*)(Symbols*,const char*)) map_contains;
void (*symbols_free)(Symbols *symt) = (void (*) (Symbols*)) map_free;
bool (*symbols_getType)(Symbols *symt, const char *id, const char **type) = (bool (*)(Symbols*,const char*,const char**)) map_get;

enum symbols_resp symbols_register(Symbols *symt, const char *id, const char *type) {
  if (symbols_defined(symt, (void*) id)) {
    return SYM_EXISTS;
  }

  map_put(symt, (void*) str_clone(id), (void*) type, FREE_KEY);
  return SYM_ADDED;
}

