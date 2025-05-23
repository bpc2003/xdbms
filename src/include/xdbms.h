#ifndef XDBMS_H
#define XDBMS_H

// init: intialize an XDBMS DB
int xdb_init(char *filename);

// exec: execute a statement on an XDBMS DB
char *exec(char *stmt);

// close: close an XDBMS DB
int close();

#endif
