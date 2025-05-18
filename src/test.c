#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "include/engine/engine.h"
#include "include/xml/xml.h"

void test_writedb(void)
{
	tablist_t *list = readdb("dbs/test.db");
	writedb("dbs/test.db", list);
	delkeys(list, -1, NULL, 0);
	free(list);
}

void test_readdb(void)
{
	tablist_t *list = readdb("dbs/test.db");
	tablist_t *indices = getkeys(list, -1, NULL, 0);
	for (int i = 0; i < indices[0].len; ++i) {
		printf("id: %d\n", i);
		for (int j = 0; indices[i].tab[j].flag; ++j)
			printf("%s\n", indices[i].tab[j].key);
	}
	free(indices);
	delkeys(list, -1, NULL, 0);
	free(list);
}


void test_setkeys(void)
{
	tablist_t *list = readdb("dbs/test.db");
	char *pairs[] = { "name:John" };
	if (setkeys(&list, -1, pairs, 1)) {
		fprintf(stderr, "test_setkeys: failed\n");
		return;
	}
	for (int i = 0; i < list[0].len; ++i) {
		printf("id: %d\n", i);
		tablist_t *indexes = getkeys(list, i, NULL, 0);
		for (int j = 0; indexes[0].tab[j].flag; ++j)
			printf("%s\n", indexes[0].tab[j].key);
		free(indexes);
	}
	delkeys(list, -1, NULL, 0);
	free(list);
}

void test_setkeys_fail(void)
{
	tablist_t *list = readdb("dbs/test.db");
	char *pairs[] = { "namejohn" };
	if (!setkeys(&list, -1, pairs, 1)) {
		fprintf(stderr, "test_setkeys_fail: failed\n");
		return;
	}
	delkeys(list, -1, NULL, 0);
	free(list);
}

void test_setkeys_multi_fail(void)
{
	tablist_t *list = readdb("dbs/test.db");
	char *pairs[] = { "name:John", NULL };
	if (!setkeys(&list, -1, pairs, 2)) {
		fprintf(stderr, "test_setkeys_multi_fail: failed\n");
		return;
	}
	delkeys(list, -1, NULL, 0);
	free(list);
}

void test_setkeys_single(void)
{
	tablist_t *list = readdb("dbs/test.db");
	char *pairs[] = { "name:Alice" };
	if (setkeys(&list, 101, pairs, 1)) {
		fprintf(stderr, "test_setkeys_single: failed\n");
		return;
	}
	delkeys(list, -1, NULL, 0);
	free(list);
}

void test_setkeys_multipairs(void)
{
	tablist_t *list = readdb("dbs/test.db");
	char *pairs[] = { "name:Bob", "active:true" };
	if (setkeys(&list, 0, pairs, 2)) {
		fprintf(stderr, "test_setkeys_multipairs: failed\n");
		return;
	}
	delkeys(list, -1, NULL, 0);
	free(list);
}

void test_delkeys(void)
{
	tablist_t *list = readdb("dbs/test.db");
	char *keys[] = { "Row_1" };
	if (delkeys(list, -1, keys, 1)) {
		fprintf(stderr, "test_delkeys: failed\n");
		return;
	}
	for (int i = 0; i < list[0].len; ++i) {
		printf("id: %d\n", i);
		tablist_t *indexes = getkeys(list, i, NULL, 0);
		for (int j = 0; indexes[0].tab[j].flag; ++j)
			printf("%s\n", indexes[0].tab[j].key);
		free(indexes);
	}
	delkeys(list, -1, NULL, 0);
	free(list);
}

void test_delkeys_all(void)
{
	tablist_t *list = readdb("dbs/test.db");
	if (delkeys(list, -1, NULL, 0)) {
		fprintf(stderr, "test_delkeys_all: failed\n");
		return;
	}
	free(list);
}

void test_delkeys_fail(void)
{
	tablist_t *list = readdb("dbs/test.db");
	char *keys[] = { "Row_4" };
	if (!delkeys(list, -1, keys, 1)) {
		fprintf(stderr, "test_delkeys_fail: failed\n");
		return;
	}
	delkeys(list, -1, NULL, 0);
	free(list);
}

void test_delkeys_single(void)
{
	tablist_t *list = readdb("dbs/test.db");
	if (delkeys(list, 0, NULL, 0)) {
		fprintf(stderr, "test_delkeys_single: failed\n");
		return;
	}
	delkeys(list, -1, NULL, 0);
	free(list);
}

void test_delkeys_multi(void)
{
	tablist_t *list = readdb("dbs/test.db");
	char *keys[] = { "Row_1", "Row_2" };
	if (delkeys(list, -1, keys, 2)) {
		fprintf(stderr, "test_delkeys_multi: failed\n");
		return;
	}
	delkeys(list, -1, NULL, 0);
	free(list);
}

void test_getkeys_multi(void)
{
	tablist_t *list = readdb("dbs/test.db");
	char *keys[] = { "Row_1", "Row_2" };
	tablist_t *ret = getkeys(list, 0, keys, 2);
	if (ret == NULL) {
		fprintf(stderr, "test_getkeys_multi: failed\n");
		return;
	}
	for (int i = 0; ret[0].tab[i].flag; ++i)
		printf("%s\n", ret[0].tab[i].key);
	free(ret);
	delkeys(list, -1, NULL, 0);
	free(list);
}

void test_getkeys_multi_fail(void)
{
	tablist_t *list = readdb("dbs/test.db");
	char *keys[] = { "Row_1", "Row_4" };
	tablist_t *ret = getkeys(list, 0, keys, 2);
	if (ret) {
		fprintf(stderr, "test_getkeys_multi_fail: failed\n");
		return;
	}
	delkeys(list, -1, NULL, 0);
	free(list);
}

void test_getkeys(void)
{
	tablist_t *list = readdb("dbs/test.db");
	tablist_t *ret = getkeys(list, -1, NULL, 0);
	if (!ret) {
		fprintf(stderr, "test_getkeys: failed\n");
		return;
	}
	free(ret);
	delkeys(list, -1, NULL, 0);
	free(list);
}

void test_encode(void)
{
	map_t *map = calloc(1, sizeof(map_t));
	map->attrs = NULL;
	map->n_attrs = 0;
	map->tag = "documents";
	map->size = sizeof(map_t);
	map->n = 2;
	map->payload = (map_t []) {
		{
			.attrs = (attr_t []) {
			  { .id = "id", .value = "0" }
			},
			.n_attrs = 1,
			.tag = "document",
			.payload = (map_t []) {
			  {
			    .attrs = NULL,
			    .n_attrs = 0,
			    .tag = "test",
			    .payload = "test",
			    .size = sizeof(char),
			    .n = 4
			  }
			},
			.size = sizeof(map_t),
			.n = 1
		},
		{
			.attrs = (attr_t []) {
			  { .id = "id", .value = "1" }
			},
			.n_attrs = 1,
			.tag = "document",
			.payload = (map_t []) {
			  {
			    .attrs = NULL,
			    .n_attrs = 0,
			    .tag = "test",
			    .payload = "test",
			    .size = sizeof(char),
			    .n = 4
			  }
			},
			.size = sizeof(map_t),
			.n = 1
		}
	};

	char *xml = encode(map, 1);
	if (xml == NULL) {
		fprintf(stderr, "test_encode: failed\n");
		return;
	}
	printf("%s\n", xml);
	free(map);
	free(xml);
}

void test_decode(void)
{
	char *xml =
		"<set id='0' test='true'><key1>value1</key1><key2>value2</key2></set>";
	int len = 0, start = 0;
	map_t *map = decode(xml, &start, &len);
	printf("%s\n", (xml = encode(map, 1)));
	free(xml);
	// TODO: create a freemap function
	free(map->tag);
	free(map->attrs[0].id);
	free(map->attrs[0].value);
	free(map->attrs[1].id);
	free(map->attrs[1].value);
	free(map->attrs);
	map_t *pl = (map_t *) map->payload;
	free(pl[0].tag);
	free(pl[1].tag);
	free(pl[0].payload);
	free(pl[1].payload);
	free(map->payload);
	free(map);
}

int main(void)
{
	// test_writedb();
	// test_readdb();
	// test_getkeys();
	// test_getkeys_multi();
	// test_getkeys_multi_fail();
	//
	// test_setkeys();
	// test_setkeys_fail();
	// test_setkeys_single();
	// test_setkeys_multipairs();
	// test_setkeys_multi_fail();
	//
	// test_delkeys();
	// test_delkeys_all();
	// test_delkeys_fail();
	// test_delkeys_single();
	// test_delkeys_multi();
	test_encode();
	test_decode();

	exit(0);
}
