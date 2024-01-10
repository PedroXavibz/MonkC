#include "lexer.h"
#include "token.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void throw_lexer_error(char *error_name, char *details, Position *pos) {
  fprintf(stderr, "Error on file \"%s\" at line %ld and column %ld\n%s: %s.\n",
          pos->file_location, pos->line, pos->column, error_name, details);
  exit(EXIT_FAILURE);
}

/**
 * Get current lexer character
 * @param lexer
 * @return Current lexer character
 */
static char get_current_char(Lexer *lexer) {
  return lexer->source[lexer->pos->index];
}

/**
 * Checks if the character is alphanumeric
 * @param char
 * @return true if the character is alphanumeric, false otherwise
 */
i8 is_alphanumeric(char c) {
  if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') ||
      (c >= '0' && c <= '9'))
    return 1;
  return 0;
}

/**
 * Checks if the character is new line
 * @param character
 * @return true if the character is a new line, false otherwise
 */
static i8 is_breakline(char character) {
  if (character == '\n' || character == '\r')
    return 1;
  return 0;
}

/**
 * Checks if the character is a blank, tabulation or new line
 * @param character
 * @return true if the character is a new line, tabulation or blank, false
 * otherwise
 */
static i8 is_whitespace(char character) {
  if (character == ' ' || character == 10 || character == '\t' ||
      is_breakline(character))
    return 1;
  return 0;
}

/**
 * Checks if the character is a digit
 * @param character
 * @return true if the character is a digit, false otherwise
 */
static i8 is_digit(char character) {
  if (character >= '0' && character <= '9')
    return 1;
  return 0;
}

/**
 * Checks if the lexer reached its end
 * @param lexer
 * @return true if lexer is at end, false otherwise
 */
static i8 is_at_end(Lexer *lexer) {
  if (lexer->pos->index >= lexer->length || get_current_char(lexer) == EOF ||
      get_current_char(lexer) == '\0')
    return 1;
  return 0;
}

/**
 * Verify if character is a operator
 * @param char
 * @return int
 */
static i8 is_operator(char c) {
  switch (c) {
  case '+':
  case '-':
  case '*':
  case '/':
  case '%':
  case '!':
  case '|':
  case '&':
  case '$':
  case '^':
  case '~':
  case '=':
  case '>':
  case '<':
  case ':':
  case '?':
    return 1;
  default:
    return 0;
  }
}

/**
 * Verify if character is a separator
 * @param char
 * @return int
 */
static i8 is_separator(char c) {
  switch (c) {
  case '{':
  case '}':
  case '[':
  case ']':
  case '(':
  case ')':
  case ';':
  case ',':
  case '.':
    return 1;
  default:
    return 0;
  }
}

static i8 is_string(char c) {
  if (c == '"' || c == '\'')
    return 1;
  return 0;
}

/**
 * Update lexer line and column
 * @param lexer
 * */
static void next_position(Lexer *lexer) {
  lexer->pos->column++;
  if (is_breakline(get_current_char(lexer)))
    lexer->pos->column = 0, lexer->pos->line++;
}

/**
 * Copy lexer position
 * @param lexer
 * @return A current position copy
 * */
static Position *copy_position(Lexer *lexer) {
  Position *pos = (Position *)malloc(sizeof(Position));
  if (pos == NULL) {
    fprintf(stderr, "MallocError: No memory to allocate\n");
    exit(EXIT_FAILURE);
  }
  pos->index = lexer->pos->index;
  pos->column = lexer->pos->column;
  pos->line = lexer->pos->line;
  pos->file_location = lexer->pos->file_location;
  return pos;
}

static TokenPosition create_token_position(Lexer *lexer) {
  TokenPosition pos = {.column = lexer->pos->column,
                       .line = lexer->pos->line,
                       .start = lexer->pos->index,
                       .end = lexer->pos->index + 1};
  return pos;
}

/**
 * Swap lexer position
 * @param Lexer *lexer,
 * @param Position *to_swap
 * */
static void swap_lexer_position(Lexer *lexer, Position *to_swap) {
  lexer->character = lexer->source[to_swap->index];
  free(lexer->pos), lexer->pos = to_swap;
}

/**
 * Print current lexer position line/column
 * @param lexer
 * */
static void debug_lexer_position(Lexer *lexer) {
  printf("\n=========[ DEBUG LEXER ]=========\n");
  char c = get_current_char(lexer);
  printf(" [ Char: %c | asciicode: %d ]\n\n", c, c);
  printf(" [Index: %ld | Line: %ld | Column: %ld ]\n", lexer->pos->index,
         lexer->pos->line, lexer->pos->column);
  printf("=================================\n\n");
}

/**
 * Checks the next character whitout update current lexer position
 * @param lexer
 * @return next character if is at the and return EOF
 */
static char peek(Lexer *lexer) {
  i64 i = lexer->pos->index;
  if (i++ >= lexer->length)
    return EOF;
  return lexer->source[i];
}

/**
 * Update Lexer current character and position
 * @param lexer
 */
static void next(Lexer *lexer) {
  lexer->pos->index++;
  if (is_at_end(lexer))
    lexer->character = EOF;
  else
    lexer->character = lexer->source[lexer->pos->index];
  next_position(lexer);
}

/**
 * Copy a slice of string
 * @param lexer
 * @param start
 * @param end
 * @return a copy of sliced string
 */
static char *cut_string(Lexer *lexer, i64 start, i64 end) {
  i64 length = end - start + 1;
  char *value = calloc(sizeof(char), length + 1);
  if (value == NULL) {
    fprintf(stderr, "MallocError: No memory to allocate\n");
    exit(EXIT_FAILURE);
  }
  memcpy(value, lexer->source + start, length);
  value[length] = '\0';
  return value;
}

/**
 * Matches lexer state with a sequence of characters. Either current states
 * matches, or not. Also if the match has been successful, it keeps the lexer's
 * position. Otherwise, it reverts the lexer to its original state
 * @param pattern
 * @param lexer
 * @return true of matches, 0 otherwise
 */
static i8 match(const char *pattern, Lexer *lexer) {
  if (get_current_char(lexer) != pattern[0])
    return 0;
  Position *p = copy_position(lexer);

  size_t start = p->index;
  size_t len = strlen(pattern);
  char *value = NULL;

  for (i64 i = 0; i < len; i++) {
    if (get_current_char(lexer) != pattern[i]) {
      swap_lexer_position(lexer, p);
      return 0;
    }

    size_t end = lexer->pos->index;
    char *value = cut_string(lexer, start, end);
    if (i + 1 == len && strcmp(value, pattern) == 0)
      break;

    free(value), value = NULL;
    next(lexer);
  }

  if (value != NULL)
    free(value);

  char c = peek(lexer);
  if (!is_whitespace(c) && !is_separator(c)) {
    swap_lexer_position(lexer, p);
    return 0;
  }

  free(p);
  return 1;
}

/**
 * Skip all blank, tabulation and new line character
 * @param lexer
 */
static void skip_whitespace(Lexer *lexer) {
  while (is_whitespace(get_current_char(lexer)))
    next(lexer);
}

/**
 * Skip whitespaces and comments
 * @param lexer
 */
static void skip(Lexer *lexer) {
  skip_whitespace(lexer);

  if (get_current_char(lexer) == '*' && peek(lexer) == '/')
    goto error_not_opened_comment;

  Position *pos = copy_position(lexer);

  // Comments
  while (get_current_char(lexer) == '/' &&
         (peek(lexer) == '/' || peek(lexer) == '*')) {
    // Single line comments
    if (get_current_char(lexer) == '/' && peek(lexer) == '/') {
      while (1) {
        if (is_breakline(peek(lexer)) || is_at_end(lexer))
          break;
        next(lexer);
      }
    }

    // Multiline comments
    if (get_current_char(lexer) == '/' && peek(lexer) == '*') {
      while (1) {
        if (get_current_char(lexer) == '*' && peek(lexer) == '/') {
          next(lexer);
          break;
        }
        if (is_at_end(lexer))
          goto error_unclosed_comment;
        next(lexer);
      }
    }

    next(lexer);
    skip_whitespace(lexer);
  }

  free(pos);
  return;

error_not_opened_comment:
  throw_lexer_error(
      "UnmatchedString",
      "Ending of comment \"*/\" is present but the beginning is not",
      lexer->pos);

error_unclosed_comment:
  throw_lexer_error(
      "UnmatchedString",
      "Beginning of comment \"/*\" is present but the ending is not", pos);
}

static Token *tokenize_operator(Lexer *lexer) {
  char current_char[2];
  current_char[0] = get_current_char(lexer);
  current_char[1] = '\0';

  char next_char = peek(lexer);
  char value[3];
  value[0] = *current_char;
  value[1] = next_char;
  value[2] = '\0';

  TokenPosition pos = create_token_position(lexer);
  switch (*current_char) {
  case '+':
    if (next_char == '=') {
      next(lexer);
      pos.end++;
      return create_token(value, ASSIGNMENT_PLUS, pos);
    } else if (next_char == '+') {
      next(lexer);
      pos.end++;
      return create_token(value, INCREMENT, pos);
    } else
      return create_token(current_char, PLUS, pos);

  case '-':
    if (next_char == '=') {
      next(lexer);
      pos.end++;
      return create_token(value, ASSIGNMENT_MINUS, pos);
    } else if (next_char == '-') {
      next(lexer);
      pos.end++;
      return create_token(value, DECREMENT, pos);
    } else
      return create_token(current_char, MINUS, pos);

  case '*':
    if (next_char == '=') {
      next(lexer);
      pos.end++;
      return create_token(value, ASSIGNMENT_MULTIPLY, pos);
    } else if (next_char == '*') {
      next(lexer);
      pos.end++;
      return create_token(value, POWER, pos);
    } else
      return create_token(current_char, MULTIPLY, pos);

  case '/':
    if (next_char == '=') {
      next(lexer);
      pos.end++;
      return create_token(value, ASSIGNMENT_DIVIDE, pos);
    } else
      return create_token(current_char, DIVIDE, pos);

  case '%':
    if (next_char == '=') {
      next(lexer);
      pos.end++;
      return create_token(value, ASSIGNMENT_MODULE, pos);
    } else
      return create_token(current_char, MODULE, pos);

  case '!':
    if (next_char == '=') {
      next(lexer);
      pos.end++;
      return create_token(value, NOT_EQUAL, pos);
    } else
      return create_token(current_char, NOT, pos);

  case '|':
    if (next_char == '|') {
      next(lexer);
      pos.end++;
      return create_token(value, OR, pos);
    } else
      return create_token(current_char, OR_TYPE, pos);

  case '&':
    if (next_char == '&') {
      next(lexer);
      pos.end++;
      return create_token(value, AND, pos);
    }
    return create_token(current_char, BITWISE_AND, pos);

  case '$':
    return create_token(current_char, BITWISE_OR, pos);

  case '^':
    return create_token(current_char, BITWISE_XOR, pos);

  case '~':
    return create_token(current_char, BITWISE_NOT, pos);

  case '=':
    if (next_char == '=') {
      next(lexer);
      pos.end++;
      return create_token(value, EQUAL, pos);
    } else if (next_char == '>') {
      next(lexer);
      pos.end++;
      return create_token(value, RETURN_OPERATOR, pos);
    } else
      return create_token(current_char, ASSIGNMENT_OPERATOR, pos);

  case '<':
    if (next_char == '=') {
      next(lexer);
      pos.end++;
      return create_token(value, LESS_EQUAL, pos);
    } else if (next_char == '<') {
      next(lexer);
      pos.end++;
      return create_token(value, LEFT_SHIFT, pos);
    } else
      return create_token(current_char, LESS_THEN, pos);

  case '>':
    if (next_char == '=') {
      next(lexer);
      pos.end++;
      return create_token(value, GREATER_EQUAL, pos);
    } else if (next_char == '>') {
      next(lexer);
      pos.end++;
      return create_token(value, RIGHT_SHIFT, pos);
    } else
      return create_token(current_char, GREATER_THEN, pos);

  case ':':
    if (next_char == '=') {
      next(lexer);
      pos.end++;
      return create_token(value, ASSIGNMENT_MUTABLE, pos);
    } else
      return create_token(current_char, TYPE_DECLARATION, pos);

  default:
    return create_token(current_char, TERNARY_OPERATOR, pos);
  }
}

static Token *tokenize_separator(Lexer *lexer) {
  char value[2];
  value[0] = get_current_char(lexer);
  value[1] = '\0';

  TokenPosition pos = create_token_position(lexer);
  switch (value[0]) {
  case '{':
    return create_token(value, LCBRACKETS, pos);
  case '}':
    return create_token(value, RCBRACKETS, pos);
  case '[':
    return create_token(value, LBRACKETS, pos);
  case ']':
    return create_token(value, RBRACKETS, pos);
  case '(':
    return create_token(value, LPARENTESES, pos);
  case ')':
    return create_token(value, RPARENTESES, pos);
  case ';':
    return create_token(value, SEMICOLON, pos);
  case ',':
    return create_token(value, COMMA, pos);
  default:
    if (peek(lexer) == '.') {
      next(lexer);
      pos.end++;
      return create_token("..", SPREAD, pos);
    }
    return create_token(value, DOT, pos);
  }
}

static Token *tokenize_keyword_identifier(Lexer *lexer) {
  char c = get_current_char(lexer);
  if (is_digit(c) || is_operator(c) || is_separator(c) || is_whitespace(c) ||
      !is_alphanumeric(c) || is_at_end(lexer))
    return NULL;

  TokenPosition pos = create_token_position(lexer);

  if (match("break", lexer)) {
    pos.end = lexer->pos->index;
    return create_token("break", BREAK, pos);
  } else if (match("boolean", lexer)) {
    pos.end = lexer->pos->index;
    return create_token("boolean", BOOLEAN, pos);
  }

  else if (match("const", lexer)) {
    pos.end = lexer->pos->index;
    return create_token("const", CONST, pos);
  } else if (match("char", lexer)) {
    pos.end = lexer->pos->index;
    return create_token("char", CHAR, pos);
  } else if (match("case", lexer)) {
    pos.end = lexer->pos->index;
    return create_token("case", CASE, pos);
  } else if (match("continue", lexer)) {
    pos.end = lexer->pos->index;
    return create_token("continue", CONTINUE, pos);
  }

  else if (match("double", lexer)) {
    pos.end = lexer->pos->index;
    return create_token("double", DOUBLE, pos);
  } else if (match("do", lexer)) {
    pos.end = lexer->pos->index;
    return create_token("do", DO, pos);
  }

  else if (match("enum", lexer)) {
    pos.end = lexer->pos->index;
    return create_token("enum", ENUM, pos);
  }
  /*
  else if (match("eof", lexer)) {
    pos.end = lexer->pos->index;
    return create_token("eof", TK_EOF, pos);

  }*/
  else if (match("else", lexer)) {
    pos.end = lexer->pos->index;
    return create_token("else", ELSE, pos);
  }

  else if (match("float", lexer)) {
    pos.end = lexer->pos->index;
    return create_token("float", FLOAT, pos);
  } else if (match("for", lexer)) {
    pos.end = lexer->pos->index;
    return create_token("for", FOR, pos);
  } else if (match("foreach", lexer)) {
    pos.end = lexer->pos->index;
    return create_token("foreach", FOREACH, pos);
  } else if (match("f8", lexer)) {
    pos.end = lexer->pos->index;
    return create_token("f8", F8, pos);
  } else if (match("f16", lexer)) {
    pos.end = lexer->pos->index;
    return create_token("f16", F16, pos);
  } else if (match("f32", lexer)) {
    pos.end = lexer->pos->index;
    return create_token("f32", F32, pos);
  } else if (match("f64", lexer)) {
    pos.end = lexer->pos->index;
    return create_token("f64", F64, pos);
  } else if (match("false", lexer)) {
    pos.end = lexer->pos->index;
    return create_token("false", FALSE, pos);
  } else if (match("from", lexer)) {
    pos.end = lexer->pos->index;
    return create_token("from", FROM, pos);
  }

  else if (match("if", lexer)) {
    pos.end = lexer->pos->index;
    return create_token("if", IF, pos);
  } else if (match("import", lexer)) {
    pos.end = lexer->pos->index;
    return create_token("import", IMPORT, pos);
  } else if (match("int", lexer)) {
    pos.end = lexer->pos->index;
    return create_token("int", INT, pos);
  } else if (match("i8", lexer)) {
    pos.end = lexer->pos->index;
    return create_token("i8", I8, pos);
  } else if (match("i16", lexer)) {
    pos.end = lexer->pos->index;
    return create_token("i16", I16, pos);
  } else if (match("i32", lexer)) {
    pos.end = lexer->pos->index;
    return create_token("i32", I32, pos);
  } else if (match("i64", lexer)) {
    pos.end = lexer->pos->index;
    return create_token("i64", I64, pos);
  }

  else if (match("long", lexer)) {
    pos.end = lexer->pos->index;
    return create_token("long", LONG, pos);
  }

  else if (match("null", lexer)) {
    pos.end = lexer->pos->index;
    return create_token("null", TK_NULL, pos);
  }

  else if (match("return", lexer)) {
    pos.end = lexer->pos->index;
    return create_token("return", RETURN, pos);
  }

  else if (match("sizeof", lexer)) {
    pos.end = lexer->pos->index;
    return create_token("sizeof", SIZEOF, pos);
  } else if (match("struct", lexer)) {
    pos.end = lexer->pos->index;
    return create_token("struct", STRUCT, pos);
  } else if (match("string", lexer)) {
    pos.end = lexer->pos->index;
    return create_token("string", STRING, pos);
  } else if (match("switch", lexer)) {
    pos.end = lexer->pos->index;
    return create_token("switch", SWITCH, pos);
  }

  else if (match("typeof", lexer)) {
    pos.end = lexer->pos->index;
    return create_token("typeof", TYPEOF, pos);
  } else if (match("true", lexer)) {
    pos.end = lexer->pos->index;
    return create_token("true", TRUE, pos);
  }

  else if (match("void", lexer)) {
    pos.end = lexer->pos->index;
    return create_token("void", VOID, pos);
  }

  else if (match("while", lexer)) {
    pos.end = lexer->pos->index;
    return create_token("while", WHILE, pos);
  }

  i64 start = lexer->pos->index;
  while (is_alphanumeric(peek(lexer)) || peek(lexer) == '_')
    next(lexer);

  if (!is_alphanumeric(c) && !is_whitespace(c) && !is_operator(c) &&
      !is_separator(c))
    throw_lexer_error("SpellingError",
                      "Identifier must start with a letter or _ and cannot "
                      "have any special character",
                      lexer->pos);
  i64 end = lexer->pos->index;

  char *value = cut_string(lexer, start, end);
  pos.end = lexer->pos->index == pos.start ? lexer->pos->index + 1
                                           : lexer->pos->index;
  return create_token(value, IDENTIFIER, pos);
}

static Token *tokenize_numeric(Lexer *lexer) {
  i64 start = lexer->pos->index;
  int dot_count = 0;

  TokenPosition pos = create_token_position(lexer);
  while (is_digit(peek(lexer)) || peek(lexer) == '.') {
    if (peek(lexer) == '.') {
      dot_count++;
    }
    next(lexer);
  }
  i64 end = lexer->pos->index;

  if (is_alphanumeric(peek(lexer)) || dot_count > 1)
    throw_lexer_error("LexicalError", "Doesn't belong within 0-9 range",
                      lexer->pos);

  char *value = cut_string(lexer, start, end);
  pos.end = lexer->pos->index == pos.start ? lexer->pos->index + 1
                                           : lexer->pos->index;
  if (dot_count == 1)
    return create_token(value, FLOAT_LITERAL, pos);
  return create_token(value, INT_LITERAL, pos);
}

static Token *tokenize_strings(Lexer *lexer) {
  Position *p = copy_position(lexer);
  TokenPosition pos = create_token_position(lexer);

  if (get_current_char(lexer) == '\'') {
    char value[2];
    value[0] = peek(lexer);
    value[1] = '\0';

    next(lexer);
    if (peek(lexer) == '\'') {
      free(p), next(lexer);
      pos.end = lexer->pos->index;
      return create_token(value, CHAR_LITERAL, pos);
    }
    throw_lexer_error(
        "UnmatchedString",
        "ending of character is not present but the beginning is present", p);
  }

  next(lexer);
  i64 start = lexer->pos->index;
  while (get_current_char(lexer) != '"') {
    next(lexer);
    if (is_at_end(lexer))
      throw_lexer_error(
          "UnmatchedString",
          "ending of string is not present but the beginning is present", p);
  }
  i64 end = lexer->pos->index - 1;
  free(p);

  pos.end = lexer->pos->index;
  char *value = cut_string(lexer, start, end);
  return create_token(value, STRING_LITERAL, pos);
}

static void append_token(TokensList *tokens, Token *token) {
  i64 length = strlen(token->value);

  Token *node = (Token *)malloc(sizeof(*token));
  node->type = token->type;
  node->value = malloc(length + 1);
  node->pos = token->pos;

  node->value[length] = '\0';
  memcpy(node->value, token->value, length);

  node->prev = node->next = NULL;
  if (tokens->head == NULL) {
    tokens->tail = tokens->head = node;
  } else {
    tokens->tail->next = node;
    node->prev = tokens->tail;
    tokens->tail = node;
  }
  free(token);
}

Lexer *create_lexer(const char *file_location, const char *source) {
  Lexer *lexer = (Lexer *)malloc(sizeof(Lexer));
  Position *pos = (Position *)malloc(sizeof(Position));
  if (lexer == NULL || pos == NULL) {
    fprintf(stderr, "MallocError: No memory to allocate\n");
    exit(EXIT_FAILURE);
  }

  const i64 length = strlen(source);

  lexer->length = length;
  lexer->source = source;
  lexer->character = source[0];

  pos->index = 0;
  pos->line = 1;
  pos->column = 1;
  pos->file_location = file_location;
  lexer->pos = pos;
  return lexer;
}

void free_lexer(Lexer *lexer) {
  free(lexer->pos), lexer->pos = NULL;
  free(lexer), lexer = NULL;
}

TokensList *tokenizer(Lexer *lexer) {
  TokensList *tokens = (TokensList *)malloc(sizeof(TokensList));
  tokens->head = tokens->tail = NULL;

  Token *token = NULL;
  while (!is_at_end(lexer)) {
    skip(lexer);

    token = tokenize_keyword_identifier(lexer);
    if (token) {
      append_token(tokens, token);
    } else if (is_operator(get_current_char(lexer))) {
      append_token(tokens, tokenize_operator(lexer));
    } else if (is_separator(get_current_char(lexer))) {
      append_token(tokens, tokenize_separator(lexer));
    } else if (is_digit(get_current_char(lexer))) {
      append_token(tokens, tokenize_numeric(lexer));
    } else if (is_string(get_current_char(lexer))) {
      append_token(tokens, tokenize_strings(lexer));
    } else if (is_at_end(lexer))
      break;
    else {
      debug_lexer_position(lexer);
      throw_lexer_error("IllegalCharacter", "Illegal character", lexer->pos);
    }

    next(lexer);
  }

  append_token(tokens, create_token("EOF", TK_EOF, create_token_position(lexer)));
  return tokens;
}
