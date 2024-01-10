#include "token.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static char *token_type_string(TokenType type) {
  switch (type) {
  // case OPERATOR:
  case PLUS:
    return "PLUS";
  case INCREMENT:
    return "INCREMENT";
  case ASSIGNMENT_PLUS:
    return "ASSIGNMENT_PLUS";
  case MINUS:
    return "MINUS";
  case DECREMENT:
    return "DECREMENT";
  case ASSIGNMENT_MINUS:
    return "ASSIGNMENT_MINUS";
  case MULTIPLY:
    return "MULTIPLY";
  case ASSIGNMENT_MULTIPLY:
    return "ASSIGNMENT_MULTIPLY";
  case DIVIDE:
    return "DIVIDE";
  case ASSIGNMENT_DIVIDE:
    return "ASSIGNMENT_DIVIDE";
  case MODULE:
    return "MODULE";
  case ASSIGNMENT_MODULE:
    return "ASSIGNMENT_MODULE";
  case POWER:
    return "POWER";
  case NOT:
    return "NOT";
  case NOT_EQUAL:
    return "NOT_EQUAL";
  case OR:
    return "OR";
  case AND:
    return "AND";
  case BITWISE_AND:
    return "BITWISE_AND";
  case BITWISE_OR:
    return "BITWISE_OR";
  case BITWISE_XOR:
    return "BITWISE_XOR";
  case BITWISE_NOT:
    return "BITWISE_NOT";
  case EQUAL:
    return "EQUAL";
  case LESS_THEN:
    return "LESS_THEN";
  case LEFT_SHIFT:
    return "LEFT_SHIFT";
  case RIGHT_SHIFT:
    return "RIGHT_SHIFT";
  case GREATER_THEN:
    return "GREATER_THEN";
  case LESS_EQUAL:
    return "LESS_EQUAL";
  case GREATER_EQUAL:
    return "GREATER_EQUAL";
  case ASSIGNMENT_OPERATOR:
    return "ASSIGNMENT_OPERATOR";
  case ASSIGNMENT_MUTABLE:
    return "ASSIGNMENT_MUTABLE";
  case TERNARY_OPERATOR:
    return "TERNARY_OPERATOR";
  case RETURN_OPERATOR:
    return "RETURN_OPERATOR";
  case OR_TYPE:
    return "OR_TYPE";
  case TYPE_DECLARATION:
    return "TYPE_DECLARATION";
  case SPREAD:
    return "SPREAD";

  // case SEPERATOR:
  case LCBRACKETS:
    return "LCBRACKETS";
  case RCBRACKETS:
    return "RCBRACKETS";
  case LBRACKETS:
    return "LBRACKETS";
  case RBRACKETS:
    return "RBRACKETS";
  case LPARENTESES:
    return "LPARENTESES";
  case RPARENTESES:
    return "RPARENTESES";
  case SEMICOLON:
    return "SEMICOLON";
  case COMMA:
    return "COMMA";
  case DOT:
    return "DOT";

  // case LITERAL:
  case IDENTIFIER:
    return "IDENTIFIER";

  case INT_LITERAL:
    return "INT_LITERAL";

  case FLOAT_LITERAL:
    return "FLOAT_LITERAL";

  case STRING_LITERAL:
    return "STRING_LITERAL";

  case CHAR_LITERAL:
    return "CHAR_LITERAL";

  case BINARY_LITERAL:
    return "BINARY_LITERAL";

  case OCT_LITERAL:
    return "OCT_LITERAL";

  case HEX_LITERAL:
    return "HEX_LITERAL";

  // case KEYWORD:
  case IF:
    return "IF";

  case ELSE:
    return "ELSE";

  case WHILE:
    return "WHILE";

  case DO:
    return "DO";

  case FOR:
    return "FOR";

  case FOREACH:
    return "FOREACH";

  case CONTINUE:
    return "CONTINUE";

  case RETURN:
    return "RETURN";

  case SWITCH:
    return "SWITCH";

  case CASE:
    return "CASE";

  case BREAK:
    return "BREAK";

  case LONG:
    return "LONG";

  case INT:
    return "INT";

  case I8:
    return "I8";

  case I16:
    return "I16";

  case I32:
    return "I32";

  case I64:
    return "I64";

  case FLOAT:
    return "FLOAT";

  case F8:
    return "F8";

  case F16:
    return "F16";

  case F32:
    return "F32";

  case F64:
    return "F64";

  case DOUBLE:
    return "F64";

  case STRING:
    return "STRING";

  case CHAR:
    return "CHAR";

  case VOID:
    return "VOID";

  case BOOLEAN:
    return "BOOLEAN";

  case TRUE:
    return "TRUE";

  case FALSE:
    return "FALSE";

  case CONST:
    return "CONST";

  case TK_NULL:
    return "TK_NULL";

  case TYPEOF:
    return "TYPEOF";

  case SIZEOF:
    return "SIZEOF";

  case TK_EOF:
    return "EOF";

  case STRUCT:
    return "STRUCT";

  case ENUM:
    return "ENUM";

  case IMPORT:
    return "IMPORT";

  case FROM:
    return "FROM";

  default:
    return "UNKNOW";
  }
}

Token *create_token(char *value, TokenType type, TokenPosition pos) {
  Token *token = malloc(sizeof(Token));
  if (token == NULL)
    return NULL;

  token->value = value;
  token->type = type;
  token->pos = pos;
  return token;
}

void print_token(Token *token) {
  printf("(%s, %s)  -> [ Start: %ld, End: %ld ] [ Line: %ld, Column: %ld ]\n", 
         token_type_string(token->type), token->value, 
         token->pos.start, token->pos.end,
         token->pos.line, token->pos.column
         );
}
