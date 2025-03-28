#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "mdb.h"

static int hash(char *key);
static char **getkv(char *pair);

// getkeys - gets every single key in a key table
int *getkeys(tablist_t *list, int id)
{
  int len = 2;
  if (id >= list[0].len)
    return NULL;
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

// getkey - gets a single key from a keytable
// if it can't find the key it will return an empty table index
tabidx_t getkey(tablist_t *list, int id, char *key)
{
  int idx = hash(key);
  if (list[id].tab[idx].key == NULL)
    return (tabidx_t) { .key = NULL, .flag = 0, .value = { 0 } };

  while (strcmp(list[id].tab[idx].key, key) && idx < TABLEN)
    idx++;
  if (idx >= TABLEN)
    return (tabidx_t) { .key = NULL, .flag = 0, .value = { 0 } };
  return list[id].tab[idx];
}

// setkey - adds the given pair to the given key table
// if setkey fails it will return 1 otherwise 0
int setkey(tablist_t **list, int id, char *pair)
{
  if (pair == NULL)
    return 1;
  if (id >= (*list)[0].len) {
    *list = realloc(*list, (id + 1) * sizeof(tablist_t));
    for (int i = (*list)[0].len; i <= id; ++i) {
      for (int j = 0; j < TABLEN; ++j)
        (*list)[i].tab[j] = (tabidx_t) { NULL, 0, { 0 } };
    }
    (*list)[0].len = id + 1;
  }
  char **kv = getkv(pair);
  if (kv == NULL)
    return 2;

  int idx = hash(kv[0]);
  while ((*list)[id].tab[idx].key != NULL &&
    strcmp((*list)[id].tab[idx].key, kv[0]) &&
    idx < TABLEN) idx++;
  if (idx >= TABLEN)
    return 2;
  if (!(*list)[id].tab[idx].key)
    (*list)[id].tab[idx].key = kv[0];
  else {
    free(kv[0]);
    if ((*list)[id].tab[idx].flag == 3)
      free((*list)[id].tab[idx].value.str);
  }
  char *end;
  double num = strtod(kv[1], &end);
  if (*end == '\0') {
    (*list)[id].tab[idx].flag = 1;
    (*list)[id].tab[idx].value.num = num;
  } else if (!strcmp(kv[1], "true") || !strcmp(kv[1], "false")) {
    (*list)[id].tab[idx].flag = 2;
    (*list)[id].tab[idx].value.boolean = !strcmp(kv[1], "true");
  } else {
    (*list)[id].tab[idx].flag = 3;
    (*list)[id].tab[idx].value.str = calloc(strlen(kv[1]) + 1, sizeof(char));
    strcpy((*list)[id].tab[idx].value.str, kv[1]);
  }
  free(kv[1]);
  free(kv);
  return 0;
}

// delkey - removes the given key from the given key table
int delkey(tablist_t *list, int id, char *key)
{
  int idx = hash(key);
  if (list[id].tab[idx].key == NULL)
    return 1;
  while (strcmp(list[id].tab[idx].key, key) && idx < TABLEN)
    idx++;
  if (idx >= TABLEN)
    return 2;
  free(list[id].tab[idx].key);
  list[id].tab[idx].key = NULL;
  if (list[id].tab[idx].flag == 3) {
    free(list[id].tab[idx].value.str);
    list[id].tab[idx].value.str = NULL;
  }
  list[id].tab[idx].flag = 0;
  return 0;
}

static int hash(char *key)
{
  unsigned long h = 5381;
  for (int i = 0; i < strlen(key); ++i)
    h = ((h << 5) + h) + key[i];
  return h % TABLEN;
}

static char **getkv(char *pair)
{
  char **kv = calloc(2, sizeof(char *));
  int i = 0;
  while (pair[i] != ':' && i < strlen(pair))
    i++;
  if (i >= strlen(pair)) {
    free(kv);
    return NULL;
  }
  kv[0] = calloc(i + 1, sizeof(char));
  strncpy(kv[0], pair, i);
  kv[1] = calloc(strlen(pair) - i, sizeof(char));
  strcpy(kv[1], pair + i + 1);
  return kv;
}
