#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "src/mdb.h"

int main(int argc, char **argv)
{
  if (argc != 2)
    exit(1);
  char *filename = argv[1];
  struct keytablist *list = readdb(filename);
  if (list == NULL)
    exit(2);

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
