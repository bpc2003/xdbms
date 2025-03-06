#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "include/fileops.h"

uint8_t *readdb(char *filename)
{
  FILE *fp = fopen(filename, "rb");
  if (fp == NULL)
    return NULL;

  int len = 10;
  uint8_t *buf = calloc(len, sizeof(uint8_t));
  int c, i = 0;
  while ((c = fgetc(fp)) != EOF) {
    if (i >= len) {
      len *= 2;
      buf = realloc(buf, len * sizeof(uint8_t));
      memset(buf + i, 0, (len - i) * sizeof(uint8_t));
    }
    buf[i++] = c;
  }

  fclose(fp);
  return buf;
}
