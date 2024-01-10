#ifndef PARSER_H
#define PARSER_H

#include "../helper.h"

typedef struct {
  i8 is_error;
  union {
    char *error;
    char *result;
  };
} Node;

typedef struct {
  Node *root;
  i64 index;
} Parser;

#endif
