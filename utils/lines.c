#include "lines.h"

#include <stdbool.h>
#include <stdlib.h>

#include <jemalloc/jemalloc.h>

static const size_t initSize = 512;

struct line line_read(FILE *file, enum errors *err) {
  struct line ret = {0, 0, calloc(sizeof(char), initSize)};

  char ch;

  *err = NOERR;

  bool loop = true;

  for(ret.len = ret.position = 0; loop; ) {

    ch = fgetc(file);

    switch(ch) {
    case EOF: {
      if (ferror(file)) {
        *err = ERROR;
      } else {
        if(!ret.len) {
          *err = FILEEND;
        }
      }
      return ret;
    }
    
    case '\n': {
      if(ret.len) {
        loop = false;
      } 
      break;  
    }

    default: {

      if ((ret.len + 1) % initSize == 0) {
        ret.val = realloc(ret.val, ret.len + initSize + 1);
      }

      ret.val[ret.len] = ch;

      ++ret.len;

      break;
    }

    }

  }

  ret.val[ret.len] = 0;

  return ret;
  
}

void line_free(struct line l) {
  if (l.val) {
    free(l.val);
  }
}

