#include "./lexer/lexer.h"
#include "./utils/utils.h"
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
  char *file_location = "code/test.monkc";
  char *source = read_file(file_location);
  printf("CODE  ↴\n");
  printf(" %s\n", source);

  printf("LOGS ⚠ ↴\n");
  Lexer *lexer = create_lexer(file_location, source);
  TokensList *tokens = tokenizer(lexer);

  // free(source), free_lexer(lexer);
  Token *head = tokens->head;
  while (head != NULL) {
    print_token(head);
    head = head->next;
  }
}
