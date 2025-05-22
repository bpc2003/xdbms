#include <stdlib.h>
#include <string.h>

#include "utils.h"

static int delkey(tablist_t *, int, char *);

void dellist(tablist_t *list) {
	for (int i = 0; i < list[0].len; ++i) {
		tablist_t *indexes = getkeys(list, i, NULL, 0);
		for (int j = 0; indexes[0].tab[j].flag; ++j)
			delkey(list, i, indexes[0].tab[j].key);
		free(indexes);
	}
}

int hash(char *key) {
	unsigned long h = 5381;
	for (unsigned long i = 0; i < strlen(key); ++i)
		h = ((h << 5) + h) + key[i];
	return h % TABLEN;
}

tablist_t *copytab(tablist_t *dst, tablist_t *src) {
	if (dst == NULL)
		return NULL;
	dst[0].len = src[0].len;
	for (int i = 0; i < src[0].len; ++i) {
		for (int j = 0; j < TABLEN; ++j) {
			dst[i].tab[j] = (tabidx_t){NULL, 0, {0}};
			if (src[i].tab[j].flag) {
				switch (src[i].tab[j].flag) {
				case 3:
					dst[i].tab[j].value.str = calloc(
						strlen(src[i].tab[j].value.str) + 1, sizeof(char));
					strcpy(dst[i].tab[j].value.str, src[i].tab[j].value.str);
					break;
				default:
					dst[i].tab[j].value = src[i].tab[j].value;
					break;
				}
				dst[i].tab[j].flag = src[i].tab[j].flag;
				dst[i].tab[j].key =
					calloc(strlen(src[i].tab[j].key) + 1, sizeof(char));
				strcpy(dst[i].tab[j].key, src[i].tab[j].key);
			}
		}
	}
	return dst;
}

static int delkey(tablist_t *list, int id, char *key) {
	int idx = hash(key);
	if (list[id].tab[idx].key == NULL)
		return 1;
	while (strcmp(list[id].tab[idx].key, key) && idx < TABLEN)
		idx++;
	if (idx >= TABLEN)
		return 2;
	free(list[id].tab[idx].key);
	list[id].tab[idx].key = NULL;
	if (list[id].tab[idx].flag == 3) {
		free(list[id].tab[idx].value.str);
		list[id].tab[idx].value.str = NULL;
	}
	list[id].tab[idx].flag = 0;
	return 0;
}
