#if !defined(_TYPES_H) 
#define _TYPES_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "../array/array.h"
#include "../treemap/map.h"

enum type_kind {
  TYPE_NONE = 0,
  TYPE_FUNC,
  TYPE_NUMERIC
};

enum type_compatible {
  TYPECOMP_NO,
  TYPECOMP_SMALLER,
  TYPECOMP_YES
};

struct type {
  enum type_kind kind;
  const char *name;
  size_t size;
};

struct ftype {
  struct type t;

  struct type *ret;
  Array* params;
};

extern struct type *type_none;

typedef Map Types;

enum type_compatible type_areCompatible(struct type *assign, struct type *assigned);
struct type* type_evalLarger(struct type *first, struct type *second);
struct type* type_evalNumberType(int64_t number);
void type_free(struct type *type);
struct type* type_getBuiltin(const char *name); 
struct type* type_getTypeDef(Types *types, const char *name);
bool type_isFunc(struct type *type);
struct type* type_makeFuncType(struct type *ret, Array *args);
char* type_str(struct type *type, char *buffer, size_t bufLen);

extern Types* (*types_new)(void);
void types_free(Types* types);

struct type* types_get(Types *types, const char *name);


#endif
