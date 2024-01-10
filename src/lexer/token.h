#ifndef TOKEN_H
#define TOKEN_H

#include "../helper.h"

typedef enum {
  // OPERATOR
  PLUS,                 // +
  INCREMENT,            // ++
  ASSIGNMENT_PLUS,      // +=
  MINUS,                // -
  DECREMENT,            // --
  ASSIGNMENT_MINUS,     // -=
  MULTIPLY,             // *
  ASSIGNMENT_MULTIPLY,  // *=
  DIVIDE,               // /
  ASSIGNMENT_DIVIDE,    // /=
  MODULE,               // %
  ASSIGNMENT_MODULE,    // %=
  POWER,                // **
  NOT,                  // !
  NOT_EQUAL,            // !=
  OR,                   // ||
  OR_TYPE,              // |
  AND,                  // &&
  BITWISE_AND,          // &
  BITWISE_OR,           // $
  BITWISE_XOR,          // ^
  BITWISE_NOT,          // ~
  EQUAL,                // ==
  LESS_THEN,            // <
  LEFT_SHIFT,           // <<
  RIGHT_SHIFT,          // >>
  GREATER_THEN,         // >
  LESS_EQUAL,           // <=
  GREATER_EQUAL,        // >=
  ASSIGNMENT_OPERATOR,  // =
  ASSIGNMENT_MUTABLE,   // :=
  TYPE_DECLARATION,     // :
  TERNARY_OPERATOR,     // IDENTIFIER ? <expr> : <expr>
  RETURN_OPERATOR,      // =>
  SPREAD,               // ..

  // SEPARATOR
  LCBRACKETS,           // {
  RCBRACKETS,           // }
  LBRACKETS,            // [
  RBRACKETS,            // ]
  LPARENTESES,          // (
  RPARENTESES,          // )
  SEMICOLON,            // ;
  COMMA,                // ,
  DOT,                  // .

  // LITERAL
  IDENTIFIER,
  INT_LITERAL,
  FLOAT_LITERAL,
  STRING_LITERAL,
  CHAR_LITERAL,
  BINARY_LITERAL,
  OCT_LITERAL,
  HEX_LITERAL,

  // KEYWORD
  IF,                  // if
  ELSE,                // else

  WHILE,               // while
  DO,                  // do
  FOR,                 // for (<TYPE> = <expr>; <expr> <op> <expr>; <op>)
  FOREACH,             // foreach
  CONTINUE,            // continue
  RETURN,              // return

  SWITCH,              // switch
  CASE,                // case
  BREAK,               // break

  LONG,                // long
  INT,                 // int
  I8,                  // i8
  I16,                 // i16
  I32,                 // i32
  I64,                 // i64
  FLOAT,               // float
  F8,                  // f8
  F16,                 // f16
  F32,                 // f32
  F64,                 // f64
  DOUBLE,              // double
  STRING,              // string
  CHAR,                // char
  VOID,                // void
  BOOLEAN,             // boolean
  TRUE,                // true
  FALSE,               // false

  CONST,               // const
  TK_NULL,             // null
  TYPEOF,              // typeof
  SIZEOF,              // sizeof
  TK_EOF,              // EOF

  STRUCT,              // struct
  ENUM,                // enum

  IMPORT,              // import
  FROM,                // from
} TokenType;

typedef struct {
  i64 start;
  i64 end;
  i64 line;
  i64 column;
} TokenPosition;

typedef struct T_K {
  char *value;
  TokenType type;
  TokenPosition pos;

  struct T_K *next;
  struct T_K *prev;
} Token;

typedef struct {
  Token *head;
  Token *tail;
} TokensList;

Token *create_token(char *value, TokenType type, TokenPosition pos);

void print_token(Token *token);

#endif
