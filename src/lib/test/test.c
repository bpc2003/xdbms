#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "../mdb.h"

int main(void)
{
  char *filename = "test.db";
  struct keytablist *list = readdb(filename);
  if (list == NULL)
    exit(1);

  writedb(filename, list);
  for (int i = 0; i < list[0].len; ++i) {
    int *indexes = getkeys(list, i);
    for (int j = 0; indexes[j]; ++j) {
      printf("%s: ", list[i].tab[indexes[j]].key);
      switch (list[i].tab[indexes[j]].flag) {
        case 1:
          printf("%.2lf\n", list[i].tab[indexes[j]].v.num);
          break;
        case 2:
          printf("%d\n", list[i].tab[indexes[j]].v.b);
          break;
        case 3:
          printf("%s\n", list[i].tab[indexes[j]].v.str);
          break;
      }
      delkey(list, i, list[i].tab[indexes[j]].key);
    }
    free(indexes);
  }

  free(list);
  exit(0);
}
