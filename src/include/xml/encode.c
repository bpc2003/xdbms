#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "xml.h"

static void check(char **str, int *len, int *used, int n);

char *encode(map_t *map, int len)
{
  int slen = 128, used = 0;
  char *str = calloc(slen, sizeof(char));

  char buf[64];
  for (int i = 0; i < len; ++i) {
    check(&str, &slen, &used, snprintf(buf, 64, "<%s", map[i].tag));
    strcat(str, buf);
    if (map[i].attrs != NULL) {
      for (int j = 0; j < map[i].n_attrs; ++j) {
        check(&str, &slen, &used,
              snprintf(buf, 64, " %s='%s'",
              map[i].attrs[j].id, map[i].attrs[j].value));
        strcat(str, buf);
      }
    }
    check(&str, &slen, &used, 1);
    strcat(str, ">");
    if (map[i].size == sizeof(map_t)) {
      char *rt = encode((map_t *) map[i].payload, map[i].n);
      if (rt == NULL)
        return NULL;
      check(&str, &slen, &used, strlen(rt));
      strcat(str, rt);
      free(rt);
    } else if (map[i].size == sizeof(char)) {
      check(&str, &slen, &used, map[i].n);
      strcat(str, map[i].payload);
    }
    else
      return NULL;
    check(&str, &slen, &used, snprintf(buf, 64, "</%s>", map[i].tag));
    strcat(str, buf);
  }
  return str;
}

static void check(char **str, int *len, int *used, int n)
{
  if (*used + n >= *len)
    *str = realloc(*str, (*len *= 2));
  *used += n;
}
