/*
 *  This file is part of First Step.
 *  
 *  First Step is free software: you can redistribute it and/or modify it under the terms of the GNU Lesser General Public License as published by the Free Software 
 *  Foundation, either version 3 of the License, or (at your option) any later version. 
 *
 *  First Step is distributed in the hope that it will be useful, but 
 *  WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with First Step.  If not, see <http://www.gnu.org/licenses/>
 *
 *  Copyright (C) Marco Cilloni <marco.cilloni@yahoo.com> 2014
 *
 */

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

void c_compile(const char *bin, const char *cfile, bool link) {
  pid_t pid;

  switch ((pid = fork())) {
  case -1:
    env.fail("Forking failed, cannot launch clang");
    break;
  case 0: {
    int hs;

    if (link) {
      hs = execlp("clang", "clang", "-Wno-parentheses-equality", "-g", "-o", bin, cfile, "helmrt.o", NULL);
    } else {
      hs = execlp("clang", "clang", "-Wno-parentheses-equality", "-g", "-c", cfile, NULL);
    }

    if (hs) {
      env.fail("Cannot launch clang: %s", strerror(errno));
    }
    break;
  }
  default: {
    int status;
    waitpid(pid, &status, 0);

    /*if (remove(cfile)) {
      env.warning("Cannot remove temporary file %s", cfile);
    }*/

    if (WEXITSTATUS(status) != EXIT_SUCCESS) {
      env.fail("clang failed");
    }

    break;
  }
  }

}

int main(int argc, char *argv[]) {
  char *progname = NULL; 
  bool link = true;

  switch (argc) {
  case 2:
    progname = (char*) argv[1];
    break;
  case 3:
    if (strcmp(argv[1], "-c")) {
      env.fail("Unknown option %s", argv[1]);
    }

    link = false;

    progname = (char*) argv[2];

    break;
  default:
    env.fail("Wrong number of arguments: %d", argc); 
  }

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

  c_compile(progname, cpname, link);

  free(cpname);
  free(cprog);

  return EXIT_SUCCESS;
}

