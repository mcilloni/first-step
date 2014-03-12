#include "types.h"

#include <string.h>

struct type type_int8 = { TYPE_NUMERIC, "int8", 8 };
struct type type_int16 = { TYPE_NUMERIC, "int16", 16 };
struct type type_int32 = { TYPE_NUMERIC, "int32", 32 };
struct type type_int64 = { TYPE_NUMERIC, "int64", 64 };

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

    if (assign->kind == TYPE_NUMERIC) {
      if (assign->size < assigned->size) {
        return TYPECOMP_SMALLER;
      }

      return TYPECOMP_YES;
    }
  
  }

  return TYPECOMP_NO;
}

struct type* type_getTypeDef(struct types *types, const char *name) {
  return type_getBuiltin(name);
}

struct type* type_evalLarger(struct type *first, struct type *second) {
  return (first->size > second->size) ? first : second;
}

#define IN(NUM, A, B) (NUM >= A && NUM <= B)

struct type* type_evalNumberType(int64_t number) {
  if (IN(number, INT8_MIN, INT16_MAX)) {
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
