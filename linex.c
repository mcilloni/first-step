#include "lines.h"

#include <stdlib.h>
#include <string.h>

int main(void) {

  struct line line;
  enum errors err = NOERR;
  
  FILE *file = fopen("base.helm", "r");

  while (!err) {
    line = line_read(file, &err);
    printf("%s (%lu == %lu)\n", line.val, line.len, strlen(line.val));
    line_free(line);
  }

  fclose(file);  
  
  if (err == ERROR) {
    perror("Error");
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;

}
