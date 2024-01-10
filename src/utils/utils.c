#include "./utils.h"
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *read_file(const char *file_location) {
  FILE *file = fopen(file_location, "r");
  if (file == NULL)
    goto file_not_found;

  // Get file length
  fseek(file, 0, SEEK_END);
  long length = ftell(file);
  fseek(file, 0, SEEK_SET);

  char *source = malloc(sizeof(char) * (length + 1));
  if (source == NULL)
    goto error_mem_size;

  // Get file content
  source[length] = '\0';
  fread(source, sizeof(char), length, file);
  fclose(file);
  return source;

error_mem_size:
  fprintf(stderr, "MallocError: %s.\n", strerror(errno));
  fclose(file);
  exit(EXIT_FAILURE);

file_not_found:
  fprintf(stderr, "FileError: file %s not found\n", file_location);
  exit(EXIT_FAILURE);
}
