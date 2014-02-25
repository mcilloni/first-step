#include "utils.h"

#include <ctype.h>
#include <string.h>

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

const char* represent(enum token_type tok) {
  switch (tok) {
  case NONE:
    return "NONE";
  case ASSIGN:
    return "ASSIGN";
  case DIFFERENT:
    return "DIFFERENT";
  case DIV:
    return "DIV";
  case ENDENTRY:
    return "ENDENTRY";
  case ENDIF:
    return "ENDIF";
  case ENTRY:
    return "ENTRY";
  case EQUAL:
    return "EQUAL";
  case ID:
    return "ID";
  case IF:
    return "IF";
  case INC:
    return "INC";
  case MAJOR:
    return "MAJOR";
  case MINOR:
    return "MINOR";
  case MINUS:
    return "MINUS";
  case NEWLINE:
    return "NEWLINE";
  case NOT:
    return "NOT";
  case NUMBER:
    return "NUMBER";
  case PLUS:
    return "PLUS";
  case TIMES:
    return "TIMES";
  case VAR:
    return "VAR";
  default:
    return "UNKNOWN";
  }

  return "";
}
