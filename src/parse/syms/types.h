#if !defined(_TYPES_H) 
#define _TYPES_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "symbols.h"

#include <array/array.h>
#include <list/pool.h>

enum type_kind {
  //don't free
  TYPE_NONE = 0,
  TYPE_BOOL,
  TYPE_NUMERIC,
  TYPE_NULL,

  //free
  TYPE_FUNC,
  TYPE_PTR,
  TYPE_STRUCT,
  TYPE_ALIAS,
  TYPE_ARRAY
};

enum type_compatible {
  TYPECOMP_NO,
  TYPECOMP_SMALLER,
  TYPECOMP_INTPTR,
  TYPECOMP_YES
};

struct type {
  enum type_kind kind;
  char *name;
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

struct atype {
  struct ptype t;

  size_t len;
};

struct stype {
  struct type t;

  Symbols *symbols;
};

struct apair {
  char *name;
  struct type *type;
};

extern struct type *type_data;
extern struct type *type_none;
extern struct type *type_null;

typedef List Aliases;

enum type_compatible type_areCompatible(struct type *assign, struct type *assigned);
struct type* type_evalLarger(struct type *first, struct type *second);
struct type* type_evalNumberType(int64_t number);
void type_free(struct type *type);
struct type* type_getBuiltin(const char *name); 
struct type* type_getTypeDef(Aliases *aliases, const char *name);
bool type_isAlias(struct type *type);
bool type_isArray(struct type *type);
bool type_isBool(struct type *type);
bool type_isFunc(struct type *type);
bool type_isNull(struct type *type);
bool type_isNumeric(struct type *type);
bool type_isPtr(struct type *type);
bool type_isStruct(struct type *type);
struct type* type_makeFuncType(Pool *pool, struct type *ret, Array *args);
struct type* type_makePtr(Pool *pool, struct type *val);
struct type* type_makeAlias(Pool *pool, const char *name);
struct type* type_makeArray(Pool *pool, struct type *val, size_t len);
struct type* type_makeStructType(Pool *pool, Symbols *args);
char* type_str(struct type *type, char *buffer, size_t bufLen);

extern Aliases* (*aliases_new)(void);
void* aliases_alias(Aliases *aliases, const char *alias, struct type *type);
void aliases_dump(Aliases *aliases, const char *title, int8_t depth);
void aliases_free(Aliases *aliases);
extern size_t (*aliases_len)(Aliases *aliases);

struct type* aliases_get(Aliases *aliases, const char *name);


#endif
