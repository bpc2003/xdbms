#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "engine.h"

static int check(char **known, int len, char *str);
static long getsz(FILE *fp);
static char *makepair(char *key, char *value);

// TODO: reimplement this to read file in FI (Fast Infoset) format
// TODO: make this smaller
tablist_t *readdb(char *filename)
{
  FILE *fp;
  int llen = 2, tlen = 2;
  char **ktags = calloc(tlen, sizeof(char *));
  tablist_t *list = calloc(llen, sizeof(tablist_t));
  list[0].len = llen;

  if (filename == NULL || (fp = fopen(filename, "rb")) == NULL)
    return list;

  long sz = getsz(fp);
  char *buf = calloc(sz, sizeof(char));
  fread(buf, sizeof(char), sz, fp);
  char *v, *k;
  for (int i = 0, j = 0, id = 0; i < sz; ++i) {
    if (buf[i] == '\n')
      continue;
    if (j >= tlen)
      ktags = realloc(ktags, ++tlen * sizeof(char *));
    if (buf[i] == '<' || buf[i] == ':') {
      if (buf[i] == '<') {
        int t;
        for (t = i + 1; buf[t] != '>'; ++t) ;
        ktags[j] = calloc(t - i, sizeof(char));
        strncpy(ktags[j++], buf + i + 1, t - i - 1);
        k = ktags[j - 1];
        i += t - i;
      } else {
        int t;
        for (t = i + 1; buf[t] != '\n'; ++t) ;
        v = calloc(t - i, sizeof(char));
        strncpy(v, buf + i + 1, t - i - 1);
        i += t - i;
        char *pair = makepair(k, v);
        setkeys(&list, id, &pair, 1);
        free(pair);
        free(v);
      }
    } else {
      int idx = 0;
      memcpy(&idx, buf + i, sizeof(int));
      i += sizeof(int) - 1;
      if (idx == 1)
        id++;
      k = ktags[idx];
    }
  }

  for (int i = 0; i < tlen - 1; ++i)
    free(ktags[i]);
  free(ktags);
  free(buf);
  fclose(fp);
  return list;
}

// TODO: Make this write to a file in the FI (Fast Infoset) format
// TODO: Make this smaller
void writedb(char *filename, tablist_t *list)
{
  FILE *fp = fopen(filename, "wb");
  if (fp == NULL)
    return;
  int len = 2;
  char **ktags = calloc(len, sizeof(char *));

  fprintf(fp, "<documents>\n");
  fprintf(fp, "<document>\n");
  ktags[0] = "documents";
  ktags[1] = "document";
  tablist_t *indexes = getkeys(list, -1, NULL, 0);
  for (int i = 0, j = 2; i < indexes[0].len; ++i) {
    int n = 1;
    if (i > 0)
      fwrite(&n, sizeof(int), 1, fp);
    for (int k = 0; indexes[i].tab[k].flag; ++k) {
      if ((n = check(ktags, len, indexes[i].tab[k].key)) >= 0)
        fwrite(&n, sizeof(int), 1, fp);
      else {
        fprintf(fp, "<%s>", indexes[i].tab[k].key);
        if (j >= len)
          ktags = realloc(ktags, ++len * sizeof(char *));
        ktags[j] = calloc(strlen(indexes[i].tab[k].key) + 1, sizeof(char));
        strcpy(ktags[j++], indexes[i].tab[k].key);
      }
      switch (indexes[i].tab[k].flag) {
        case 1:
          fprintf(fp, ":%g\n", indexes[i].tab[k].value.num);
          break;
        case 2:
          fprintf(fp, ":%s\n", indexes[i].tab[k].value.boolean ? "true" : "false");
          break;
        case 3:
          fprintf(fp, ":%s\n", indexes[i].tab[k].value.str);
          break;
      }
    }
  }
  for (int i = 2; i < len; ++i)
    free(ktags[i]);
  free(ktags);
  free(indexes);
  fclose(fp);
}

static int check(char **known, int len, char *str)
{
  for (int i = 0; i < len; ++i) {
    if (!strcmp(str, known[i]))
      return i;
  }
  return -1;
}

static long getsz(FILE *fp)
{
  fseek(fp, 0, SEEK_END);
  long sz = ftell(fp);
  fseek(fp, 0, SEEK_SET);
  return sz;
}

static char *makepair(char *key, char *value)
{
  char *pair = calloc(strlen(key) + strlen(value) + 2, sizeof(char));
  sprintf(pair, "%s:%s", key, value);
  return pair;
}
