/***************************************************************************/ /**

   @file         compare_strings.c

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
	struct json_token tokens[1];
	struct json_parser p = json_parse(input, tokens, 1);
	TEST_ASSERT(p.error == JSONERR_NO_ERROR);
	TEST_ASSERT(p.tokenidx == 1);
	TEST_ASSERT(p.textidx == sizeof(input) / sizeof(char) - 1);
	TEST_ASSERT(json_string_match(input, tokens, 0, string));
	TEST_ASSERT(tokens[0].start == 0);
	TEST_ASSERT(tokens[0].end == 6);
	TEST_ASSERT(tokens[0].length == 5);
}

static void test_normal_nomatch(void)
{
	char input[] = "\"hello\"";
	char string[] = "hellO";
	struct json_token tokens[1];
	struct json_parser p = json_parse(input, tokens, 1);
	TEST_ASSERT(p.error == JSONERR_NO_ERROR);
	TEST_ASSERT(p.tokenidx == 1);
	TEST_ASSERT(p.textidx == sizeof(input) / sizeof(char) - 1);
	TEST_ASSERT(!json_string_match(input, tokens, 0, string));
	TEST_ASSERT(tokens[0].start == 0);
	TEST_ASSERT(tokens[0].end == 6);
	TEST_ASSERT(tokens[0].length == 5);
}

static void test_normal_too_long(void)
{
	char input[] = "\"hello\"";
	char string[] = "hello there";
	struct json_token tokens[1];
	struct json_parser p = json_parse(input, tokens, 1);
	TEST_ASSERT(p.error == JSONERR_NO_ERROR);
	TEST_ASSERT(p.tokenidx == 1);
	TEST_ASSERT(p.textidx == sizeof(input) / sizeof(char) - 1);
	TEST_ASSERT(!json_string_match(input, tokens, 0, string));
	TEST_ASSERT(tokens[0].start == 0);
	TEST_ASSERT(tokens[0].end == 6);
	TEST_ASSERT(tokens[0].length == 5);
}

static void test_normal_too_short(void)
{
	char input[] = "\"hello\"";
	char string[] = "he";
	struct json_token tokens[1];
	struct json_parser p = json_parse(input, tokens, 1);
	TEST_ASSERT(p.error == JSONERR_NO_ERROR);
	TEST_ASSERT(p.tokenidx == 1);
	TEST_ASSERT(p.textidx == sizeof(input) / sizeof(char) - 1);
	TEST_ASSERT(!json_string_match(input, tokens, 0, string));
	TEST_ASSERT(tokens[0].start == 0);
	TEST_ASSERT(tokens[0].end == 6);
	TEST_ASSERT(tokens[0].length == 5);
}

static void test_escape_quote(void)
{
	char input[] = "\"he\\\"llo\"";
	char string[] = "he\"llo";
	struct json_token tokens[1];
	struct json_parser p = json_parse(input, tokens, 1);
	TEST_ASSERT(p.error == JSONERR_NO_ERROR);
	TEST_ASSERT(p.tokenidx == 1);
	TEST_ASSERT(p.textidx == sizeof(input) / sizeof(char) - 1);
	TEST_ASSERT(json_string_match(input, tokens, 0, string));
	TEST_ASSERT(tokens[0].start == 0);
	TEST_ASSERT(tokens[0].end == 8);
	TEST_ASSERT(tokens[0].length == 6);
}

static void test_escape_backslash(void)
{
	char input[] = "\"he\\\\llo\"";
	char string[] = "he\\llo";
	struct json_token tokens[1];
	struct json_parser p = json_parse(input, tokens, 1);
	TEST_ASSERT(p.error == JSONERR_NO_ERROR);
	TEST_ASSERT(p.tokenidx == 1);
	TEST_ASSERT(p.textidx == sizeof(input) / sizeof(char) - 1);
	TEST_ASSERT(json_string_match(input, tokens, 0, string));
	TEST_ASSERT(tokens[0].start == 0);
	TEST_ASSERT(tokens[0].end == 8);
	TEST_ASSERT(tokens[0].length == 6);
}

static void test_escape_slash(void)
{
	char input[] = "\"he\\/llo\"";
	char string[] = "he/llo";
	struct json_token tokens[1];
	struct json_parser p = json_parse(input, tokens, 1);
	TEST_ASSERT(p.error == JSONERR_NO_ERROR);
	TEST_ASSERT(p.tokenidx == 1);
	TEST_ASSERT(p.textidx == sizeof(input) / sizeof(char) - 1);
	TEST_ASSERT(json_string_match(input, tokens, 0, string));
	TEST_ASSERT(tokens[0].start == 0);
	TEST_ASSERT(tokens[0].end == 8);
	TEST_ASSERT(tokens[0].length == 6);
}

static void test_escape_backspace(void)
{
	char input[] = "\"he\\bllo\"";
	char string[] = "he\bllo";
	struct json_token tokens[1];
	struct json_parser p = json_parse(input, tokens, 1);
	TEST_ASSERT(p.error == JSONERR_NO_ERROR);
	TEST_ASSERT(p.tokenidx == 1);
	TEST_ASSERT(p.textidx == sizeof(input) / sizeof(char) - 1);
	TEST_ASSERT(json_string_match(input, tokens, 0, string));
	TEST_ASSERT(tokens[0].start == 0);
	TEST_ASSERT(tokens[0].end == 8);
	TEST_ASSERT(tokens[0].length == 6);
}

static void test_escape_formfeed(void)
{
	char input[] = "\"he\\fllo\"";
	char string[] = "he\fllo";
	struct json_token tokens[1];
	struct json_parser p = json_parse(input, tokens, 1);
	TEST_ASSERT(p.error == JSONERR_NO_ERROR);
	TEST_ASSERT(p.tokenidx == 1);
	TEST_ASSERT(p.textidx == sizeof(input) / sizeof(char) - 1);
	TEST_ASSERT(json_string_match(input, tokens, 0, string));
	TEST_ASSERT(tokens[0].start == 0);
	TEST_ASSERT(tokens[0].end == 8);
	TEST_ASSERT(tokens[0].length == 6);
}

static void test_escape_newline(void)
{
	char input[] = "\"he\\nllo\"";
	char string[] = "he\nllo";
	struct json_token tokens[1];
	struct json_parser p = json_parse(input, tokens, 1);
	TEST_ASSERT(p.error == JSONERR_NO_ERROR);
	TEST_ASSERT(p.tokenidx == 1);
	TEST_ASSERT(p.textidx == sizeof(input) / sizeof(char) - 1);
	TEST_ASSERT(json_string_match(input, tokens, 0, string));
	TEST_ASSERT(tokens[0].start == 0);
	TEST_ASSERT(tokens[0].end == 8);
	TEST_ASSERT(tokens[0].length == 6);
}

static void test_escape_carriage_return(void)
{
	char input[] = "\"he\\rllo\"";
	char string[] = "he\rllo";
	struct json_token tokens[1];
	struct json_parser p = json_parse(input, tokens, 1);
	TEST_ASSERT(p.error == JSONERR_NO_ERROR);
	TEST_ASSERT(p.tokenidx == 1);
	TEST_ASSERT(p.textidx == sizeof(input) / sizeof(char) - 1);
	TEST_ASSERT(json_string_match(input, tokens, 0, string));
	TEST_ASSERT(tokens[0].start == 0);
	TEST_ASSERT(tokens[0].end == 8);
	TEST_ASSERT(tokens[0].length == 6);
}

static void test_escape_tab(void)
{
	char input[] = "\"he\\tllo\"";
	char string[] = "he\tllo";
	struct json_token tokens[1];
	struct json_parser p = json_parse(input, tokens, 1);
	TEST_ASSERT(p.error == JSONERR_NO_ERROR);
	TEST_ASSERT(p.tokenidx == 1);
	TEST_ASSERT(p.textidx == sizeof(input) / sizeof(char) - 1);
	TEST_ASSERT(json_string_match(input, tokens, 0, string));
	TEST_ASSERT(tokens[0].start == 0);
	TEST_ASSERT(tokens[0].end == 8);
	TEST_ASSERT(tokens[0].length == 6);
}

static void test_unicode_escape(void)
{
	char input[] = "\"he\\u006Clo\"";
	char string[] = "hello";
	struct json_token tokens[1];
	struct json_parser p = json_parse(input, tokens, 1);
	TEST_ASSERT(p.error == JSONERR_NO_ERROR);
	TEST_ASSERT(p.tokenidx == 1);
	TEST_ASSERT(p.textidx == sizeof(input) / sizeof(char) - 1);
	TEST_ASSERT(json_string_match(input, tokens, 0, string));
	TEST_ASSERT(tokens[0].start == 0);
	TEST_ASSERT(tokens[0].end == 11);
	TEST_ASSERT(tokens[0].length == 5);
}

static void test_surrogate_pair(void)
{
	char input[] = "\"\\uD83D\\uDCA9\"";
	char string[] = "💩"; // directly included poop :)
	struct json_token tokens[1];
	struct json_parser p = json_parse(input, tokens, 1);
	TEST_ASSERT_EQUAL_INT(JSONERR_NO_ERROR, p.error);
	TEST_ASSERT_EQUAL_INT(1, p.tokenidx);
	TEST_ASSERT(p.textidx == sizeof(input) / sizeof(char) - 1);
	TEST_ASSERT(json_string_match(input, tokens, 0, string));
	TEST_ASSERT_EQUAL_INT(0, tokens[0].start);
	TEST_ASSERT_EQUAL_INT(13, tokens[0].end);
	TEST_ASSERT_EQUAL_INT(4, tokens[0].length);
}

static void test_incomplete_surrogate(void)
{
	char input[] = "\"\\uD83D\"";
	struct json_token tokens[1];
	struct json_parser p = json_parse(input, tokens, 1);
	TEST_ASSERT(p.error == JSONERR_INVALID_SURROGATE);
}

static void test_invalid_surrogate_char(void)
{
	char input[] = "\"\\uD83Da\"";
	struct json_token tokens[1];
	struct json_parser p = json_parse(input, tokens, 1);
	TEST_ASSERT(p.error == JSONERR_INVALID_SURROGATE);
}

static void test_invalid_surrogate_pair(void)
{
	char input[] = "\"\\uD83D\\u1234\""; // 1234 is not a valid
	                                     // surrogate to follow
	struct json_token tokens[1];
	struct json_parser p = json_parse(input, tokens, 1);
	TEST_ASSERT(p.error == JSONERR_INVALID_SURROGATE);
}

int main(void)
{
	UNITY_BEGIN();
	RUN_TEST(test_normal_string);
	RUN_TEST(test_normal_nomatch);
	RUN_TEST(test_normal_too_long);
	RUN_TEST(test_normal_too_short);
	RUN_TEST(test_escape_quote);
	RUN_TEST(test_escape_backslash);
	RUN_TEST(test_escape_slash);
	RUN_TEST(test_escape_backspace);
	RUN_TEST(test_escape_formfeed);
	RUN_TEST(test_escape_newline);
	RUN_TEST(test_escape_carriage_return);
	RUN_TEST(test_escape_tab);
	RUN_TEST(test_unicode_escape);
	RUN_TEST(test_surrogate_pair);
	RUN_TEST(test_incomplete_surrogate);
	RUN_TEST(test_invalid_surrogate_char);
	RUN_TEST(test_invalid_surrogate_pair);
	return UNITY_END();
}
