#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "mdb.h"

static char *getpair(int *c, FILE *fp);

struct keytablist *readdb(char *filename)
{
  int len = 2;
  struct keytablist *list = calloc(len, sizeof(struct keytablist));
  FILE *fp = fopen(filename, "rb");
  if (fp == NULL)
    return list;

  int c, i = 0, open = 0;
  char *p;
  while ((c = fgetc(fp)) != EOF) {
    switch (c) {
      case 250:
        p = getpair(&c, fp);
        if (p == NULL) {
          fprintf(stderr, "missing pair closing byte!\n");
          return NULL;
        }
        setkey(&list, &len, i, p);
        free(p);
        break;
      case 251:
        if (open == 1) {
          fprintf(stderr, "missing object closing byte!\n");
          return NULL;
        }
        open = 1;
        break;
      case 254:
        open = 0;
        i++;
        break;
      default:
        fprintf(stderr, "Unknown byte: %d\n", c);
        return NULL;
    }
  }
  fclose(fp);
  return list;
}

void writedb(char *filename, struct keytablist *list)
{
  FILE *fp = fopen(filename, "wb");
  for (int i = 0; i < list[0].len; ++i) {
    fprintf(fp, "\xfb");
    int *indexes = getkeys(list, i);
    for (int j = 0; indexes[j]; ++j) {
      fprintf(fp, "\xfa%s:", list[i].tab[indexes[j]].key);
      switch (list[i].tab[indexes[j]].flag) {
        case 1:
          fprintf(fp, "%.2lf\xfc", list[i].tab[indexes[j]].v.num);
          break;
        case 2:
          fprintf(fp, "%s\xfc", list[i].tab[indexes[j]].v.b == 1 ? "true" : "false");
          break;
        case 3:
          fprintf(fp, "%s\xfc", list[i].tab[indexes[j]].v.str);
          break;
      }
    }
    free(indexes);
    fprintf(fp, "\xfe");
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
