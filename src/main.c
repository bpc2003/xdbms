#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "lib/mdb.h"
#include "cmd.h"

int getid(char *selector);
int printkeys(tablist_t **list, int id, char **keys, int klen);
void printkey(tabidx_t idx);
// TODO: rename these functions
int setkeys_main(tablist_t **list, int id, char **pairs, int plen);
int delkeys_main(tablist_t **list, int id, char **keys, int klen);
int exec(int (*tabop)(tablist_t **, int, char **, int),
          tablist_t **list, int id, char **keys, int klen);

int main(int argc, char **argv)
{
  if (argc > 2) {
    printf("usage: %s [db file]\n", argv[0]);
    exit(1);
  }
  char *filename = NULL;
  if (argc == 2)
    filename = argv[1];
  tablist_t *list = readdb(filename);
  if (list == NULL)
    exit(1);
  char *cmd = calloc(1024, sizeof(char));

  while (fgets(cmd, 1024, stdin) != NULL) {
    cmd[strlen(cmd) - 1] = '\0';
    if (!strcmp(cmd, "exit") || !strcmp(cmd, ""))
      break;
    struct cmd evaled = eval(cmd);
    int id = getid(evaled.selector);
    switch (evaled.type) {
      case GET:
        exec(printkeys, &list, id, evaled.params, evaled.plen);
        break;
      case SET:
      case DEL:
        id = exec(evaled.type == SET ? setkeys_main : delkeys_main,
             &list, id, evaled.params, evaled.plen);
        if (!id && filename)
          writedb(filename, list);
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

int printkeys(tablist_t **list, int id, char **keys, int klen)
{
  if (keys == NULL) {
    int *indexes = getkeys(*list, id);
    printf("{ id: %d ", id);
    for (int i = 0; indexes[i]; ++i)
      printkey((*list)[id].tab[indexes[i]]);
    free(indexes);
    printf("}\n");
  } else {
    printf("{ id: %d ", id);
    for (int i = 0; i < klen; ++i) {
      tabidx_t idx = getkey(*list, id, keys[i]);
      if (idx.flag == 0)
        continue;
      printkey(idx);
    }
    printf("}\n");
  }
  return 0;
}

void printkey(tabidx_t idx)
{
  printf("%s: ", idx.key);
  switch (idx.flag) {
    case 1:
      printf("%.2lf ", idx.value.num);
      break;
    case 2:
      printf("%s ", idx.value.boolean ? "true" : "false");
      break;
    case 3:
      printf("%s ", idx.value.str);
      break;
  }
}

int setkeys_main(tablist_t **list, int id, char **pairs, int plen)
{
  if (pairs == NULL)
    return 1;
  for (int i = 0; i < plen; ++i) {
    char *tmp = calloc(strlen(pairs[i]) + 1, sizeof(char));
    strcpy(tmp, pairs[i]);
    if (setkey(list, id, tmp))
      return 1;
    free(tmp);
  }
  return 0;
}

int delkeys_main(tablist_t **list, int id, char **keys, int klen)
{
  if (keys == NULL) {
    int *indexes = getkeys(*list, id);
    for (int i = 0; indexes[i]; ++i)
      if (delkey(*list, id, (*list)[id].tab[indexes[i]].key))
        return 1;
    free(indexes);
  } else {
    for (int i = 0; i < klen; ++i)
      if (delkey(*list, id, keys[i]))
        return 1;
  }
  return 0;
}

int exec(int (*tabop)(tablist_t **, int, char **, int),
          tablist_t **list, int id, char **keys, int klen)
{
  if (id >= 0)
    return tabop(list, id, keys, klen);
  else if (id == -1) {
    for (int i = 0; i < (*list)[0].len; ++i)
      if (tabop(list, i, keys, klen))
        return 1;
    return 0;
  } else {
    fprintf(stderr, "Invalid id\n");
    return 1;
  }
}
