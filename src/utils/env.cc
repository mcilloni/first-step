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
#include "utils.h"

#include <cstdarg>
#include <cstdio>
#include <cstdlib>
#include <cstring>

#include <libgen.h>

static bool debug_on = false;
static uintmax_t errors = 0;
static const uintmax_t MAX_ERRORS = 20;

int env::printout(FILE *out, const char *level, const char *color, const char *fmt, va_list va) {

  int ret = fprintf(out, ANSI_COLOR_BOLD "(%s%s" ANSI_COLOR_RESET ANSI_COLOR_BOLD, color, level);
  if (this->lastLineno && this->filename) {
    char *tmp = str_clone(this->filename);
    ret += fprintf(out, ": in file %s, line %ju): ", basename(tmp), *this->lastLineno);
    free(tmp);
  } else {
    ret += fputs("): ", out);
  }

  ret += vfprintf(out, fmt, va);

  fputs(ANSI_COLOR_RESET "\n", out);

  return ret + 1;
}

int env::debug(const char *fmt, ...) {

  int ret = 0;

  if (debug_on) {
    va_list va;
    va_start(va, fmt);

    ret = this->printout(stderr, "DEBUG", ANSI_COLOR_GREEN, fmt, va);

    va_end(va);
  }

  return ret;

}

int env::error(const char *fmt, ...) {

  va_list va;
  va_start(va, fmt);

  int ret = this->printout(stderr, "ERROR", ANSI_COLOR_RED, fmt, va);

  va_end(va);

  if ((++errors) > MAX_ERRORS) {
    this->fail("Too many errors");
  }

  return ret;

}

int env::fail(const char *fmt, ...) {

  va_list va;
  va_start(va, fmt);

  this->printout(stderr, "FAIL", ANSI_COLOR_MAGENTA, fmt, va);

  va_end(va);

  exit(EXIT_FAILURE);

  return -1; //unreachable

}

int env::info(const char *fmt, ...) {

  va_list va;
  va_start(va, fmt);

  int ret = this->printout(stderr, "INFO", ANSI_COLOR_CYAN, fmt, va);

  va_end(va);

  return ret;

}

int env::warning(const char *fmt, ...) {

  va_list va;
  va_start(va, fmt);

  int ret = this->printout(stderr, "WARNING", ANSI_COLOR_YELLOW, fmt, va);

  va_end(va);

  return ret;

}

struct env env = {0};

bool env_error(void) {
  return errors;
}

void env_reset(void) {
  std::memset(&env, 0, sizeof env);
}

void env_set(struct env set) {
  env = set;
}

void env_setDebug(bool on) {
  debug_on = on;
}

void lineno_setLoc(uintmax_t *loc) {
  env.lastLineno = loc;
}

void env_setFilename(char *name) {
  env.filename = name;
}
