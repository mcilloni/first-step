#include "cgen.h"

#include "../utils/env.h"

#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void file_indent(FILE *out, uint8_t indent) {
  for (uint8_t i = 0; i < indent; ++i) {
    fputc(' ', out);
  }
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
  case PR_BINOP:
    if (array_len(root->leaves) != 2) {
      env.fail("Unacceptable len: %zu", array_len(root->leaves));
    }
    
    fputs("( ", out);    
    ccode_genRecExpr(*leaves_get(root->leaves, 0), out);
    fprintf(out, " %s ", token_str(&tok));
    ccode_genRecExpr(*leaves_get(root->leaves, 1), out);
    fputs(" )", out);
    break;

  case PR_UNOP:
    if (array_len(root->leaves) != 1) {
      env.fail("Unacceptable len: %zu", array_len(root->leaves));
    }

    fputs("( ", out);    
    fprintf(out, "%s", token_str(&tok));
    ccode_genRecExpr(*leaves_get(root->leaves, 0), out);
    fputs(" )", out);
    break;
  case PR_ID:
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
  file_indent(out, indent);
  ccode_genRecExpr(expr, out);
}

void ccode_genReturn(struct pnode *ret, FILE *out, uint8_t indent) {
  file_indent(out, indent);
  fputs("return ", out);
  ccode_genExpr(*leaves_get(ret->leaves, 0), out, 0);
  fputs(";\n", out);
}

void ccode_genIf(struct pnode *ifNode, FILE *out, uint8_t indent) {
  file_indent(out, indent);
  fputs("if ( ", out);
  ccode_genExpr(*leaves_get(ifNode->leaves, 0), out, 0);
  fputs(" ) {\n\n", out);
  ccode_genBody(*leaves_get(ifNode->leaves, 1), out, indent + 2);
  file_indent(out, indent);
  fputs("}\n", out);
}

void ccode_genStmt(struct pnode *stmt, FILE *out, uint8_t indent) {
  switch (stmt->id) {
  case PR_IF:
    ccode_genIf(stmt, out, indent);
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

char* ccode_csym(struct type *type, const char *name, bool first);

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

    par = ccode_csym((struct type*) *array_get(parms, i), "", false);
    fputs(par, out);
    free(par);
  }
}

char* ccode_csym(struct type *type, const char *name, bool first) {
  char *str;
  size_t size;
  FILE *strFile = open_memstream(&str, &size);
  if (type_isFunc(type)) {
    struct ftype *ftype = (struct ftype*) type;
    size_t pms, agz;
    char *fmt = ccode_csym(ftype->ret, "%s", false), *pm, *ags;
    puts(fmt);
    FILE *pmFile = open_memstream(&pm, &pms); 
    FILE *agsFile = open_memstream(&ags, &agz);
    cgen_cfuncparms(agsFile, ftype->params);
    fputc(')', agsFile);
    fclose(agsFile);
    fprintf(pmFile, fmt, (first) ? "%s(%s" : "(*%s)(%s");
    free(fmt);
    fclose(pmFile);
    fprintf(strFile, pm, name, ags);
    free(ags);
    free(pm);
  } else {
    char tmp[4096];
    fprintf(strFile, "%s %s", (type == type_none) ? "void" : type_str(type, tmp, 4096), name);
  }

  fclose(strFile);

  return str;
}

void ccode_declSyms(Symbols *syms, FILE *out, uint8_t indent) {
  
  MapIter *iter = mapiter_start(syms);

  Pair *decl;
  struct symbol *sym;
  
  while ((decl = mapiter_next(iter))) {
    sym = (struct symbol*) decl->value;
    file_indent(out, indent);
    char *csym = ccode_csym(sym->type, (char*) decl->key, true);
    fprintf(out, "%s%s;\n", (sym->decl ? "extern " : ""), csym);
    pair_free(decl);
    free(csym);
  }

  mapiter_free(iter);

}

void ccode_genBody(struct pnode *body, FILE *out, uint8_t indent) {
  size_t len = array_len(body->leaves);

  ccode_declSyms(pnode_getSyms(body), out, indent);

  for (size_t i = 0; i < len; ++i) {
    ccode_genStmt(*leaves_get(body->leaves, i), out, indent);
  }
}

void ccode_genFuncHead(struct pfunc *node, FILE *out) {
  char *tmp; 
  size_t size;

  FILE *tmf = open_memstream(&tmp, &size);

  fprintf(tmf, "%s(", node->name);
  
  if (!node->params->size) {
    fputs("void", tmf);
  } else {
    MapIter *iter = mapiter_start(node->params);
    Pair *pair;
    bool first = true;
    char *param;
    
    while ((pair = mapiter_next(iter))) {
      if (first) {
        first = false;
      } else {
        fputs(", ", tmf);
      }

      param = ccode_csym(((struct symbol*) pair->value)->type, (char*) pair->key, true);

      fputs(param, tmf);

      free(param);

      pair_free(pair);
    }

    mapiter_free(iter);
  }
 
  fclose(tmf); 

  char *buf = ccode_csym(node->ftype->ret, tmp, false);

  fprintf(out, "%s) {\n", buf);

  free(buf);

}

void ccode_genFunc(struct pnode *node, FILE *out) {

  ccode_genFuncHead((struct pfunc*) node, out);

  ccode_genBody(*leaves_get(node->leaves, 0), out, 2);

  fputs("}\n\n", out);

}

void ccode_genDef(struct pnode *def, FILE *out) {

  switch (def->id) {
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
        " * Generated by the helmc experimental compiler.\n"
        " * This software is experimental and should not be used except for experimental purposes.\n"
        " */\n\n", out);
  fputs("#include <stdbool.h>\n"
        "#include <stddef.h>\n"
        "#include <stdint.h>\n\n"
        "typedef int8_t int8;\n"
        "typedef int16_t int16;\n"
        "typedef int32_t int32;\n"
        "typedef int64_t int64;\n\n", out);
}

void cgen(struct pnode *tree, FILE *out) {

  ccode_printDefaultHeaders(out);

  if (tree->id != PR_PROGRAM) {
    env.fail("Cannot generate anything from a broken tree. Expected %s, found %s", nt_str(PR_PROGRAM), nt_str(tree->id));
  }
 
  ccode_declSyms(pnode_getSyms(tree), out, 0);

  size_t len = array_len(tree->leaves);
  for (size_t i = 0; i < len; ++i) {
    ccode_genDef(*leaves_get(tree->leaves, i), out);
  }
  
}

