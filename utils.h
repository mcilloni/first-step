#if !defined(UTILS_H)
#define UTILS_H

#include "lex.h"

#include <stdbool.h>

bool isStrAlnum(const char *str);
bool isStrNum(const char *str);

const char* represent(enum token_type tok);

#endif
