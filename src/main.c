#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "lib/mdb.h"
#include "cmd.h"

int getid(char *selector);
void printkeys(struct keytablist *list, int id, char **keys, int klen);
void setkeys(struct keytablist **list, int id, char **pairs, int plen);
void delkeys(struct keytablist *list, int id, char **keys, int klen);

int main(int argc, char **argv)
{
  if (argc > 2) {
    printf("usage: %s [db file]\n", argv[0]);
    exit(1);
  }
  char *filename = NULL;
  if (argc == 2)
    filename = argv[1];
  struct keytablist *list = readdb(filename);
  char *cmd = calloc(1024, sizeof(char));

  while (fgets(cmd, 1024, stdin) != NULL) {
    cmd[strlen(cmd) - 1] = '\0';
    if (!strcmp(cmd, "exit") || !strcmp(cmd, ""))
      break;
    struct cmd evaled = eval(cmd);
    int id = getid(evaled.selector);
    switch (evaled.type) {
      case GET:
        if (id >= 0)
          printkeys(list, id, evaled.params, evaled.plen);
        else if (id == -1) {
          for (int i = 0; i < list[0].len; ++i)
            printkeys(list, i, evaled.params, evaled.plen);
        } else
          fprintf(stderr, "Invalid selector: %s\n", evaled.selector);
        break;
      case SET:
        if (id >= 0)
          setkeys(&list, id, evaled.params, evaled.plen);
        else if (id == -1) {
          for (int i = 0; i < list[0].len; ++i)
            setkeys(&list, i, evaled.params, evaled.plen);
        } else
          fprintf(stderr, "Invalid selector: %s\n", evaled.selector);
        break;
      case DEL:
        if (id >= 0)
          delkeys(list, id, evaled.params, evaled.plen);
        else if (id == -1) {
          for (int i = 0; i < list[0].len; ++i)
            delkeys(list, i, evaled.params, evaled.plen);
        } else
          fprintf(stderr, "Invalid selector: %s\n", evaled.selector);
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

  if (filename != NULL)
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

int getid(char *selector) {
  if (selector == NULL)
    return -2;
  int id;
  if (!strcmp(selector, "*"))
    return -1;
  else if (isdigit(selector[0]) &&
      (id = atoi(selector)) >= 0)
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

void setkeys(struct keytablist **list, int id, char **pairs, int plen)
{
  if (pairs == NULL)
    return;
  for (int i = 0; i < plen; ++i) {
    char *tmp = calloc(strlen(pairs[i]) + 1, sizeof(char));
    strcpy(tmp, pairs[i]);
    setkey(list, id, tmp);
    free(tmp);
  }
}

void delkeys(struct keytablist *list, int id, char **keys, int klen)
{
  if (keys == NULL) {
    int *indexes = getkeys(list, id);
    for (int i = 0; indexes[i]; ++i)
      delkey(list, id, list[id].tab[indexes[i]].key);
    free(indexes);
  } else {
    for (int i = 0; i < klen; ++i)
      delkey(list, id, keys[i]);
  }
}
