#include <stdlib.h>

#include "../unity/unity.h"
#include "../src/include/engine/engine.h"

tablist_t *list;

void setUp(void) {
	list = readdb("dbs/test.xdb");	
	return;
}

void tearDown(void) {
	delkeys(list, -1, NULL, 0);
	free(list);
	return;
}

extern void test_readdb(void);
extern void test_getkeys(void);

int main(void) {
	UNITY_BEGIN();
	RUN_TEST(test_readdb);
	RUN_TEST(test_getkeys);
	return UNITY_END();
}
