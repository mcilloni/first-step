#include "env.h"

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

static bool debug_on = false;
static uintmax_t errors = 0;
static const uintmax_t MAX_ERRORS = 20;

int std_printout(FILE *out, const char *level, const char *fmt, va_list va) {

  int ret = fprintf(out, "(%s, line %lu, pos %lu): ", level, env.line.lineno, env.line.position) + vfprintf(out, fmt, va);

  putc('\n', out);

  return ret + 1;
}
 
int default_print_debug(const char *fmt, ...) {

  int ret = 0;

  if (debug_on) {
    va_list va;
    va_start(va, fmt);

    ret = std_printout(stderr, "DEBUG", fmt, va);

    va_end(va);
  }

  return ret;

}

int default_print_error(const char *fmt, ...) {

  va_list va;
  va_start(va, fmt);

  int ret = std_printout(stderr, "ERROR", fmt, va);

  va_end(va);

  if ((++errors) > MAX_ERRORS) {
    env.fail("Too many errors");
  }

  return ret;

}

int default_print_fail(const char *fmt, ...) {

  va_list va;
  va_start(va, fmt);

  std_printout(stderr, "FAIL", fmt, va);

  va_end(va);

  exit(EXIT_FAILURE);

  return -1; //unreachable

}

int default_print_info(const char *fmt, ...) {

  va_list va;
  va_start(va, fmt);

  int ret = std_printout(stderr, "INFO", fmt, va);

  va_end(va);

  return ret;

}

int default_print_warning(const char *fmt, ...) {

  va_list va;
  va_start(va, fmt);

  int ret = std_printout(stderr, "WARNING", fmt, va);

  va_end(va);

  return ret;

}

struct env env = {default_print_debug, default_print_error, default_print_fail, default_print_info, default_print_warning, {0}};

bool env_error(void) {
  return errors;
}

struct lineno env_getLineno(void) {
  struct lineno ret = {env.line.position,env.line.lineno}; 
  return ret;
}

void env_set(struct env set) {
  env = set;
}

void env_setDebug(bool on) {
  debug_on = on;
}

void env_setLine(struct line line) {
  line_free(env.line);
  env.line = line;
}
