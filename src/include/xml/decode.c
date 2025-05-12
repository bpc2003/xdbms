#include <stdlib.h>
#include <string.h>

#include "xml.h"

static char *gettag(char *xml, int *pos);
static char *getvalue(char *xml, int *pos);

// TODO: make this smaller
map_t *decode(char *xml, int *pos, int *len)
{
  if (*len)
    ++(*len);
  else
    *len = 1;
  map_t *decoded = calloc(1, sizeof(map_t));

  int err = 0, closed = 1;
  for (int i = *pos; i < strlen(xml); ++i) {
    if (!strncmp(xml + i, "</", 2)) {
      i += 2;
      char *tmp = gettag(xml, &i);
      if (!strcmp(decoded->tag, tmp))
        *pos = i;
      else
        err = 1;
      free(tmp);
      goto end;
    } else if (xml[i] == '<' && closed) {
      ++i;
      decoded->tag = gettag(xml, &i);
      closed = 0;
    } else if (xml[i] == '<' && !closed) {
      map_t *ndec = decode(xml, &i, &(decoded->n));
      decoded->size = sizeof(map_t);
      if (decoded->n > 1)
        decoded->payload = realloc(decoded->payload, decoded->n * sizeof(map_t));
      else
        decoded->payload = calloc(1, sizeof(map_t));
      ((map_t *) decoded->payload)[decoded->n - 1] = *ndec;
      free(ndec);
    } else {
      decoded->payload = getvalue(xml, &i);
      decoded->size = sizeof(char);
      decoded->n = strlen(decoded->payload);
    }
  }
  end:
    if (err) {
      free(decoded);
      decoded = NULL;
    }
    return decoded;
}

static char *gettag(char *xml, int *pos)
{
  int len, i;
  for (i = *pos, len = 0; xml[i] != '>'; ++i, ++len) ;
  char *title = calloc(len + 1, sizeof(char));
  strncpy(title, xml + *pos, len);
  *pos += len;
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
