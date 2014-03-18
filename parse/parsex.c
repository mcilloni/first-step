#include "parse.h"

#include <stdlib.h>

int main(void) {

  struct pnode *ret = parse("../base.helm");

  ptree_dump(ret);
  pnode_free(ret);

  return EXIT_SUCCESS;
}
