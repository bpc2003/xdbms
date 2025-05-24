#ifndef XDBMS_H
#define XDBMS_H

// xdb_init: intialize an XDBMS DB
int xdb_init(char *filename);

// xdb_stmt: execute a statement on an XDBMS DB
char *xdb_stmt(char *stmt);

// xdb_close: close an XDBMS DB
void xdb_close(void);

#endif
