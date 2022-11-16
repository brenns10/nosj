/***************************************************************************/ /**

   @file         parse_arrays.c

   @author       Stephen Brennan

   @date         Created Tuesday, 24 November 2015

   @brief        Test array parsing.

   @copyright    Copyright (c) 2015, Stephen Brennan.  Released under the
 Revised BSD License.  See LICENSE.txt for details.

 *******************************************************************************/

#include <unity.h>

#include "nosj.h"

void setUp(void)
{
	// set stuff up here
}

void tearDown(void)
{
	// clean stuff up here
}

static void test_empty_array(void)
{
	char input[] = "[]";
	uint32_t ntok = 1;
	struct json_token tokens[ntok];
	struct json_parser p = json_parse(input, tokens, ntok);
	TEST_ASSERT(p.error == JSON_OK);
	TEST_ASSERT(p.tokenidx == ntok);
	TEST_ASSERT(p.textidx == sizeof(input) / sizeof(char) - 1);
	TEST_ASSERT(tokens[0].type == JSON_ARRAY);
	TEST_ASSERT(tokens[0].start == 0);
	TEST_ASSERT(tokens[0].length == 0);
	TEST_ASSERT(tokens[0].next == 0);
}

static void test_single_element(void)
{
	char input[] = "[1]";
	uint32_t ntok = 2, i;
	struct json_token tokens[ntok];
	struct json_token expected[] = {
		{ .type = JSON_ARRAY, .start = 0, .length = 1, .next = 0 },
		{ .type = JSON_NUMBER, .start = 1, .length = 0, .next = 0 },
	};
	struct json_parser p = json_parse(input, tokens, ntok);
	TEST_ASSERT(p.error == JSON_OK);
	TEST_ASSERT(p.tokenidx == ntok);
	TEST_ASSERT(p.textidx == sizeof(input) / sizeof(char) - 1);
	for (i = 0; i < ntok; i++) {
		TEST_ASSERT(tokens[i].type == expected[i].type);
		TEST_ASSERT(tokens[i].start == expected[i].start);
		TEST_ASSERT(tokens[i].next == expected[i].next);
	}
}

static void test_multiple_elements(void)
{
	uint32_t ntok = 3, i;
	char input[] = "[1, 2]";
	struct json_token tokens[ntok];
	struct json_token expected[] = {
		{ .type = JSON_ARRAY, .start = 0, .length = 2, .next = 0 },
		{ .type = JSON_NUMBER, .start = 1, .length = 0, .next = 2 },
		{ .type = JSON_NUMBER, .start = 4, .length = 0, .next = 0 },
	};
	struct json_parser p = json_parse(input, tokens, ntok);
	TEST_ASSERT(p.error == JSON_OK);
	TEST_ASSERT(p.tokenidx == ntok);
	TEST_ASSERT(p.textidx == sizeof(input) / sizeof(char) - 1);
	for (i = 0; i < ntok; i++) {
		TEST_ASSERT(tokens[i].type == expected[i].type);
		TEST_ASSERT(tokens[i].start == expected[i].start);
		TEST_ASSERT(tokens[i].next == expected[i].next);
	}
}

static void test_extra_comma(void)
{
	char input[] = "[1,]";
	uint32_t ntok = 2, i;
	struct json_token tokens[ntok];
	struct json_token expected[] = {
		{ .type = JSON_ARRAY, .start = 0, .length = 1, .next = 0 },
		{ .type = JSON_NUMBER, .start = 1, .length = 0, .next = 0 },
	};
	struct json_parser p = json_parse(input, tokens, ntok);
	TEST_ASSERT(p.error == JSON_OK);
	TEST_ASSERT(p.tokenidx == ntok);
	TEST_ASSERT(p.textidx == sizeof(input) / sizeof(char) - 1);
	for (i = 0; i < ntok; i++) {
		TEST_ASSERT(tokens[i].type == expected[i].type);
		TEST_ASSERT(tokens[i].start == expected[i].start);
		TEST_ASSERT(tokens[i].next == expected[i].next);
	}
}

static void test_no_end(void)
{
	char input[] = "[1,";
	struct json_parser p = json_parse(input, NULL, 0);
	TEST_ASSERT(p.error == JSONERR_PREMATURE_EOF);
}

static void test_error_within_list(void)
{
	char input[] = "[1e,";
	struct json_parser p = json_parse(input, NULL, 0);
	TEST_ASSERT(p.error == JSONERR_INVALID_NUMBER);
}

static void test_no_comma(void)
{
	char input[] = "[1 2 3]";
	struct json_parser p = json_parse(input, NULL, 0);
	TEST_ASSERT(p.error == JSONERR_MISSING_COMMA);
}

static void test_get(void)
{
	char input[] = "[1, null, true, \"hi\", {}]";
	struct json_token tokens[7];
	struct json_parser p = json_parse(input, tokens, 7);
	uint32_t res;
	int ret;

	// assertions for parsing correctly
	TEST_ASSERT(p.error == JSON_OK);
	TEST_ASSERT(p.tokenidx == 6);
	TEST_ASSERT(p.textidx == sizeof(input) / sizeof(char) - 1);

	// assertions for the results of json_array_get()
	ret = json_array_get(input, tokens, 0, 0, &res);
	TEST_ASSERT(ret == JSON_OK);
	TEST_ASSERT(res == 1);
	TEST_ASSERT(tokens[res].type == JSON_NUMBER);

	ret = json_array_get(input, tokens, 0, 1, &res);
	TEST_ASSERT(ret == JSON_OK);
	TEST_ASSERT(res == 2);
	TEST_ASSERT(tokens[res].type == JSON_NULL);

	ret = json_array_get(input, tokens, 0, 2, &res);
	TEST_ASSERT(ret == JSON_OK);
	TEST_ASSERT(res == 3);
	TEST_ASSERT(tokens[res].type == JSON_TRUE);

	ret = json_array_get(input, tokens, 0, 3, &res);
	TEST_ASSERT(ret == JSON_OK);
	TEST_ASSERT(res == 4);
	TEST_ASSERT(tokens[res].type == JSON_STRING);

	ret = json_array_get(input, tokens, 0, 4, &res);
	TEST_ASSERT(ret == JSON_OK);
	TEST_ASSERT(res == 5);
	TEST_ASSERT(tokens[res].type == JSON_OBJECT);

	ret = json_array_get(input, tokens, 0, 5, &res);
	TEST_ASSERT(ret == JSONERR_INDEX);
}

static void test_get_empty(void)
{
	char input[] = "[]";
	struct json_token tokens[1];
	json_parse(input, tokens, 1);
	uint32_t res;

	res = json_array_get(input, tokens, 0, 0, &res);
	TEST_ASSERT(res == JSONERR_INDEX);
}

int main(void)
{
	UNITY_BEGIN();

	RUN_TEST(test_empty_array);
	RUN_TEST(test_single_element);
	RUN_TEST(test_multiple_elements);
	RUN_TEST(test_extra_comma);
	RUN_TEST(test_no_end);
	RUN_TEST(test_error_within_list);
	RUN_TEST(test_no_comma);
	RUN_TEST(test_get);
	RUN_TEST(test_get_empty);

	return UNITY_END();
}
