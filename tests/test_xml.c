#include <stdlib.h>

#include "unity/unity.h"
#include "src/include/xml/xml.h"

void test_xml_encode(void) {
	map_t map = (map_t) {
		.tag = "get",
		.payload = (map_t []) {
			{
				.tag = "test",
				.payload = "test",
				.n = 4,
				.size = sizeof(char),
				.attrs = NULL,
				.n_attrs = 0
			}
		},
		.size = sizeof(map_t),
		.n = 1,
		.attrs = NULL,
		.n_attrs = 0
	};
	char *xml = encode(&map);
	TEST_ASSERT_EQUAL_STRING("<get><test>test</test></get>", xml);
	free(xml);
}

void test_xml_decode(void) {
	map_t *decoded = decode("<get><test>test</test></get>");
	char *xml = encode(decoded);
	
	TEST_ASSERT_EQUAL_STRING("<get><test>test</test></get>", xml);
	free(xml);
}
