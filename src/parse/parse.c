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

#include <lex/lex.h>
#include <utils/env.h>
#include <utils/utils.h>

#include <treemap/map.h>

#include <errno.h>
#include <stdlib.h>
#include <string.h>

struct pnode* body(struct parser *prs, struct pnode *this, bodyender be);
extern struct pnode* expr(struct parser *prs, struct pnode *root, bodyender be);
struct type* type(struct parser *prs, struct pnode *this);

void parser_loadToken(struct parser *prs) {
  prs->nextTok = token_get(prs->lex);
  
  if (prs->nextTok) {
    if (lexer_eof(prs->lex)) {
      prs->nextTok = NULL;
    }
    if (lexer_error(prs->lex)) {
      env.fail("Error reading file: %s", strerror(errno));
    }
  }
}

struct token* parser_getTok(struct parser *prs) {

  struct token *tok;
  tok = prs->nextTok;
  prs->nextTok = token_get(prs->lex);

  if (tok) {
    prs->lastLineno = tok->lineno;
  }

  if (!tok) {
    if (lexer_eof(prs->lex)) {
      return NULL;
    }
    if (lexer_error(prs->lex)) {
      env.fail("Error reading file: %s", strerror(errno));
    }
  }

  if (prs->curTok) {
    prs->precTok = prs->curTok;
  } else {
    prs->precTok = tok;
  }

  prs->curTok = tok;

  return tok;
}

struct type* idType(struct parser *prs, struct pnode *this) {
  struct token *type = parser_getTok(prs);
  
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

struct type* funcType(struct parser *prs, struct pnode *this) {

  parser_getTok(prs); //discard 'func'

  struct token *tok = parser_getTok(prs);

  if (!tok) {
    env.fail("Unexpected EOF in func type, expected '('");
  }

  if (tok->type != LEX_OPAR) {
    env.fail("Unexpected %s in func type, expected '('", token_str(tok));
  }

  Array *arr = array_new(3U);
  struct type *tp;
  struct type *retType = type_none;
  
  do {

    tp = type(prs, this);
    if (tp) {
      array_append(arr, (void*) tp);  
    }

    tok = parser_getTok(prs);

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
  } while (tp);

endparms:

  if (prs->nextTok && (prs->nextTok->type == LEX_ID || prs->nextTok->type == LEX_FUNC || prs->nextTok->type == LEX_PTR || prs->nextTok->type == LEX_VAL)) {
    retType = type(prs, this);    
  }

  return type_makeFuncType(prs->types, retType, arr);

}

Pair* argTypeList(struct parser *prs, struct pnode *this) {
  Pair *ret = malloc(sizeof(Pair));
  Array *names = array_new(3U);
  struct token *tok;
  bool first = true;

  do {
    tok = parser_getTok(prs);

    if (!tok) {
     env.fail("Unexpected EOF, expected an identifier");
    }

    if (!first) {
      if (tok->type != LEX_COMMA) {
        env.fail("Unexpected %s, expected an identifier", token_str(tok));
      }

      tok = parser_getTok(prs);

      if (!tok) {
       env.fail("Unexpected EOF, expected an identifier");
      }

    }

    if (tok->type != LEX_ID) {
      env.fail("Unexpected %s, expected an identifier", token_str(tok));
    }

    array_append(names, str_clone((char*) tok->value));

    first = false;
  } while (prs->nextTok->type == LEX_COMMA);

  struct type *tp = type(prs, this);

  if (tp->kind == TYPE_ALIAS) {
    env.fail("Cannot instantiate non-specified type %s", tp->name);
  }

  *ret = (Pair) { tp, names };

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

Symbols* argParams(struct parser *prs, struct pnode *this) {
  Symbols *ret = symbols_new();

  bool first = true;
  struct token *tok;
  while (prs->nextTok->type != LEX_CPAR && prs->nextTok->type != LEX_NEWLINE) {
    if (first) {
      first = false;
    } else {
      tok = parser_getTok(prs);

      if (tok->type != LEX_COMMA) {
        env.fail("Unexpected %s in function declaration, expected ','", token_str(tok));
      }
    }

    if (prs->nextTok->type == LEX_NEWLINE) {

      parser_getTok(prs); //allow multiline parameter declarations

    }

    symPairAdd(ret, argTypeList(prs, this));
    
  }

  return ret;
}

Symbols* structDef(struct parser *prs, struct pnode *root) {
  struct token *tok = parser_getTok(prs);

  if (!tok) {
    env.fail("Unexpected EOF, expected '('");
  }

  if (tok->type != LEX_OPAR) {
    env.fail("Unexpected token %s, expected '('", token_str(tok));
  }

  if (prs->nextTok->type == LEX_NEWLINE) {
    parser_getTok(prs); //discard newline after (
  }

  bool notEmpty = prs->nextTok && prs->nextTok->type != LEX_CPAR;
  Symbols *syms = argParams(prs, root);

  if (notEmpty) {

    if (prs->nextTok->type == LEX_NEWLINE) {
      parser_getTok(prs); //discard newline before )
    }

  } 
  
  tok = parser_getTok(prs);

  if (!tok) {
    env.fail("Unexpected EOF, expected ')'");
  }

  if (tok->type != LEX_CPAR) {
    env.fail("Unexpected token %s, expected ')'", token_str(tok));
  }

  return syms;
}

struct type* structType(struct parser *prs, struct pnode *this) {
  parser_getTok(prs); //discard 'struct'
  
  return type_makeStructType(prs->types, structDef(prs, this));
}

struct type* type(struct parser *prs, struct pnode *this) {
  if (!prs->nextTok) {
    env.fail("Unexpected EOF, expected a type");
  }

  switch (prs->nextTok->type) {
  case LEX_FUNC: {
    return funcType(prs, this);
  }

  case LEX_STRUCT: {
    return structType(prs, this);
  }

  case LEX_OBRAC: {
    parser_getTok(prs); 
    struct token *tok = parser_getTok(prs);

    if (!tok) {
      env.fail("Unexpected EOF, expected an integer");
    }

    if (tok->type != LEX_NUMBER) {
      env.fail("Unexpected %s, expected a number", token_str(tok));
    }

    size_t size = (size_t) tok->value;

    tok = parser_getTok(prs);

    if (!tok) {
      env.fail("Unexpected EOF, expected ']'");
    }

    if (tok->type != LEX_CBRAC) {
      env.fail("Unexpected %s, expected ']'", token_str(tok));
    }

    return type_makeArray(prs->types, type(prs, this), size);
  }

  case LEX_PTR: {
    parser_getTok(prs);
    return type_makePtr(prs->types, type(prs, this));
  }

  case LEX_VAL: {
    parser_getTok(prs); //discard the meaningless 'val' 
    return type(prs, this);
  }

  default: { 
    if ((prs->nextTok->type != LEX_ID)) {
      return NULL;
    }
    return idType(prs, this);
  }
  }
}

bool nextIsEndEofAware(struct token *tok) {
  if (!tok || tok->type == LEX_NEWLINE) {
    return true;
  }

  return false;
}

struct pnode* varDeclGeneric(struct parser *prs, struct pnode *this, bool decl) {

  struct token *tok = parser_getTok(prs);

  if (!tok) {
    env.fail("Unexpected eof, expected an identifier");
  }  

  if (tok->type != LEX_ID) {
    env.fail("Unexpected token %s, expected an identifier", token_str(tok));
  }

  const char *id = (const char*) tok->value;

  if (symbol_getBuiltin(id)) {
    env.fail("%s is a reserved identifier", id);
  }

  struct type *tp = NULL;
  
  if (!prs->nextTok) {
    env.fail("Unexpected EOF, expected '=' or a type");
  }

  struct pnode *extra = NULL;

  if (prs->nextTok->type != LEX_ASSIGN) {
    tp = type(prs, this); //side effect will change nextTok, so I should stop changing the line below with an else breaking everything
    if (!tp) {
      char buf[2048];
      env.fail("Cannot declare symbol %s of not defined type %s", id, type_str(tp, buf, 2048));
    }
  }

  struct type *assType;

  if (prs->nextTok->type == LEX_ASSIGN) {
    if (decl) {
      env.fail("Cannot assign on declaration to a decl variable");
    }

    parser_getTok(prs); //discard '='
    bodyender be = NULL;
    if (this->id == PR_ROOT) {
      be = nextIsEndEofAware;
    }

    extra = expr(prs, this, be);
    assType = pnode_evalType(prs->types, extra, this);

    if (!tp) {
      tp = assType;
    } else {
      switch (type_areCompatible(tp, assType)) {
      case TYPECOMP_NO: {
        char buf[4096], cuf[4096];
        env.fail("Cannot assign expression of type %s to type %s", type_str(assType, cuf, 4096), type_str(tp, buf, 4096));
        break;
      }
      case TYPECOMP_SMALLER: {
        char buf[4096], cuf[4096];
        env.warning("Assigning expression of type %s to smaller type %s", type_str(assType, cuf, 4096), type_str(tp, buf, 4096));
        break;
      }
      default:
        break;
      }
    }
  }    

  struct type *declType = pnode_symbolType(this,id);

  enum symbols_resp resp;
  bool res = false;
  
  if (extra) {
    res = pnode_addSymbolAndInit(this, id, tp, extra, &resp);
  } else {
    bool (*symreg)(struct pnode*, const char*, struct type*, enum symbols_resp*) = decl ? pnode_declSymbol : pnode_addSymbol;

    res = symreg(this, id, tp, &resp);
  }

  if (!res) {
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

  return pnode_newval(PR_DECLARATION, (uintmax_t) str_clone(id));

}

struct pnode* var(struct parser *prs, struct pnode *this) {
  return varDeclGeneric(prs, this, false);
}

struct pnode* decl(struct parser *prs, struct pnode *this) {
  return varDeclGeneric(prs, this, true);
}

void alias(struct parser *prs, struct pnode *this) {

  struct token *tok = parser_getTok(prs);

  if (!tok) {
    env.fail("Unexpected eof, expected an identifier");
  }  

  if (tok->type != LEX_ID) {
    env.fail("Unexpected token %s, expected an identifier", token_str(tok));
  }

  const char *id = (const char*) tok->value;
  
  pnode_alias(this, id, type_makeAlias(prs->types, id));

  struct type *tp = type(prs, this);

  if (!tp) {
    char buf[4096];
    env.fail("Cannot create alias %s of not defined type %s", id, type_str(tp, buf, 4096));
  }

  pnode_alias(this, id, tp);

}

bool ifElseBe(struct token *tok) {
  return tok->type == LEX_ENDIF || tok->type == LEX_ELSE;
}

bool ifBe(struct token *tok) {
  return tok->type == LEX_ENDIF;
}

struct pnode* condStmt(struct parser *prs, struct pnode *root, enum nonterminals nt, bodyender be) {

  struct pnode* ret = pnode_new(nt);

  struct pnode *cond = expr(prs, root, NULL);
  struct type *condType = pnode_evalType(prs->types, cond, root);

  if (condType != type_getBuiltin("bool")) {
    char buf[4096];
    env.fail("Expected expression of bool type in statement condition, got %s", type_str(condType, buf, 4096));
  }

  pnode_addLeaf(ret, cond);

  struct token *tok = parser_getTok(prs);

  if (!tok) {
    env.fail("Unexpected end of file, expected a new line");
  }

  if (tok->type != LEX_NEWLINE) {
    env.fail("Unexpected token, got %s, expected a new line", token_str(tok));
  }

  ret->root = root;

  pnode_addLeaf(ret, body(prs, ret, be));

  return ret;
}

bool whileBe(struct token *tok) {
  return tok->type == LEX_ENDWHILE;
}

struct pnode* whileStmt(struct parser *prs, struct pnode *root) {
  bool wasInBreakableBody = prs->inBreakableBody;
  prs->inBreakableBody = true;

  struct pnode *ret = condStmt(prs, root, PR_WHILE, whileBe);

  struct token *tok = parser_getTok(prs);

  if (!tok) {
    env.fail("Unexpected end of file, expected '/while'");
  }

  if (tok->type != LEX_ENDWHILE) {
    env.fail("Unexpected token, got %s, expected '/while'", token_str(tok));
  }

  prs->inBreakableBody = wasInBreakableBody;

  return ret;
}

struct pnode* ifStmt(struct parser *prs, struct pnode *root) {

  struct pnode *ret = condStmt(prs, root, PR_IF, ifElseBe);

  struct token *tok = parser_getTok(prs);

  if (!tok) {
    env.fail("Unexpected end of file, expected '/if' or 'else'");
  }

  if (tok->type == LEX_ELSE) {
    ret->id = PR_IFELSE;
    
    tok = parser_getTok(prs);

    if (!tok) {
      env.fail("Unexpected end of file, expected a new line");
    }

    if (tok->type != LEX_NEWLINE) {
      env.fail("Unexpected token, got %s, expected a new line", token_str(tok));
    }

    pnode_addLeaf(ret, body(prs, ret, ifBe));
    
    tok = parser_getTok(prs);
  }

  if (tok->type != LEX_ENDIF) {
    env.fail("Unexpected token, got %s, expected '/if'", token_str(tok));
  }

  return ret;

}

struct pnode* returnStmt(struct parser *prs, struct pnode *root) {

  parser_getTok(prs); //scrap 'return' token

  struct type *rType = pnode_funcReturnType(root);

  if (!rType) {
    env.fail("return outside of a function");
  }

  if (!prs->nextTok) {
    env.fail("Unexpected EOF");
  }

  struct pnode *exprNode = (prs->nextTok->type == LEX_NEWLINE) ? expr_empty : expr(prs, root, NULL);

  struct type *type = pnode_evalType(prs->types, exprNode, root);

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

struct pnode* stmt(struct parser *prs, struct pnode *root) {

  if (!prs->nextTok) {
    env.fail("Unexpected end of file, expected a statement");
  }

  struct pnode *ret;

  struct token savedNext = *prs->nextTok;

  switch (prs->nextTok->type) {
  case LEX_BREAK: 
    parser_getTok(prs); //discard
    if (!prs->inBreakableBody) {
      env.fail("break outside of a breakable body");
    }

    ret = pnode_new(PR_BREAK);
    break;

  case LEX_CONTINUE:
    parser_getTok(prs); //discard
    if (!prs->inBreakableBody) {
      env.fail("continue outside of a breakable body");
    }

    ret = pnode_new(PR_CONTINUE);
    break;

  case LEX_DECL: {
    parser_getTok(prs); //discard
    ret = decl(prs, root);
    break;
  }

  case LEX_IF: {
    parser_getTok(prs); //discard
    ret = ifStmt(prs, root);
    break;
  }

  case LEX_RETURN: 
    ret = returnStmt(prs, root);
    break;  

  case LEX_VAR: {
    parser_getTok(prs); //discard
    ret = var(prs, root);
    break;
  }

  case LEX_WHILE: {
    parser_getTok(prs); //discard 
    ret = whileStmt(prs, root);
    break;
  }

  default: {
    if (!(ret = expr(prs, root, NULL))) {
      env.fail("Unexpected token %s, expected 'var', 'if' or anything evaluable as an expression", token_str(&savedNext));
    }
  }
  }

  struct token *tok = parser_getTok(prs);

  if (tok->type != LEX_NEWLINE) {
    env.fail("Unexpected token, got %s, expected a new line", token_str(tok));
  }

  ret->startLine = savedNext.lineno;
  ret->endLine = prs->precTok->lineno;

  return ret;

}

struct pnode* body(struct parser *prs, struct pnode *root, bodyender be) {

  struct pnode *ret = pnode_new(PR_BODY);

  ret->root = root;

  struct pnode *nextStmt;

  while(!be(prs->nextTok)) {
    nextStmt = stmt(prs, ret);
    pnode_addLeaf(ret, nextStmt);
  }

  return ret;

}

bool entryBe(struct token *tok) {
  return tok->type == LEX_ENDENTRY;
}

bool funcBe(struct token *tok) {
  return tok->type == LEX_ENDFUNC;
}

void funcCommons(struct parser *prs, struct pnode *this, bodyender be) {

  pnode_addLeaf(this, body(prs, this, be));

  struct token *tok = parser_getTok(prs);

  if (!be(tok)) {
    env.fail("Unexpected token found: got %s, expected end of function (or entry)", token_str(tok)); 
  }

}

struct pnode* entry(struct parser *prs, struct pnode *root) {
  struct token *tok = parser_getTok(prs);

  if (!tok) {
    env.fail("Unexpected end of file in function");
  }
  
  if (tok->type != LEX_NEWLINE) {
    env.fail("Unexpected token found: got %s, expected a new line after entry definition", token_str(tok));
  }

  struct pnode *ret = pnode_newfunc(prs->types, PR_FUNC, "__helm_entry", type_none, symbols_new());

  ret->root = root; //in this case, this is needed

  funcCommons(prs, ret, entryBe);

  return ret;
}

struct pnode* func(struct parser *prs, struct pnode *root) {

  struct token *tok = parser_getTok(prs);

  if (!tok) {
    env.fail("Unexpected EOF, expected identifier");
  }

  if (tok->type != LEX_ID) {
    env.fail("Unexpected token %s, expected an identifier", token_str(tok));
  }

  char *fName = str_clone((char*) tok->value);

  Symbols *syms = structDef(prs, root);

  if (!prs->nextTok) {
    env.fail("Unexpected EOF, expected type or a newline");
  }

  struct type *rType = type_none;

  switch(prs->nextTok->type) {
  case LEX_ID:
  case LEX_FUNC:
  case LEX_PTR:
  case LEX_STRUCT:
  case LEX_VAL:
    rType = type(prs, root);
    break;
  case LEX_NEWLINE:
    break;
  default:
    env.fail("Unexpected %s, expected type or a new line", token_str(prs->nextTok));
    break;
  }

  parser_getTok(prs); //discard new line

  struct pnode *ret = pnode_newfunc(prs->types, PR_FUNC, fName, rType, syms);

  ret->root = root;

  free(fName);

  funcCommons(prs, ret, funcBe);
  return ret;
  
}

struct pnode declaration_fake_node = {0};

struct pnode* definition(struct parser *prs, struct pnode *root) {
  struct token *tok = parser_getTok(prs);

  if (!tok) {
    return NULL;
  }

  enum token_type type = tok->type;
  struct pnode *ret = NULL;

  switch (type) {
  case LEX_ALIAS:
    alias(prs, root);
    ret = &declaration_fake_node;
    break;
  case LEX_DECL:
    ret = decl(prs, root);
    break;
  case LEX_ENTRY:
    ret = entry(prs, root);
    break;
  case LEX_FUNC:
    ret = func(prs, root);
    break;
  case LEX_VAR:
    ret = var(prs, root);
    break;
  default: 
    env.fail("Unexpected token found: got %s, expected 'decl', 'entry', 'func', 'var'", token_str(tok));
    break;
  }

  ret->startLine = tok->lineno;

  tok = parser_getTok(prs);

  if (tok && tok->type != LEX_NEWLINE) {
    env.fail("Got token %s, expected a newline or EOF", token_str(tok));
  }

  if (prs->precTok) {
    ret->endLine = prs->precTok->lineno;
  } else {
    ret->endLine = prs->lastLineno;
  }

  return ret;
}

struct pnode* parser_parse(struct parser *prs, FILE *file) {
  lineno_setLoc(&prs->lastLineno);

  prs->lex = lexer_fromFile(file);

  parser_loadToken(prs);

  if (!prs->nextTok) {
    return NULL;
  }

  char *module;

  if (prs->nextTok->type == LEX_MODULE) {
    parser_getTok(prs); //discard 'module'

    struct token *tok = parser_getTok(prs);

    if (!tok) {
      env.fail("Unexpected EOF after 'module'");
    }

    if (tok->type != LEX_ID) {
      env.fail("Unexpected %s, expected an identifier", token_str(tok));
    }

    module = str_clone((char*) tok->value);

    tok = parser_getTok(prs);

    if (!tok) {
      env.fail("Unexpected EOF after module declaration");
    }

    if (tok->type != LEX_NEWLINE) {
      env.fail("Unexpected %s, expected a newline", token_str(tok));
    }
  } else {
    module = str_clone(""); //should be freeable
  }

  struct pnode *root = pnode_newroot(module), *nextDef;  

  while ((nextDef = definition(prs, root))) {
    if (nextDef != &declaration_fake_node) {
      pnode_addLeaf(root, nextDef);    
    }
  }

  if (!array_len(root->leaves)) {
    pnode_free(root);
    return NULL;
  }

  lexer_close(prs->lex);

  return root;
}

