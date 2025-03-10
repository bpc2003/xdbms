#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "include/fileops.h"
#include "include/parser.h"
#include "include/keytab.h"

int main(int argc, char **argv)
{
  int open = 0;
  int len = 2;
  struct keytablist *list = calloc(len, sizeof(struct keytablist));
  if (argc != 2)
    exit(1);
  char *filename = argv[1];

  uint8_t *buf = readdb(filename);
  if (buf == NULL)
    exit(0);
  struct byte *bytes = parse(buf);
  free(buf);

  for (int i = 0, j = 0; i < blen; ++i) {
    switch (bytes[i].type) {
      case BEGIN:
        if (open == 1) {
          fprintf(stderr, "missing close!\n");
          free(bytes);
          exit(1);
        }
        open = 1;
        break;
      case END:
        open = 0;
        j++;
        break;
      case PAIR:
        setkey(&list, &len, j, bytes[i].value);
        free(bytes[i].value);
        break;
      case ERROR:
        fprintf(stderr, "%s\n", bytes[i].value);
        exit(1);
    }
  }

  for (int i = 0; i < len; ++i) {
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
  free(bytes);
  exit(0);
}
