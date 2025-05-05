#include <stdlib.h>
#include <string.h>

#include "xml.h"

static char *gettag(char *xml, int *pos);

map_t *decode(char *xml, int *len)
{
  map_t *decoded = calloc(1, sizeof(map_t));
  *len = 1;
  
  int closed = 0;
  char *title = NULL, *tmp;
  for (int i = 0, j = 0; i < strlen(xml); ++i) {
    switch (xml[i]) {
      case '<':
        ++i;
        if (xml[i] == '/') {
          ++i;
          if (!strcmp(title, (tmp = gettag(xml, &i))))
            closed = 1;
          else {
            free(tmp);
            return NULL;
          }
          free(tmp);
        } else {
          if (title != NULL)
            free(title);
          closed = 0;
          title = gettag(xml, &i);
        }
        printf("%s\n", title);
        break;
      case '>':
        // TODO: make this recursive
        if (xml[i + 1] == '<')
          printf("nested tag\n");
        else
          ;
        break;
    }
  }
  free(title);
  return decoded;
}

static char *gettag(char *xml, int *pos)
{
  int len, i;
  for (i = *pos, len = 0; xml[i] != '>'; ++i, ++len) ;
  char *title = calloc(len + 1, sizeof(char));
  strncpy(title, xml + *pos, len);
  *pos += i - 2;
  return title;
}
