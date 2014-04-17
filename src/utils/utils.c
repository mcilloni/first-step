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

#include "utils.h"

#include <ctype.h>
#include <stdlib.h>
#include <string.h>



bool isStrAlnum(const char *str) {
  size_t len = strlen(str);

  for(size_t i = 0; i < len; ++i) {
    if(!isalnum((int) str[i])) {
      return false;
    }
  }

  return true;

}

bool isStrNum(const char *str) {
  size_t len = strlen(str);
  
  if (!len) {
    return false;
  }

  char first = *str;
  bool firstDigit = isdigit((int) first);

  if(first != '-' && first != '+' && !firstDigit) {
    return false;
  }

  size_t i;

  for (i = 1; i < len; ++i) {
    if(!isdigit((int) str[i])) {
      return false;
    }
  }

  return (i != 1) || firstDigit;

}

char* str_clone(const char *str) {
    size_t len = strlen(str);
    char *new = malloc((len + 1) * sizeof(char));
    return strcpy(new, str);
}

