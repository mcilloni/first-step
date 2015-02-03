#if !defined(_LINE_H)
#define _LINE_H

#include "errors.h"

#include <cstdint>
#include <cstdio>

#include <string>

struct filereader {
  size_t lineno;
  FILE *file;
};

struct line {
  size_t position;
  uintmax_t lineno;
  std::string val;
};

struct filereader* filereader_open(const char *path);
struct filereader* filereader_fromFile(FILE *file);
void filereader_close(struct filereader *fr);
void filereader_free(struct filereader *fr);

struct line* line_read(struct filereader *file, enum errors *err);

void line_free(struct line *l);

#endif
