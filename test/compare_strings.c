/***************************************************************************/ /**

   @file         compare_strings.c

   @author       Stephen Brennan

   @date         Created Wednesday,  9 December 2015

   @brief        Tests for comparing strings.

   @copyright    Copyright (c) 2015, Stephen Brennan.  Released under the
 Revised BSD License.  See LICENSE.txt for details.

 *******************************************************************************/

#include "libstephen/ut.h"
#include "nosj.h"

static int test_normal_string(void)
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
	return 0;
}

static int test_normal_nomatch(void)
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
	return 0;
}

static int test_normal_too_long(void)
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
	return 0;
}

static int test_normal_too_short(void)
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
	return 0;
}

static int test_escape_quote(void)
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
	return 0;
}

static int test_escape_backslash(void)
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
	return 0;
}

static int test_escape_slash(void)
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
	return 0;
}

static int test_escape_backspace(void)
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
	return 0;
}

static int test_escape_formfeed(void)
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
	return 0;
}

static int test_escape_newline(void)
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
	return 0;
}

static int test_escape_carriage_return(void)
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
	return 0;
}

static int test_escape_tab(void)
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
	return 0;
}

static int test_unicode_escape(void)
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
	return 0;
}

static int test_surrogate_pair(void)
{
	char input[] = "\"\\uD83D\\uDCA9\"";
	char string[] = "💩"; // directly included poop :)
	struct json_token tokens[1];
	struct json_parser p = json_parse(input, tokens, 1);
	TEST_ASSERT(p.error == JSONERR_NO_ERROR);
	TEST_ASSERT(p.tokenidx == 1);
	TEST_ASSERT(p.textidx == sizeof(input) / sizeof(char) - 1);
	TEST_ASSERT(json_string_match(input, tokens, 0, string));
	TEST_ASSERT(tokens[0].start == 0);
	TEST_ASSERT(tokens[0].end == 13);
	TEST_ASSERT(tokens[0].length == 1);
	return 0;
}

static int test_incomplete_surrogate(void)
{
	char input[] = "\"\\uD83D\"";
	struct json_token tokens[1];
	struct json_parser p = json_parse(input, tokens, 1);
	TEST_ASSERT(p.error == JSONERR_INVALID_SURROGATE);
	return 0;
}

static int test_invalid_surrogate_char(void)
{
	char input[] = "\"\\uD83Da\"";
	struct json_token tokens[1];
	struct json_parser p = json_parse(input, tokens, 1);
	TEST_ASSERT(p.error == JSONERR_INVALID_SURROGATE);
	return 0;
}

static int test_invalid_surrogate_pair(void)
{
	char input[] = "\"\\uD83D\\u1234\""; // 1234 is not a valid
	                                     // surrogate to follow
	struct json_token tokens[1];
	struct json_parser p = json_parse(input, tokens, 1);
	TEST_ASSERT(p.error == JSONERR_INVALID_SURROGATE);
	return 0;
}

void test_compare_strings(void)
{
	smb_ut_group *group = su_create_test_group("test/compare_strings.c");

	smb_ut_test *normal_string =
	        su_create_test("normal_string", test_normal_string);
	su_add_test(group, normal_string);

	smb_ut_test *normal_nomatch =
	        su_create_test("normal_nomatch", test_normal_nomatch);
	su_add_test(group, normal_nomatch);

	smb_ut_test *normal_too_long =
	        su_create_test("normal_too_long", test_normal_too_long);
	su_add_test(group, normal_too_long);

	smb_ut_test *normal_too_short =
	        su_create_test("normal_too_short", test_normal_too_short);
	su_add_test(group, normal_too_short);

	smb_ut_test *escape_quote =
	        su_create_test("escape_quote", test_escape_quote);
	su_add_test(group, escape_quote);

	smb_ut_test *escape_backslash =
	        su_create_test("escape_backslash", test_escape_backslash);
	su_add_test(group, escape_backslash);

	smb_ut_test *escape_slash =
	        su_create_test("escape_slash", test_escape_slash);
	su_add_test(group, escape_slash);

	smb_ut_test *escape_backspace =
	        su_create_test("escape_backspace", test_escape_backspace);
	su_add_test(group, escape_backspace);

	smb_ut_test *escape_formfeed =
	        su_create_test("escape_formfeed", test_escape_formfeed);
	su_add_test(group, escape_formfeed);

	smb_ut_test *escape_newline =
	        su_create_test("escape_newline", test_escape_newline);
	su_add_test(group, escape_newline);

	smb_ut_test *escape_carriage_return = su_create_test(
	        "escape_carriage_return", test_escape_carriage_return);
	su_add_test(group, escape_carriage_return);

	smb_ut_test *escape_tab = su_create_test("escape_tab", test_escape_tab);
	su_add_test(group, escape_tab);

	smb_ut_test *unicode_escape =
	        su_create_test("unicode_escape", test_unicode_escape);
	su_add_test(group, unicode_escape);

	smb_ut_test *surrogate_pair =
	        su_create_test("surrogate_pair", test_surrogate_pair);
	su_add_test(group, surrogate_pair);

	smb_ut_test *incomplete_surrogate = su_create_test(
	        "incomplete_surrogate", test_incomplete_surrogate);
	su_add_test(group, incomplete_surrogate);

	smb_ut_test *invalid_surrogate_char = su_create_test(
	        "invalid_surrogate_char", test_invalid_surrogate_char);
	su_add_test(group, invalid_surrogate_char);

	smb_ut_test *invalid_surrogate_pair = su_create_test(
	        "invalid_surrogate_pair", test_invalid_surrogate_pair);
	su_add_test(group, invalid_surrogate_pair);

	su_run_group(group);
	su_delete_group(group);
}
