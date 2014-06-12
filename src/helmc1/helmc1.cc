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

#include <parse/parse.h>
#include <cgen/cgen.h>
#include <utils/env.h>
#include <utils/utils.h>

#include <cerrno>
#include <cstdio>
#include <cstdlib>
#include <cstring>

int main(int argc, char *argv[]) {

  if (argc != 2) {
    env.fail("Wrong argc: %d", argc);
  }

  FILE *file = nullptr;
  const char *filename = nullptr;

  if (!strcmp(argv[1], "-")) {
    file = stdin;
    filename = "stdin";
  } else {
    file = fopen(argv[1], "r");
    filename = argv[1];
  }

  if (!file) {
    env.fail("Error opening %s: %s", filename, strerror(errno));
  }

  struct parser *parser = parser_new(filename, nullptr);

  struct pnode *parsed = parser_parse(parser, file);
  if (parsed) {
    cgen(filename, parsed, stdout);
  }

  parser_close(parser);
  fclose(file);

  return EXIT_SUCCESS;
}

