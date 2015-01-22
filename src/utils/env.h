#if !defined(_ENV_H)
#define _ENV_H

#include "lines.h"

#include <cstdio>
#include <cstdarg>
#include <cstdint>

typedef int (*printfmt)(const char *fmt, ...);

struct env {
private:
  int printout(FILE *out, const char *level, const char *color, const char *fmt, va_list va);
public:
  int debug(const char *fmt, ...);
  int error(const char *fmt, ...);
  int fail(const char *fmt, ...);
  int info(const char *fmt, ...);
  int warning(const char *fmt, ...);
  uintmax_t *lastLineno;
  const char *filename;
};

extern struct env env;

void lineno_setLoc(uintmax_t *linLoc);
struct lineno env_getLineno(void);
void env_reset(void);
void env_set(struct env set);
void env_setDebug(bool on);
void env_setFilename(char *name);
void env_setLine(struct line *line);

#endif
