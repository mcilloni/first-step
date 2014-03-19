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

void decl(struct pnode *this, struct lexer *lex) {

  struct token *tok = token_getOrDie(lex);

  if (!tok) {
    env.fail("Unexpected eof, expected an identifier");
  }

  struct token *type = token_getOrDie(lex);
  
  if (!(tok && type)) {
    env.fail("Unexpected end of file during var declaration");
  }

  if ((tok->type != type->type) || (tok->type != LEX_ID)) {
    env.fail("Unexpected token(s), expected two IDs in variable declaration");
  }

  const char *id = (const char*) tok->value;
  struct type *declType = pnode_symbolType(this,id);

  enum symbols_resp resp;

  if (!pnode_addSymbol(this, id, (const char*) type->value, &resp)) {
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

  token_free(type);
  token_free(tok);

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
  case LEX_IF: {
    token_getOrDie(lex); //discard
    ret = ifStmt(root, lex);
    break;
  }
  case LEX_VAR: {
    token_getOrDie(lex); //discard
    decl(root, lex);
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

struct pnode* definition(struct pnode *root, struct lexer *lex) {
  struct token *tok = token_getOrDie(lex);
  if (!tok) {
    return NULL;
  }

  switch (tok->type) {
  case LEX_ENTRY:
    token_free(tok);
    return entry(root, lex);
  default: 
    env.fail("Unexpected token found: got %s, expected 'entry'", token_str(tok));
    break;
  }

  return NULL;
}

struct pnode* parse(const char *filename) {

  struct pnode *program = pnode_new(PR_PROGRAM), *nextDef;  
  struct lexer *lex = lexer_open(filename);

  while ((nextDef = definition(program, lex))) {
    pnode_addLeaf(program, nextDef);    
  }

  if (!array_len(program->leaves)) {
    pnode_free(program);
    return NULL;
  }

  lexer_close(lex);

  return program;
}

