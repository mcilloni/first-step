#include "lex.h"
#include "utils.h"

#include <ctype.h>
#include <errno.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

static inline struct lexer* err(struct lexer *lex, const char *error) {
  lex->errcode = ERROR;
  lex->error = error;
  return lex;
}

struct lexer* lexer_open(const char *path) {
  struct lexer *lex = calloc(1,sizeof(struct lexer)); 
  
  lex->file = fopen(path, "r");
  if (!lex->file) {
    return err(lex, "Cannot open file");
  }

  lex->consider = false;

  return lex;
  
}

void lexer_close(struct lexer *lex) {
  
  if (lex->file) {
    fclose(lex->file);
  }

  free(lex);
  
}

bool isWhite(char ch) {
  return ch == ' ' || ch == '\t';
}

bool stok(struct lexer *lex, char *data, size_t max) {

  char ch;
  size_t i;
  bool sym = false;
  for(i = 0; i < max; ++i) {
    if(isWhite(ch = fgetc(lex->file))) {
      if(i) {
        break;
      } else {
        --i;
        continue;
      }
    }

    if(ch == EOF) {
      lex->errcode = FILEEND;
      lex->error = "EOF";
      break;
    }
    
    if (ch == '\n') {
      if (i) {
        ungetc('\n', lex->file);
        lex->consider = true;
      } else {
        if(lex->consider) {
          *data = '\n';
          ++i;
          lex->consider = false;
        } else {
          --i;
          continue;
        }
      }
      break;
    }

    if (isalnum(ch)) { //if not symbol,
      if (sym) { //and we are in a symbol sequence,
        ungetc(ch, lex->file); //put this char back and then end this token
        break;
      } 
    } else { //else if symbol,
      if(!i) { //and start of token,
        if(ch != '/') { //and not an end id slash,
          sym = true; //we are now recognizing a string of symbols
        }
      } else { //else if in middle of operation, 
        if (!sym) { //and a in a normal id sequence,
          ungetc(ch, lex->file); //give char back and end this token
          break;
        }
      }
    } 
       
    if (ferror(lex->file)) {
      lex->errcode = ERROR;
      lex->error = strerror(errno);
      return false;
    }

    data[i] = ch;

  }

  if (i == max) {
    lex->errcode = ERROR;
    lex->error = "Token too long";
    return false;
  }

  data[i] = 0;

  return true;   

}

struct token gettok(struct lexer *lex) {

  static const uint16_t max = 1024;

  const char data[max];

  struct token res = {NONE, 0};

  if (!stok(lex, (char*) data, max)) {
    return res;
  }

  // \n
  if (!strcmp("\n", data)) {
    res.type = NEWLINE;
    return res;
  }

  // /
  if (!strcmp("/", data)) {
    res.type = DIV;
    return res;
  }

  // =

  if (!strcmp("=", data)) {
    res.type = ASSIGN;
    return res;
  }

  // ==
  if (!strcmp("==", data)) {
    res.type = EQUAL;
    return res;
  }

  // >
  if (!strcmp(">", data)) {
    res.type = MAJOR;
    return res;
  }

  // <
  if (!strcmp("<", data)) {
    res.type = MINOR;
    return res;
  }

  // *
  if (!strcmp("*", data)) {
    res.type = TIMES;
    return res;
  }

  // +
  if (!strcmp("+", data)) {
    res.type = PLUS;
    return res;
  }
  
  // -
  if (!strcmp("-", data)) {
    res.type = MINUS;
    return res;
  }

  // ++
  if (!strcmp("++", data)) {
    res.type = INC;
    return res;
  }

  // !
  if (!strcmp("!", data)) {
    res.type = NOT;
    return res;
  }

  // entry
  if (!strcmp("entry", data)) {
    res.type = ENTRY;
    return res;
  }

  // /entry
  if (!strcmp("/entry", data)) {
    res.type = ENDENTRY;
    return res;
  }

  // if
  if (!strcmp("if", data)) {
    res.type = IF;
    return res;
  }

  // /if
  if (!strcmp("/if", data)) {
    res.type = ENDIF;
    return res;
  }

  if (!strcmp("var", data)) {
    res.type = VAR;
    return res;
  }

  // number
  if (isStrNum(data)) {
    res.type = NUMBER;
    res.value = atoll(data);
    return res;
  }

  //generic id
  res.type = ID;
  res.value = (intmax_t) strdup(data);
  
  return res;
}

void freetok(struct token tok) {

  if (tok.type == ID) {
    free( (void*) tok.value);
  }

}
