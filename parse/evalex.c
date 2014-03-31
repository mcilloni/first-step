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

#include "../lex/lex.h"

#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>

extern struct pnode* expr_evalBinary(struct token*,struct pnode*,struct pnode*);
extern struct pnode* expr_evalUnary(struct token*,struct pnode*);

int main(void) {
  struct pnode *val1 = pnode_newval(PR_NUMBER, 33);
  struct pnode *val2 = pnode_newval(PR_NUMBER, 22);

  struct token tok = {LEX_PLUS};

  struct pnode *res = expr_evalBinary(&tok, val1, val2);
  printf("%" PRIuMAX "\n", pnode_getval(res));
  pnode_free(res);
  
  tok.type = LEX_MINUS;

  res = expr_evalUnary(&tok, val1);
  printf("%" PRIdPTR "\n", (intptr_t) pnode_getval(res));
  pnode_free(res);

  tok.type = LEX_NOT;

  res = expr_evalUnary(&tok, val1);
  printf("%" PRIdPTR "\n", (intptr_t) pnode_getval(res));
  pnode_free(res);
  
  tok.type = LEX_MAJOR;

  res = expr_evalBinary(&tok, val1, val2);
  printf("%" PRIdPTR "\n", (intptr_t) pnode_getval(res));
  pnode_free(res);
  
  pnode_free(val1);
  pnode_free(val2);

  return EXIT_SUCCESS;
}

