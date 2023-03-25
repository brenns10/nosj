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
	uint32_t ntok = 1;
	struct json_token tokens[ntok];
	struct json_parser p = json_parse(input, tokens, ntok);
	TEST_ASSERT(p.error == JSON_OK);
	TEST_ASSERT(p.tokenidx == ntok);
	TEST_ASSERT(p.textidx == sizeof(input) / sizeof(char) - 1);
	TEST_ASSERT(tokens[0].type == JSON_OBJECT);
	TEST_ASSERT(tokens[0].start == 0);
	TEST_ASSERT(tokens[0].length == 0);
	TEST_ASSERT(tokens[0].next == 0);
}

static void test_single_element(void)
{
	char input[] = "{\"a\": 1}";
	uint32_t ntok = 3, i;
	struct json_token tokens[ntok];
	struct json_token expected[] = {
		{ .type = JSON_OBJECT, .start = 0, .length = 1, .next = 0 },
		{ .type = JSON_STRING, .start = 1, .length = 1, .next = 0 },
		{ .type = JSON_NUMBER, .start = 6, .length = 1, .next = 0 },
	};
	struct json_parser p = json_parse(input, tokens, ntok);
	TEST_ASSERT(p.error == JSON_OK);
	TEST_ASSERT(p.tokenidx == ntok);
	TEST_ASSERT(p.textidx == sizeof(input) / sizeof(char) - 1);
	for (i = 0; i < ntok; i++) {
		TEST_ASSERT(tokens[i].type == expected[i].type);
		TEST_ASSERT(tokens[i].start == expected[i].start);
		TEST_ASSERT(tokens[i].length == expected[i].length);
		TEST_ASSERT(tokens[i].next == expected[i].next);
	}
}

static void test_multiple_elements(void)
{
	char input[] = "{\"a\": 1, \"b\": 2}";
	uint32_t ntok = 5, i;
	struct json_token tokens[ntok];
	struct json_token expected[] = {
		{ .type = JSON_OBJECT, .start = 0, .length = 2, .next = 0 },
		{ .type = JSON_STRING, .start = 1, .length = 1, .next = 3 },
		{ .type = JSON_NUMBER, .start = 6, .length = 1, .next = 0 },
		{ .type = JSON_STRING, .start = 9, .length = 1, .next = 0 },
		{ .type = JSON_NUMBER, .start = 14, .length = 1, .next = 0 },
	};
	struct json_parser p = json_parse(input, tokens, ntok);
	TEST_ASSERT(p.error == JSON_OK);
	TEST_ASSERT(p.tokenidx == ntok);
	TEST_ASSERT(p.textidx == sizeof(input) / sizeof(char) - 1);
	for (i = 0; i < ntok; i++) {
		TEST_ASSERT(tokens[i].type == expected[i].type);
		TEST_ASSERT(tokens[i].start == expected[i].start);
		TEST_ASSERT(tokens[i].length == expected[i].length);
		TEST_ASSERT(tokens[i].next == expected[i].next);
	}
}

static void test_extra_comma(void)
{
	char input[] = "{\"a\": 1,}";
	uint32_t ntok = 3, i;
	struct json_token tokens[ntok];
	struct json_token expected[] = {
		{ .type = JSON_OBJECT, .start = 0, .length = 1, .next = 0 },
		{ .type = JSON_STRING, .start = 1, .length = 1, .next = 0 },
		{ .type = JSON_NUMBER, .start = 6, .length = 1, .next = 0 },
	};
	struct json_parser p = json_parse(input, tokens, ntok);
	TEST_ASSERT(p.error == JSON_OK);
	TEST_ASSERT(p.tokenidx == ntok);
	TEST_ASSERT(p.textidx == sizeof(input) / sizeof(char) - 1);
	for (i = 0; i < ntok; i++) {
		TEST_ASSERT(tokens[i].type == expected[i].type);
		TEST_ASSERT(tokens[i].start == expected[i].start);
		TEST_ASSERT(tokens[i].length == expected[i].length);
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
	TEST_ASSERT(p.error == JSONERR_MISSING_COLON);
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
	TEST_ASSERT(p.error == JSONERR_MISSING_COMMA);
}

static void test_get_object(void)
{
	char input[] = "{\"a\":2, \"b\":\"blah\"}";
	struct json_token tokens[5];
	struct json_parser p = json_parse(input, tokens, 5);
	uint32_t value;
	TEST_ASSERT(p.error == JSON_OK);
	TEST_ASSERT(p.tokenidx == 5);
	TEST_ASSERT(p.textidx == sizeof(input) / sizeof(char) - 1);
	TEST_ASSERT(!json_object_get(input, tokens, 0, "a", &value));
	TEST_ASSERT(value == 2);
	TEST_ASSERT(tokens[value].type == JSON_NUMBER);
	TEST_ASSERT(!json_object_get(input, tokens, 0, "b", &value));
	TEST_ASSERT(value == 4);
	TEST_ASSERT(tokens[value].type == JSON_STRING);
	TEST_ASSERT(tokens[value].start == 12);
	TEST_ASSERT_EQUAL(JSONERR_LOOKUP,
	                  json_object_get(input, tokens, 0, "c", &value));
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
