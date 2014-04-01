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

#include "colors.h"
#include "env.h"

#include <execinfo.h>

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

static bool debug_on = false;
static uintmax_t errors = 0;
static const uintmax_t MAX_ERRORS = 20;

int std_printout(FILE *out, const char *level, const char *color, const char *fmt, va_list va) {

  int ret = fprintf(out, "(%s%s" ANSI_COLOR_RESET, color, level);
  if (env.line) {
    ret += fprintf(out, ", line %zu): ", env.line->lineno);
  } else {
    ret += fputs("): ", out);
  }

  ret += vfprintf(out, fmt, va);

  putc('\n', out);

  if (env.line) {
    ret += puts(env.line->val);
  }

  return ret + 1;
}
 
int default_print_debug(const char *fmt, ...) {

  int ret = 0;

  if (debug_on) {
    va_list va;
    va_start(va, fmt);

    ret = std_printout(stderr, "DEBUG", ANSI_COLOR_GREEN, fmt, va);

    va_end(va);
  }

  return ret;

}

int default_print_error(const char *fmt, ...) {

  va_list va;
  va_start(va, fmt);

  int ret = std_printout(stderr, "ERROR", ANSI_COLOR_RED, fmt, va);

  va_end(va);

  if ((++errors) > MAX_ERRORS) {
    env.fail("Too many errors");
  }

  return ret;

}

int default_print_fail(const char *fmt, ...) {

  va_list va;
  va_start(va, fmt);

  std_printout(stderr, "FAIL", ANSI_COLOR_MAGENTA, fmt, va);

  va_end(va);

  void *array[100];
  size_t size = backtrace(array, 100);
  char** strings = backtrace_symbols(array, size);

  for (size_t i = 0; i < size; ++i) {
    puts(strings[i]);
  }
  
  free(strings);

  exit(EXIT_FAILURE);

  return -1; //unreachable

}

int default_print_info(const char *fmt, ...) {

  va_list va;
  va_start(va, fmt);

  int ret = std_printout(stderr, "INFO", ANSI_COLOR_CYAN, fmt, va);

  va_end(va);

  return ret;

}

int default_print_warning(const char *fmt, ...) {

  va_list va;
  va_start(va, fmt);

  int ret = std_printout(stderr, "WARNING", ANSI_COLOR_YELLOW, fmt, va);

  va_end(va);

  return ret;

}

struct env env = {default_print_debug, default_print_error, default_print_fail, default_print_info, default_print_warning, NULL};

bool env_error(void) {
  return errors;
}

struct lineno env_getLineno(void) {
  struct lineno ret = {env.line->position, env.line->lineno}; 
  return ret;
}

void env_set(struct env set) {
  env = set;
}

void env_setDebug(bool on) {
  debug_on = on;
}

void env_setLine(struct line *line) {
  line_free(env.line);
  env.line = line;
}

