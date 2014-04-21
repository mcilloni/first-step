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

#include <utils/env.h>

#include <list/list.h>

#include <sys/time.h>
#include <inttypes.h>
#include <stdlib.h>

void printlist(List *list) {
  bool inside = false;
  fputs("[ ", stdout);
  size_t len = list_len(list);
  struct token *tok;

  for (uint8_t i = 0; i < len; ++i) {
    tok = *list_get(list, i);

    if (inside) {
      fputs(", ",stdout);
    } else {
      inside = true;
    }

    fputs(token_str(tok), stdout);

    switch (tok->type) {
    case LEX_ID:
      printf(": %s", (char*) tok->value);
      break;

    case LEX_NUMBER:
      printf(": %" PRIuMAX, tok->value);
      break;

    default:
      break;
    }
  }
  puts(" ]");
}                      

int main(int argc, char *argv[]) {

  if (argc != 2) {
    env.fail("Wrong argc: %d", argc);
  }

  struct timeval t;
  struct timezone tzp;
  gettimeofday(&t, &tzp);

  uintmax_t t1 = t.tv_sec*1000000LU + t.tv_usec;

  struct parser *parser = parser_new(argv[1]);

  struct pnode *ret = parser_parse(parser, fopen(argv[1], "r"));

  gettimeofday(&t, &tzp);

  ptree_dump(ret);

  printf("\nParsing took %" PRIuMAX " µs\n", (t.tv_sec*1000000LU + t.tv_usec) - t1);

  parser_close(parser);

  return EXIT_SUCCESS;
}
