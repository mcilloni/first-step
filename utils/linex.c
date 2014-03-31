#include "lines.h"
#include "env.h"

#include <stdlib.h>
#include <string.h>

int main(void) {

  enum errors err = NOERR;
  
  FILE *file = fopen("../base.helm", "r");

  while (!err) {
    env_setLine(line_read(file, &err));
    env.info("%s (%zu == %zu)", env.line->val, env.line->len, strlen(env.line->val));
  }

  line_free(env.line);
  fclose(file);  
  
  if (err == ERROR) {
    perror("Error");
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;

}
