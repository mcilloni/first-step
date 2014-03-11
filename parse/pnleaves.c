#include "pnleaves.h"

inline struct pnode** leaves_get(Leaves *leaves, size_t index) {
  return (struct pnode**) array_get(leaves, index);
}
 

