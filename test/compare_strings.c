/***************************************************************************//**

  @file         compare_strings.c

  @author       Stephen Brennan

  @date         Created Wednesday,  9 December 2015

  @brief        Tests for comparing strings.

  @copyright    Copyright (c) 2015, Stephen Brennan.  Released under the Revised
                BSD License.  See LICENSE.txt for details.

*******************************************************************************/

#include "libstephen/ut.h"
#include "json.h"

static int test_normal_string(void)
{
  wchar_t input[] = L"\"hello\"";
  wchar_t string[] = L"hello";
  struct json_token tokens[1];
  struct json_parser p = json_parse(input, tokens, 1);
  TEST_ASSERT(p.error == JSONERR_NO_ERROR);
  TEST_ASSERT(p.tokenidx == 1);
  TEST_ASSERT(p.textidx == sizeof(input)/sizeof(wchar_t) - 1);
  TEST_ASSERT(json_string_match(input, tokens, 0, string));
  return 0;
}

static int test_normal_nomatch(void)
{
  wchar_t input[] = L"\"hello\"";
  wchar_t string[] = L"hellO";
  struct json_token tokens[1];
  struct json_parser p = json_parse(input, tokens, 1);
  TEST_ASSERT(p.error == JSONERR_NO_ERROR);
  TEST_ASSERT(p.tokenidx == 1);
  TEST_ASSERT(p.textidx == sizeof(input)/sizeof(wchar_t) - 1);
  TEST_ASSERT(!json_string_match(input, tokens, 0, string));
  return 0;
}

static int test_normal_too_long(void)
{
  wchar_t input[] = L"\"hello\"";
  wchar_t string[] = L"hello there";
  struct json_token tokens[1];
  struct json_parser p = json_parse(input, tokens, 1);
  TEST_ASSERT(p.error == JSONERR_NO_ERROR);
  TEST_ASSERT(p.tokenidx == 1);
  TEST_ASSERT(p.textidx == sizeof(input)/sizeof(wchar_t) - 1);
  TEST_ASSERT(!json_string_match(input, tokens, 0, string));
  return 0;
}

static int test_normal_too_short(void)
{
  wchar_t input[] = L"\"hello\"";
  wchar_t string[] = L"he";
  struct json_token tokens[1];
  struct json_parser p = json_parse(input, tokens, 1);
  TEST_ASSERT(p.error == JSONERR_NO_ERROR);
  TEST_ASSERT(p.tokenidx == 1);
  TEST_ASSERT(p.textidx == sizeof(input)/sizeof(wchar_t) - 1);
  TEST_ASSERT(!json_string_match(input, tokens, 0, string));
  return 0;
}

static int test_escape_quote(void)
{
  wchar_t input[] = L"\"he\\\"llo\"";
  wchar_t string[] = L"he\"llo";
  struct json_token tokens[1];
  struct json_parser p = json_parse(input, tokens, 1);
  TEST_ASSERT(p.error == JSONERR_NO_ERROR);
  TEST_ASSERT(p.tokenidx == 1);
  TEST_ASSERT(p.textidx == sizeof(input)/sizeof(wchar_t) - 1);
  TEST_ASSERT(json_string_match(input, tokens, 0, string));
  return 0;
}

static int test_escape_backslash(void)
{
  wchar_t input[] = L"\"he\\\\llo\"";
  wchar_t string[] = L"he\\llo";
  struct json_token tokens[1];
  struct json_parser p = json_parse(input, tokens, 1);
  TEST_ASSERT(p.error == JSONERR_NO_ERROR);
  TEST_ASSERT(p.tokenidx == 1);
  TEST_ASSERT(p.textidx == sizeof(input)/sizeof(wchar_t) - 1);
  TEST_ASSERT(json_string_match(input, tokens, 0, string));
  return 0;
}

static int test_escape_slash(void)
{
  wchar_t input[] = L"\"he\\/llo\"";
  wchar_t string[] = L"he/llo";
  struct json_token tokens[1];
  struct json_parser p = json_parse(input, tokens, 1);
  TEST_ASSERT(p.error == JSONERR_NO_ERROR);
  TEST_ASSERT(p.tokenidx == 1);
  TEST_ASSERT(p.textidx == sizeof(input)/sizeof(wchar_t) - 1);
  TEST_ASSERT(json_string_match(input, tokens, 0, string));
  return 0;
}

static int test_escape_backspace(void)
{
  wchar_t input[] = L"\"he\\bllo\"";
  wchar_t string[] = L"he\bllo";
  struct json_token tokens[1];
  struct json_parser p = json_parse(input, tokens, 1);
  TEST_ASSERT(p.error == JSONERR_NO_ERROR);
  TEST_ASSERT(p.tokenidx == 1);
  TEST_ASSERT(p.textidx == sizeof(input)/sizeof(wchar_t) - 1);
  TEST_ASSERT(json_string_match(input, tokens, 0, string));
  return 0;
}

static int test_escape_formfeed(void)
{
  wchar_t input[] = L"\"he\\fllo\"";
  wchar_t string[] = L"he\fllo";
  struct json_token tokens[1];
  struct json_parser p = json_parse(input, tokens, 1);
  TEST_ASSERT(p.error == JSONERR_NO_ERROR);
  TEST_ASSERT(p.tokenidx == 1);
  TEST_ASSERT(p.textidx == sizeof(input)/sizeof(wchar_t) - 1);
  TEST_ASSERT(json_string_match(input, tokens, 0, string));
  return 0;
}

static int test_escape_newline(void)
{
  wchar_t input[] = L"\"he\\nllo\"";
  wchar_t string[] = L"he\nllo";
  struct json_token tokens[1];
  struct json_parser p = json_parse(input, tokens, 1);
  TEST_ASSERT(p.error == JSONERR_NO_ERROR);
  TEST_ASSERT(p.tokenidx == 1);
  TEST_ASSERT(p.textidx == sizeof(input)/sizeof(wchar_t) - 1);
  TEST_ASSERT(json_string_match(input, tokens, 0, string));
  return 0;
}

static int test_escape_carriage_return(void)
{
  wchar_t input[] = L"\"he\\rllo\"";
  wchar_t string[] = L"he\rllo";
  struct json_token tokens[1];
  struct json_parser p = json_parse(input, tokens, 1);
  TEST_ASSERT(p.error == JSONERR_NO_ERROR);
  TEST_ASSERT(p.tokenidx == 1);
  TEST_ASSERT(p.textidx == sizeof(input)/sizeof(wchar_t) - 1);
  TEST_ASSERT(json_string_match(input, tokens, 0, string));
  return 0;
}

static int test_escape_tab(void)
{
  wchar_t input[] = L"\"he\\tllo\"";
  wchar_t string[] = L"he\tllo";
  struct json_token tokens[1];
  struct json_parser p = json_parse(input, tokens, 1);
  TEST_ASSERT(p.error == JSONERR_NO_ERROR);
  TEST_ASSERT(p.tokenidx == 1);
  TEST_ASSERT(p.textidx == sizeof(input)/sizeof(wchar_t) - 1);
  TEST_ASSERT(json_string_match(input, tokens, 0, string));
  return 0;
}

static int test_unicode_escape(void)
{
  wchar_t input[] = L"\"he\\u006Clo\"";
  wchar_t string[] = L"hello";
  struct json_token tokens[1];
  struct json_parser p = json_parse(input, tokens, 1);
  TEST_ASSERT(p.error == JSONERR_NO_ERROR);
  TEST_ASSERT(p.tokenidx == 1);
  TEST_ASSERT(p.textidx == sizeof(input)/sizeof(wchar_t) - 1);
  TEST_ASSERT(json_string_match(input, tokens, 0, string));
  return 0;
}

static int test_surrogate_pair(void)
{
  wchar_t input[] = L"\"\\uD83D\\uDCA9\"";
  wchar_t string[] = L"💩"; // directly included poop :)
  struct json_token tokens[1];
  struct json_parser p = json_parse(input, tokens, 1);
  TEST_ASSERT(p.error == JSONERR_NO_ERROR);
  TEST_ASSERT(p.tokenidx == 1);
  TEST_ASSERT(p.textidx == sizeof(input)/sizeof(wchar_t) - 1);
  TEST_ASSERT(json_string_match(input, tokens, 0, string));
  return 0;
}

static int test_incomplete_surrogate(void)
{
  wchar_t input[] = L"\"\\uD83D\"";
  struct json_token tokens[1];
  struct json_parser p = json_parse(input, tokens, 1);
  TEST_ASSERT(p.error == JSONERR_INVALID_SURROGATE);
  return 0;
}

static int test_invalid_surrogate_char(void)
{
  wchar_t input[] = L"\"\\uD83Da\"";
  struct json_token tokens[1];
  struct json_parser p = json_parse(input, tokens, 1);
  TEST_ASSERT(p.error == JSONERR_INVALID_SURROGATE);
  return 0;
}

static int test_invalid_surrogate_pair(void)
{
  wchar_t input[] = L"\"\\uD83D\\u1234\""; // 1234 is not a valid surrogate to
                                           // follow
  struct json_token tokens[1];
  struct json_parser p = json_parse(input, tokens, 1);
  TEST_ASSERT(p.error == JSONERR_INVALID_SURROGATE);
  return 0;
}

void test_compare_strings(void)
{
  smb_ut_group *group = su_create_test_group("test/compare_strings.c");

  smb_ut_test *normal_string = su_create_test("normal_string", test_normal_string);
  su_add_test(group, normal_string);

  smb_ut_test *normal_nomatch = su_create_test("normal_nomatch", test_normal_nomatch);
  su_add_test(group, normal_nomatch);

  smb_ut_test *normal_too_long = su_create_test("normal_too_long", test_normal_too_long);
  su_add_test(group, normal_too_long);

  smb_ut_test *normal_too_short = su_create_test("normal_too_short", test_normal_too_short);
  su_add_test(group, normal_too_short);

  smb_ut_test *escape_quote = su_create_test("escape_quote", test_escape_quote);
  su_add_test(group, escape_quote);

  smb_ut_test *escape_backslash = su_create_test("escape_backslash", test_escape_backslash);
  su_add_test(group, escape_backslash);

  smb_ut_test *escape_slash = su_create_test("escape_slash", test_escape_slash);
  su_add_test(group, escape_slash);

  smb_ut_test *escape_backspace = su_create_test("escape_backspace", test_escape_backspace);
  su_add_test(group, escape_backspace);

  smb_ut_test *escape_formfeed = su_create_test("escape_formfeed", test_escape_formfeed);
  su_add_test(group, escape_formfeed);

  smb_ut_test *escape_newline = su_create_test("escape_newline", test_escape_newline);
  su_add_test(group, escape_newline);

  smb_ut_test *escape_carriage_return = su_create_test("escape_carriage_return", test_escape_carriage_return);
  su_add_test(group, escape_carriage_return);

  smb_ut_test *escape_tab = su_create_test("escape_tab", test_escape_tab);
  su_add_test(group, escape_tab);

  smb_ut_test *unicode_escape = su_create_test("unicode_escape", test_unicode_escape);
  su_add_test(group, unicode_escape);

  smb_ut_test *surrogate_pair = su_create_test("surrogate_pair", test_surrogate_pair);
  su_add_test(group, surrogate_pair);

  smb_ut_test *incomplete_surrogate = su_create_test("incomplete_surrogate", test_incomplete_surrogate);
  su_add_test(group, incomplete_surrogate);

  smb_ut_test *invalid_surrogate_char = su_create_test("invalid_surrogate_char", test_invalid_surrogate_char);
  su_add_test(group, invalid_surrogate_char);

  smb_ut_test *invalid_surrogate_pair = su_create_test("invalid_surrogate_pair", test_invalid_surrogate_pair);
  su_add_test(group, invalid_surrogate_pair);

  su_run_group(group);
  su_delete_group(group);
}
