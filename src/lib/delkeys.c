#include <threads.h>
#include <stdlib.h>
#include <string.h>

#include "mdb.h"
#include "utils.h"

static int delkey_helper(void *thr_data);
static int delkey(tablist_t *, int, char *);
static struct params *pass(mtx_t *mtx, tablist_t *list, char **keys, int len, int id);


struct params {
  mtx_t *mtx;
  tablist_t *copy;
  char **keys;
  int len;
  int id;
};


int delkeys(tablist_t *list, int id, char **keys, int len)
{
  mtx_t mtx;
  if (id < -1 || len < 0)
    return -1;
  if (mtx_init(&mtx, mtx_plain) != thrd_success)
    return -2;
  int rc = 0;
  tablist_t *delkey_copy = calloc(list[0].len, sizeof(tablist_t));
  copytab(delkey_copy, list);

  if (id == -1) {
    thrd_t *thrds = calloc(list[0].len, sizeof(thrd_t));
    for (int i = 0; i < delkey_copy[0].len; ++i)
      thrd_create(&thrds[i], delkey_helper, pass(&mtx, delkey_copy, keys, len, i));
    for (int i = 0; i < delkey_copy[0].len; ++i) {
      if (rc)
        thrd_join(thrds[i], NULL);
      else
        thrd_join(thrds[i], &rc);
    }
    free(thrds);
  } else
    rc = delkey_helper(pass(&mtx, delkey_copy, keys, len, id));

  if (!rc) {
    dellist(list);
    memmove(list, delkey_copy, delkey_copy[0].len * sizeof(tablist_t));
  } else
    dellist(delkey_copy);
  mtx_destroy(&mtx);
  free(delkey_copy);
  return rc;
}

static int delkey_helper(void *thr_data)
{
  int rc = 0;
  struct params *p = (struct params *) thr_data;

  mtx_lock(p->mtx);
  if (p->len > 0 && p->keys != NULL)
    for (int i = 0; i < p->len; ++i) {
      if (p->keys[i] == NULL) {
        mtx_unlock(p->mtx);
        free(p);
        return -3;
      }
      rc = delkey(p->copy, p->id, p->keys[i]);
    }
  else {
    tablist_t *indexes = getkeys(p->copy, p->id, NULL, 0);
    for (int i = 0; indexes[0].tab[i].flag; ++i)
      rc = delkey(p->copy, p->id, indexes[0].tab[i].key);
    free(indexes);
  }
  mtx_unlock(p->mtx);

  free(p);
  return rc;
}

static int delkey(tablist_t *list, int id, char *key)
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

static struct params *pass(mtx_t *mtx, tablist_t *list, char **keys, int len, int id)
{
  struct params *p = calloc(1, sizeof(struct params));
  p->mtx = mtx;
  p->copy = list;
  p->keys = keys;
  p->len = len;
  p->id = id;
  return p;
}
