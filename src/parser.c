#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "include/parser.h"

int blen;

static void addbyte(struct byte *bytes, int *pos, enum btype type, char *value);

struct byte *parse(uint8_t *buf)
{
  int len = 2;
  struct byte *bytes = calloc(len, sizeof(struct byte));
  for (int i = 0, j = 0; buf[i]; ++i) {
    if (j >= len)
      len *= 2;
      bytes = realloc(bytes, len * sizeof(struct byte));
    switch (buf[i]) {
      case '\xb':
        addbyte(bytes, &j, BEGIN, NULL);
        break;
      case '\xe':
        addbyte(bytes, &j, END, NULL);
        break;
    }
    blen = j;
  }
  return bytes;
}

static void addbyte(struct byte *bytes, int *pos, enum btype type, char *value)
{
  bytes[*pos].type = type;
  
  if (value != NULL) {
    bytes[*pos].value = calloc(strlen(value) + 1, sizeof(char));
    strcpy(bytes[*pos].value, value);
  }
  else
    bytes[*pos].value = NULL;
  
  (*pos)++;
}
