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

#include "types.h"

#include <stdio.h>
#include <stdlib.h>

const char* represent(enum type_compatible t) {
  switch (t) {
  case TYPECOMP_NO:
    return "no";
  case TYPECOMP_SMALLER:
    return "yes, but smaller";
  case TYPECOMP_YES:
    return "yes";
  default:
    return "don't know";
  }
}

int main(void) {
  struct type *first = type_getTypeDef(NULL, "int8");
  struct type *second = type_getTypeDef(NULL, "int32");

  printf("%s and %s are compatible? %s.\n", first->name, second->name, represent(type_areCompatible(first, second)));
  printf("%s and %s are compatible? %s.\n", second->name, first->name, represent(type_areCompatible(second, first)));

  return EXIT_SUCCESS;
}
