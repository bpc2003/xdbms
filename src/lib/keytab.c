#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "mdb.h"

static const struct keytablist empty;

static int hash(char *key);

int *getkeys(struct keytablist *list, int id)
{
  int len = 2;
  int *indexes = calloc(len, sizeof(int));
  for (int i = 0, j = 0; i < TABLEN; ++i) {
    if (j >= len) {
      indexes = realloc(indexes, ++len * sizeof(int));
      indexes[len - 1] = 0;
    }
    if (list[id].tab[i].key)
      indexes[j++] = i;
  }
  return indexes;
}

struct keytab getkey(struct keytablist *list, int id, char *key)
{
  int idx = hash(key);
  if (list[id].tab[idx].key == NULL)
    return (struct keytab) { .key = NULL, .flag = 0, .v = 0 };

  while (strcmp(list[id].tab[idx].key, key) && idx < TABLEN)
    idx++;
  if (idx >= TABLEN)
    return (struct keytab) { .key = NULL, .flag = 0, .v = 0 };
  return list[id].tab[idx];
}

int setkey(struct keytablist **list, int id, char *pair)
{
  if (id >= (*list)[0].len) {
    *list = realloc(*list, (id + 1) * sizeof(struct keytablist));
    for (int i = (*list)[0].len; i <= id; ++i) {
      for (int j = 0; j < TABLEN; ++j)
        (*list)[i].tab[j] = (struct keytab) {NULL, 0, {0}};
    }
    (*list)[0].len = id + 1;
  }
  char *tok = strtok(pair, ":");
  char *key = calloc(strlen(tok) + 1, sizeof(char));
  strcpy(key, tok);

  tok = strtok(NULL, ":");
  if (tok == NULL) {
    fprintf(stderr, "Invalid key-value pair\n");
    return 1;
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
  while ((*list)[id].tab[idx].key != NULL &&
    strcmp((*list)[id].tab[idx].key, key) &&
    idx < TABLEN) idx++;
  if (idx >= TABLEN) {
    fprintf(stderr, "No more room in table\n");
    return 2;
  }
  if (!(*list)[id].tab[idx].key)
    (*list)[id].tab[idx].key = key;
  else
    free(key);
  (*list)[id].tab[idx].v = v;
  (*list)[id].tab[idx].flag = flag;
  return 0;
}

void delkey(struct keytablist *list, int id, char *key)
{
  int idx = hash(key);
  if (list[id].tab[idx].key == NULL)
    return;
  while (strcmp(list[id].tab[idx].key, key) && idx < TABLEN)
    idx++;
  if (idx >= TABLEN) {
    fprintf(stderr, "Invalid key: %s\n", key);
    return;
  }
  free(list[id].tab[idx].key);
  list[id].tab[idx].key = NULL;
  if (list[id].tab[idx].flag == 3) {
    free(list[id].tab[idx].v.str);
    list[id].tab[idx].v.str = NULL;
  }
  list[id].tab[idx].flag = 0;
}

static int hash(char *key)
{
  unsigned long h = 5381;
  for (int i = 0; i < strlen(key); ++i)
    h = ((h << 5) + h) + key[i];
  return h % TABLEN;
}
