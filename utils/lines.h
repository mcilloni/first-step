#if !defined(_LINE_H)
#define _LINE_H

#include "errors.h"

#include <stdio.h>

struct line{
  size_t len;
  size_t position;
  size_t lineno;
  char *val;
};

struct lineno {
  size_t no,pos;
};

struct line* line_read(FILE *file, enum errors *err);

void line_free(struct line *l);

#endif
