/*
 *  This file is part of First Step.
 *  
 *  First Step is free software: you can redistribute it and/or modify it under the terms of the GNU Lesser General Public License as published by the Free Software 
 *  Foundation, either version 3 of the License, or (at your option) any later version. 
 *
 *  First Step is distributed in the hope that it will be useful, but 
 *  WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with First Step.  If not, see <http://www.gnu.org/licenses/>
 *
 *  Copyright (C) Marco Cilloni <marco.cilloni@yahoo.com> 2014
 *
 */

#include "symbols.h"
#include "types.h"
#include "../utils/utils.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

Symbols* (*symbols_new) (void) = strmap_new;
bool (*symbols_defined)(Symbols *symt, const char *id) = (bool (*)(Symbols*,const char*)) map_contains;

void printdepth(int8_t depth) {
  for (int8_t i = 0; i < depth; ++i) {
    fputs("| ", stdout);
  }
}

void symbols_dump(Symbols *symt, const char *title, int8_t depth) {
  if (!symt->size) {
    return;
  }

  printdepth(depth);
  puts(title);

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

  mapiter_free(iter);
}

void symbol_free(struct symbol *sym) {
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

bool id_isReservedBool(const char *id) {
  return !strcmp(id, "true") || !strcmp(id, "false");
}

