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

#include <cerrno>

#include <cstdlib>
#include <cstring>
#include <cctype>

struct filereader* filereader_open(const char *path) {
  return filereader_fromFile(fopen(path, "r"));
}

struct filereader* filereader_fromFile(FILE *file){
  if (!file) {
    return nullptr;
  }

  struct filereader *fr = new filereader();

  fr->file = file;

  return fr;
}

void filereader_close(struct filereader *fr) {
  if (fr) {
    fclose(fr->file);
  }
}

void filereader_free(struct filereader *fr) {
  if (fr) {
    delete fr;
  }
}

struct line* line_read(struct filereader *file, enum errors *err) {
  struct line *ret = new line;
  *ret = {0, file->lineno};

  char ch;

  *err = NOERR;

  bool loop = true, linecomment = false;

  while(loop) {

    ch = fgetc(file->file);

    switch(ch) {
    case EOF: {
      if (ferror(file->file)) {
        env.fail("Error while reading file, %s", strerror(errno));
        *err = ERROR;
      } else {
        if(!ret->val.length()) {
          *err = FILEEND;
        }
      }
      return ret;
    }

    case '\n': {
      linecomment = false;
      if(ret->val.length()) {
        loop = false;
      } 

      ++file->lineno;

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

      if(!ret->val.length() && isblank((int) ch)) {
        break;
      }

      ret->val += ch;

      break;
    }

    }

  }

  ret->lineno = file->lineno;

  return ret;
  
}

void line_free(struct line *l) {
  if (l) {
    delete l;
  }
}

