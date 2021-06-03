/***************************************************************************/ /**

   @file         parse_objects.c

   @author       Stephen Brennan

   @date         Created Tuesday, 24 November 2015

   @brief        Test object parsing.

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

static void test_empty_object(void)
{
	char input[] = "{}";
	size_t ntok = 1;
	struct json_token tokens[ntok];
	struct json_parser p = json_parse(input, tokens, ntok);
	TEST_ASSERT(p.error == JSONERR_NO_ERROR);
	TEST_ASSERT(p.tokenidx == ntok);
	TEST_ASSERT(p.textidx == sizeof(input) / sizeof(char) - 1);
	TEST_ASSERT(tokens[0].type == JSON_OBJECT);
	TEST_ASSERT(tokens[0].start == 0);
	TEST_ASSERT(tokens[0].end == 1);
	TEST_ASSERT(tokens[0].length == 0);
	TEST_ASSERT(tokens[0].child == 0);
	TEST_ASSERT(tokens[0].next == 0);
}

static void test_single_element(void)
{
	char input[] = "{\"a\": 1}";
	size_t ntok = 3, i;
	struct json_token tokens[ntok];
	struct json_token expected[] = {
		{ .type = JSON_OBJECT,
		  .start = 0,
		  .end = 7,
		  .length = 1,
		  .child = 1,
		  .next = 0 },
		{ .type = JSON_STRING,
		  .start = 1,
		  .end = 3,
		  .length = 1,
		  .child = 2,
		  .next = 0 },
		{ .type = JSON_NUMBER,
		  .start = 6,
		  .end = 6,
		  .length = 0,
		  .child = 0,
		  .next = 0 },
	};
	struct json_parser p = json_parse(input, tokens, ntok);
	TEST_ASSERT(p.error == JSONERR_NO_ERROR);
	TEST_ASSERT(p.tokenidx == ntok);
	TEST_ASSERT(p.textidx == sizeof(input) / sizeof(char) - 1);
	for (i = 0; i < ntok; i++) {
		TEST_ASSERT(tokens[i].type == expected[i].type);
		TEST_ASSERT(tokens[i].start == expected[i].start);
		TEST_ASSERT(tokens[i].end == expected[i].end);
		TEST_ASSERT(tokens[i].length == expected[i].length);
		TEST_ASSERT(tokens[i].child == expected[i].child);
		TEST_ASSERT(tokens[i].next == expected[i].next);
	}
}

static void test_multiple_elements(void)
{
	char input[] = "{\"a\": 1, \"b\": 2}";
	size_t ntok = 5, i;
	struct json_token tokens[ntok];
	struct json_token expected[] = {
		{ .type = JSON_OBJECT,
		  .start = 0,
		  .end = 15,
		  .length = 2,
		  .child = 1,
		  .next = 0 },
		{ .type = JSON_STRING,
		  .start = 1,
		  .end = 3,
		  .length = 1,
		  .child = 2,
		  .next = 3 },
		{ .type = JSON_NUMBER,
		  .start = 6,
		  .end = 6,
		  .length = 0,
		  .child = 0,
		  .next = 0 },
		{ .type = JSON_STRING,
		  .start = 9,
		  .end = 11,
		  .length = 1,
		  .child = 4,
		  .next = 0 },
		{ .type = JSON_NUMBER,
		  .start = 14,
		  .end = 14,
		  .length = 0,
		  .child = 0,
		  .next = 0 },
	};
	struct json_parser p = json_parse(input, tokens, ntok);
	TEST_ASSERT(p.error == JSONERR_NO_ERROR);
	TEST_ASSERT(p.tokenidx == ntok);
	TEST_ASSERT(p.textidx == sizeof(input) / sizeof(char) - 1);
	for (i = 0; i < ntok; i++) {
		TEST_ASSERT(tokens[i].type == expected[i].type);
		TEST_ASSERT(tokens[i].start == expected[i].start);
		TEST_ASSERT(tokens[i].end == expected[i].end);
		TEST_ASSERT(tokens[i].length == expected[i].length);
		TEST_ASSERT(tokens[i].child == expected[i].child);
		TEST_ASSERT(tokens[i].next == expected[i].next);
	}
}

static void test_extra_comma(void)
{
	char input[] = "{\"a\": 1,}";
	size_t ntok = 3, i;
	struct json_token tokens[ntok];
	struct json_token expected[] = {
		{ .type = JSON_OBJECT,
		  .start = 0,
		  .end = 8,
		  .length = 1,
		  .child = 1,
		  .next = 0 },
		{ .type = JSON_STRING,
		  .start = 1,
		  .end = 3,
		  .length = 1,
		  .child = 2,
		  .next = 0 },
		{ .type = JSON_NUMBER,
		  .start = 6,
		  .end = 6,
		  .length = 0,
		  .child = 0,
		  .next = 0 },
	};
	struct json_parser p = json_parse(input, tokens, ntok);
	TEST_ASSERT(p.error == JSONERR_NO_ERROR);
	TEST_ASSERT(p.tokenidx == ntok);
	TEST_ASSERT(p.textidx == sizeof(input) / sizeof(char) - 1);
	for (i = 0; i < ntok; i++) {
		TEST_ASSERT(tokens[i].type == expected[i].type);
		TEST_ASSERT(tokens[i].start == expected[i].start);
		TEST_ASSERT(tokens[i].end == expected[i].end);
		TEST_ASSERT(tokens[i].length == expected[i].length);
		TEST_ASSERT(tokens[i].child == expected[i].child);
		TEST_ASSERT(tokens[i].next == expected[i].next);
	}
}

static void test_no_end(void)
{
	char input[] = "{\"a\": 1,";
	struct json_parser p = json_parse(input, NULL, 0);
	TEST_ASSERT(p.error == JSONERR_PREMATURE_EOF);
}

static void test_no_colon(void)
{
	char input[] = "{\"blah\" 2}";
	struct json_parser p = json_parse(input, NULL, 0);
	TEST_ASSERT(p.error == JSONERR_EXPECTED_TOKEN);
	TEST_ASSERT(p.errorarg == ':');
}

static void test_missing_value(void)
{
	char input[] = "{\"blah\":}";
	struct json_parser p = json_parse(input, NULL, 0);
	TEST_ASSERT(p.error == JSONERR_UNEXPECTED_TOKEN);
}

static void test_no_key(void)
{
	char input[] = "{:2}";
	struct json_parser p = json_parse(input, NULL, 0);
	TEST_ASSERT(p.error == JSONERR_UNEXPECTED_TOKEN);
}

static void test_number_key(void)
{
	char input[] = "{1:2}";
	struct json_parser p = json_parse(input, NULL, 0);
	TEST_ASSERT(p.error == JSONERR_UNEXPECTED_TOKEN);
}

static void test_true_key(void)
{
	char input[] = "{true:2}";
	struct json_parser p = json_parse(input, NULL, 0);
	TEST_ASSERT(p.error == JSONERR_UNEXPECTED_TOKEN);
}

static void test_false_key(void)
{
	char input[] = "{false:2}";
	struct json_parser p = json_parse(input, NULL, 0);
	TEST_ASSERT(p.error == JSONERR_UNEXPECTED_TOKEN);
}

static void test_null_key(void)
{
	char input[] = "{null:2}";
	struct json_parser p = json_parse(input, NULL, 0);
	TEST_ASSERT(p.error == JSONERR_UNEXPECTED_TOKEN);
}

static void test_list_key(void)
{
	char input[] = "{[]:2}";
	struct json_parser p = json_parse(input, NULL, 0);
	TEST_ASSERT(p.error == JSONERR_UNEXPECTED_TOKEN);
}

static void test_object_key(void)
{
	char input[] = "{{}:2}";
	struct json_parser p = json_parse(input, NULL, 0);
	TEST_ASSERT(p.error == JSONERR_UNEXPECTED_TOKEN);
}

static void test_no_comma(void)
{
	char input[] = "{\"a\":2 \"b\":\"blah\"}";
	struct json_parser p = json_parse(input, NULL, 0);
	TEST_ASSERT(p.error == JSONERR_EXPECTED_TOKEN);
	TEST_ASSERT(p.errorarg == L',');
}

static void test_get_object(void)
{
	char input[] = "{\"a\":2, \"b\":\"blah\"}";
	struct json_token tokens[5];
	struct json_parser p = json_parse(input, tokens, 5);
	TEST_ASSERT(p.error == JSONERR_NO_ERROR);
	TEST_ASSERT(p.tokenidx == 5);
	TEST_ASSERT(p.textidx == sizeof(input) / sizeof(char) - 1);
	size_t value = json_object_get(input, tokens, 0, "a");
	TEST_ASSERT(value == 2);
	TEST_ASSERT(tokens[value].type == JSON_NUMBER);
	value = json_object_get(input, tokens, 0, "b");
	TEST_ASSERT(value == 4);
	TEST_ASSERT(tokens[value].type == JSON_STRING);
	TEST_ASSERT(tokens[value].start == 12);
	value = json_object_get(input, tokens, 0, "c");
	TEST_ASSERT(value == 0);
}

int main(void)
{
	UNITY_BEGIN();

	RUN_TEST(test_empty_object);
	RUN_TEST(test_single_element);
	RUN_TEST(test_multiple_elements);
	RUN_TEST(test_extra_comma);
	RUN_TEST(test_no_end);
	RUN_TEST(test_no_colon);
	RUN_TEST(test_missing_value);
	RUN_TEST(test_no_key);
	RUN_TEST(test_number_key);
	RUN_TEST(test_true_key);
	RUN_TEST(test_false_key);
	RUN_TEST(test_null_key);
	RUN_TEST(test_list_key);
	RUN_TEST(test_object_key);
	RUN_TEST(test_no_comma);
	RUN_TEST(test_get_object);

	return UNITY_END();
}
