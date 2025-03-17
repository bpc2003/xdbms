#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "mdb.h"

static char *getpair(int *c, FILE *fp);

// readdb - reads the given file into a key table list
// if fp returns NULL it will return the empty list
// if readdb fails it will return NULL
tablist_t *readdb(char *filename)
{
  int len = 2;
  tablist_t *list = calloc(len, sizeof(tablist_t));
  list[0].len = len;
  FILE *fp = fopen(filename, "rb");
  if (fp == NULL)
    return list;

  int c, i = 0, open = 0;
  char *p;
  while ((c = fgetc(fp)) != EOF) {
    switch (c) {
      case 250:
        p = getpair(&c, fp);
        if (p == NULL)
          goto fail;
        setkey(&list, i, p);
        free(p);
        break;
      case 251:
        if (open == 1)
          goto fail;
        open = 1;
        break;
      case 254:
        open = 0;
        i++;
        break;
      default:
        return NULL;
    }
  }
  fclose(fp);
  return list;

fail:
  fclose(fp);
  for (int i = 0; i < list[0].len; ++i) {
    int *indexes = getkeys(list, i);
    for (int j = 0; indexes[j]; ++j)
      delkey(list, i, list[i].tab[indexes[j]].key);
    free(indexes);
  }
  free(list);
  return NULL;
}

// writedb - writes a keytablist to a given file
void writedb(char *filename, tablist_t *list)
{
  FILE *fp = fopen(filename, "wb");
  for (int i = 0; i < list[0].len; ++i) {
    fputc(0xFB, fp);
    int *indexes = getkeys(list, i);
    for (int j = 0; indexes[j]; ++j) {
      fprintf(fp, "\xfa%s:", list[i].tab[indexes[j]].key);
      switch (list[i].tab[indexes[j]].flag) {
        case 1:
          fprintf(fp, "%.2lf\xfc", list[i].tab[indexes[j]].value.num);
          break;
        case 2:
          fprintf(fp, "%s\xfc", list[i].tab[indexes[j]].value.boolean ?
                  "true" : "false");
          break;
        case 3:
          fprintf(fp, "%s\xfc", list[i].tab[indexes[j]].value.str);
          break;
      }
    }
    free(indexes);
    fputc(0xFE, fp);
  }
  fclose(fp);
}

static char *getpair(int *c, FILE *fp)
{
  char *pair = calloc(3, sizeof(char));
  int i = 0, len = 3;
  while ((*c = fgetc(fp)) != 252 && *c != EOF) {
    if (i >= len)
      pair = realloc(pair, ++len * sizeof(char));
    pair[i++] = *c;
  }
  pair = realloc(pair, ++len * sizeof(char));
  pair[i] = '\0';
  if (*c != 252) {
    free(pair);
    return NULL;
  }
  return pair;
}
