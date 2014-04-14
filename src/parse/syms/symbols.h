#if !defined(_SYMBOLS_H)
#define _SYMBOLS_H

#include <list/list.h>

#include <stdbool.h>

struct type;
typedef List Symbols;

struct symbol {
  bool decl;
  struct type *type;
};

struct spair {
  const char *id;
  struct symbol *sym;
};

enum symbols_resp {
  SYM_ADDED,
  SYM_EXISTS
};

extern Symbols* (*symbols_new) (void);
enum symbols_resp symbols_register(Symbols *symt, const char *id, struct type *type, bool decl);
extern size_t (*symbols_len)(Symbols*);
bool symbols_defined(Symbols *symt, const char *id);
void symbols_dump(Symbols *symt, const char *title, int8_t depth);
struct symbol* symbols_get(Symbols *symt, const char *id);
void symbols_free(Symbols *symt);

struct symbol* symbol_getBuiltin(const char *id);

#endif

