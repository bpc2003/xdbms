#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lib/mdb.h"
#include "cmd.h"

int main(int argc, char **argv)
{
  if (argc != 2)
    exit(1);
  char *filename = argv[1];
  struct keytablist *list = readdb(filename);
  char *cmd = calloc(1024, sizeof(char));

  while (fgets(cmd, 1024, stdin) != NULL) {
    cmd[strlen(cmd) - 1] = '\0';
    if (!strcmp(cmd, "exit"))
      break;
    struct cmd evaled = eval(cmd);
    switch (evaled.type) {
      case GET:
        printf("GET\n");
        break;
      case SET:
        printf("SET\n");
        break;
      case DEL:
        printf("DEL\n");
        break;
      case ERR:
        printf("ERR\n");
        break;
    }
  }

  free(cmd);
  free(list);
  exit(0);
}
