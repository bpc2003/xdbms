#include <stdio.h>
#include <stdlib.h>

#include "lib/mdb.h"

void test_setkeys(void)
{
  tablist_t *list = readdb("dbs/test.db");
  char *pairs[] = { "name:John" };
  if (setkeys(&list, -1, pairs, 1))
    fprintf(stderr, "test_setkeys: failed\n");
  for (int i = 0; i < list[0].len; ++i) {
    printf("id: %d\n", i);
    int *indexes = getkeys(list, i);
    for (int j = 0; indexes[j]; ++j)
      printf("%s\n", list[i].tab[indexes[j]].key);
    free(indexes);
  }
  delkeys(list, -1, NULL, 0);
  free(list);
}

void test_setkeys_fail(void)
{
  tablist_t *list = readdb("dbs/test.db");
  char *pairs[] = { "namejohn" };
  if (!setkeys(&list, -1, pairs, 1))
    fprintf(stderr, "test_setkeys_fail: failed\n");
  delkeys(list, -1, NULL, 0);
  free(list);
}

void test_setkeys_multi_fail(void)
{
  tablist_t *list = readdb("dbs/test.db");
  char *pairs[] = { "name:John", NULL };
  if (!setkeys(&list, -1, pairs, 2))
    fprintf(stderr, "test_setkeys_multi_fail: failed\n");
  delkeys(list, -1, NULL, 0);
  free(list);
}

void test_setkeys_single(void)
{
  tablist_t *list = readdb("dbs/test.db");
  char *pairs[] = { "name:Alice" };
  if (setkeys(&list, 101, pairs, 1))
    fprintf(stderr, "test_setkeys_single: failed\n");
  delkeys(list, -1, NULL, 0);
  free(list);
}

void test_setkeys_multipairs(void)
{
  tablist_t *list = readdb("dbs/test.db");
  char *pairs[] = { "name:Bob", "active:true" };
  if (setkeys(&list, 0, pairs, 2))
    fprintf(stderr, "test_setkeys_multipairs: failed\n");
  delkeys(list, -1, NULL, 0);
  free(list);
}

void test_delkeys(void)
{
  tablist_t *list = readdb("dbs/test.db");
  char *keys[] = { "Row_1" };
  if (delkeys(list, -1, keys, 1))
    fprintf(stderr, "test_delkeys: failed\n");
  for (int i = 0; i < list[0].len; ++i) {
    printf("id: %d\n", i);
    int *indexes = getkeys(list, i);
    for (int j = 0; indexes[j]; ++j)
      printf("%s\n", list[i].tab[indexes[j]].key);
    free(indexes);
  }
  delkeys(list, -1, NULL, 0);
  free(list);
}

void test_delkeys_all(void)
{
  tablist_t *list = readdb("dbs/test.db");
  if (delkeys(list, -1, NULL, 0))
    fprintf(stderr, "test_delkeys_all: failed\n");
  free(list);
}

void test_delkeys_fail(void)
{
  tablist_t *list = readdb("dbs/test.db");
  char *keys[] = { "Row_4" };
  if (!delkeys(list, -1, keys, 1))
    fprintf(stderr, "test_delkeys_fail: failed\n");
  delkeys(list, -1, NULL, 0);
  free(list);
}

void test_delkeys_single(void)
{
  tablist_t *list = readdb("dbs/test.db");
  if (delkeys(list, 0, NULL, 0))
    fprintf(stderr, "test_delkeys_single: failed\n");
  delkeys(list, -1, NULL, 0);
  free(list);
}

void test_delkeys_multi(void)
{
  tablist_t *list = readdb("dbs/test.db");
  char *keys[] = { "Row_1", "Row_2" };
  if (delkeys(list, -1, keys, 2))
    fprintf(stderr, "test_delkeys_multi: failed\n");
  delkeys(list, -1, NULL, 0);
  free(list);
}

int main(void)
{
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
