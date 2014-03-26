#include "symbols.h"
#include "../utils/utils.h"

#include <stdio.h>
#include <stdlib.h>

Symbols* (*symbols_new) (void) = strmap_new;
bool (*symbols_defined)(Symbols *symt, const char *id) = (bool (*)(Symbols*,const char*)) map_contains;

void printdepth(int8_t depth) {
  for (int8_t i = 0; i < depth; ++i) {
    fputs("| ", stdout);
  }
}

void symbols_dump(Symbols *symt, int8_t depth) {
  printdepth(depth);
  puts("Declarations:");

  ++depth;

  if (!symt) {
    printdepth(depth);
    puts("(nil)");
    return;
  }

  MapIter *iter = mapiter_start(symt);
  Pair *pair;
  const char *name;
  struct symbol *sym;
  char buf[2048];

  while((pair = mapiter_next(iter))) {
    printdepth(depth);
    name = (const char*) pair->key;
    sym = (struct symbol*) pair->value;

    printf("%s: %s %s\n", name, (sym->decl ? "decl" : "var"), type_str(sym->type, buf, 2048));

    pair_free(pair);
  }

}

void symbol_free(struct symbol *sym) {
  type_free(sym->type);
  free(sym);
}

void symbols_free(Symbols *symt) {
  map_freeSpec(symt, NULL, (void (*)(void*)) symbol_free);  
}

struct symbol* symbols_get(Symbols *symt, const char *id) {
  struct symbol *ret;

  if (map_get(symt, id, (void**) &ret)) {
    return ret;
  }

  return NULL;
}

enum symbols_resp symbols_register(Symbols *symt, const char *id, struct type *type, bool decl) {
  if (symbols_defined(symt, (void*) id)) {
    return SYM_EXISTS;
  }

  struct symbol *sym = malloc(sizeof(struct symbol));

  *sym = (struct symbol) {decl, type};


  map_put(symt, (void*) str_clone(id), (void*) sym, FREE_KEY | FREE_VALUE);
  return SYM_ADDED;
}

