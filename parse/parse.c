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

#include "parse.h"
#include "../lex/lex.h"
#include "../utils/env.h"
#include "../utils/utils.h"

#include <errno.h>
#include <stdlib.h>
#include <string.h>

static bool firstTok = true;
struct token *nextTok = NULL;

struct pnode* body(struct pnode *this, struct lexer *lex, bodyender be);
extern struct pnode* expr(struct pnode *root, struct lexer *lex, bodyender be);
struct type* type(struct pnode *this, struct lexer *lex);

struct token* token_getOrDie(struct lexer *lex) {

  struct token *tok;
  if (firstTok) {
    nextTok = token_get(lex);
    
    firstTok = false;
  }
  tok = nextTok;
  nextTok = token_get(lex);

  if (!tok) {
    if (lexer_eof(lex)) {
      return NULL;
    }
    if (lexer_error(lex)) {
      env.fail("Error reading file: %s", strerror(errno));
    }
  }

  return tok;
}

//there is no need for declarations to be into the syntax tree, so this placeholder will returned by stmt() and then discarded.
struct pnode declaration_fake_node = {0};

struct type* idType(struct pnode *this, struct lexer *lex) {
  struct token *type = token_getOrDie(lex);
  
  if(!type) {
    env.fail("Unexpected end of file during var declaration");
  }
  
  const char *id = (const char*) type->value;

  struct type *ret = pnode_getType(this, id);

  if (!ret) {
    env.fail("Undefined type %s", id);
  }

  return ret;
}

struct type* funcType(struct pnode *this, struct lexer *lex) {

  token_free(token_getOrDie(lex)); //discard 'func'

  struct token *tok = token_getOrDie(lex);

  if (!tok) {
    env.fail("Unexpected EOF in func type, expected '('");
  }

  if (tok->type != LEX_OPAR) {
    env.fail("Unexpected %s in func type, expected '('", token_str(tok));
  }

  token_free(tok);

  Array *arr = array_new(3U);
  struct type *tp;
  struct type *retType = type_none;
  
  while ((tp = type(this, lex))) {

    array_append(arr, (void*) tp);  

    tok = token_getOrDie(lex);

    if (!tok) {
      env.fail("Unexpected EOF during func type, expected ',' or ')'");
    }

    switch(tok->type) {
    case LEX_COMMA:
      break;
    case LEX_CPAR:
      goto endparms;
    default:
      env.fail("Unexpected token %s, expected ',' or ')'", token_str(tok));
    }
  }

endparms: 


  if (nextTok && (nextTok->type == LEX_ID || nextTok->type == LEX_FUNC)) {
    retType = type(this, lex);    
  }

  return type_makeFuncType(retType, arr);

}

struct type* type(struct pnode *this, struct lexer *lex) {
  if (!nextTok) {
    env.fail("Unexpected EOF, expected a type");
  }

  switch (nextTok->type) {
  case LEX_FUNC: {
    return funcType(this, lex);
  }

  case LEX_PTR: {
    token_free(token_getOrDie(lex));
    return type_makePtr(type(this, lex));
  }

  case LEX_VAL: {
    token_free(token_getOrDie(lex)); //discard the meaningless 'val' 
    return type(this, lex);
  }

  default: { 
    if ((nextTok->type != LEX_ID)) {
      return NULL;
    }
    return idType(this, lex);
  }
  }
}

void varDeclGeneric(struct pnode *this, struct lexer *lex, bool decl) {

  struct token *tok = token_getOrDie(lex);

  if (!tok) {
    env.fail("Unexpected eof, expected an identifier");
  }  

  if (tok->type != LEX_ID) {
    env.fail("Unexpected token %s, expected an identifier", token_str(tok));
  }

  const char *id = (const char*) tok->value;
  struct type *tp = type(this, lex);

  if (!tp) {
    char buf[2048];
    env.fail("Cannot declare symbol %s of not defined type %s", id, type_str(tp, buf, 2048));
  }

  struct type *declType = pnode_symbolType(this,id);

  enum symbols_resp resp;

  bool (*symreg)(struct pnode*, const char*, struct type*, enum symbols_resp*) = decl ? pnode_declSymbol : pnode_addSymbol;

  if (!symreg(this, id, tp, &resp)) {
    env.fail("Internal error, cannot add symbol to table");
  }

  //declType will not be null if symbol is defined somewhere; resp will be SYM_EXISTS only if it is in current scope
  if (declType) {
    if (resp == SYM_EXISTS) {
      env.fail("Error, %s is already defined in this scope with type %s", id, declType->name);
    } else {
      env.info("Declaration hides id %s with type %s", id, declType->name);
    }
  }

  token_free(tok);

}

void var(struct pnode *this, struct lexer *lex) {
  varDeclGeneric(this, lex, false);
}

void decl(struct pnode *this, struct lexer *lex) {
  varDeclGeneric(this, lex, true);
}

bool ifBe(struct token *tok) {
  return tok->type == LEX_ENDIF;
}

struct pnode* ifStmt(struct pnode *root, struct lexer *lex) {

  struct pnode* ret = pnode_new(PR_IF);

  pnode_addLeaf(ret, expr(root, lex, NULL));

  struct token *tok = token_getOrDie(lex);

  if (!tok) {
    env.fail("Unexpected end of file, expected a new line");
  }

  if (tok->type != LEX_NEWLINE) {
    env.fail("Unexpected token, got %s, expected a new line", token_str(tok));
  }

  token_free(tok);

  ret->root = root;

  pnode_addLeaf(ret, body(ret, lex, ifBe));

  tok = token_getOrDie(lex);

  if (!tok) {
    env.fail("Unexpected end of file, expected '/if'");
  }

  if (tok->type != LEX_ENDIF) {
    env.fail("Unexpected token, got %s, expected '/if'", token_str(tok));
  }

  token_free(tok);  

  return ret;

}

struct pnode* returnStmt(struct pnode *root, struct lexer *lex) {

  token_free(token_getOrDie(lex)); //scrap 'return' token

  struct type *rType = pnode_funcReturnType(root);

  if (!rType) {
    env.fail("return outside of a function");
  }

  if (!nextTok) {
    env.fail("Unexpected EOF");
  }

  struct pnode *exprNode = (nextTok->type == LEX_NEWLINE) ? expr_empty : expr(root, lex, NULL);

  struct type *type = pnode_evalType(exprNode, root);

  switch(type_areCompatible(rType, type)) {
  case TYPECOMP_NO:
    env.fail("Type of return expression is incompatible with declaration type");
    break;
  case TYPECOMP_SMALLER:
    env.warning("Returning an expression larger than return type");
    break;
  default:
    break;
  }

  struct pnode *ret = pnode_new(PR_RETURN);

  pnode_addLeaf(ret, exprNode);

  return ret;

}

struct pnode* stmt(struct pnode *root, struct lexer *lex) {

  if (!nextTok) {
    env.fail("Unexpected end of file, expected a statement");
  }

  struct pnode *ret;

  struct token savedNext = *nextTok;

  switch (nextTok->type) {
  case LEX_DECL: {
    token_free(token_getOrDie(lex)); //discard
    decl(root, lex);
    ret = &declaration_fake_node;
    break;
  }
  case LEX_IF: {
    token_free(token_getOrDie(lex)); //discard
    ret = ifStmt(root, lex);
    break;
  }
  case LEX_VAR: {
    token_free(token_getOrDie(lex)); //discard
    var(root, lex);
    ret = &declaration_fake_node;
    break;
  }
  case LEX_RETURN: 
    ret = returnStmt(root, lex);
    break;  
  default: {
    if (!(ret = expr(root, lex, NULL))) {
      env.fail("Unexpected token %s, expected 'var', 'if' or anything evaluable as an expression", token_str(&savedNext));
    }
  }
  }

  struct token *tok = token_getOrDie(lex);

  if (tok->type != LEX_NEWLINE) {
    env.fail("Unexpected token, got %s, expected a new line", token_str(tok));
  }

  token_free(tok);

  return ret;

}

struct pnode* body(struct pnode *root, struct lexer *lex, bodyender be) {

  struct pnode *ret = pnode_new(PR_BODY);

  ret->root = root;

  struct pnode *nextStmt;

  while(!be(nextTok)) {
    nextStmt = stmt(ret, lex);
    if (nextStmt != &declaration_fake_node) {
      pnode_addLeaf(ret, nextStmt);
    }
  }

  return ret;

}

bool entryBe(struct token *tok) {
  return tok->type == LEX_ENDENTRY;
}

bool funcBe(struct token *tok) {
  return tok->type == LEX_ENDFUNC;
}

Pair* funcTypeParams(struct pnode *this, struct lexer *lex) {
  Pair *ret = malloc(sizeof(Pair));
  Array *names = array_new(3U);
  struct token *tok;
  bool first = true;

  do {
    tok = token_getOrDie(lex);

    if (!tok) {
     env.fail("Unexpected EOF, expected an identifier");
    }

    if (!first) {
      if (tok->type != LEX_COMMA) {
        env.fail("Unexpected %s, expected an identifier", token_str(tok));
      }

      token_free(tok);
      tok = token_getOrDie(lex);

      if (!tok) {
       env.fail("Unexpected EOF, expected an identifier");
      }
    }

    if (tok->type != LEX_ID) {
      env.fail("Unexpected %s, expected an identifier", token_str(tok));
    }

    array_append(names, str_clone((char*) tok->value));

    token_free(tok);

    first = false;
  } while (nextTok->type == LEX_COMMA);

  *ret = (Pair) { type(this, lex), names };

  return ret;
}

void symPairAdd(Symbols *syms, Pair *pair) {
  
  struct type *type = (struct type*) pair->key;
  Array *names = (Array*) pair->value;
  size_t len = array_len(names); 

  for (size_t i = 0; i < len; ++i) {
    if (symbols_register(syms, (char*) *array_get(names, i), type, false) != SYM_ADDED) {
      env.fail("Function parameter %s already defined", (char*) array_get(names, i));
    }
  }

  pair_free(pair);
}

Symbols* funcList(struct pnode *this, struct lexer *lex) {
  Symbols *ret = symbols_new();

  bool first = true;
  struct token *tok;
  while (nextTok->type != LEX_CPAR) {
    if (first) {
      first = false;
    } else {
      tok = token_getOrDie(lex);

      if (tok->type != LEX_COMMA) {
        env.fail("Unexpected %s in function declaration, expected ','", token_str(tok));
      }
    }

    symPairAdd(ret, funcTypeParams(this, lex));
    
  }

  return ret;
}

void funcCommons(struct pnode *this, struct lexer *lex, bodyender be) {

  pnode_addLeaf(this, body(this, lex, be));

  struct token *tok = token_getOrDie(lex);

  if (!be(tok)) {
    env.fail("Unexpected token found: got %s, expected end of function (or entry)", token_str(tok)); 
  }

  token_free(tok);

}

struct pnode* entry(struct pnode *root, struct lexer *lex) {
  struct token *tok = token_getOrDie(lex);

  if (!tok) {
    env.fail("Unexpected end of file in function");
  }
  
  if (tok->type != LEX_NEWLINE) {
    env.fail("Unexpected token found: got %s, expected a new line after entry definition", token_str(tok));
  }

  token_free(tok);

  struct pnode *ret = pnode_newfunc(PR_FUNC, "__helm_entry", type_none, symbols_new());

  ret->root = root; //in this case, this is needed

  funcCommons(ret, lex, entryBe);

  return ret;
}

struct pnode* func(struct pnode *root, struct lexer *lex) {

  struct token *tok = token_getOrDie(lex);

  if (!tok) {
    env.fail("Unexpected EOF, expected identifier");
  }

  if (tok->type != LEX_ID) {
    env.fail("Unexpected token %s, expected an identifier", token_str(tok));
  }

  char *fName = str_clone((char*) tok->value);

  token_free(tok);

  tok = token_getOrDie(lex);

  if (!tok) {
    env.fail("Unexpected EOF, expected '('");
  }

  if (tok->type != LEX_OPAR) {
    env.fail("Unexpected token %s, expected '('", token_str(tok));
  }

  token_free(tok);
  Symbols *syms = NULL; 

  if (nextTok && nextTok->type != LEX_CPAR) {
    syms = funcList(root, lex);
  } 
  
  tok = token_getOrDie(lex);

  if (!tok) {
    env.fail("Unexpected EOF, expected ')'");
  }

  if (tok->type != LEX_CPAR) {
    env.fail("Unexpected token %s, expected ')'", token_str(tok));
  }

  token_free(tok);

  if (!nextTok) {
    env.fail("Unexpected EOF, expected type or a newline");
  }

  struct type *rType = type_none;

  switch(nextTok->type) {
  case LEX_ID:
  case LEX_FUNC:
    rType = type(root, lex);
    break;
  case LEX_NEWLINE:
    break;
  default:
    env.fail("Unexpected %s, expected type or a new line", token_str(nextTok));
    break;
  }

  token_free(token_getOrDie(lex)); //discard new line

  struct pnode *ret = pnode_newfunc(PR_FUNC, fName, rType, syms);

  ret->root = root;

  free(fName);

  funcCommons(ret, lex, funcBe);
  return ret;
  
}

struct pnode* definition(struct pnode *root, struct lexer *lex) {
  struct token *tok = token_getOrDie(lex);

  if (!tok) {
    return NULL;
  }

  enum token_type type = tok->type;
  struct pnode *ret = NULL;

  token_free(tok);

  switch (type) {
  case LEX_DECL:
    decl(root, lex);
    ret = &declaration_fake_node;
    break;
  case LEX_ENTRY:
    ret = entry(root, lex);
    break;
  case LEX_FUNC:
    ret = func(root, lex);
    break;
  case LEX_VAR:
    var(root, lex);
    ret = &declaration_fake_node;
    break;
  default: 
    env.fail("Unexpected token found: got %s, expected 'decl', 'entry', 'func', 'var'", token_str(tok));
    break;
  }

  tok = token_getOrDie(lex);

  if (tok && tok->type != LEX_NEWLINE) {
    env.fail("Got token %s, expected a newline or EOF", token_str(tok));
  }

  token_free(tok);

  return ret;
}

struct pnode* parse(const char *filename) {

  struct pnode *program = pnode_new(PR_PROGRAM), *nextDef;  
  struct lexer *lex = lexer_open(filename);

  while ((nextDef = definition(program, lex))) {
    if (nextDef != &declaration_fake_node) {
      pnode_addLeaf(program, nextDef);    
    }
  }

  if (!array_len(program->leaves)) {
    pnode_free(program);
    return NULL;
  }

  lexer_close(lex);

  return program;
}

