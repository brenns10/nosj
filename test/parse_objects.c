/***************************************************************************//**

  @file         parse_objects.c

  @author       Stephen Brennan

  @date         Created Tuesday, 24 November 2015

  @brief        Test object parsing.

  @copyright    Copyright (c) 2015, Stephen Brennan.  Released under the Revised
                BSD License.  See LICENSE.txt for details.

*******************************************************************************/

#include "libstephen/ut.h"

#include "json.h"

static int test_empty_object(void)
{
  char *input = "{}";
  struct json_parser p = json_parse(input, NULL, 0);
  TEST_ASSERT(p.error == JSONERR_NO_ERROR);
  TEST_ASSERT(p.tokenidx == 1);
  TEST_ASSERT(p.textidx == 2);
  return 0;
}

static int test_single_element(void)
{
  char *input = "{\"a\": 1}";
  struct json_parser p = json_parse(input, NULL, 0);
  TEST_ASSERT(p.error == JSONERR_NO_ERROR);
  TEST_ASSERT(p.tokenidx == 3);
  TEST_ASSERT(p.textidx == 8);
  return 0;
}

static int test_multiple_elements(void)
{
  char *input = "{\"a\": 1, \"b\": 2}";
  struct json_parser p = json_parse(input, NULL, 0);
  TEST_ASSERT(p.error == JSONERR_NO_ERROR);
  TEST_ASSERT(p.tokenidx == 5);
  TEST_ASSERT(p.textidx == 16);
  return 0;
}

static int test_extra_comma(void)
{
  char *input = "{\"a\": 1,}";
  struct json_parser p = json_parse(input, NULL, 0);
  TEST_ASSERT(p.error == JSONERR_NO_ERROR);
  TEST_ASSERT(p.tokenidx == 3);
  TEST_ASSERT(p.textidx == 9);
  return 0;
}

static int test_no_end(void)
{
  char *input = "{\"a\": 1,";
  struct json_parser p = json_parse(input, NULL, 0);
  TEST_ASSERT(p.error == JSONERR_PREMATURE_EOF);
  return 0;
}

static int test_no_colon(void)
{
  char *input = "{\"blah\" 2}";
  struct json_parser p = json_parse(input, NULL, 0);
  TEST_ASSERT(p.error == JSONERR_EXPECTED_TOKEN);
  TEST_ASSERT(p.errorarg == ':');
  return 0;
}

static int test_missing_value(void)
{
  char *input = "{\"blah\":}";
  struct json_parser p = json_parse(input, NULL, 0);
  TEST_ASSERT(p.error == JSONERR_UNEXPECTED_TOKEN);
  return 0;
}

static int test_no_key(void)
{
  char *input = "{:2}";
  struct json_parser p = json_parse(input, NULL, 0);
  TEST_ASSERT(p.error == JSONERR_UNEXPECTED_TOKEN);
  return 0;
}

static int test_number_key(void)
{
  char *input = "{1:2}";
  struct json_parser p = json_parse(input, NULL, 0);
  TEST_ASSERT(p.error == JSONERR_UNEXPECTED_TOKEN);
  return 0;
}

static int test_true_key(void)
{
  char *input = "{true:2}";
  struct json_parser p = json_parse(input, NULL, 0);
  TEST_ASSERT(p.error == JSONERR_UNEXPECTED_TOKEN);
  return 0;
}

static int test_false_key(void)
{
  char *input = "{false:2}";
  struct json_parser p = json_parse(input, NULL, 0);
  TEST_ASSERT(p.error == JSONERR_UNEXPECTED_TOKEN);
  return 0;
}

static int test_null_key(void)
{
  char *input = "{null:2}";
  struct json_parser p = json_parse(input, NULL, 0);
  TEST_ASSERT(p.error == JSONERR_UNEXPECTED_TOKEN);
  return 0;
}

static int test_list_key(void)
{
  char *input = "{[]:2}";
  struct json_parser p = json_parse(input, NULL, 0);
  TEST_ASSERT(p.error == JSONERR_UNEXPECTED_TOKEN);
  return 0;
}

static int test_object_key(void)
{
  char *input = "{{}:2}";
  struct json_parser p = json_parse(input, NULL, 0);
  TEST_ASSERT(p.error == JSONERR_UNEXPECTED_TOKEN);
  return 0;
}

void test_parse_objects(void)
{
  smb_ut_group *group = su_create_test_group("test/parse_objects.c");

  smb_ut_test *empty_object = su_create_test("empty_object", test_empty_object);
  su_add_test(group, empty_object);

  smb_ut_test *single_element = su_create_test("single_element", test_single_element);
  su_add_test(group, single_element);

  smb_ut_test *multiple_elements = su_create_test("multiple_elements", test_multiple_elements);
  su_add_test(group, multiple_elements);

  smb_ut_test *extra_comma = su_create_test("extra_comma", test_extra_comma);
  su_add_test(group, extra_comma);

  smb_ut_test *no_end = su_create_test("no_end", test_no_end);
  su_add_test(group, no_end);

  smb_ut_test *no_colon = su_create_test("no_colon", test_no_colon);
  su_add_test(group, no_colon);

  smb_ut_test *missing_value = su_create_test("missing_value", test_missing_value);
  su_add_test(group, missing_value);

  smb_ut_test *no_key = su_create_test("no_key", test_no_key);
  su_add_test(group, no_key);

  smb_ut_test *number_key = su_create_test("number_key", test_number_key);
  su_add_test(group, number_key);

  smb_ut_test *true_key = su_create_test("true_key", test_true_key);
  su_add_test(group, true_key);

  smb_ut_test *false_key = su_create_test("false_key", test_false_key);
  su_add_test(group, false_key);

  smb_ut_test *null_key = su_create_test("null_key", test_null_key);
  su_add_test(group, null_key);

  smb_ut_test *list_key = su_create_test("list_key", test_list_key);
  su_add_test(group, list_key);

  smb_ut_test *object_key = su_create_test("object_key", test_object_key);
  su_add_test(group, object_key);

  su_run_group(group);
  su_delete_group(group);
}
