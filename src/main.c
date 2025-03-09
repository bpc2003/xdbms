#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "include/fileops.h"
#include "include/parser.h"
#include "include/keytab.h"

int main(int argc, char **argv)
{
  struct keytab tab[TABLEN] = {{ NULL, 0, { 0 } }};
  if (argc != 2)
    exit(1);
  char *filename = argv[1];
  
  uint8_t *buf = readdb(filename);
  if (buf == NULL)
    exit(0);
  struct byte *bytes = parse(buf);
  free(buf);

  for (int i = 0; i < blen; ++i) {
    switch (bytes[i].type) {
      case BEGIN:
        break;
      case END:
        break;
      case PAIR:
        setkey(tab, bytes[i].value);
        free(bytes[i].value);
        break;
      case ERROR:
        fprintf(stderr, "%s\n", bytes[i].value);
        exit(1);
    }
  }

  int *indexes = getkeys(tab);
  for (int i = 0; indexes[i] != 0; ++i) {
    printf("%s: ", tab[indexes[i]].key);
    if (tab[indexes[i]].flag == 1) {
      printf("%.2lf\n", tab[indexes[i]].v.num);
    } else if (tab[indexes[i]].flag == 2) {
      printf("%d\n", tab[indexes[i]].v.b);
    } else if (tab[indexes[i]].flag == 3) {
      printf("%s\n", tab[indexes[i]].v.str);
    }
    delkey(tab, tab[indexes[i]].key);
  }
  free(indexes);
  free(bytes);
  exit(0);
}
