#ifndef XML_H
#define XML_H

#include <stdio.h>

struct map {
  char *tag;
  void *payload;
  size_t size;
  size_t n;
  struct map *attrs;
  size_t n_attrs;
};

typedef struct map map_t;

/* decode: decodes the provided xml statement into a map_t */
map_t *decode(char *xml_str, int *len);

/* encode: encodes the provided map_t into a xml statement */
char *encode(map_t *map, int len);


#endif
