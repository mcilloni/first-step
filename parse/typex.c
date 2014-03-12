#include "types.h"

#include <stdio.h>
#include <stdlib.h>

const char* represent(enum type_compatible t) {
  switch (t) {
  case TYPECOMP_NO:
    return "no";
  case TYPECOMP_SMALLER:
    return "yes, but smaller";
  case TYPECOMP_YES:
    return "yes";
  default:
    return "don't know";
  }
}

int main(void) {
  struct type *first = type_getTypeDef(NULL, "int8");
  struct type *second = type_getTypeDef(NULL, "int32");

  printf("%s and %s are compatible? %s.\n", first->name, second->name, represent(type_areCompatible(first, second)));
  printf("%s and %s are compatible? %s.\n", second->name, first->name, represent(type_areCompatible(second, first)));

  return EXIT_SUCCESS;
}
