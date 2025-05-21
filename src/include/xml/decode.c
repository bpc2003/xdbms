#include <stdlib.h>
#include <string.h>

#include "xml.h"

static char *get_tag(char *xml, int *pos);
static attr_t *get_attrs(char *parsed, char **tag, int *n_attrs);

static int check_closing_tag(char *xml, int *pos, char *tag, int *err);
static void set_tag(char *xml, map_t **map, int *closed, int *pos);
static void set_value(char *xml, int *pos, map_t **map);

static map_t *decode_helper(char *xml, int *pos, int *len);

map_t *decode(char *xml) {
	int start = 0, len = 0;
	return decode_helper(xml, &start, &len);
}

static map_t *decode_helper(char *xml, int *pos, int *len) {
	if (*len)
		++(*len);
	else
		*len = 1;

	map_t *decoded = calloc(1, sizeof(map_t));
	int err = 0, closed = 1;
	for (int i = *pos; i < strlen(xml); ++i) {
		if (check_closing_tag(xml, &i, decoded->tag, &err)) {
			if (!err)
				*pos = i;
			break;
		} else if (xml[i] == '<' && closed)
			set_tag(xml, &decoded, &closed, &i);
		else if (xml[i] == '<' && !closed) {
			map_t *ndec = decode_helper(xml, &i, &(decoded->n));
			decoded->size = sizeof(map_t);
			if (decoded->n > 1)
				decoded->payload =
					realloc(decoded->payload, decoded->n * sizeof(map_t));
			else
				decoded->payload = calloc(1, sizeof(map_t));
			((map_t *)decoded->payload)[decoded->n - 1] = *ndec;
			free(ndec);
		} else
			set_value(xml, &i, &decoded);
	}
	if (err) {
		free(decoded);
		decoded = NULL;
	}
	return decoded;
}

static char *get_tag(char *xml, int *pos) {
	int len, i;
	for (i = *pos, len = 0; xml[i] != '>' && i < strlen(xml); ++i, ++len)
		;
	if (i >= strlen(xml))
		return NULL;
	char *title = calloc(len + 1, sizeof(char));
	strncpy(title, xml + *pos, len);
	*pos += len;
	return title;
}

static void set_value(char *xml, int *pos, map_t **map) {
	int len, i;
	for (i = *pos, len = 0; xml[i] != '<' && i < strlen(xml); ++i, ++len)
		;
	if (i >= strlen(xml))
		return;

	(*map)->payload = calloc(len + 1, sizeof(char));
	strncpy((*map)->payload, xml + *pos, len);
	(*map)->size = sizeof(char);
	(*map)->n = strlen((*map)->payload);
	*pos += len - 1;
}

static attr_t *get_attrs(char *parsed, char **tag, int *n_attrs) {
	attr_t *attrs;
	char *iter = strtok(parsed, " ");
	*tag = calloc(strlen(iter) + 1, sizeof(char));
	strcpy(*tag, iter);

	if ((iter = strtok(NULL, " ")))
		attrs = calloc((*n_attrs = 1), sizeof(attr_t));
	else {
		free(parsed);
		return NULL;
	}

	int pos = 0;
	do {
		if (!iter)
			break;
		int i, len = strlen(iter);
		for (i = 0; i < len && iter[i] != '='; ++i)
			;
		if (i >= len || (iter[i] == '=' && i >= len - 1) ||
			(iter[i + 1] != '\'' && iter[i + 1] != '"')) {
			free(parsed);
			free(attrs);
			return NULL;
		}

		char *name = calloc(i + 1, sizeof(char));
		char *value = calloc(len - i, sizeof(char));
		strncpy(name, iter, i);
		strcpy(value, iter + i + 1);
		memmove(value, value + 1, strlen(value) - 1);
		value[strlen(value) - 2] = '\0';
		if (pos >= *n_attrs)
			attrs = realloc(attrs, (*n_attrs *= 2) * sizeof(attr_t));
		attrs[pos].id = name;
		attrs[pos++].value = value;
	} while ((iter = strtok(NULL, " ")));

	*n_attrs = pos;
	free(parsed);
	return attrs;
}

static void set_tag(char *xml, map_t **map, int *closed, int *pos) {
	(*pos)++;
	(*map)->attrs =
		get_attrs(get_tag(xml, pos), &((*map)->tag), &((*map)->n_attrs));
	*closed = 0;
}

static int check_closing_tag(char *xml, int *pos, char *tag, int *err) {
	if (!strncmp(xml + *pos, "</", 2)) {
		*pos += 2;
		char *tmp = get_tag(xml, pos);
		if (strcmp(tag, tmp))
			*err = 1;
		free(tmp);
		return 1;
	}
	return 0;
}
