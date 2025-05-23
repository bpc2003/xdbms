#include <stdlib.h>

#include "include/xdbms.h"

int main(int argc, char **argv) {
	if (argc == 1)
		exit(1);
	char *filename = argv[1];
	xdb_init(filename);
	exit(0);
}
