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

  delkeys(list, -1, NULL, 0);
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
  tablist_t *indexes = getkeys(*list, id, keys, klen);
  if (indexes == NULL)
    return 1;
  for (int i = 0; i < indexes[0].len; ++i) {
    printf("{ id: %d ", i);
    for (int j = 0; indexes[i].tab[j].flag; ++j)
      printkey(indexes[i].tab[j]);
    printf("}\n");
  }
  free(indexes);
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
  if (pairs == NULL || id < -1)
    return 1;
  if (setkeys(list, id, pairs, plen))
    return 2;
  return 0;
}

int delkeys_main(tablist_t **list, int id, char **keys, int klen)
{
  if (keys == NULL || id < -1)
    return 1;
  if (delkeys(*list, id, keys, klen))
    return 2;
  return 0;
}

int exec(int (*tabop)(tablist_t **, int, char **, int),
          tablist_t **list, int id, char **keys, int klen)
{
  if (id >= -1)
    return tabop(list, id, keys, klen);
  else {
    fprintf(stderr, "Invalid id\n");
    return 1;
  }
}
