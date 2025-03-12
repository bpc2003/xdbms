#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lib/mdb.h"
#include "cmd.h"

int getrange(char *selector);
void printkeys(struct keytablist *list, int id, char **keys, int klen);

int main(int argc, char **argv)
{
  if (argc != 2)
    exit(1);
  char *filename = argv[1];
  struct keytablist *list = readdb(filename);
  char *cmd = calloc(1024, sizeof(char));

  while (fgets(cmd, 1024, stdin) != NULL) {
    cmd[strlen(cmd) - 1] = '\0';
    if (!strcmp(cmd, "exit") || !strcmp(cmd, ""))
      break;
    struct cmd evaled = eval(cmd);
    int range = getrange(evaled.selector);
    switch (evaled.type) {
      case GET:
        if (range >= 0)
          printkeys(list, range, evaled.params, evaled.plen);
        else if (range == -1) {
          for (int i = 0; i < list[0].len; ++i)
            printkeys(list, i, evaled.params, evaled.plen);
        }
        break;
      case SET:
        break;
      case DEL:
        break;
      case ERR:
        fprintf(stderr, "Unkown command: %s\n", cmd);
        break;
    }
    if (evaled.params != NULL) {
      for (int i = 0; i < evaled.plen; ++i)
        free(evaled.params[i]);
      free(evaled.params);
    }
    free(evaled.selector);
  }
  free(cmd);

  writedb(filename, list);
  for (int i = 0; i < list[0].len; ++i) {
    int *indexes = getkeys(list, i);
    for (int j = 0; indexes[j]; ++j)
      delkey(list, i, list[i].tab[indexes[j]].key);
    free(indexes);
  }
  free(list);
  exit(0);
}

int getrange(char *selector) {
  int id;
  if (!strcmp(selector, "*"))
    return -1;
  else if ((id = atoi(selector)) >= 0)
    return id;
  else
    return -2;
}

void printkeys(struct keytablist *list, int id, char **keys, int klen)
{
  if (keys == NULL) {
    int *indexes = getkeys(list, id);
    printf("{ id: %d ", id);
    for (int i = 0; indexes[i]; ++i) {
      printf("%s: ", list[id].tab[indexes[i]].key);
      switch (list[id].tab[indexes[i]].flag) {
        case 1:
          printf("%.2lf ", list[id].tab[indexes[i]].v.num);
          break;
        case 2:
          printf("%s ", list[id].tab[indexes[i]].v.b ? "true" : "false");
          break;
        case 3:
          printf("%s ", list[id].tab[indexes[i]].v.str);
          break;
      }
    }
    free(indexes);
    printf("}\n");
  } else {
    printf("{ id: %d ", id);
    for (int i = 0; i < klen; ++i) {
      struct keytab tabidx = getkey(list, id, keys[i]);
      if (tabidx.flag == 0)
        continue;
      printf ("%s: ", keys[i]);
      switch (tabidx.flag) {
        case 1:
          printf("%.2lf ", tabidx.v.num);
          break;
        case 2:
          printf("%s ", tabidx.v.b ? "true" : "false");
          break;
        case 3:
          printf("%s ", tabidx.v.str);
          break;
      }
    }
    printf("}\n");
  }
}
