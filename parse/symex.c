#include "symbols.h"

#include <stdio.h>

int main(void) {
    Symbols *symt = symbols_new();

    if (symbols_register(symt, "potato", "int8") == SYM_ADDED) {
        puts("Added symbol");
    }

    if (symbols_defined(symt, "potato")) {
        puts("Yes, symbol is defined");
    }

    if (symbols_register(symt, "potato", "int8") == SYM_EXISTS) {
        puts("Symbol exists,  yippie");
    }

    symbols_register(symt, "puree", "int8");
    symbols_register(symt, "ponye", "string");

    const char *t;
    if((t = symbols_getType(symt, "ponye"))) {
        printf("ponye is of type %s\n", t);
    }

    symbols_free(symt);

}
