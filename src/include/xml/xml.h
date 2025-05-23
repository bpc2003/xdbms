#ifndef XML_H
#define XML_H

#include <stdio.h>

typedef struct {
	char *id;
	char *value;
} attr_t;

typedef struct {
	char *tag;
	void *payload;
	size_t size;
	int n;
	attr_t *attrs;
	int n_attrs;
} map_t;

/* decode: decodes the provided xml statement into a map_t */
map_t *decode(char *xml);

/* encode: encodes the provided map_t into a xml statement */
char *encode(map_t *map);

/* freemap: frees a map and its children */
void freemap(map_t *map);

#endif
