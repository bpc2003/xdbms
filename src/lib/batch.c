#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <threads.h>
#include <stdatomic.h>

#include "mdb.h"

static int setkey_helper(void *thr_data);
static int delkey_helper(void *thr_data);
static int *clone(int n);

tablist_t *setkey_copy;
char *setkey_str;

mtx_t setkey_mtx;
int setkeys(tablist_t **list, char *pair)
{
  int rc;
  setkey_copy = calloc(list[0]->len, sizeof(tablist_t));
  memcpy(setkey_copy, *list, list[0]->len * sizeof(tablist_t));
  setkey_str = pair;

  thrd_t *thrds = calloc(list[0]->len, sizeof(thrd_t));
  if (mtx_init(&setkey_mtx, mtx_plain) != thrd_success)
    return 1;

  for (int i = 0; i < setkey_copy[0].len; ++i)
    thrd_create(&thrds[i], setkey_helper, clone(i));
  for (int i = 0; i < setkey_copy[0].len; ++i) {
    thrd_join(thrds[i], &rc);
    if (rc)
      break;
  }
  if (!rc)
    memcpy(*list, setkey_copy, setkey_copy[0].len * sizeof(tablist_t));
  free(thrds);
  free(setkey_copy);
  return rc;
}

tablist_t *delkey_copy;
char *delkey_str;

mtx_t delkey_mtx;
int delkeys(tablist_t *list, char *key)
{
  int rc;
  delkey_copy = calloc(list[0].len, sizeof(tablist_t));
  memcpy(delkey_copy, list, list[0].len * sizeof(tablist_t));
  delkey_str = key;

  thrd_t *thrds = calloc(list[0].len, sizeof(thrd_t));
  if (mtx_init(&delkey_mtx, mtx_plain) != thrd_success)
    return 1;

  for (int i = 0; i < delkey_copy[0].len; ++i)
    thrd_create(&thrds[i], delkey_helper, clone(i));
  for (int i = 0; i < delkey_copy[0].len; ++i) {
    thrd_join(thrds[i], &rc);
    if (rc)
      break;
  }
  if (!rc)
    memcpy(list, delkey_copy, delkey_copy[0].len * sizeof(tablist_t));
  free(thrds);
  free(delkey_copy);
  return rc;
}

static int setkey_helper(void *thr_data)
{
  int *id = (int *) thr_data;
  int rc;
  
  mtx_lock(&setkey_mtx); 
  rc = setkey(&setkey_copy, *id, setkey_str);
  mtx_unlock(&setkey_mtx);
  
  free(id);
  return rc;
}

static int delkey_helper(void *thr_data)
{
  int *id = (int *) thr_data;
  int rc;

  mtx_lock(&delkey_mtx);
  rc = delkey(delkey_copy, *id, delkey_str);
  mtx_unlock(&delkey_mtx);

  free(id);
  return rc;
}

static int *clone(int n)
{
  int *c = calloc(1, sizeof(int));
  *c = n;
  return c;
}
