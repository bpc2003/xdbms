#include <stdio.h>
#include <stdlib.h>

#include "include/mdb.h"

void test_writedb(void)
{
  tablist_t *list = readdb("dbs/new.db");
  writedb("dbs/test.db", list);
  delkeys(list, -1, NULL, 0);
  free(list);
}

void test_readdb(void)
{
  tablist_t *list = readdb("dbs/new.db");
  tablist_t *indices = getkeys(list, -1, NULL, 0);
  for (int i = 0; i < indices[0].len; ++i) {
    printf("id: %d\n", i);
    for (int j = 0; indices[i].tab[j].flag; ++j)
      printf("%s\n", indices[i].tab[j].key);
  }
  free(indices);
  delkeys(list, -1, NULL, 0);
  free(list);
}


void test_setkeys(void)
{
  tablist_t *list = readdb("dbs/test.db");
  char *pairs[] = { "name:John" };
  if (setkeys(&list, -1, pairs, 1)) {
    fprintf(stderr, "test_setkeys: failed\n");
    return;
  }
  for (int i = 0; i < list[0].len; ++i) {
    printf("id: %d\n", i);
    tablist_t *indexes = getkeys(list, i, NULL, 0);
    for (int j = 0; indexes[0].tab[j].flag; ++j)
      printf("%s\n", indexes[0].tab[j].key);
    free(indexes);
  }
  delkeys(list, -1, NULL, 0);
  free(list);
}

void test_setkeys_fail(void)
{
  tablist_t *list = readdb("dbs/test.db");
  char *pairs[] = { "namejohn" };
  if (!setkeys(&list, -1, pairs, 1)) {
    fprintf(stderr, "test_setkeys_fail: failed\n");
    return;
  }
  delkeys(list, -1, NULL, 0);
  free(list);
}

void test_setkeys_multi_fail(void)
{
  tablist_t *list = readdb("dbs/test.db");
  char *pairs[] = { "name:John", NULL };
  if (!setkeys(&list, -1, pairs, 2)) {
    fprintf(stderr, "test_setkeys_multi_fail: failed\n");
    return;
  }
  delkeys(list, -1, NULL, 0);
  free(list);
}

void test_setkeys_single(void)
{
  tablist_t *list = readdb("dbs/test.db");
  char *pairs[] = { "name:Alice" };
  if (setkeys(&list, 101, pairs, 1)) {
    fprintf(stderr, "test_setkeys_single: failed\n");
    return;
  }
  delkeys(list, -1, NULL, 0);
  free(list);
}

void test_setkeys_multipairs(void)
{
  tablist_t *list = readdb("dbs/test.db");
  char *pairs[] = { "name:Bob", "active:true" };
  if (setkeys(&list, 0, pairs, 2)) {
    fprintf(stderr, "test_setkeys_multipairs: failed\n");
    return;
  }
  delkeys(list, -1, NULL, 0);
  free(list);
}

void test_delkeys(void)
{
  tablist_t *list = readdb("dbs/test.db");
  char *keys[] = { "Row_1" };
  if (delkeys(list, -1, keys, 1)) {
    fprintf(stderr, "test_delkeys: failed\n");
    return;
  }
  for (int i = 0; i < list[0].len; ++i) {
    printf("id: %d\n", i);
    tablist_t *indexes = getkeys(list, i, NULL, 0);
    for (int j = 0; indexes[0].tab[j].flag; ++j)
      printf("%s\n", indexes[0].tab[j].key);
    free(indexes);
  }
  delkeys(list, -1, NULL, 0);
  free(list);
}

void test_delkeys_all(void)
{
  tablist_t *list = readdb("dbs/test.db");
  if (delkeys(list, -1, NULL, 0)) {
    fprintf(stderr, "test_delkeys_all: failed\n");
    return;
  }
  free(list);
}

void test_delkeys_fail(void)
{
  tablist_t *list = readdb("dbs/test.db");
  char *keys[] = { "Row_4" };
  if (!delkeys(list, -1, keys, 1)) {
    fprintf(stderr, "test_delkeys_fail: failed\n");
    return;
  }
  delkeys(list, -1, NULL, 0);
  free(list);
}

void test_delkeys_single(void)
{
  tablist_t *list = readdb("dbs/test.db");
  if (delkeys(list, 0, NULL, 0)) {
    fprintf(stderr, "test_delkeys_single: failed\n");
    return;
  }
  delkeys(list, -1, NULL, 0);
  free(list);
}

void test_delkeys_multi(void)
{
  tablist_t *list = readdb("dbs/test.db");
  char *keys[] = { "Row_1", "Row_2" };
  if (delkeys(list, -1, keys, 2)) {
    fprintf(stderr, "test_delkeys_multi: failed\n");
    return;
  }
  delkeys(list, -1, NULL, 0);
  free(list);
}

void test_getkeys_multi(void)
{
  tablist_t *list = readdb("dbs/test.db");
  char *keys[] = { "Row_1", "Row_2" };
  tablist_t *ret = getkeys(list, 0, keys, 2);
  if (ret == NULL) {
    fprintf(stderr, "test_getkeys_multi: failed\n");
    return;
  }
  for (int i = 0; ret[0].tab[i].flag; ++i)
    printf("%s\n", ret[0].tab[i].key);
  free(ret);
  delkeys(list, -1, NULL, 0);
  free(list);
}

void test_getkeys_multi_fail(void)
{
  tablist_t *list = readdb("dbs/test.db");
  char *keys[] = { "Row_1", "Row_4" };
  tablist_t *ret = getkeys(list, 0, keys, 2);
  if (ret) {
    fprintf(stderr, "test_getkeys_multi_fail: failed\n");
    return;
  }
  delkeys(list, -1, NULL, 0);
  free(list);
}

void test_getkeys(void)
{
  tablist_t *list = readdb("dbs/test.db");
  tablist_t *ret = getkeys(list, -1, NULL, 0);
  if (!ret) {
    fprintf(stderr, "test_getkeys: failed\n");
    return;
  }
  free(ret);
  delkeys(list, -1, NULL, 0);
  free(list);
}

int main(void)
{
  test_writedb();
  test_readdb();
  test_getkeys();
  test_getkeys_multi();
  test_getkeys_multi_fail();

  test_setkeys();
  test_setkeys_fail();
  test_setkeys_single();
  test_setkeys_multipairs();
  test_setkeys_multi_fail();

  test_delkeys();
  test_delkeys_all();
  test_delkeys_fail();
  test_delkeys_single();
  test_delkeys_multi();

  exit(0);
}
