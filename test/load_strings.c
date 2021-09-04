/***************************************************************************/ /**

   @file         load_strings.c

   @author       Stephen Brennan

   @date         Created Wednesday,  9 December 2015

   @brief        Tests for comparing strings.

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

static void test_normal_string(void)
{
	char input[] = "\"hello\"";
	char string[] = "hello";
	char buffer[6];
	struct json_token tokens[1];
	struct json_parser p = json_parse(input, tokens, 1);
	TEST_ASSERT(p.error == JSONERR_NO_ERROR);
	TEST_ASSERT(p.tokenidx == 1);
	TEST_ASSERT(p.textidx == sizeof(input) / sizeof(char) - 1);
	TEST_ASSERT(tokens[0].start == 0);
	TEST_ASSERT(tokens[0].end == 6);
	TEST_ASSERT(tokens[0].length == 5);
	json_string_load(input, tokens, 0, buffer);
	TEST_ASSERT(0 == strcmp(buffer, string));
}

static void test_escape_quote(void)
{
	char input[] = "\"he\\\"llo\"";
	char string[] = "he\"llo";
	char buffer[7];
	struct json_token tokens[1];
	struct json_parser p = json_parse(input, tokens, 1);
	TEST_ASSERT(p.error == JSONERR_NO_ERROR);
	TEST_ASSERT(p.tokenidx == 1);
	TEST_ASSERT(p.textidx == sizeof(input) / sizeof(char) - 1);
	TEST_ASSERT(tokens[0].start == 0);
	TEST_ASSERT(tokens[0].end == 8);
	TEST_ASSERT(tokens[0].length == 6);
	json_string_load(input, tokens, 0, buffer);
	TEST_ASSERT(0 == strcmp(buffer, string));
}

static void test_escape_backslash(void)
{
	char input[] = "\"he\\\\llo\"";
	char string[] = "he\\llo";
	char buffer[7];
	struct json_token tokens[1];
	struct json_parser p = json_parse(input, tokens, 1);
	TEST_ASSERT(p.error == JSONERR_NO_ERROR);
	TEST_ASSERT(p.tokenidx == 1);
	TEST_ASSERT(p.textidx == sizeof(input) / sizeof(char) - 1);
	TEST_ASSERT(tokens[0].start == 0);
	TEST_ASSERT(tokens[0].end == 8);
	TEST_ASSERT(tokens[0].length == 6);
	json_string_load(input, tokens, 0, buffer);
	TEST_ASSERT(0 == strcmp(buffer, string));
}

static void test_unicode_escape(void)
{
	char input[] = "\"he\\u006Clo\"";
	char expected[] = "hello";
	char buffer[6];
	struct json_token tokens[1];
	struct json_parser p = json_parse(input, tokens, 1);
	TEST_ASSERT_EQUAL_INT(JSONERR_NO_ERROR, p.error);
	TEST_ASSERT_EQUAL_INT(1, p.tokenidx);
	TEST_ASSERT_EQUAL_INT(sizeof(input) - 1, p.textidx);
	TEST_ASSERT_EQUAL_INT(0, tokens[0].start);
	TEST_ASSERT_EQUAL_INT(11, tokens[0].end);
	TEST_ASSERT_EQUAL_INT(5, tokens[0].length);
	json_string_load(input, tokens, 0, buffer);
	TEST_ASSERT_EQUAL_STRING(expected, buffer);
}

static void test_surrogate_pair(void)
{
	char input[] = "\"\\u00a2\\u0939\\u20ac\\uD83D\\uDCA9\"";
	char expected[] = "¢ह€💩";
	char buffer[13];
	struct json_token tokens[1];
	struct json_parser p = json_parse(input, tokens, 1);
	TEST_ASSERT_EQUAL_INT(JSONERR_NO_ERROR, p.error);
	TEST_ASSERT_EQUAL_INT(1, p.tokenidx);
	TEST_ASSERT_EQUAL_INT(sizeof(input) - 1, p.textidx);
	TEST_ASSERT_EQUAL_INT(0, tokens[0].start);
	TEST_ASSERT_EQUAL_INT(31, tokens[0].end);
	TEST_ASSERT_EQUAL_INT(12, tokens[0].length);
	json_string_load(input, tokens, 0, buffer);
	TEST_ASSERT_EQUAL_STRING(expected, buffer);
}

static void test_unicode_undisturbed(void)
{
	char input[] = "\"¢ह€💩\"";
	char expected[] = "¢ह€💩";
	char buffer[13];
	struct json_token tokens[1];
	struct json_parser p = json_parse(input, tokens, 1);
	TEST_ASSERT_EQUAL_INT(JSONERR_NO_ERROR, p.error);
	TEST_ASSERT_EQUAL_INT(1, p.tokenidx);
	TEST_ASSERT_EQUAL_INT(sizeof(input) - 1, p.textidx);
	TEST_ASSERT_EQUAL_INT(0, tokens[0].start);
	TEST_ASSERT_EQUAL_INT(13, tokens[0].end);
	TEST_ASSERT_EQUAL_INT(12, tokens[0].length);
	json_string_load(input, tokens, 0, buffer);
	TEST_ASSERT_EQUAL_STRING(expected, buffer);
}

int main(void)
{
	UNITY_BEGIN();
	RUN_TEST(test_normal_string);
	RUN_TEST(test_escape_quote);
	RUN_TEST(test_escape_backslash);
	RUN_TEST(test_unicode_escape);
	RUN_TEST(test_surrogate_pair);
	RUN_TEST(test_unicode_undisturbed);
	return UNITY_END();
}
