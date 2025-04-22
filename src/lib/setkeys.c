#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <threads.h>

#include "mdb.h"
#include "utils.h"

static int setkey_helper(void *thr_data);
static int setkey(tablist_t **, int, char *);
static char **getkv(char *pair);

tablist_t *setkey_copy;
char *setkey_str;

mtx_t setkey_mtx;
int setkeys(tablist_t **list, int id, char **pairs, int len)
{
  if (id < -1 || pairs == NULL || len <= 0)
    return -1;
  if (mtx_init(&setkey_mtx, mtx_plain) != thrd_success)
    return -2;
  int rc = 0;
  setkey_copy = calloc((*list)[0].len, sizeof(tablist_t));
  copytab(setkey_copy, *list);

  for (int i = 0; i < len; ++i) {
    setkey_str = pairs[i];
    if (id == -1) {
      thrd_t *thrds = calloc((*list)[0].len, sizeof(thrd_t));
      for (int i = 0; i < setkey_copy[0].len; ++i)
        thrd_create(&thrds[i], setkey_helper, clone(i));
      for (int i = 0; i < setkey_copy[0].len; ++i) {
        if (rc)
          thrd_join(thrds[i], NULL);
        else
          thrd_join(thrds[i], &rc);
      }
      free(thrds);
    }
    else
     rc = setkey_helper(clone(id));
  }

  if (!rc) {
    if (setkey_copy[0].len > (*list)[0].len)
      *list = realloc(*list, setkey_copy[0].len * sizeof(tablist_t));
    dellist(*list);
    copytab(*list, setkey_copy);
  }
  mtx_destroy(&setkey_mtx);
  dellist(setkey_copy);
  free(setkey_copy);
  return rc;
}

static int setkey_helper(void *thr_data)
{
  int rc;
  int *id = (int *) thr_data;

  mtx_lock(&setkey_mtx);
  rc = setkey(&setkey_copy, *id, setkey_str);
  mtx_unlock(&setkey_mtx);

  free(id);
  return rc;
}

static int setkey(tablist_t **list, int id, char *pair)
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
