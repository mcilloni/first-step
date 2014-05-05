#if !defined(_ENV_H)
#define _ENV_H

#include "lines.h"

#include <stdbool.h>
#include <stdint.h>

typedef int (*printfmt)(const char *fmt, ...);

struct env {
  printfmt debug;
  printfmt error;
  printfmt fail;
  printfmt info;
  printfmt warning;
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
