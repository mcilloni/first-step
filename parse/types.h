#if !defined(_TYPES_H) 
#define _TYPES_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "symbols.h"

#include "../array/array.h"
#include "../treemap/map.h"

enum type_kind {
  TYPE_NONE = 0,
  TYPE_FUNC,
  TYPE_NUMERIC,
  TYPE_PTR,
  TYPE_STRUCT
};

enum type_compatible {
  TYPECOMP_NO,
  TYPECOMP_SMALLER,
  TYPECOMP_INTPTR,
  TYPECOMP_YES
};

struct type {
  enum type_kind kind;
  const char *name;
  size_t size;
  bool uns;
};

struct ftype {
  struct type t;

  struct type *ret;
  Array* params;
};

struct ptype {
  struct type t;

  struct type *val;
};

struct stype {
  struct type t;

  Symbols *symbols;
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
bool type_isPtr(struct type *type);
struct type* type_makeFuncType(struct type *ret, Array *args);
struct type* type_makePtr(struct type *val);
struct type* type_makeStructType(Symbols *args);
struct type* type_secptr(struct type *type);
char* type_str(struct type *type, char *buffer, size_t bufLen);

extern Types* (*types_new)(void);
void types_free(Types* types);

struct type* types_get(Types *types, const char *name);


#endif
