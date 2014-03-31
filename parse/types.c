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

struct type type_int8 = { TYPE_NUMERIC, "int8", 1 };
struct type type_int16 = { TYPE_NUMERIC, "int16", 2 };
struct type type_int32 = { TYPE_NUMERIC, "int32", 4 };
struct type type_int64 = { TYPE_NUMERIC, "int64", 8 };
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

 return NULL;
}

enum type_compatible type_areCompatible(struct type *assign, struct type *assigned) {
  if (assign->kind == assigned->kind) {

    switch (assign->kind) { 
    case TYPE_NUMERIC:
      if (assign->size < assigned->size) {
        return TYPECOMP_SMALLER;
      }

      return TYPECOMP_YES;
    case TYPE_FUNC: {
      char buf[2048], cuf[2048]; //horrible hack that makes things quicker. Maybe I will change this one day.
      type_str(assign, buf, 2048);
      type_str(assigned, cuf, 2048);

      return strcmp(buf, cuf) ? TYPECOMP_NO : TYPECOMP_YES;
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

void type_free(struct type *type) {
  if (type->kind != TYPE_NUMERIC) {
    free(type);
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
