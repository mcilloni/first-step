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

#include "types.h"

#include <array/array.h>
#include <utils/env.h>
#include <utils/utils.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

//According to gcc, a const value is not good inside an initializer. 
//According to me, gcc is a huge pile of crap.
#define ptrSize  sizeof(uintptr_t)

struct type nTNex = {0};
struct type nTNull = { TYPE_NULL, "null", ptrSize, false };
struct ptype nPTDat = {{ TYPE_PTR, "data", ptrSize, false }, &nTNex };
struct type type_bool = { TYPE_BOOL, "bool", 1, true };
struct type type_int8 = { TYPE_NUMERIC, "int8", 1, false };
struct type type_int16 = { TYPE_NUMERIC, "int16", 2, false };
struct type type_int32 = { TYPE_NUMERIC, "int32", 4, false };
struct type type_int64 = { TYPE_NUMERIC, "int64", 8, false };
struct type type_uint8 = { TYPE_NUMERIC, "uint8", 1, true };
struct type type_uint16 = { TYPE_NUMERIC, "uint16", 2, true };
struct type type_uint32 = { TYPE_NUMERIC, "uint32", 4, true };
struct type type_uint64 = { TYPE_NUMERIC, "uint64", 8, true };
struct type type_uintptr = { TYPE_NUMERIC, "uintptr", ptrSize, true };

struct type *type_data = (struct type*) &nPTDat;
struct type *type_none = &nTNex;
struct type *type_null = &nTNull;

struct type* type_getBuiltin(const char *name) {
  if (!strcmp(name, "bool")) {
    return &type_bool;
  }

  if (!strcmp(name, "int8")) {
    return &type_int8;
  }

  if (!strcmp(name, "int16")) {
    return &type_int16;
  }

  if (!strcmp(name, "int32")) {
    return &type_int32;
  }

  if (!strcmp(name, "int64")) {
    return &type_int64;
  }
   
  if (!strcmp(name, "uint8")) { 
    return &type_uint8;                                                                                                           
  }                                                                                                                                    

  if (!strcmp(name, "uint16")) {                                                                        
    return &type_uint16;                                                                                     
  }                                                                                                                                              
                                                                                                                                               
  if (!strcmp(name, "uint32")) {                                                                                     
    return &type_uint32;                                                                                                     
  }                                                                                                                      
                                                                                                                         
  if (!strcmp(name, "uint64")) {                                                                                              
    return &type_uint64;                                                                                  
  }                

  if (!strcmp(name, "uintptr")) {
    return &type_uintptr;
  }

  if (!strcmp(name, "data")) {
    return type_data;
  }

  if (!strcmp(name, "null")) {
    env.fail("null is a reserved type");
  }
   
  return NULL;
}

bool type_equal(struct type *a, struct type *b) {
  if (a->kind != b->kind && a->size != b->size && a->uns != b->uns) {
    return false;
  }

  switch(a->kind) {
  case TYPE_ALIAS:
    return !strcmp(a->name, b->name);
  case TYPE_FUNC: {
    struct ftype *f1 = (struct ftype*) a;
    struct ftype *f2 = (struct ftype*) b;

    if (!type_equal(f1->ret, f2->ret)) {
      return false;
    }

    size_t len = array_len(f1->params);

    if (len != array_len(f2->params)) {
      return false;
    }

    for (size_t i = 0; i < len; ++i) {

      if (!type_equal((struct type*) *array_get(f1->params, i), (struct type*) *array_get(f2->params, i))) {
        return false;
      }

    }

    break;
  }

  case TYPE_PTR: {
    if (!type_equal(((struct ptype*) a)->val, ((struct ptype*) b)->val)) {
      return false;
    }
    break;
  }

  case TYPE_STRUCT: {
    struct stype *s1 = (struct stype*) a;
    struct stype *s2 = (struct stype*) b;

    size_t len = symbols_len(s1->symbols);

    if (len != symbols_len(s2->symbols)) {
      return false;
    }

    struct spair *pair1, *pair2;

    for (size_t i = 0; i < len; ++i) {
      pair1 = *list_get(s1->symbols, i); 
      pair2 = *list_get(s2->symbols, i);

      if (strcmp(pair1->id, pair2->id)) {
        return false;
      }


      if (!type_equal(pair1->sym->type, pair2->sym->type)) {
        return false;
      }

    }

    break;
  }

  default: 
    break;
  }

  return true;
}

enum type_compatible type_areCompatible(struct type *assign, struct type *assigned) {
  enum type_kind first = type_isArray(assign) ? TYPE_PTR : assign->kind;
  enum type_kind second = type_isArray(assigned) ? TYPE_PTR : assigned->kind;

  if (first == TYPE_NULL) {
    return TYPECOMP_NO;
  }

  if (first == second) {

    switch (first) { 
    case TYPE_ALIAS:
      return strcmp(assign->name, assigned->name) ? TYPECOMP_NO : TYPECOMP_YES;
    case TYPE_BOOL:
      return TYPECOMP_YES;
    case TYPE_NUMERIC:
      if (assign->size < assigned->size) {
        return TYPECOMP_SMALLER;
      }
      
      if (assign->size == assigned->size && !assign->uns && assigned->uns) {
       	char buf[2048], cuf[2048]; 
        type_str(assign, buf, 2048);
        type_str(assigned, cuf, 2048);
	      env.warning("Assigning unsigned type %s to same-size signed type %s", cuf, buf);
      }

      return TYPECOMP_YES;
      
    case TYPE_FUNC:
    case TYPE_STRUCT: {
      return type_equal(assign, assigned) ? TYPECOMP_YES : TYPECOMP_NO;
    }   

    case TYPE_PTR: {
      if (assign == type_data) {
        return TYPECOMP_YES;
      }

      struct ptype *ptr1 = (struct ptype*) assign;
      struct ptype *ptr2 = (struct ptype*) assigned;

      return type_areCompatible(ptr1->val, ptr2->val);
    } 
    default:
      break;
    }
  }

  if (first == TYPE_PTR && second == TYPE_NULL) {
    return TYPECOMP_YES;
  }

  return TYPECOMP_NO;
}

struct type* type_getTypeDef(Aliases *aliases, const char *name) {
  if (aliases) {
    struct type *type;
    if (!(type = aliases_get(aliases, name))) {
      return type;
    }
  }
  return type_getBuiltin(name);
}

struct type* type_evalLarger(struct type *first, struct type *second) {
  return (first->size > second->size) ? first : second;
}

#define IN(NUM, A, B) (NUM >= A && NUM <= B)

struct type* type_evalNumberType(int64_t number) {
  if (IN(number, INT8_MIN, INT8_MAX)) {
    return &type_int8;
  }

  if (IN(number, INT16_MIN, INT16_MAX)) {
    return &type_int16;
  }

  if (IN(number, INT32_MIN, INT32_MAX)) {
    return &type_int32;
  }

  return &type_int64;
}

bool type_isAlias(struct type *type) {
  return type->kind == TYPE_ALIAS;
}

bool type_isArray(struct type *type) {
  return type->kind == TYPE_ARRAY;
}

bool type_isBool(struct type *type) {
  return type->kind == TYPE_BOOL;
}

bool type_isNull(struct type *type) {
  return type == type_null;
}

bool type_isNumeric(struct type *type) {
  return type->kind == TYPE_NUMERIC;
}

bool type_isFunc(struct type *type) {
  return type->kind == TYPE_FUNC;
}

bool type_isPtr(struct type *type) {
  return type->kind == TYPE_PTR || type_isArray(type);
}

bool type_isStruct(struct type *type) {
  return type->kind == TYPE_STRUCT;
}

void type_free(struct type *type) {
  if (type && type != type_data && type->kind >= TYPE_FUNC) {
    if (type->kind == TYPE_ALIAS) {
      free(type->name);
    }

    free(type);
  }
}

Aliases* (*aliases_new)(void) = (Aliases* (*)(void)) strmap_new;

void* aliases_alias(Aliases* aliases, const char* id, struct type* type) {
  return map_put(aliases, str_clone(id), type, FREE_KEY);
}

void aliases_free(Aliases* aliases) {
  map_freeSpec(aliases, NULL, (void (*)(void*)) type_free);
}

struct type* aliases_get(Aliases *aliases, const char *name) {

  if (!aliases) {
    return NULL;
  }

  struct type *ret;
  if (!map_get(aliases, name, (void**) &ret)) {
    return NULL;
  }

  return ret;
}

struct type* type_makeAlias(Pool *pool, const char *name) {
  struct type *alias = pool_zalloc(pool, sizeof(struct type));

  *alias = (struct type) {TYPE_ALIAS, str_clone(name), 0};

  return alias;
}

struct type* type_makeFuncType(Pool *pool, struct type *ret, Array *args) {
  struct ftype *ftype = pool_zalloc(pool, sizeof(struct ftype));

  *ftype = (struct ftype) {{TYPE_FUNC, NULL, ptrSize}, ret, args};

  return (struct type*) ftype;
}

struct type *type_makePtr(Pool *pool, struct type *val) {
  struct ptype *ptype = pool_zalloc(pool, sizeof(struct ptype));

  *ptype = (struct ptype) {{TYPE_PTR, NULL, ptrSize}, val};

  return (struct type*) ptype;
}

struct type* type_makeStructType(Pool *pool, Symbols *args) {
  struct stype *stype = pool_zalloc(pool, sizeof(struct stype));

  *stype = (struct stype) {{TYPE_STRUCT, NULL, 0}, args};

  return (struct type*) stype;
}

struct type* type_makeArray(Pool *pool, struct type *val, size_t len) {
  struct atype *atype = pool_zalloc(pool, sizeof(struct atype));

  *atype = (struct atype) {{{TYPE_ARRAY, NULL, ptrSize}, val}, len};

  return (struct type*) atype;
}

void aliases_defineFuncId(Aliases *aliases, Pool *pool, const char *name, struct type *ret, Array *args) {

  struct type *type = type_makeFuncType(pool, ret, args);

  map_put(aliases, str_clone(name), type, FREE_KEY);

}

enum type_kind aliases_isFuncDefined(Aliases *aliases, const char *name, const char *ret, Array *args) {
  struct type *type; 
  return (type = aliases_get(aliases, name)) && (type->kind == TYPE_FUNC);
}

extern void printdepth(int8_t depth);

void aliases_dump(Aliases *aliases, const char *title, int8_t depth) {
  if (!aliases->size) {
    return;
  }

  printdepth(depth);
  puts(title);

  ++depth;

  if (!aliases) {
    printdepth(depth);
    puts("(nil)");
    return;
  }

  MapIter *iter = mapiter_start(aliases);
  Pair *pair;
  const char *name;
  struct type *type;
  char buf[2048];

  while((pair = mapiter_next(iter))) {
    printdepth(depth);
    name = (const char*) pair->key;
    type = (struct type*) pair->value;

    printf("%s: %s\n", name, type_str(type, buf, 2048));

    pair_free(pair);
  }

  mapiter_free(iter);
}

char* type_str(struct type *type, char *buffer, size_t bufLen) {
  switch (type->kind) {
  case TYPE_ALIAS: {
    strncpy(buffer, type->name, bufLen);
    return buffer;
  }
  
  case TYPE_NULL: {
    strncpy(buffer, "nullptr", bufLen);
    return buffer;
  }

  case TYPE_STRUCT: {
    Symbols *syms = ((struct stype*) type)->symbols;
    size_t wrtn = 7U;
    char *base = buffer;
    strncpy(buffer, "struct(", bufLen);
    
    if (wrtn >= bufLen) {
      return base;
    }

    buffer += wrtn;

    size_t len = symbols_len(syms);
    struct spair *pair;
    bool first = true;

    for (size_t i = 0; i < len; ++i) {
      pair = *list_get(syms, i);
      if (first) {
        first = false;
      } else {
        strncpy(buffer, ", ", bufLen - wrtn);
        wrtn += 2;
      
        if (wrtn >= bufLen) {
          return base;
        }
      
        buffer += 2;
      }

      strncpy(buffer, (char*) pair->id, bufLen - wrtn);
      wrtn += strlen((char*) pair->id);
      
      buffer = base + wrtn;

      if (wrtn >= bufLen) {
        return base;
      }

      *buffer = ' ';
      ++buffer;
      if (++wrtn >= bufLen) {
        return base;
      }
      
      buffer = base + wrtn;
      type_str(pair->sym->type, buffer, bufLen - wrtn);
      wrtn += strlen(buffer);
      
      if (wrtn >= bufLen) {
        return base;
      }
      
      buffer = base + wrtn;
    }

    strncpy(buffer, ")", bufLen - wrtn);

    return base;
  }

  case TYPE_FUNC: {
    struct ftype *ftype = (struct ftype*) type;
    size_t wrtn = 5U;
    char *base = buffer, *tmp;
    strncpy(buffer, "func(", bufLen);
    
    if (wrtn >= bufLen) {
      return base;
    }

    buffer += wrtn;

    size_t pLen = array_len(ftype->params);
    bool first = true;

    for (size_t i = 0; i < pLen; ++i) {

      if (wrtn >= bufLen) {
        return base;
      }

      if (first) {
        first = false;
      } else {
        *buffer = ',';
        buffer = base + ++wrtn;
      }

      if (wrtn >= bufLen) {
        return base;
      }

      tmp = type_str(*array_get(ftype->params, i), buffer, bufLen - wrtn);
      wrtn += strlen(tmp);

      if (wrtn >= bufLen) {
        return base;
      }

      buffer = base + wrtn;
    }

    strncpy(buffer, ") ", bufLen - wrtn);
    buffer += 2;
    wrtn += 2;

    if (ftype->ret != type_none) {
      type_str(ftype->ret, buffer, bufLen - wrtn);
    }
    
    return base;
  }
                  
  case TYPE_ARRAY:
  case TYPE_PTR: {
    if (bufLen < 10) {
      return buffer;
    }

    if (type == type_data) {
      strncpy(buffer, "data", bufLen);
      return buffer;
    }

    size_t i = 4U;

    if (type->kind == TYPE_ARRAY) {
      i = snprintf(buffer, bufLen, "[%zu] ", ((struct atype*) type)->len);
    } else {
      strncpy(buffer, "ptr ", bufLen);
    }

    type_str(((struct ptype*) type)->val, buffer + i, bufLen - i);

    return buffer;
  }

  default:
    strncpy(buffer, type->name, bufLen);
    return buffer;  
  }
}
