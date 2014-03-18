#include "ptree.h"

#include "../list/list.h"
#include "../lex/lex.h"
#include "../utils/env.h"

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>

extern struct pnode* expr(struct pnode*, struct lexer*);
extern struct token* token_getOrDie(struct lexer*);
extern struct token *next;

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
      printf(": %ld", tok->value);
      break;

    default:
      break;
    }
  }
  puts(" ]");
}                      

void printtree(struct pnode *root) {
  if (!root) {
    return;
  }

  struct token tok;
  uintptr_t val = pnode_getval(root);

  tok.type = (enum token_type) val;

  switch (root->id) {
  case PR_BINOP:
    if (array_len(root->leaves) != 2) {
      env.fail("Unacceptable len: %lu", array_len(root->leaves));
    }
    
    printtree(*leaves_get(root->leaves, 0));
    printf(" %s ", token_str(&tok));
    printtree(*leaves_get(root->leaves, 1));
    break;

  case PR_UNOP:
    if (array_len(root->leaves) != 1) {
      env.fail("Unacceptable len: %lu", array_len(root->leaves));
    }
    
    printf("%s", token_str(&tok));
    printtree(*leaves_get(root->leaves, 0));
    break;
  case PR_ID:
    fputs((char*) val, stdout);
    break;
  default:
    printf("%ld", (intptr_t) val);
    break;

  }
  
}

int main(int argc, const char *argv[]) {
  char *toLex = "a + 3 - 7\na";

  fputs("Expr: ", stdout);

  fwrite(toLex, sizeof(*toLex), 9, stdout);

  putchar('\n');

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

  if (lexer_error(lex)) {
    env.fail("Errors during lexing of file");
  }

  struct pnode *root = pnode_new(PR_PROGRAM);
  pnode_addSymbol(root, "a", "int8", NULL);
  struct pnode *res = expr(root, lex);
  pnode_addLeaf(root, res);

  printtree(res);

  putchar('\n');

  token_free(next);
  lexer_close(lex);
  pnode_free(root);

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__)
  remove("crappyTempFile.win32crap");
#endif

  return EXIT_SUCCESS;
}