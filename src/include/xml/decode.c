#include <stdlib.h>
#include <string.h>

#include "xml.h"

static char *gettag(char *xml, int *pos);
static char *getvalue(char *xml, int *pos);

map_t *decode(char *xml, int *pos, int *len)
{
  printf("%s\n", xml);
  if (*len)
    ++(*len);
  else
    *len = 1;
  map_t *decoded = calloc(1, sizeof(map_t));
  decoded->payload = NULL;

  char *tag = NULL;
  int err = 0;
  for (int i = *pos; i < strlen(xml); ++i) {
    if (xml[i] == '<') {
      ++i;
      if (xml[i] == '/') {
        ++i;
        char *tmp = gettag(xml, &i);
        if (!strcmp(tag, tmp)) {
          free(tmp);
          *pos = i + 1;
          goto end;
        } else {
          err = 1;
          free(tmp);
          goto end;
        }
      }
      else if (!tag)
        tag = gettag(xml, &i);
    } else if (xml[i] == '>') {
      ++i;
      if (xml[i] == '<') {
        map_t *ndec = decode(xml, &i, &(decoded->n));
        free(ndec);
      } else {
        char *value = getvalue(xml, &i);
        printf("%s\n", value);
        free(value);
      }
    }
  }
  end:
    if (err) {
      free(decoded);
      decoded = NULL;
    }
    free(tag);
    return decoded;
}

static char *gettag(char *xml, int *pos)
{
  int len, i;
  for (i = *pos, len = 0; xml[i] != '>'; ++i, ++len) ;
  char *title = calloc(len + 1, sizeof(char));
  strncpy(title, xml + *pos, len);
  *pos += len - 1;
  return title;
}

static char *getvalue(char *xml, int *pos)
{
  int len, i;
  for (i = *pos, len = 0; xml[i] != '<'; ++i, ++len) ;
  char *value = calloc(len + 1, sizeof(char));
  strncpy(value, xml + *pos, len);
  *pos += len - 1;
  return value;
}
