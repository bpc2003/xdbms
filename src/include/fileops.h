#ifndef FILEOPS_H
#define FILEOPS_H

#include "keytab.h"

unsigned char *readdb(char *filename);
void writedb(char *filename, struct keytablist *list, int len);

#endif
