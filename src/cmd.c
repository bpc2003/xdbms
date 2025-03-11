#include <string.h>

#include "cmd.h"


struct cmd eval(char *str)
{
  struct cmd ret;
  ret.params = NULL;
  ret.selector = NULL;
  if (!strncmp(str, "GET", 3)) {
    ret.type = GET;
  } else if (!strncmp(str, "SET", 3)) {
    ret.type = SET;
  } else if (!strncmp(str, "DEL", 3)) {
    ret.type = DEL;
  } else {
    ret.type = ERR;
  }

  return ret;
}
