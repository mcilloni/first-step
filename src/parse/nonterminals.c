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

#include "nonterminals.h"

const char* nt_str(enum nonterminals nt) {
  switch(nt) {
  case PR_ROOT:
    return "Program Root";
  case PR_DEFINITION:
    return "Definition";
  case PR_ENTRY:
    return "Entry";
  case PR_BODY:
    return "Block body";
  case PR_STRING:
    return "String";
  case PR_STMTS:
    return "Statements";
  case PR_STMT:
    return "Statement";
  case PR_IF:
    return "If";
  case PR_IFELSE:
    return "IfElse";
  case PR_WHILE:
    return "While";
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
  case PR_STRUCTID:
    return "Struct member";
  case PR_EXPR:
    return "Expression";
  case PR_EXPRC:
    return "Expression (C)";
  case PR_ARITHEXP: 
    return "Arithmetic expression";
  case PR_CONST:
    return "Constant";
  case PR_ACCESS:
    return "Array access";
  case PR_CALL:
    return "Function call";
  case PR_CAST:
    return "Type cast";
  case PR_SIZE:
    return "Size operator";
  case PR_RETURN:
    return "Return";
  case PR_BREAK:
    return "Break";
  case PR_CONTINUE:
    return "Continue";
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

