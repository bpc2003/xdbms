#include <stdio.h>
#include <stdlib.h>

#include "unity/unity.h"
#include "src/include/engine/engine.h"

extern tablist_t *list;

void test_readdb(void) {
	tablist_t *db = readdb("dbs/test.xdb");
	TEST_ASSERT_NOT_NULL(db);
	delkeys(db, -1, NULL, 0);
	free(db);
}

void test_getkeys(void) {
	tablist_t *tmp = getkeys(list, -1, NULL, 0);
	TEST_ASSERT_NOT_NULL(tmp);
	for (int i = 0; i < tmp[0].len; ++i) {
		printf("id: %d\t", i);
		for (int j = 0; tmp[i].tab[j].flag; ++j)
			printf("%s\t", tmp[i].tab[j].key);
		printf("\n");
	}
	free(tmp);
}

void test_getkeys_single(void) {
	tablist_t *tmp = getkeys(list, 0, NULL, 0);
	TEST_ASSERT_NOT_NULL(tmp);
	free(tmp);
}

void test_getkeys_singlekey(void) {
	char *keys[] = { "Row_1" };
	tablist_t *tmp = getkeys(list, 0, keys, 1);
	TEST_ASSERT_NOT_NULL(tmp);
	free(tmp);
}

void test_getkeys_fail(void) {
	char *keys[] = { "Row_4" };
	tablist_t *tmp = getkeys(list, -1, keys, 1);
	TEST_ASSERT_NULL(tmp);
}
