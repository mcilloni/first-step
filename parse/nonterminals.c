#include "nonterminals.h"

const char* nt_str(enum nonterminals nt) {
  switch(nt) {
  case PR_PROGRAM:
    return "Program Root";
  case PR_DEFINITION:
    return "Definition";
  case PR_ENTRY:
    return "Entry";
  case PR_BODY:
    return "Block body";
  case PR_STMTS:
    return "Statements";
  case PR_STMT:
    return "Statement";
  case PR_IF:
    return "If";
  case PR_DECLARATION:
    return "Declaration";
  case PR_FUNC:
    return "Function";
  case PR_STMTEND:
    return "End of statement";  
  case PR_MULVARDECL:
    return "Multiple variable declaration";
  case PR_SINGLEVARDECL:
    return "Single variable declaration";
  case PR_VARDECL:
    return "Variable declaration";
  case PR_ID:
    return "Identifier";
  case PR_EXPR:
    return "Expression";
  case PR_EXPRC:
    return "Expression (C)";
  case PR_ARITHEXP: 
    return "Arithmetic expression";
  case PR_CONST:
    return "Constant";
  case PR_CALL:
    return "Function call";
  case PR_RETURN:
    return "Return";
  case PR_NUMBER:
    return "Number";
  case PR_BINOP:
    return "Binary Operation";
  case PR_UNOP:
    return "Unary Operation";
  case PR_INC:
    return "Increment";
  case PR_DEC:
    return "Decrement";
  default:
    return "Unknown";
  }
}

