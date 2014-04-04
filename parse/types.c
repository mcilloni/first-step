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

#include "../array/array.h"
#include "../utils/env.h"
#include "../utils/utils.h"

#include <stdlib.h>
#include <string.h>

struct type type_int8 = { TYPE_NUMERIC, "int8", 1, false};
struct type type_int16 = { TYPE_NUMERIC, "int16", 2, false };
struct type type_int32 = { TYPE_NUMERIC, "int32", 4, false };
struct type type_int64 = { TYPE_NUMERIC, "int64", 8, false };
struct type type_uint8 = { TYPE_NUMERIC, "uint8", 1, true };
struct type type_uint16 = { TYPE_NUMERIC, "uint16", 2, true };
struct type type_uint32 = { TYPE_NUMERIC, "uint32", 4, true };
struct type type_uint64 = { TYPE_NUMERIC, "uint64", 8, true };
struct type nTNex = {0};

size_t ptrSize = 8;

struct type *type_none = &nTNex;

struct type* type_getBuiltin(const char *name) {
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
 
 return NULL;
}

bool type_equal(struct type *a, struct type *b) {
  if (a->kind != b->kind && a->size != b->size && a->uns != b->uns) {
    return false;
  }

  switch(a->kind) {
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

    if (s1->symbols->size != s2->symbols->size) {
      return false;
    }

    MapIter *iter1 = mapiter_start(s1->symbols);
    MapIter *iter2 = mapiter_start(s2->symbols);
    Pair *pair1, *pair2;
    struct symbol *sym1, *sym2;

    while ((pair1 = mapiter_next(iter1)), (pair2 = mapiter_next(iter2))) {

      if (strcmp((char*) pair1->key, (char*) pair2->key)) {
        return false;
      }

      sym1 = (struct symbol*) pair1->value;
      sym2 = (struct symbol*) pair2->value;

      if (!type_equal(sym1->type, sym2->type)) {
        return false;
      }

      pair_free(pair1);
      pair_free(pair2);

    }

    mapiter_free(iter1);
    mapiter_free(iter2);

    break;
  }

  default: 
    break;
  }

  return true;
}

enum type_compatible type_areCompatible(struct type *assign, struct type *assigned) {
  if (assign->kind == assigned->kind) {

    switch (assign->kind) { 
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
      
    case TYPE_FUNC: {
      return type_equal(assign, assigned) ? TYPECOMP_YES : TYPECOMP_NO;
    }   
    case TYPE_PTR: {
      struct ptype *ptr1 = (struct ptype*) assign;
      struct ptype *ptr2 = (struct ptype*) assigned;

      return type_areCompatible(ptr1->val, ptr2->val);
    } 
    default:
      break;
    }
  }

  return TYPECOMP_NO;
}

struct type* type_getTypeDef(Types *types, const char *name) {
  if (types) {
    struct type *type;
    if (!(type = types_get(types, name))) {
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

bool type_isFunc(struct type *type) {
  return type->kind == TYPE_FUNC;
}

bool type_isPtr(struct type *type) {
  return type->kind == TYPE_PTR;
}

bool type_isStruct(struct type *type) {
  return type->kind == TYPE_STRUCT;
}

void type_free(struct type *type) {
  if (type) {
    if (type->kind != TYPE_NUMERIC) {
      free(type);
    }
  }
}

Types* (*types_new)(void) = (Types* (*)(void)) strmap_new;

void types_free(Types* types) {
  map_freeSpec(types, NULL, (void (*)(void*)) type_free);
}

struct type* types_get(Types *types, const char *name) {

  if (!types) {
    return NULL;
  }

  struct type *ret;
  if (!map_get(types, name, (void**) &ret)) {
    return NULL;
  }

  return ret;
}

struct type* type_makeFuncType(struct type *ret, Array *args) {
  struct ftype *ftype = calloc(1, sizeof(struct ftype));

  *ftype = (struct ftype) {{TYPE_FUNC, NULL, ptrSize}, ret, args};

  return (struct type*) ftype;
}

struct type *type_makePtr(struct type *val) {
  struct ptype *ptype = calloc(1, sizeof(struct ptype));

  *ptype = (struct ptype) {{TYPE_PTR, NULL, ptrSize}, val};

  return (struct type*) ptype;
}

struct type* type_makeStructType(Symbols *args) {
  struct stype *stype = calloc(1, sizeof(struct stype));

  *stype = (struct stype) {{TYPE_STRUCT, NULL, 0}, args};

  return (struct type*) stype;
}

void types_defineFuncId(Types *types, const char *name, struct type *ret, Array *args) {

  struct type *type = type_makeFuncType(ret, args);

  map_put(types, str_clone(name), type, FREE_KEY | FREE_VALUE);

}

enum type_kind types_isFuncDefined(Types *types, const char *name, const char *ret, Array *args) {
  struct type *type; 
  return (type = types_get(types, name)) && (type->kind == TYPE_FUNC);
}

struct type* type_secptr(struct type *type) {
  if (type_isFunc(type)) {
    struct ftype *ftype = (struct ftype*) type;
    size_t len = array_len(ftype->params);
    Array *params = array_new(len);

    for (size_t i = 0; i < len; ++i) {
      array_append(params, type_secptr((struct type*) *array_get(ftype->params, i)));
    }

    return type_makeFuncType(type_secptr(ftype->ret), params);
  }

  return type;
}

char* type_str(struct type *type, char *buffer, size_t bufLen) {
  switch (type->kind) {
  case TYPE_STRUCT: {
    Symbols *syms = ((struct stype*) type)->symbols;
    size_t wrtn = 7U;
    char *base = buffer;
    strncpy(buffer, "struct(", bufLen);
    
    if (wrtn >= bufLen) {
      return base;
    }

    buffer += wrtn;

    struct symbol *sym;
    MapIter *iter = mapiter_start(syms);
    Pair *pair;
    bool first = true;

    while((pair = mapiter_next(iter))) {
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

      strncpy(buffer, (char*) pair->key, bufLen - wrtn);
      wrtn += strlen((char*) pair->key);
      
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
      sym = (struct symbol*) pair->value;
      type_str(sym->type, buffer, bufLen - wrtn);
      wrtn += strlen(buffer);
      
      if (wrtn >= bufLen) {
        return base;
      }
      
      buffer = base + wrtn;
      pair_free(pair);
    }

    strncpy(buffer, ")", bufLen - wrtn);

    mapiter_free(iter);

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

  case TYPE_PTR: {
    if (bufLen < 5) {
      return buffer;
    }

    strncpy(buffer, "ptr ", bufLen);

    type_str(((struct ptype*) type)->val, buffer + 4, bufLen - 4);

    return buffer;
  }

  default:
    strncpy(buffer, type->name, bufLen);
    return buffer;  
  }
}
