#include <stdlib.h>
#include <string.h>

#include "cmd.h"

char *getselector(char *str);
char **getparams(char *str, int *len);

struct cmd eval(char *str) {
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
		return ret;
	}

	ret.selector = getselector(str);
	ret.params = getparams(str, &ret.plen);
	return ret;
}

char *getselector(char *str) {
	char *selector = calloc(1, sizeof(char));
	int pos = 4, i;
	for (i = 0; str[pos] != '/' && pos < strlen(str); ++pos) {
		if (i >= 1) {
			selector = realloc(selector, (i + 1) * sizeof(char));
		}
		selector[i++] = str[pos];
	}
	selector = realloc(selector, (i + 1) * sizeof(char));
	selector[i] = '\0';
	return selector;
}

char **getparams(char *str, int *len) {
	char **params = calloc(1, sizeof(char *));
	char *tok = strtok(str, "/");
	int i = 0;
	while ((tok = strtok(NULL, "/"))) {
		if (i >= 1)
			params = realloc(params, (i + 1) * sizeof(char *));
		params[i] = calloc(strlen(tok) + 1, sizeof(char));
		strcpy(params[i++], tok);
	}
	if (i == 0) {
		free(params);
		return NULL;
	}
	*len = i;
	return params;
}
