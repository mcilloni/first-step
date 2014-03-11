#if !defined(_SYMBOLS_H)
#define _SYMBOLS_H

#include "../treemap/map.h"

#include <stdbool.h>

struct symbol {
  const char *id;
  const char *type;
};

enum symbols_resp {
  SYM_ADDED,
  SYM_EXISTS
};

typedef StringMap Symbols;


extern Symbols* (*symbols_new) (void);
enum symbols_resp symbols_register(Symbols *symt, const char *id, const char *type);
extern bool (*symbols_defined)(Symbols *symt, const char *id);
const char* symbols_getType(Symbols *symt, const char *id);
extern void (*symbols_free)(Symbols *symt);

#endif
