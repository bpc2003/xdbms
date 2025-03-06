#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "include/fileops.h"

int main(int argc, char **argv)
{
  if (argc != 2)
    exit(1);
  char *filename = argv[1];
  
  uint8_t *buf = readdb(filename);
  if (buf == NULL)
    exit(0);

  printf("[ ");
  for (int i = 0; buf[i]; ++i) {
    printf("%d ", buf[i]);
  }
  printf("]\n");
  free(buf);
  exit(0);
}
