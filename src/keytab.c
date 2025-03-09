#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "include/keytab.h"

static int hash(char *key);

int *getkeys(struct keytab *tab)
{
  int len = 2;
  int *indexes = calloc(len, sizeof(int));
  for (int i = 0, j = 0; i < TABLEN; ++i) {
    if (j >= len) {
      indexes = realloc(indexes, ++len * sizeof(int));
      indexes[len - 1] = 0;
    }
    if (tab[i].key)
      indexes[j++] = i;
  }
  return indexes;
}

struct keytab getkey(struct keytab *tab, char *key)
{
  int idx = hash(key);
  if (tab[idx].key == NULL)
    return (struct keytab) { .key = NULL, .v = 0 };

  while (strcmp(tab[idx].key, key) && idx < TABLEN)
    idx++;
  if (idx >= TABLEN)
    return (struct keytab) { .key = NULL, .v = 0 };
  return tab[idx];
}

void setkey(struct keytab *tab, char *pair)
{
  char *tok = strtok(pair, ":");
  char *key = calloc(strlen(tok) + 1, sizeof(char));
  strcpy(key, tok);

  tok = strtok(NULL, ":");
  if (tok == NULL) {
    fprintf(stderr, "Invalid key-value pair\n");
    return;
  }
  union value v;
  int flag;
  if (isdigit(*tok)) {
    flag = 1;
    v.num = atof(tok);
  } else if (!strcmp(tok, "true") || !strcmp(tok, "false")) {
    flag = 2;
    v.b = !strcmp(tok, "true");
  } else {
    flag = 3;
    v.str = calloc(strlen(tok) + 1, sizeof(char));
    strcpy(v.str, tok);
  }

  int idx = hash(key);
  while (tab[idx].key != NULL && strcmp(tab[idx].key, key) && idx < TABLEN)
    idx++;
  if (idx >= TABLEN) {
    fprintf(stderr, "No more room in table\n");
    return;
  }
  if (!tab[idx].key)
    tab[idx].key = key;
  else
    free(key);
  tab[idx].v = v;
  tab[idx].flag = flag;
}

void delkey(struct keytab *tab, char *key)
{
  int idx = hash(key);
  while (strcmp(tab[idx].key, key) && idx < TABLEN)
    idx++;
  if (idx >= TABLEN) {
    fprintf(stderr, "Invalid key: %s\n", key);
    return;
  }
  free(tab[idx].key);
  tab[idx].key = NULL;
  if (tab[idx].flag == 3) {
    free(tab[idx].v.str);
    tab[idx].v.str = NULL;
  }
  tab[idx].flag = 0;
}

static int hash(char *key)
{
  unsigned long h = 5381;
  for (int i = 0; i < strlen(key); ++i)
    h = ((h << 5) + h) + key[i];
  return h % TABLEN;
}
