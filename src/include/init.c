#include <stdio.h>
#include <stdlib.h>

#include "xdbms.h"
#include "engine/engine.h"

static tablist_t *db;

int xdb_init(char *filename)
{
	if (filename == NULL)
		fprintf(stderr, "Warning: running in-memory database\n");
	db = readdb(filename);

	tablist_t *tmp = getkeys(db, -1, NULL, 0);
	for (int i = 0; i < tmp[0].len; ++i) {
		printf("id: %d\n", i);
		for (int j = 0; tmp[0].tab[j].flag; ++j)
			printf("%s\n", tmp[0].tab[j].key);
	}
	free(tmp);
	delkeys(db, -1, NULL, 0);
	free(db);
	return 0;
}
