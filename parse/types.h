#if !defined(_TYPES_H) 
#define _TYPES_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

enum type_kind {
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

struct types;

struct type* type_evalNumberType(int64_t number);
struct type* type_getBuiltin(const char *name); 
enum type_compatible type_areCompatible(struct type *assign, struct type *assigned);
struct type* type_getTypeDef(struct types *types, const char *name);
struct type* type_evalLarger(struct type *first, struct type *second);

#endif
