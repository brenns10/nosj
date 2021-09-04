/***************************************************************************/ /**

   @file         parse_strings.c

   @author       Stephen Brennan

   @date         Created Tuesday, 24 November 2015

   @brief        Test string parsing.

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

static void test_empty_string(void)
{
	char input[] = "\"\"";
	struct json_parser p = json_parse(input, NULL, 0);
	TEST_ASSERT(p.error == JSONERR_NO_ERROR);
	TEST_ASSERT(p.tokenidx == 1);
	TEST_ASSERT(p.textidx == sizeof(input) / sizeof(char) - 1);
}

static void test_single_char(void)
{
	char input[] = "\"a\"";
	struct json_parser p = json_parse(input, NULL, 0);
	TEST_ASSERT(p.error == JSONERR_NO_ERROR);
	TEST_ASSERT(p.tokenidx == 1);
	TEST_ASSERT(p.textidx == sizeof(input) / sizeof(char) - 1);
}

static void test_no_end(void)
{
	char input[] = "\"blah";
	struct json_parser p = json_parse(input, NULL, 0);
	TEST_ASSERT(p.error == JSONERR_PREMATURE_EOF);
}

static void test_escape(void)
{
	char input[] = "\"blah\\\"blah\"";
	struct json_parser p = json_parse(input, NULL, 0);
	TEST_ASSERT(p.error == JSONERR_NO_ERROR);
	TEST_ASSERT(p.tokenidx == 1);
	TEST_ASSERT(p.textidx == sizeof(input) / sizeof(char) - 1);
}

static void test_escaped_end(void)
{
	char input[] = "\"blah\\";
	struct json_parser p = json_parse(input, NULL, 0);
	TEST_ASSERT(p.error == JSONERR_PREMATURE_EOF);
}

static void test_valid_uesc(void)
{
	char input[] = "\"blah\\u1a2Bblah\"";
	struct json_parser p = json_parse(input, NULL, 0);
	TEST_ASSERT(p.error == JSONERR_NO_ERROR);
	TEST_ASSERT(p.tokenidx == 1);
	TEST_ASSERT(p.textidx == sizeof(input) / sizeof(char) - 1);
}

static void test_too_short_uesc(void)
{
	char input[] = "\"blah\\u1a\"";
	struct json_parser p = json_parse(input, NULL, 0);
	TEST_ASSERT(p.error == JSONERR_UNEXPECTED_TOKEN);
}

static void test_string_end_uesc(void)
{
	char input[] = "\"blah\\u1a";
	struct json_parser p = json_parse(input, NULL, 0);
	TEST_ASSERT(p.error == JSONERR_PREMATURE_EOF);
}

static void test_invalid_char_uesc(void)
{
	char input[] = "\"blah\\u1aG-\"";
	struct json_parser p = json_parse(input, NULL, 0);
	TEST_ASSERT(p.error == JSONERR_UNEXPECTED_TOKEN);
}

static void test_valid_esc(void)
{
	char input[] = "\"\\a\"";
	char valid[] = "\"\\/bfnrt";
	size_t i;
	struct json_parser p;
	for (i = 0; valid[i] != '\0'; i++) {
		input[2] = valid[i];
		p = json_parse(input, NULL, 0);
		TEST_ASSERT(p.error == JSONERR_NO_ERROR);
		TEST_ASSERT(p.tokenidx == 1);
		TEST_ASSERT(p.textidx == sizeof(input) / sizeof(char) - 1);
	}
}

static void test_invalid_esc(void)
{
	char input[] = "\"\\a\"";
	char valid[] = "aAB12.,[(%!"; // something of a cross-section!
	size_t i;
	struct json_parser p;
	for (i = 0; valid[i] != '\0'; i++) {
		input[2] = valid[i];
		p = json_parse(input, NULL, 0);
		TEST_ASSERT(p.error == JSONERR_UNEXPECTED_TOKEN);
	}
}

int main(void)
{
	UNITY_BEGIN();

	RUN_TEST(test_empty_string);
	RUN_TEST(test_single_char);
	RUN_TEST(test_no_end);
	RUN_TEST(test_escape);
	RUN_TEST(test_escaped_end);
	RUN_TEST(test_valid_uesc);
	RUN_TEST(test_too_short_uesc);
	RUN_TEST(test_string_end_uesc);
	RUN_TEST(test_invalid_char_uesc);
	RUN_TEST(test_valid_esc);
	RUN_TEST(test_invalid_esc);

	return UNITY_END();
}
