#include <stdlib.h>

#include "include/xdbms.h"

int main(int argc, char **argv) {
	if (argc == 1)
		exit(1);
	char *filename = argv[1];
	if (xdb_init(filename))
		exit(1);
	// TODO: make decoder be able to decode empty tags
	xdb_stmt("<get><all>true</all></get>");
	xdb_close();
	exit(0);
}
