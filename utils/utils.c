#include "utils.h"

#include <ctype.h>
#include <string.h>

#include <jemalloc/jemalloc.h>

bool isStrAlnum(const char *str) {
  size_t len = strlen(str);

  for(size_t i = 0; i < len; ++i) {
    if(!isalnum(str[i])) {
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
  bool firstDigit = isdigit(first);

  if(first != '-' && first != '+' && !firstDigit) {
    return false;
  }

  size_t i;

  for (i = 1; i < len; ++i) {
    if(!isdigit(str[i])) {
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

