#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "engine/engine.h"
#include "xdbms.h"
#include "xml/xml.h"

static tablist_t *db;

int xdb_init(char *filename) {
	if (filename == NULL)
		fprintf(stderr, "Warning: running in-memory database\n");
	db = readdb(filename);
	if (db == NULL) {
		fprintf(stderr, "Error: Invalid DB file\n");
		return 1;
	}
	return 0;
}

void xdb_close(void) {
	delkeys(db, -1, NULL, 0);
	free(db);
}

char *xdb_stmt(char *stmt) {
	map_t *decoded = decode(stmt);

	if (!strcmp(decoded->tag, "get")) {
		int id = -1, len = 0;
		char **keys = NULL;
		if (decoded->n_attrs > 0)
			id = atoi(decoded->attrs[0].value);
		tablist_t *tmp = getkeys(db, id, keys, len);
		for (int i = 0; i < tmp[0].len; ++i) {
			printf("{ id: %d ", tmp[0].len > 1 ? i : id);
			for (int j = 0; tmp[i].tab[j].flag; ++j) {
				printf("%s: ", tmp[i].tab[j].key);
				switch (tmp[i].tab[j].flag) {
				case 1:
					printf("%g ", tmp[i].tab[j].value.num);
					break;
				case 2:
					printf("%s ",
						   tmp[i].tab[j].value.boolean ? "true" : "false");
					break;
				case 3:
					printf("%s ", tmp[i].tab[j].value.str);
					break;
				}
			}
			printf("}\n");
		}
		free(tmp);
	} else if (!strcmp(decoded->tag, "set")) {
		;
	} else if (!strcmp(decoded->tag, "del")) {
		;
	} else {
		printf("Unknown request: %s\n", decoded->tag);
	}

	freemap(decoded);
	return NULL;
}
