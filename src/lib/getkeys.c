#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <threads.h>

#include "mdb.h"
#include "utils.h"

static tabidx_t getkey(tablist_t *list, int id, char *key);

static int getkeys_helper(void *data);

struct params {
  tablist_t *list;
  tablist_t *ret;
  char **keys;
  int len;
  int lid;
  int pid;
};

static struct params *pass(tablist_t *list, tablist_t *ret, char **keys, int len, int lid, int pid);

mtx_t mtx;

tablist_t *getkeys(tablist_t *list, int id, char **keys, int klen)
{
  if (id >= list[0].len || id < -1 || mtx_init(&mtx, mtx_plain) != thrd_success)
    return NULL;
  int rc = 0;
  int len = id == -1 ? list[0].len : 1;
  tablist_t *indexes = calloc(len, sizeof(tablist_t));
  indexes[0].len = len;
  if (id >= 0)
    rc = getkeys_helper(pass(list, indexes, keys, klen, id, 0));
  else {
    thrd_t *thrds = calloc(list[0].len, sizeof(thrd_t));
    for (int i = 0; i < list[0].len; ++i)
      thrd_create(&thrds[i], getkeys_helper, pass(list, indexes, keys, klen, i, i));
    for (int i = 0; i < list[0].len; ++i) {
      if (rc)
        thrd_join(thrds[i], NULL);
      else
        thrd_join(thrds[i], &rc);
    }
    free(thrds);
  }

  mtx_destroy(&mtx);
  if (!rc)
    return indexes;
  else {
    free(indexes);
    return NULL;
  }
}

static int getkeys_helper(void *data)
{
  struct params *p = (struct params *) data;
  int rc = 0;

  mtx_lock(&mtx);
  if (p->keys == NULL) {
    for (int i = 0, j = 0; i < TABLEN; ++i)
      if (p->list[p->lid].tab[i].flag)
        p->ret[p->pid].tab[j++] = p->list[p->lid].tab[i];
  } else {
    for (int i = 0, j = 0; i < p->len; ++i) {
      tabidx_t idx = getkey(p->list, p->lid, p->keys[i]);
      if (idx.flag == 0)
        rc = 1;
      else
        p->ret[p->pid].tab[j++] = idx;
    }
  }
  mtx_unlock(&mtx);
  free(p);
  return rc;
}

static struct params *pass(tablist_t *list, tablist_t *ret, char **keys, int len, int lid, int pid)
{
  struct params *p = calloc(1, sizeof(struct params));
  p->list = list;
  p->ret = ret;
  p->keys = keys;
  p->len = len;
  p->lid = lid;
  p->pid = pid;
  return p;
}

static tabidx_t getkey(tablist_t *list, int id, char *key)
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
