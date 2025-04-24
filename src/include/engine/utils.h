#ifndef UTILS_H
#define UTILS_H

#include "engine.h"

// dellist: deletes the provided table
void dellist(tablist_t *list);

/* hash: calculates the DJB2 hash of a string, 
 * and returns that mod TABLEN */
int hash(char *str);

// copytab: copys table src into table dst
tablist_t *copytab(tablist_t *dst, tablist_t *src);

#endif
