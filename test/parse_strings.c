/***************************************************************************//**

  @file         parse_strings.c

  @author       Stephen Brennan

  @date         Created Tuesday, 24 November 2015

  @brief        Test string parsing.

  @copyright    Copyright (c) 2015, Stephen Brennan.  Released under the Revised
                BSD License.  See LICENSE.txt for details.

*******************************************************************************/

#include "libstephen/ut.h"

#include "json.h"

static int test_empty_string(void)
{
  wchar_t input[] = L"\"\"";
  struct json_parser p = json_parse(input, NULL, 0);
  TEST_ASSERT(p.error == JSONERR_NO_ERROR);
  TEST_ASSERT(p.tokenidx == 1);
  TEST_ASSERT(p.textidx == sizeof(input)/sizeof(wchar_t) - 1);
  return 0;
}

static int test_single_char(void)
{
  wchar_t input[] = L"\"a\"";
  struct json_parser p = json_parse(input, NULL, 0);
  TEST_ASSERT(p.error == JSONERR_NO_ERROR);
  TEST_ASSERT(p.tokenidx == 1);
  TEST_ASSERT(p.textidx == sizeof(input)/sizeof(wchar_t) - 1);
  return 0;
}

static int test_no_end(void)
{
  wchar_t input[] = L"\"blah";
  struct json_parser p = json_parse(input, NULL, 0);
  TEST_ASSERT(p.error == JSONERR_PREMATURE_EOF);
  return 0;
}

static int test_escape(void)
{
  wchar_t input[] = L"\"blah\\\"blah\"";
  struct json_parser p = json_parse(input, NULL, 0);
  TEST_ASSERT(p.error == JSONERR_NO_ERROR);
  TEST_ASSERT(p.tokenidx == 1);
  TEST_ASSERT(p.textidx == sizeof(input)/sizeof(wchar_t) - 1);
  return 0;
}

static int test_escaped_end(void)
{
  wchar_t input[] = L"\"blah\\";
  struct json_parser p = json_parse(input, NULL, 0);
  TEST_ASSERT(p.error == JSONERR_PREMATURE_EOF);
  return 0;
}

void test_parse_strings(void)
{
  smb_ut_group *group = su_create_test_group("test/parse_strings.c");

  smb_ut_test *empty_string = su_create_test("empty_string", test_empty_string);
  su_add_test(group, empty_string);

  smb_ut_test *single_char = su_create_test("single_char", test_single_char);
  su_add_test(group, single_char);

  smb_ut_test *no_end = su_create_test("no_end", test_no_end);
  su_add_test(group, no_end);

  smb_ut_test *escape = su_create_test("escape", test_escape);
  su_add_test(group, escape);

  smb_ut_test *escaped_end = su_create_test("escaped_end", test_escaped_end);
  su_add_test(group, escaped_end);

  su_run_group(group);
  su_delete_group(group);
}
