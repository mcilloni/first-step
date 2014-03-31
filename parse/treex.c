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

#include "ptree.h"

#include <stdio.h>
#include <stdlib.h>

int main(void) {
  struct pnode *st = pnode_new(PR_PROGRAM);
  pnode_addLeaf(st, pnode_newval(PR_NUMBER, 33));
  struct pnode *en = pnode_new(PR_ENTRY);
  pnode_addLeaf(st, en);

  struct pnode *plus = pnode_newval(PR_UNOP, LEX_PLUS); 
  pnode_addLeaf(en, plus);

  enum symbols_resp resp;
  struct type *int8 = type_getBuiltin("int8");
  struct type *int16 = type_getBuiltin("int16");

  if (pnode_addSymbol(plus, "potato", int8, &resp)) {
    puts((resp == SYM_ADDED) ? "Added" : "Exists");
  }
  
  if (pnode_addSymbol(st, "motato", int16, &resp)) {
    puts((resp == SYM_ADDED) ? "Added" : "Exists");
  }

  if (pnode_addSymbol(plus, "potato", int8, &resp)) {
    puts((resp == SYM_ADDED) ? "Added" : "Exists");
  }

  puts(pnode_symbolType(plus, "motato")->name);

  puts(pnode_isInCurrentScope(plus, "motato") ? "Yup" : "Nope");

  pnode_free(st);

  return EXIT_SUCCESS;
}
