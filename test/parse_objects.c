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

  su_run_group(group);
  su_delete_group(group);
}
