#ifndef UTILS_H
#define UTILS_H

#include "mdb.h"

// dellist: deletes the provided table
void dellist(tablist_t *list);

/* hash: calculates the DJB2 hash of a string, 
 * and returns that mod TABLEN */
int hash(char *str);

// clone: clones and returns a pointer to int
int *clone(int n);

// copytab: copys table src into table dst
tablist_t *copytab(tablist_t *dst, tablist_t *src);

#endif
