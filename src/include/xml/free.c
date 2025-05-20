#include <stdlib.h>

#include "xml.h"

void freemap(map_t *map)
{
	if (map->n_attrs) {
		for (int i = 0; i < map->n_attrs; ++i) {
			free(map->attrs[i].id);
			free(map->attrs[i].value);
		}
		free(map->attrs);
	}
	if(map->size == sizeof(map_t)) {
		for (int i = 0; i < map->n; ++i) {
			map_t *pl = (map_t *)map->payload;
			if (pl[i].n_attrs) {
				for (int j = 0; j < pl[i].n_attrs; ++j) {
					free(pl[i].attrs[j].id);
					free(pl[i].attrs[j].value);
				}
				free(pl[i].attrs);
			}
			if (pl[i].size == sizeof(map_t))
				freemap(pl[i].payload);
			else
				free(pl[i].payload);
			free(pl[i].tag);
		}
		free(map->payload);
	}
	else
		free(map->payload);
	free(map->tag);
	free(map);
}
