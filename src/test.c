#include <stdio.h>
#include <stdlib.h>

#include "lib/mdb.h"

void test_setkeys(void)
{
  tablist_t *list = readdb("dbs/test.db");
  if (setkeys(&list, "name:john"))
    fprintf(stderr, "test_setkeys: failed\n");
  for (int i = 0; i < list[0].len; ++i) {
    int *indexes = getkeys(list, i);
    for (int j = 0; indexes[j]; ++j) {
      printf("%s\n", list[i].tab[indexes[j]].key);
      delkey(list, i, list[i].tab[indexes[j]].key);
    }
    free(indexes);
  }
  free(list);
}

int main(void)
{
  test_setkeys();
  exit(0);
}
