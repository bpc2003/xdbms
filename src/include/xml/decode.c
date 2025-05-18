#include <stdlib.h>
#include <string.h>

#include "xml.h"

static char *gettag(char *xml, int *pos);
static char *getvalue(char *xml, int *pos);
static attr_t *getattrs(char *parsed, char **tag, int *n_attrs);

// TODO: make this smaller
// TODO: add attribute parsing
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
			decoded->attrs =
				getattrs(gettag(xml, &i),
			 &(decoded->tag), &(decoded->n_attrs));
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
	for (i = *pos, len = 0; xml[i] != '>' && i < strlen(xml); ++i, ++len)
		;
	if (i >= strlen(xml))
		return NULL;
	char *title = calloc(len + 1, sizeof(char));
	strncpy(title, xml + *pos, len);
	*pos += len;
	return title;
}

static char *getvalue(char *xml, int *pos)
{
	int len, i;
	for (i = *pos, len = 0; xml[i] != '<' && i < strlen(xml); ++i, ++len)
		;
	if (i >= strlen(xml))
		return NULL;
	char *value = calloc(len + 1, sizeof(char));
	strncpy(value, xml + *pos, len);
	*pos += len - 1;
	return value;
}

static attr_t *getattrs(char *parsed, char **tag, int *n_attrs)
{
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
		if (i >= len ||
			(iter[i] == '=' && i >= len - 1) ||
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
	} while((iter = strtok(NULL, " ")));
	
	*n_attrs = pos;
	free(parsed);
	return attrs;
}
