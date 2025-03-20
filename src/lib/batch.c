#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <threads.h>
#include <stdatomic.h>

#include "mdb.h"

static int setkey_helper(void *thr_data);
/*static int delkey_helper(void *thr_data);*/


tablist_t *copy;
char *str;

mtx_t setkey_mtx;
int setkeys(tablist_t **list, char *pair)
{
  int rc;
  copy = calloc(list[0]->len, sizeof(tablist_t));
  memcpy(copy, *list, list[0]->len * sizeof(tablist_t));
  str = pair;

  thrd_t *thrds = calloc(list[0]->len, sizeof(thrd_t));
  if (mtx_init(&setkey_mtx, mtx_plain) != thrd_success)
    return 1;

  for (int i = 0; i < copy[0].len; ++i) {
    int *arg = calloc(1, sizeof(int));
    *arg = i;
    thrd_create(&thrds[i], setkey_helper, arg);
  }
  for (int i = 0; i < copy[0].len; ++i) {
    thrd_join(thrds[i], &rc);
    if (rc)
      break;
  }
  if (!rc)
    memcpy(*list, copy, copy[0].len * sizeof(tablist_t));
  free(thrds);
  free(copy);
  return rc;
}

int delkeys(tablist_t *list, char *key)
{
  return 0;
}

static int setkey_helper(void *thr_data)
{
  int *id = (int *) thr_data;
  int rc;
  
  mtx_lock(&setkey_mtx); 
  rc = setkey(&copy, *id, str);
  mtx_unlock(&setkey_mtx);
  
  free(id);
  return rc;
}
