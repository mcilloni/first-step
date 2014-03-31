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

#include "lex.h"
#include "../utils/utils.h"
#include "../utils/env.h"

#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(void) {
  struct lexer *lex = lexer_open("../base.helm");

  if (lex->errcode) {
    env.fail("Cannot init lexer, errcode=%d", lex->errcode);
  }

  struct token *tok;

  while((tok = token_get(lex))) {
    
    fputs(token_str(tok), stdout);

    switch (tok->type) {
    case LEX_ID:
      printf(": %s\n", (char*) tok->value);
      break;

    case LEX_NUMBER:
      printf(": %" PRIuMAX "\n", tok->value);
      break;

    default:
      putchar('\n');
      break;
    }

    token_free(tok);
    
  } 

  int ret = EXIT_SUCCESS;

  if (lexer_error(lex)) {
    env.error("Errors during lexing of file");
    ret = EXIT_FAILURE;
  }

  lexer_close(lex);

  return ret;
}
