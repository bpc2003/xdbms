#include <stdio.h>
#include <stdlib.h>

#include "engine/engine.h"
#include "xml/xml.h"
#include "xdbms.h"

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
	printf("%s\n", encode(decoded));
	freemap(decoded);
	return NULL;
}
