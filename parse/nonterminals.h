#if !defined(_NONTERMINALS_H)
#define _NONTERMINALS_H

#include "symbols.h"
#include "../lex/lex.h"

enum nonterminals {
  PR_PROGRAM,
  PR_DEFINITION,
  PR_ENTRY,
  PR_BODY,
  PR_STMTS,
  PR_STMT,
  PR_STRING,
  PR_IF,
  PR_FUNC,
  PR_DECLARATION,
  PR_STMTEND,
  PR_MULVARDECL,
  PR_SINGLEVARDECL,
  PR_VARDECL,
  PR_ID,
  PR_EXPR,
  PR_EXPRC,
  PR_ARITHEXP,
  PR_CONST,
  PR_CALL,
  PR_RETURN,
  PR_NUMBER,
  PR_BINOP,
  PR_UNOP,
  PR_INC,
  PR_DEC
};

const char* nt_str(enum nonterminals nt);

#endif

