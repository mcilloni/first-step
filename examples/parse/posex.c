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

#include <lex/lex.h>
#include <list/list.h>
#include <utils/utils.h>
#include <utils/env.h>

#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern size_t expr_findLowPriorityOp(List *expr);
extern void expr_fixMinus(List *expr);

void printlist(List *list) {
  bool inside = false;
  fputs("[ ", stdout);
  size_t len = list_len(list);
  struct token *tok;

  for (uint8_t i = 0; i < len; ++i) {
    tok = *list_get(list, i);

    if (inside) {
      fputs(", ",stdout);
    } else {
      inside = true;
    }

    fputs(token_str(tok), stdout);

    switch (tok->type) {
    case LEX_ID:
      printf(": %s", (char*) tok->value);
      break;

    case LEX_NUMBER:
      printf(": %" PRIuMAX, tok->value);
      break;

    default:
      break;
    }
  }
  puts(" ]");
}
 
int main(void) {
  char *toLex = "-5-6-7-8";

  printf("Expr: %s\n", toLex);
  
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__)       
  FILE *crappyWin = fopen("crappyTempFile.win32crap", "w"); 
  fputs(toLex, crappyWin);                                                                                                                   
  fclose(crappyWin);                                                                                                                          
  fflush(crappyWin);                                                                                                                         
  freopen(NULL, "r", crappyWin);                                                                                                              
  struct lexer *lex = lexer_fromFile(crappyWin);                                                                                               
#else                                                                                                                                         
  struct lexer *lex = lexer_fromFile(fmemopen(toLex, strlen(toLex), "r")); 
#endif

  if (lex->errcode) {
    env.fail("Cannot init lexer, errcode=%d", lex->errcode);
  }

  List *list = list_new();

  struct token *tok;

  while((tok = token_get(lex))) {
    list_append(list, tok);        
  } 

  int ret = EXIT_SUCCESS;

  if (lexer_error(lex)) {
    env.error("Errors during lexing of file");
    ret = EXIT_FAILURE;
  }

  printlist(list);

  size_t pos = expr_findLowPriorityOp(list);

  printf("Lowest rightest low priority op: %zu (%s)\n", pos, token_str(*list_get(list, pos)));

  puts(token_str((struct token*) *list_get(list, pos)));

  expr_fixMinus(list);

  printlist(list);

  lexer_close(lex);

  list_free(list);

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__)                                                                
  remove("crappyTempFile.win32crap");                                                                                       
#endif

  return ret;
}
