#include <threads.h>
#include <stdlib.h>
#include <string.h>

#include "mdb.h"
#include "utils.h"

static int delkey_helper(void *thr_data);
static int delkey(tablist_t *, int, char *);

tablist_t *delkey_copy;
char **delkeys_keys;
int keys_len;

mtx_t delkey_mtx;


int delkeys(tablist_t *list, int id, char **keys, int len)
{
  if (id < -1 || len < 0)
    return -1;
  if (mtx_init(&delkey_mtx, mtx_plain) != thrd_success)
    return -2;
  int rc = 0;
  delkey_copy = calloc(list[0].len, sizeof(tablist_t));
  copytab(delkey_copy, list);
  delkeys_keys = keys;
  keys_len = len;

  if (id == -1) {
    thrd_t *thrds = calloc(list[0].len, sizeof(thrd_t));
    for (int i = 0; i < delkey_copy[0].len; ++i)
      thrd_create(&thrds[i], delkey_helper, clone(i));
    for (int i = 0; i < delkey_copy[0].len; ++i) {
      if (rc)
        thrd_join(thrds[i], NULL);
      else
        thrd_join(thrds[i], &rc);
    }
    free(thrds);
  } else
    rc = delkey_helper(clone(id));

  if (!rc) {
    dellist(list);
    memmove(list, delkey_copy, delkey_copy[0].len * sizeof(tablist_t));
  } else
    dellist(delkey_copy);
  mtx_destroy(&delkey_mtx);
  free(delkey_copy);
  return rc;
}

static int delkey_helper(void *thr_data)
{
  int rc = 0;
  int *id = (int *) thr_data;

  mtx_lock(&delkey_mtx);
  if (keys_len > 0 && delkeys_keys != NULL)
    for (int i = 0; i < keys_len; ++i) {
      if (delkeys_keys[i] == NULL)
        return -3;
      rc = delkey(delkey_copy, *id, delkeys_keys[i]);
    }
  else {
    tablist_t *indexes = getkeys(delkey_copy, *id, NULL, 0);
    for (int i = 0; indexes[0].tab[i].flag; ++i)
      rc = delkey(delkey_copy, *id, indexes[0].tab[i].key);
    free(indexes);
  }
  mtx_unlock(&delkey_mtx);

  free(id);
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
