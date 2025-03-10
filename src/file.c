#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "include/mdb.h"

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

void writedb(char *filename, struct keytablist *list, int len)
{
  FILE *fp = fopen(filename, "wb");
  for (int i = 0; i < len; ++i) {
    fprintf(fp, "\xfb");
    int *indexes = getkeys(list, i);
    for (int j = 0; indexes[j]; ++j) {
      fprintf(fp, "\xfa%s:", list[i].tab[indexes[j]].key);
      switch (list[i].tab[indexes[j]].flag) {
        case 1:
          fprintf(fp, "%.2lf", list[i].tab[indexes[j]].v.num);
          break;
        case 2:
          fprintf(fp, "%s", list[i].tab[indexes[j]].v.b == 1 ? "true" : "false");
          break;
        case 3:
          fprintf(fp, "%s", list[i].tab[indexes[j]].v.str);
          break;
      }
    }
    free(indexes);
    fprintf(fp, "\xfe");
  }
}
