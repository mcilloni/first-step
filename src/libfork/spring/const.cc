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

#include <cinttypes>
#include <cstdio>

extern "C" {

auto stderr_file(void) -> FILE* {
  return stderr;
}

auto stdin_file(void) -> FILE* {
  return stdin;
}

auto stdout_file(void) -> FILE* {
  return stdout;
}

auto outint(uint64_t n) -> void {
  printf("%" PRIu64, n);
}

}
