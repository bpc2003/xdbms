#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <threads.h>

#include "mdb.h"

static int setkey_helper(void *thr_data);
static int delkey_helper(void *thr_data);
static int *clone(int n);
static tablist_t *copytab(tablist_t *dst, tablist_t *src);
static void dellist(tablist_t *list);

extern int setkey(tablist_t **, int, char *);
extern int delkey(tablist_t *, int, char *);

tablist_t *setkey_copy;
char *setkey_str;

mtx_t setkey_mtx;
int setkeys(tablist_t **list, int id, char *pair)
{
  int rc = 0;
  setkey_copy = calloc((*list)[0].len, sizeof(tablist_t));
  copytab(setkey_copy, *list);
  setkey_str = pair;
  if (id < -1)
    return -1;
  if (mtx_init(&setkey_mtx, mtx_plain) != thrd_success)
    return -2;

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

  if (!rc) {
    if (setkey_copy[0].len > (*list)[0].len)
      *list = realloc(*list, setkey_copy[0].len * sizeof(tablist_t));
    dellist(*list);
    copytab(*list, setkey_copy);
  }
  dellist(setkey_copy);
  free(setkey_copy);
  return rc;
}

tablist_t *delkey_copy;
char *delkey_str;

mtx_t delkey_mtx;
int delkeys(tablist_t *list, int id, char *key)
{
  int rc = 0;
  delkey_copy = calloc(list[0].len, sizeof(tablist_t));
  copytab(delkey_copy, list);
  delkey_str = key;

  if (id < -1)
    return -1;
  if (mtx_init(&delkey_mtx, mtx_plain) != thrd_success)
    return -2;
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
  free(delkey_copy);
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

static int delkey_helper(void *thr_data)
{
  int rc = 0;
  int *id = (int *) thr_data;

  mtx_lock(&delkey_mtx);
  if (delkey_str)
    rc = delkey(delkey_copy, *id, delkey_str);
  else {
    int *indexes = getkeys(delkey_copy, *id);
    for (int i = 0; indexes[i]; ++i)
      rc = delkey(delkey_copy, *id, delkey_copy[*id].tab[indexes[i]].key);
    free(indexes);
  }
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

static tablist_t *copytab(tablist_t *dst, tablist_t *src)
{
  if (dst == NULL)
    return NULL;
  dst[0].len = src[0].len;
  for (int i = 0; i < src[0].len; ++i) {
    for (int j = 0; j < TABLEN; ++j) {
      dst[i].tab[j] = (tabidx_t) { NULL, 0, { 0 } };
      if (src[i].tab[j].flag) {
        switch (src[i].tab[j].flag) {
          case 3:
            dst[i].tab[j].value.str =
              calloc(strlen(src[i].tab[j].value.str) + 1, sizeof(char));
            strcpy(dst[i].tab[j].value.str, src[i].tab[j].value.str);
            break;
          default:
            dst[i].tab[j].value = src[i].tab[j].value;
            break;
        }
        dst[i].tab[j].flag = src[i].tab[j].flag;
        dst[i].tab[j].key = calloc(strlen(src[i].tab[j].key) + 1, sizeof(char));
        strcpy(dst[i].tab[j].key, src[i].tab[j].key);
      }
    }
  }
  return dst;
}

static void dellist(tablist_t *list)
{
  for (int i = 0; i < list[0].len; ++i) {
    int *indexes = getkeys(list, i);
    for (int j = 0; indexes[j]; ++j)
      delkey(list, i, list[i].tab[indexes[j]].key);
    free(indexes);
  }
}
