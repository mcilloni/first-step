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

#include "cgen.h"

#include <utils/env.h>
#include <syms/types.h>

#include <treemap/map.h>

#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#if defined(__NetBSD__) || (defined(__APPLE__) && defined(__MACH__))
  #include <memstream/memstream.h>
#endif

const char *filename = NULL;

void ccode_lineTag(size_t lineno, FILE *out) {
  fprintf(out, "#line %zu \"%s\"\n", lineno, filename);
}

char* ccode_csym(struct type *type, const char *name, bool pedantic, bool first);

void file_indent(FILE *out, uint8_t indent) {
  for (uint8_t i = 0; i < indent; ++i) {
    fputc(' ', out);
  }
}

const char* ccode_opConv(struct token *tok, struct pnode *leftSupportNode) {
  Pool *pool = pool_new();
  const char *ret = NULL;
  switch(tok->type) {
  case LEX_AND:
    ret = "&&";
    break;
  case LEX_APOS: {
    ret = (pnode_evalType(pool, leftSupportNode, NULL)->kind == TYPE_PTR) ? "->" : ".";
    break;
  }
  case LEX_MOD:
    ret = "%";
    break;
  case LEX_OR:
    ret = "||";
    break;
  case LEX_XOR:
    ret = "^";
    break;
  default: 
    ret = token_str(tok);
    break;
  }

  pool_release(pool, (void (*)(void*)) type_free);

  return ret;
}

void ccode_genBody(struct pnode *body, FILE *out, uint8_t indent);

void ccode_genRecExpr(struct pnode *root, FILE *out) {
  if (!root || root == expr_empty) {
    return;
  }

  struct token tok;
  uintmax_t val = pnode_getval(root);

  tok.type = (enum token_type) val;

  switch (root->id) {
  case PR_ACCESS: {
    if (array_len(root->leaves) != 2) {
      env.fail("Unacceptable len: %zu", array_len(root->leaves));
    }

    ccode_genRecExpr(*leaves_get(root->leaves, 0), out);
    fputc('[', out);
    ccode_genRecExpr(*leaves_get(root->leaves, 1), out);
    fputc(']', out);
    break;
  }

  case PR_CAST: {
    if (array_len(root->leaves) != 1) {
      env.fail("Unacceptable len: %zu", array_len(root->leaves));
    }

    char *tCast = ccode_csym((struct type*) val, "", false, false);
    fprintf(out, "((%s) ", tCast);
    free(tCast); 
    ccode_genRecExpr(*leaves_get(root->leaves, 0), out);
    fputc(')', out);
    break;
  }

  case PR_SIZE: {
    char *tType = ccode_csym((struct type*) val, "", false, false);
    fprintf(out, "sizeof(%s) ", tType);
    free(tType); 
    break;
  }

  case PR_BINOP:
    if (array_len(root->leaves) != 2) {
      env.fail("Unacceptable len: %zu", array_len(root->leaves));
    }
    
    fputs("( ", out);    
    if (tok.type != LEX_COLON) {
      struct pnode *left = *leaves_get(root->leaves, 0);
      ccode_genRecExpr(left, out);
      fprintf(out, "%s", ccode_opConv(&tok, left));
    }
    ccode_genRecExpr(*leaves_get(root->leaves, 1), out);
    fputs(" )", out);
    break;

  case PR_UNOP:
    if (array_len(root->leaves) != 1) {
      env.fail("Unacceptable len: %zu", array_len(root->leaves));
    }

    fputs("( ", out);  
    const char *op;
    switch (tok.type) {
    case LEX_PTR:
      op = "&";
      break;
    case LEX_VAL:
      op = "*";
      break;
    default:
      op = token_str(&tok);
      break;
    }

    fprintf(out, "%s", op);
    ccode_genRecExpr(*leaves_get(root->leaves, 0), out);
    fputs(" )", out);
    break;

  case PR_STRING:
    fprintf(out, "(uint8*) \"%s\"", (char*) val);
    break;
  case PR_ID:
  case PR_STRUCTID:
    fputs((char*) val, out);
    break;
  case PR_CALL: {
    size_t len = array_len(root->leaves);
    if (len < 1) {
      env.fail("Unacceptable len: %zu", len);
    }

    ccode_genRecExpr(*leaves_get(root->leaves, 0), out);
    fputc('(', out);

    for (size_t i = 1; i < len; ++i) {
      if (i - 1) {
        fputc(',', out);
      }

      ccode_genRecExpr(*leaves_get(root->leaves, i), out);
    }

    fputc(')', out);

    break;
  }
  default:
    fprintf(out, "%" PRIdMAX, (intmax_t) val);
    break;

  }
  
}

void ccode_genExpr(struct pnode *expr, FILE *out, uint8_t indent) {
  if (expr->startLine) {
    file_indent(out, indent);
    ccode_lineTag(expr->startLine, out);
  }

  file_indent(out, indent);
  ccode_genRecExpr(expr, out);
}

void ccode_genReturn(struct pnode *ret, FILE *out, uint8_t indent) {
  if (ret->startLine) {
    file_indent(out, indent);
    ccode_lineTag(ret->startLine, out);
  }

  file_indent(out, indent);
  fputs("return ", out);
  ccode_genExpr(*leaves_get(ret->leaves, 0), out, 0);
  fputs(";\n", out);
}

void ccode_genIfElse(struct pnode *ifNode, FILE *out, uint8_t indent) {
  if (ifNode->startLine) {
    file_indent(out, indent);
    ccode_lineTag(ifNode->startLine, out);
  }

  file_indent(out, indent);
  fputs("if ( ", out);
  ccode_genExpr(*leaves_get(ifNode->leaves, 0), out, 0);
  fputs(" ) {\n\n", out);
  ccode_genBody(*leaves_get(ifNode->leaves, 1), out, indent + 2);
  file_indent(out, indent);
  fputs("} else {\n", out);
  ccode_genBody(*leaves_get(ifNode->leaves, 2), out, indent + 2);
  
  if (ifNode->endLine) {
    file_indent(out, indent);
    ccode_lineTag(ifNode->endLine, out);
  }

  file_indent(out, indent);
  fputs("}\n", out);
}

void ccode_genSimpleBlock(struct pnode *node, const char *stmt, FILE *out, uint8_t indent) {
  if (node->startLine) {
    file_indent(out, indent);
    ccode_lineTag(node->startLine, out);
  }

  file_indent(out, indent);
  fprintf(out, "%s ( ", stmt);
  ccode_genExpr(*leaves_get(node->leaves, 0), out, 0);
  fputs(" ) {\n\n", out);
  ccode_genBody(*leaves_get(node->leaves, 1), out, indent + 2);
  
  if (node->endLine) {
    file_indent(out, indent);
    ccode_lineTag(node->endLine, out);
  }

  file_indent(out, indent);
  fputs("}\n", out);
}

void ccode_genIf(struct pnode *ifNode, FILE *out, uint8_t indent) {
  ccode_genSimpleBlock(ifNode, "if", out, indent);
}

void ccode_genWhile(struct pnode *whileNode, FILE *out, uint8_t indent) {
  ccode_genSimpleBlock(whileNode, "while", out, indent);
}

void ccode_genDecl(struct pnode *decl, FILE *out, uint8_t indent) {
  if (decl->startLine) {
    file_indent(out, indent);
    ccode_lineTag(decl->startLine, out);
  }

  char *id = (char*) pnode_getval(decl);
  struct symbol *sym = pnode_matchSymbolForDeclaration(decl, id);

  if (!sym) {
    env.fail("Broken tree, can't find symbol info into table");
  }

  file_indent(out, indent);
  char *csym = ccode_csym(sym->type, id, false, true);
  fprintf(out, "%s%s", (sym->decl ? "extern " : ""), csym);
  if (sym->optData) {
    if (sym->decl) {
      env.fail("Cannot assign anything to extern variable");
    }
    fputs(" = ", out);
    ccode_genRecExpr((struct pnode*) sym->optData, out);
  }
  fputs(";\n\n", out);
  free(csym);
}

void ccode_genStmt(struct pnode *stmt, FILE *out, uint8_t indent) {
 
  switch (stmt->id) {
  case PR_BREAK:
    if (stmt->startLine) {
      file_indent(out, indent);
      ccode_lineTag(stmt->startLine, out);
    }

    file_indent(out, indent);
    fputs("break;\n", out);
    break;
  case PR_CONTINUE:
    if (stmt->startLine) {
      file_indent(out, indent);
      ccode_lineTag(stmt->startLine, out);
    }

    file_indent(out, indent);
    fputs("continue;\n", out);
    break;
  case PR_DECLARATION:
    ccode_genDecl(stmt, out, indent);
    break;
  case PR_IF:
    ccode_genIf(stmt, out, indent);
    break;
  case PR_IFELSE:
    ccode_genIfElse(stmt, out, indent);
    break;
  case PR_WHILE:
    ccode_genWhile(stmt, out, indent);
    break;
  case PR_RETURN:
    ccode_genReturn(stmt, out, indent);
    break;
  default:
    ccode_genExpr(stmt, out, indent);
    fputs(";\n", out);
    break;
  }
}

void cgen_cfuncparms(FILE *out, Array *parms) {
  size_t len = array_len(parms);

  if (!len) {
    fputs("void", out);
    return;
  }

  char *par;
  for(size_t i = 0; i < len; ++i) {
    if (i) {
      fputc(',', out);
    }

    par = ccode_csym((struct type*) *array_get(parms, i), "", false, false);
    fputs(par, out);
    free(par);
  }
}

char* ccode_csym(struct type *type, const char *name, bool pedantic, bool toplevel) {
  char *str;
  size_t size;
  FILE *strFile = open_memstream(&str, &size);

  if (type->name && !pedantic) {
    fprintf(strFile, "%s %s", (type == type_none) ? "void" : type->name, name);
    fclose(strFile);
    return str;
  }

  switch (type->kind) {
  case TYPE_FUNC: {
    struct ftype *ftype = (struct ftype*) type;
    size_t pms, agz;
    char *fmt = ccode_csym(ftype->ret, "%s", false, false), *pm, *ags;
    FILE *pmFile = open_memstream(&pm, &pms); 
    FILE *agsFile = open_memstream(&ags, &agz);
    cgen_cfuncparms(agsFile, ftype->params);
    fputc(')', agsFile);
    fclose(agsFile);
    fprintf(pmFile, fmt, "%s(%s");
    free(fmt);
    fclose(pmFile);
    fprintf(strFile, pm, name, ags);
    free(ags);
    free(pm);
    break;
  }

  case TYPE_ARRAY: {
    struct atype *atype = (struct atype*) type;
    char buf[4096];
    snprintf(buf, 4095,"%s[%zu]", name, atype->len);
    return ccode_csym(((struct ptype*) type)->val, buf, false, false);
  }
                  
  case TYPE_PTR: {
    char buf[4096];
    snprintf(buf, 4095,"(*%s)", name);
    return ccode_csym(((struct ptype*) type)->val, buf, false, false);
  }

  case TYPE_STRUCT: {
    struct stype *stype = (struct stype*) type;
    fputs("struct { ", strFile);

    struct spair *pair;
    size_t len = symbols_len(stype->symbols);
    char *tmp;

    for (size_t i = 0; i < len; ++i) {
      pair = *list_get(stype->symbols, i);
      tmp = ccode_csym(pair->sym->type, pair->id, false, false);
      fprintf(strFile, "%s; ", tmp);
    }

    fprintf(strFile, "} %s", name);
    break;

  }

  default: {
    char tmp[4096];
    fprintf(strFile, "%s %s", (type == type_none) ? "void" : type_str(type, tmp, 4096), name);
    break;
  }
  }
  fclose(strFile);

  return str;
}

void ccode_declAliases(Aliases *syms, FILE *out, uint8_t indent) {
  
  size_t len = aliases_len(syms);
  struct apair *decl;
  
  for (size_t i = 0; i < len; ++i) {
    decl = *list_get(syms, i);
    file_indent(out, indent);
    char *csym = NULL;
    
    if (type_isStruct(decl->type)) {
      csym = ccode_csym(decl->type, "", true, false);
      fprintf(out, "typedef struct %s %s;\n", decl->name, decl->name);
      fprintf(out, "struct %s %s;\n", decl->name, csym + 7);
    } else {
      if (type_isFunc(decl->type)) {
        csym = ccode_csym(decl->type, decl->name, true, false);
      } else {
        csym = ccode_csym(decl->type, decl->name, false, true);
      }
      fprintf(out, "typedef %s;\n", csym);
    }

    free(csym);
  }

}

/*void ccode_declSyms(Symbols *syms, FILE *out, uint8_t indent) {
  
  size_t len = symbols_len(syms);

  struct spair *decl;
  
  for (size_t i = 0; i < len; ++i) {
    decl = *list_get(syms, i);
    file_indent(out, indent);
    char *csym = ccode_csym(decl->sym->type, decl->id);
    fprintf(out, "%s%s", (decl->sym->decl ? "extern " : ""), csym);
    if (decl->sym->optData) {
      if (decl->sym->decl) {
        env.fail("Cannot assign anything to extern variable");
      }
      fputs(" = ", out);
      ccode_genRecExpr((struct pnode*) decl->sym->optData, out);
    }
    fputs(";\n", out);
    free(csym);
  }

}*/

void ccode_genBody(struct pnode *body, FILE *out, uint8_t indent) {
  size_t len = array_len(body->leaves);

//ccode_declAliases(pnode_getAliases(body), out, indent);
//ccode_declSyms(pnode_getSyms(body), out, indent);

  for (size_t i = 0; i < len; ++i) {
    ccode_genStmt(*leaves_get(body->leaves, i), out, indent);
  }
}

void ccode_genFuncHead(struct pfunc *node, FILE *out) {
  char *tmp; 
  size_t size;

  FILE *tmf = open_memstream(&tmp, &size);

  fprintf(tmf, "%s(", node->name);
  
  size_t len = symbols_len(node->params);

  if (!len) {
    fputs("void", tmf);
  } else {
    struct spair *pair;
    bool first = true;
    char *param;
    
    for (size_t i = 0; i < len; ++i) {
      pair = *list_get(node->params, i);
      if (first) {
        first = false;
      } else {
        fputs(", ", tmf);
      }

      param = ccode_csym(pair->sym->type, pair->id, false, false);

      fputs(param, tmf);

      free(param);
    }
  }
 
  fclose(tmf); 

  char *buf = ccode_csym(node->ftype->ret, tmp, false, false);

  fprintf(out, "%s) {\n", buf);

  free(buf);

}

void ccode_genFunc(struct pnode *node, FILE *out) {

  if (node->startLine) {
    ccode_lineTag(node->startLine, out);
  }

  ccode_genFuncHead((struct pfunc*) node, out);

  ccode_genBody(*leaves_get(node->leaves, 0), out, 2);

  if (node->endLine) {
    ccode_lineTag(node->endLine, out);
  }

  fputs("}\n\n", out);

}

void ccode_genDef(struct pnode *def, FILE *out) {

  switch (def->id) {
  case PR_DECLARATION: {
    ccode_genDecl(def, out, 0);
    break;
  }

  case PR_FUNC:
    ccode_genFunc(def, out);
    break;
  default:
    env.fail("Not a definition: %s", nt_str(def->id));
    break;
  }

}

void ccode_printDefaultHeaders(FILE *out) {
  fputs("/* \n"
        " * Generated by the First Step Helm experimental compiler.\n"
        " * This software is experimental and should not be used except for experimental purposes.\n"
        " */\n\n", out);
  fputs("#include <stdbool.h>\n"
        "#include <stddef.h>\n"
        "#include <stdint.h>\n\n"
        "static const void *null = NULL;\n\n"
        "typedef int8_t int8;\n"
        "typedef int16_t int16;\n"
        "typedef int32_t int32;\n"
        "typedef int64_t int64;\n"
        "typedef intptr_t intptr;\n\n"
        "typedef uint8_t uint8;\n"
        "typedef uint16_t uint16;\n"
        "typedef uint32_t uint32;\n"
        "typedef uint64_t uint64;\n"
        "typedef uintptr_t uintptr;\n"
        "typedef void* data;\n\n", out);
}

Array *imported;

bool ccode_imported(char *name) {
  size_t len = array_len(imported);
  const char *elem;

  for (size_t i = 0; i < len; ++i) {
    elem = *array_get(imported, i);
    if (!strcmp(elem, name)) {
      return true;
    }
  }

  return false;
}

void ccode_genImports(Imports *imports, FILE *out) {
  MapIter *iter = mapiter_start(imports);
  Pair *pair;
  char *name;
  struct proot *module;
  struct pnode *node;

  while ((pair = mapiter_next(iter))) {
    name = (char*) pair->key;

    if (ccode_imported(name)) {
      return;
    } else {
      array_append(imported, name);
    }

    module = (struct proot*) pair->value;
    filename = module->filename;
    node = (struct pnode*) module;

    ccode_genImports(module->imports, out);

    fprintf(out, "// Module %s\n\n", name);

    ccode_declAliases(pnode_getAliases(node), out, 0);

    size_t len = array_len(node->leaves);
    for (size_t i = 0; i < len; ++i) {
      ccode_genDef(*leaves_get(node->leaves, i), out);
    }

    fprintf(out, "// End of module %s\n\n", name);

    pair_free(pair);
  }

  mapiter_free(iter);
}

void cgen(const char *fName, struct pnode *tree, FILE *out) {

  imported = array_new(3U);

  ccode_printDefaultHeaders(out);

  if (!pnode_isRoot(tree)) {
    env.fail("Cannot generate anything from a broken tree. Expected %s, found %s", nt_str(PR_ROOT), nt_str(tree->id));
  }
 
  ccode_genImports(((struct proot*) tree)->imports, out);

  array_freeContents(imported, free);
  array_free(imported);

  filename = fName;

  ccode_declAliases(pnode_getAliases(tree), out, 0);
//ccode_declSymbols(pnode_getSymbols(tree), out, 0);

  size_t len = array_len(tree->leaves);
  for (size_t i = 0; i < len; ++i) {
    ccode_genDef(*leaves_get(tree->leaves, i), out);
  }

}

