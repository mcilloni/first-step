#include "../parse/parse.h"
#include "../cgen/cgen.h"
#include "../utils/env.h"
#include "../utils/utils.h"

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

void c_compile(const char *bin, const char *cfile) {
  pid_t pid;

  switch ((pid = fork())) {
  case -1:
    env.fail("Forking failed, cannot launch clang");
    break;
  case 0:
    if (execlp("clang", "clang", "-g", "-o", bin, cfile, "helmrt.o", NULL)) {
      env.fail("Cannot launch clang: %s", strerror(errno));
    }
    break;
  default: {
    int status;
    waitpid(pid, &status, 0);

    if (remove(cfile)) {
      env.warning("Cannot remove temporary file %s", cfile);
    }

    if (WEXITSTATUS(status) != EXIT_SUCCESS) {
      env.fail("clang failed");
    }

    break;
  }
  }

}

int main(int argc, char *argv[]) {
  if (argc != 2) {
    env.fail("Wrong number of arguments: %d", argc); 
  }

  char *progname = (char*) argv[1];
  char *ext = strrchr(progname, '.'); 

  puts("First Step - Helm Experimental compiler\n"
       "Built for experimental purposes. WORK IN PROGRESS");

  if (!ext) {
    env.fail("Cannot compile a file without .helm extension");
  }

  if (strcmp(ext, ".helm")) {
    env.fail("Unknown extension '%s'");
  }


  struct pnode *parsed = parse(progname);
  char *cprog;
  size_t size;
  FILE *memstr = open_memstream(&cprog, &size);

  strcpy(ext, ".c");

  char *div = strrchr(progname, '/');

  if (div) {
    progname = div + 1;
  }

  cgen(parsed, memstr);

  fclose(memstr);
  FILE *cfile = fopen(progname, "w");
  
  if (!cfile) {
    env.fail("Cannot create file %s", progname);
  }

  puts(cprog);

  fputs(cprog, cfile);
  fclose(cfile);
  char *cpname = str_clone(progname);

  *ext = 0;

  c_compile(progname, cpname);

  free(cpname);
  free(cprog);

  return EXIT_SUCCESS;
}

