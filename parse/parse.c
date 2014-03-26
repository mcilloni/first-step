#include "ptree.h"
#include "../lex/lex.h"
#include "../utils/env.h"

#include <errno.h>
#include <string.h>

static bool firstTok = true;
struct token *nextTok = NULL;

typedef bool (*bodyender)(struct token*);
struct pnode* body(struct pnode *this, struct lexer *lex, bodyender be);
extern struct pnode* expr(struct pnode *root, struct lexer *lex);
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

  return pnode_getType(this, id);
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

  if (nextTok->type == LEX_FUNC) {
    return funcType(this, lex);
  } else { 
    if ((nextTok->type != LEX_ID)) {
      return NULL;
    }
    return idType(this, lex);
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

  pnode_addLeaf(ret, expr(root, lex));

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
  default: {
    if (!(ret = expr(root, lex))) {
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

struct pnode* entry(struct pnode *root, struct lexer *lex) {
  struct token *tok = token_getOrDie(lex);

  if (!tok) {
    env.fail("Unexpected end of file in entry");
  }
  
  if (tok->type != LEX_NEWLINE) {
    env.fail("Unexpected token found: got %s, expected a new line after entry definition", token_str(tok));
  }

  token_free(tok);

  struct pnode *ret = pnode_new(PR_ENTRY);

  ret->root = root; //in this case, this is needed

  pnode_addLeaf(ret, body(ret, lex, entryBe));

  tok = token_getOrDie(lex);

  if (tok->type != LEX_ENDENTRY) {
    env.fail("Unexpected token found: got %s, expected '/entry'", token_str(tok)); 
  }

  token_free(tok);

  return ret;
}

struct pnode* func(struct pnode *root, struct lexer *lex) {
  return NULL;
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

