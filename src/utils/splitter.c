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

#include "splitter.h"
#include "utils.h"

#include <stddef.h>
#include <stdlib.h>
#include <string.h>

struct splitter {
  char *orig;
  char *tmp;
  char c;
};

struct splitter* splitter_new(const char *toSplit, char c) {
  struct splitter *ret = malloc(sizeof(struct splitter));
  ret->orig = ret->tmp = str_clone(toSplit);
  ret->c = c;
  return ret;
}

char* splitter_next(struct splitter *spl) {
  if (!spl->tmp) {
    return NULL;
  }
  char *base = spl->tmp;
  for (;*spl->tmp && *spl->tmp != spl->c; ++spl->tmp);

  ptrdiff_t diff = spl->tmp - base;
  char *ret = NULL;

  if (diff) {
    ret = malloc(diff * sizeof(char));
    strncpy(ret, base, diff);
  }

  if (*spl->tmp == spl->c) {
    ++spl->tmp;
  }

  return ret;
}

void splitter_free(struct splitter *splitter) {
  free(splitter->orig);
  free(splitter);
}

