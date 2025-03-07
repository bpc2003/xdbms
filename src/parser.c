#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "include/parser.h"

int blen;

static void addbyte(struct byte *bytes, int *pos, enum btype type, char *value);
static char *getpair(uint8_t *buf, int *pos);

struct byte *parse(uint8_t *buf)
{
  char *value;
  int len = 2;
  struct byte *bytes = calloc(len, sizeof(struct byte));
  for (int i = 0, j = 0; buf[i]; ++i) {
    if (j >= len)
      len *= 2;
      bytes = realloc(bytes, len * sizeof(struct byte));
    switch (buf[i]) {
      case 250:
        value = getpair(buf, &i);
        if (value == NULL)
          addbyte(bytes, &j, ERROR, "Invalid Key-Value pair");
        else
          addbyte(bytes, &j, PAIR, value);
        break;
      case 251:
        addbyte(bytes, &j, BEGIN, NULL);
        break;
      case 254:
        addbyte(bytes, &j, END, NULL);
        break;
      default:
        addbyte(bytes, &j, ERROR, "Invalid chunk");
    }
    blen = j;
  }
  return bytes;
}

static void addbyte(struct byte *bytes, int *pos, enum btype type, char *value)
{
  bytes[*pos].type = type;
  
  if (value != NULL) {
    bytes[*pos].value = value;
  }
  else
    bytes[*pos].value = NULL;
  
  (*pos)++;
}

static char *getpair(uint8_t *buf, int *pos)
{
  int i;
  for (i = *pos + 1; buf[i] != 250 && buf[i] != 254 && buf[i]; ++i)
    ;
  if (buf[i] != 250 && buf[i] != 254)
    return NULL;

  char *pair = calloc(i - *pos, sizeof(char));
  int j;
  for (j = *pos + 1; j < i; ++j)
    pair[j - (*pos + 1)] = buf[j];
  pair[j - (*pos + 1)] = '\0';
  *pos = i - 1;
  return pair;
}
