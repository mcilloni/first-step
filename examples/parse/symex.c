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

#include "symbols.h"
#include "types.h"

#include <stdio.h>

int main(void) {
    Symbols *symt = symbols_new();
    struct type *int8 = type_getBuiltin("int8");    
    struct type *int16 = type_getBuiltin("int16");

    if (symbols_register(symt, "potato", int8, false) == SYM_ADDED) {
        puts("Added symbol");
    }

    if (symbols_defined(symt, "potato")) {
        puts("Yes, symbol is defined");
    }

    if (symbols_register(symt, "potato", int8, false) == SYM_EXISTS) {
        puts("Symbol exists,  yippie");
    }

    symbols_register(symt, "puree", int8, false);
    symbols_register(symt, "ponye", int16, false);

    struct symbol *t;
    if((t = symbols_get(symt, "ponye"))) {
        printf("ponye is of type %s\n", t->type->name);
    }

    symbols_free(symt);

}
