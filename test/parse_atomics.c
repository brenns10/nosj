/***************************************************************************/ /**

   @file         parse_atomics.c

   @author       Stephen Brennan

   @date         Created Wednesday, 25 November 2015

   @brief        Tests for parsing true, false, null.

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

static void test_parse_true(void)
{
	char input[] = "true";
	struct json_token tokens[1];
	struct json_parser p = json_parse(input, tokens, 1);
	TEST_ASSERT(p.error == JSONERR_NO_ERROR);
	TEST_ASSERT(p.tokenidx == 1);
	TEST_ASSERT(p.textidx == sizeof(input) / sizeof(char) - 1);
	TEST_ASSERT(tokens[0].type == JSON_TRUE);
	TEST_ASSERT(tokens[0].start == 0);
	TEST_ASSERT(tokens[0].end == 3);
	TEST_ASSERT(tokens[0].child == 0);
	TEST_ASSERT(tokens[0].next == 0);
}

static void test_parse_false(void)
{
	char input[] = "false";
	struct json_token tokens[1];
	struct json_parser p = json_parse(input, tokens, 1);
	TEST_ASSERT(p.error == JSONERR_NO_ERROR);
	TEST_ASSERT(p.tokenidx == 1);
	TEST_ASSERT(p.textidx == sizeof(input) / sizeof(char) - 1);
	TEST_ASSERT(tokens[0].type == JSON_FALSE);
	TEST_ASSERT(tokens[0].start == 0);
	TEST_ASSERT(tokens[0].end == 4);
	TEST_ASSERT(tokens[0].child == 0);
	TEST_ASSERT(tokens[0].next == 0);
}

static void test_parse_null(void)
{
	char input[] = "null";
	struct json_token tokens[1];
	struct json_parser p = json_parse(input, tokens, 1);
	TEST_ASSERT(p.error == JSONERR_NO_ERROR);
	TEST_ASSERT(p.tokenidx == 1);
	TEST_ASSERT(p.textidx == sizeof(input) / sizeof(char) - 1);
	TEST_ASSERT(tokens[0].type == JSON_NULL);
	TEST_ASSERT(tokens[0].start == 0);
	TEST_ASSERT(tokens[0].end == 3);
	TEST_ASSERT(tokens[0].child == 0);
	TEST_ASSERT(tokens[0].next == 0);
}

static void test_parse_empty_string(void)
{
	char input[] = "";
	struct json_parser p = json_parse(input, NULL, 0);
	TEST_ASSERT(p.error == JSONERR_PREMATURE_EOF);
}

static void test_parse_whitespace_string(void)
{
	char input[] = " \t\r\n";
	struct json_parser p = json_parse(input, NULL, 0);
	TEST_ASSERT(p.error == JSONERR_PREMATURE_EOF);
}

static void test_parse_invalid_true(void)
{
	char input[] = "trua";
	struct json_parser p = json_parse(input, NULL, 0);
	TEST_ASSERT(p.error == JSONERR_UNEXPECTED_TOKEN);
}

static void test_parse_invalid_false(void)
{
	char input[] = "flase";
	struct json_parser p = json_parse(input, NULL, 0);
	TEST_ASSERT(p.error == JSONERR_UNEXPECTED_TOKEN);
}

static void test_parse_invalid_null(void)
{
	char input[] = "nul";
	struct json_parser p = json_parse(input, NULL, 0);
	TEST_ASSERT(p.error == JSONERR_UNEXPECTED_TOKEN);
}

int main(void)
{
	UNITY_BEGIN();

	RUN_TEST(test_parse_true);
	RUN_TEST(test_parse_false);
	RUN_TEST(test_parse_null);
	RUN_TEST(test_parse_empty_string);
	RUN_TEST(test_parse_whitespace_string);
	RUN_TEST(test_parse_invalid_true);
	RUN_TEST(test_parse_invalid_false);
	RUN_TEST(test_parse_invalid_null);

	return UNITY_END();
}
