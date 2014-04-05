#if !defined(_SYMBOLS_H)
#define _SYMBOLS_H

#include "../treemap/map.h"

#include <stdbool.h>

struct type;
typedef StringMap Symbols;

struct symbol {
  bool decl;
  struct type *type;
};

enum symbols_resp {
  SYM_ADDED,
  SYM_EXISTS
};

extern Symbols* (*symbols_new) (void);
enum symbols_resp symbols_register(Symbols *symt, const char *id, struct type *type, bool decl);
extern bool (*symbols_defined)(Symbols *symt, const char *id);
void symbols_dump(Symbols *symt, const char *title, int8_t depth);
struct symbol* symbols_get(Symbols *symt, const char *id);
void symbols_free(Symbols *symt);

#endif
