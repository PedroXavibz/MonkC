#ifndef LEXER_H
#define LEXER_H

#include "token.h"
#include "../helper.h"

typedef struct {
  const char *file_location;
  i64 index;
  i64 line;
  i64 column;
} Position;

typedef struct {
  const char *source;
  char character;

  i64 length;
  Position *pos;
} Lexer;

Lexer *create_lexer(const char* file_location, const char *source);

void free_lexer(Lexer *lexer);

TokensList *tokenizer(Lexer *lexer);
#endif
