#if !defined(_TYPES_H)
#define _TYPES_H


#include <cstddef>
#include <cstdint>

#include "symbols.h"

#include <array/array.h>
#include <list/pool.h>

//According to gcc, a const value is not good inside an initializer.
//According to me, gcc is a huge pile of crap.
#define ptrSize  sizeof(uintptr_t)

typedef List Aliases;

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
  type_kind kind;
  const char *name;
  size_t size;
  bool uns;
  Aliases *discover;

  type(type_kind kind, const char *name, size_t size, bool uns = false, Aliases *discover = nullptr)
    : kind(kind), name(name), size(size), uns(uns), discover(discover) {}
};

struct ftype {
  struct type t;

  struct type *ret;
  Symbols* params;

  ftype(size_t size, struct type *ret, Symbols *params) : t(TYPE_FUNC, nullptr, size), ret(ret), params(params) {}
};

struct ptype {
  struct type t;

  struct type *val;

  ptype(struct type *val, const char *name = nullptr) : t(TYPE_PTR, name, ptrSize), val(val) {}
};

struct atype {
  ptype t;

  size_t len;

  atype(struct type *val, size_t len) : t(val), len(len) {
    t.t.kind = TYPE_ARRAY;
  }
};

struct stype {
  struct type t;

  Symbols *symbols;
  stype(Symbols *syms) : t(TYPE_STRUCT, nullptr, 0), symbols(syms) {}
};

struct apair {
  char *name;
  struct type *type;

  apair(char *name, struct type *type) : name(name), type(type) {}
};

extern struct type *type_data;
extern struct type *type_none;
extern struct type *type_null;

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
struct type* type_makeFuncType(Pool *pool, struct type *ret, Symbols *args);
struct type* type_makePtr(Pool *pool, struct type *val);
struct type* type_makeAlias(Pool *pool, const char *name, Aliases *discover);
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
