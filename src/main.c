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
  free(buf);

  for (int i = 0; i < blen; ++i) {
    switch (bytes[i].type) {
      case BEGIN:
        printf("BEGIN\n");
        break;
      case END:
        printf("END\n");
        break;
      case PAIR:
        printf("PAIR: %s\n", bytes[i].value);
        free(bytes[i].value);
        break;
      case ERROR:
        fprintf(stderr, "%s\n", bytes[i].value);
        exit(1);
    }
  }

  free(bytes);
  exit(0);
}
