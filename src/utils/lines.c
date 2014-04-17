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

#include "lines.h"
#include "env.h"

#include <errno.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>



static const size_t initSize = 512;
size_t lineno = 0;

struct line* line_read(FILE *file, enum errors *err) {
  struct line *ret = malloc(sizeof(struct line));
  *ret = (struct line){0, 0, lineno, calloc(sizeof(char), initSize)};

  char ch;

  *err = NOERR;

  bool loop = true, linecomment = false;

  while(loop) {

    ch = fgetc(file);

    switch(ch) {
    case EOF: {
      if (ferror(file)) {
        env.fail("Error while reading file, %s", strerror(errno));
        *err = ERROR;
      } else {
        if(!ret->len) {
          *err = FILEEND;
        }
      }
      return ret;
    }

    case '\n': {
      linecomment = false;
      if(ret->len) {
        loop = false;
      } 

      ++lineno;

      break;  
    }

    case '#': {
      linecomment = true;
      break;
    }

    default: {
      if (linecomment) {
        break;
      }

      if(!ret->len && isblank((int) ch)) {
        break;
      }

      if ((ret->len + 1) % initSize == 0) {
        ret->val = realloc(ret->val, ret->len + initSize + 1);
      }

      ret->val[ret->len] = ch;

      ++ret->len;

      break;
    }

    }

  }

  ret->lineno = lineno;
  ret->val[ret->len] = 0;

  return ret;
  
}

void line_free(struct line *l) {
  if (l) {
    free(l->val);
    free(l);
  }
}

