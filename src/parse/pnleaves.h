#if !defined(_PNLEAVES_H)
#define _PNLEAVES_H

#include <array/array.h>

typedef Array Leaves;

struct pnode;

struct pnode** leaves_get(Leaves *leaves, size_t index);

#endif
