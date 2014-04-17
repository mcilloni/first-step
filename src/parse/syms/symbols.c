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
#include <utils/utils.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

Symbols* (*symbols_new) (void) = list_new;
size_t (*symbols_len)(Symbols*) = list_len;

bool symbols_defined(Symbols *symt, const char *id) {
  return (bool) symbols_get(symt, id);
}

struct symbol* symbols_get(Symbols *symt, const char *id) {
  struct spair *sp;
  struct symbol *sym;

  if ((sym = symbol_getBuiltin(id))) {
    return sym;
  } 

  size_t len = list_len(symt);

  for (size_t i = 0; i < len; ++i) {
    sp = (struct spair*) *list_get(symt, i);

    if (!strcmp(sp->id, id)) {
      return sp->sym;
    }
  }

  return NULL;
}

void printdepth(int8_t depth) {
  for (int8_t i = 0; i < depth; ++i) {
    fputs("| ", stdout);
  }
}

void symbols_dump(Symbols *symt, Pool *pool, const char *title, void (*dumpExtra)(Pool*, void*, uint64_t), int8_t depth) {
  size_t len = list_len(symt);
  if (!len) {
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

  struct spair *sp;
  char buf[2048];

  for (size_t i = 0; i < len; ++i) {
    printdepth(depth);
    sp = (struct spair*) *list_get(symt, i);

    printf("%s: %s %s\n", sp->id, (sp->sym->decl ? "decl" : "var"), type_str(sp->sym->type, buf, 2048));

    if (sp->sym->optData) {
      dumpExtra(pool, sp->sym->optData, depth + 1);
    }
  }
}

void symbol_free(struct symbol *sym) {
  if (sym) {
    if (sym->freeOpt) {
      sym->freeOpt(sym->optData);
    }

    free(sym);
  }
}

void spair_free(struct spair *sp) {
  free((void*) sp->id);
  symbol_free(sp->sym);
  free(sp);
}

void symbols_free(Symbols *symt) {
  list_freeAll(symt, (void (*)(void*)) spair_free);  
}

enum symbols_resp symbols_register(Symbols *symt, const char *id, struct type *type, bool decl) {
  return symbols_registerWithOpt(symt, id, type, NULL, NULL, decl);
}

enum symbols_resp symbols_registerWithOpt(Symbols *symt, const char *id, struct type *type, void *optData, void (*freeOpt)(void*), bool decl) {
  if (symbols_defined(symt, (void*) id)) {
    return SYM_EXISTS;
  }

  struct spair *sp = malloc(sizeof(struct spair));
  sp->sym = malloc(sizeof(struct symbol));

  *sp->sym = (struct symbol) {decl, type, optData, freeOpt};
  sp->id = str_clone(id);

  list_append(symt, sp);

  return SYM_ADDED;
}

extern struct type type_bool;

struct symbol sym_false = { false, &type_bool };
struct symbol sym_true = { false, &type_bool };

extern struct type nTNull;

struct symbol smNll = { false, &nTNull };

struct symbol *sym_null = &smNll;

struct symbol* symbol_getBuiltin(const char *name) {
  if (!strcmp(name, "false")) {
    return &sym_false;
  }

  if (!strcmp(name, "true")) {
    return &sym_true;
  }

  if (!strcmp(name, "null")) {
    return sym_null;
  }

  return NULL;
}

