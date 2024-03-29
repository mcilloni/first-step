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

#include <utils/lines.h>
#include <utils/env.h>

#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[]) {

  if (argc != 2) {
    env.fail("Wrong args");
  }

  enum errors err = NOERR;
  
  struct filereader *fr = filereader_open(argv[1]);
  struct line *line;

  while (!err) {
    line = line_read(fr, &err);
    env.info("%s (%zu == %zu)", line->val, line->len, strlen(line->val));
    line_free(line);
  }

  filereader_close(fr);  
  filereader_free(fr);  
  
  if (err == ERROR) {
    perror("Error");
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;

}

