#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "include/fileops.h"
#include "include/parser.h"

int main(int argc, char **argv)
{
  if (argc != 2)
    exit(1);
  char *filename = argv[1];
  
  uint8_t *buf = readdb(filename);
  if (buf == NULL)
    exit(0);
  struct byte *bytes = parse(buf);
  for (int i = 0; i < blen; ++i) {
    printf("%d\n", bytes[i].type);
  }

  free(bytes);
  free(buf);
  exit(0);
}
