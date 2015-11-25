/***************************************************************************//**

  @file         parse_numbers.c

  @author       Stephen Brennan

  @date         Created Tuesday, 24 November 2015

  @brief        Test number parsing.

  @copyright    Copyright (c) 2015, Stephen Brennan.  Released under the Revised
                BSD License.  See LICENSE.txt for details.

*******************************************************************************/

#include "libstephen/ut.h"
#include "json.h"

static int test_single_digit(void)
{
  char input[] = "0";
  struct json_parser p = json_parse(input, NULL, 0);
  TEST_ASSERT(p.error == JSONERR_NO_ERROR);
  TEST_ASSERT(p.tokenidx == 1);
  TEST_ASSERT(p.textidx == sizeof(input) - 1);
  return 0;
}

static int test_multiple_digit(void)
{
  char input[] = "12";
  struct json_parser p = json_parse(input, NULL, 0);
  TEST_ASSERT(p.error == JSONERR_NO_ERROR);
  TEST_ASSERT(p.tokenidx == 1);
  TEST_ASSERT(p.textidx == sizeof(input) - 1);
  return 0;
}

static int test_starts_with_zero(void)
{
  /*
    It would be nicer if this input actually failed.  But the way the current
    state machine works is that if it receives incorrect while in an accepting
    state, it accepts and leaves the next input to be parsed.  Unfortunately,
    this means that something like this will be accepted, and the 1 will go
    unparsed.  (TODO) In the future, I should address the way this works (would
    produce nicer error messages), but for now this is expected behavior.
   */
  char input[] = "01";
  struct json_parser p = json_parse(input, NULL, 0);
  TEST_ASSERT(p.error == JSONERR_NO_ERROR);
  TEST_ASSERT(p.tokenidx == 1);
  TEST_ASSERT(p.textidx == 1); // ONLY PARSED ONE CHARACTER
  return 0;
}

static int test_decimal(void)
{
  char input[] = "1.1";
  struct json_parser p = json_parse(input, NULL, 0);
  TEST_ASSERT(p.error == JSONERR_NO_ERROR);
  TEST_ASSERT(p.tokenidx == 1);
  TEST_ASSERT(p.textidx == sizeof(input) - 1);
  return 0;
}

static int test_zero_decimal(void)
{
  char input[] = "0.1";
  struct json_parser p = json_parse(input, NULL, 0);
  TEST_ASSERT(p.error == JSONERR_NO_ERROR);
  TEST_ASSERT(p.tokenidx == 1);
  TEST_ASSERT(p.textidx == sizeof(input) - 1);
  return 0;
}

static int test_negative_sign(void)
{
  char input[] = "-1";
  struct json_parser p = json_parse(input, NULL, 0);
  TEST_ASSERT(p.error == JSONERR_NO_ERROR);
  TEST_ASSERT(p.tokenidx == 1);
  TEST_ASSERT(p.textidx == sizeof(input) - 1);
  return 0;
}

static int test_exponent_upper(void)
{
  char input[] = "1E5";
  struct json_parser p = json_parse(input, NULL, 0);
  TEST_ASSERT(p.error == JSONERR_NO_ERROR);
  TEST_ASSERT(p.tokenidx == 1);
  TEST_ASSERT(p.textidx == sizeof(input) - 1);
  return 0;
}

static int test_exponent_lower(void)
{
  char input[] = "1e5";
  struct json_parser p = json_parse(input, NULL, 0);
  TEST_ASSERT(p.error == JSONERR_NO_ERROR);
  TEST_ASSERT(p.tokenidx == 1);
  TEST_ASSERT(p.textidx == sizeof(input) - 1);
  return 0;
}

static int test_exponent_plus(void)
{
  char input[] = "1e+5";
  struct json_parser p = json_parse(input, NULL, 0);
  TEST_ASSERT(p.error == JSONERR_NO_ERROR);
  TEST_ASSERT(p.tokenidx == 1);
  TEST_ASSERT(p.textidx == sizeof(input) - 1);
  return 0;
}

static int test_exponent_minus(void)
{
  char input[] = "1e-5";
  struct json_parser p = json_parse(input, NULL, 0);
  TEST_ASSERT(p.error == JSONERR_NO_ERROR);
  TEST_ASSERT(p.tokenidx == 1);
  TEST_ASSERT(p.textidx == sizeof(input) - 1);
  return 0;
}

static int test_sign_decimal_exponent(void)
{
  char input[] = "-1.5e+5";
  struct json_parser p = json_parse(input, NULL, 0);
  TEST_ASSERT(p.error == JSONERR_NO_ERROR);
  TEST_ASSERT(p.tokenidx == 1);
  TEST_ASSERT(p.textidx == sizeof(input) - 1);
  return 0;
}

static int test_sign_alone(void)
{
  char input[] = "-";
  struct json_parser p = json_parse(input, NULL, 0);
  TEST_ASSERT(p.error == JSONERR_INVALID_NUMBER);
  return 0;
}

static int test_decimal_without_digits(void)
{
  char input[] = "1.";
  struct json_parser p = json_parse(input, NULL, 0);
  TEST_ASSERT(p.error == JSONERR_INVALID_NUMBER);
  return 0;
}

static int test_exponent_without_digits(void)
{
  char input[] = "1e";
  struct json_parser p = json_parse(input, NULL, 0);
  TEST_ASSERT(p.error == JSONERR_INVALID_NUMBER);
  return 0;
}

static int test_exponent_sign_without_digits(void)
{
  char input[] = "1e+";
  struct json_parser p = json_parse(input, NULL, 0);
  TEST_ASSERT(p.error == JSONERR_INVALID_NUMBER);
  return 0;
}

static int test_negative_zero(void)
{
  char input[] = "-0";  // believe it or not, this is valid JSON.
  struct json_parser p = json_parse(input, NULL, 0);
  TEST_ASSERT(p.error == JSONERR_NO_ERROR);
  TEST_ASSERT(p.tokenidx == 1);
  TEST_ASSERT(p.textidx == sizeof(input) - 1);
  return 0;
}

static int test_zero_exp(void)
{
  char input[] = "0e5";  // again, doesn't make sense, but is valid
  struct json_parser p = json_parse(input, NULL, 0);
  TEST_ASSERT(p.error == JSONERR_NO_ERROR);
  TEST_ASSERT(p.tokenidx == 1);
  TEST_ASSERT(p.textidx == sizeof(input) - 1);
  return 0;
}

static int test_double_digit_decimal(void)
{
  char input[] = "1.23";
  struct json_parser p = json_parse(input, NULL, 0);
  TEST_ASSERT(p.error == JSONERR_NO_ERROR);
  TEST_ASSERT(p.tokenidx == 1);
  TEST_ASSERT(p.textidx == sizeof(input) - 1);
  return 0;
}

static int test_double_digit_exp(void)
{
  char input[] = "1e23";
  struct json_parser p = json_parse(input, NULL, 0);
  TEST_ASSERT(p.error == JSONERR_NO_ERROR);
  TEST_ASSERT(p.tokenidx == 1);
  TEST_ASSERT(p.textidx == sizeof(input) - 1);
  return 0;
}

void test_parse_numbers(void)
{
  smb_ut_group *group = su_create_test_group("test/parse_numbers.c");

  smb_ut_test *single_digit = su_create_test("single_digit", test_single_digit);
  su_add_test(group, single_digit);

  smb_ut_test *multiple_digit = su_create_test("multiple_digit", test_multiple_digit);
  su_add_test(group, multiple_digit);

  smb_ut_test *starts_with_zero = su_create_test("starts_with_zero", test_starts_with_zero);
  su_add_test(group, starts_with_zero);

  smb_ut_test *decimal = su_create_test("decimal", test_decimal);
  su_add_test(group, decimal);

  smb_ut_test *zero_decimal = su_create_test("zero_decimal", test_zero_decimal);
  su_add_test(group, zero_decimal);

  smb_ut_test *negative_sign = su_create_test("negative_sign", test_negative_sign);
  su_add_test(group, negative_sign);

  smb_ut_test *exponent_upper = su_create_test("exponent_upper", test_exponent_upper);
  su_add_test(group, exponent_upper);

  smb_ut_test *exponent_lower = su_create_test("exponent_lower", test_exponent_lower);
  su_add_test(group, exponent_lower);

  smb_ut_test *exponent_plus = su_create_test("exponent_plus", test_exponent_plus);
  su_add_test(group, exponent_plus);

  smb_ut_test *exponent_minus = su_create_test("exponent_minus", test_exponent_minus);
  su_add_test(group, exponent_minus);

  smb_ut_test *sign_decimal_exponent = su_create_test("sign_decimal_exponent", test_sign_decimal_exponent);
  su_add_test(group, sign_decimal_exponent);

  smb_ut_test *sign_alone = su_create_test("sign_alone", test_sign_alone);
  su_add_test(group, sign_alone);

  smb_ut_test *decimal_without_digits = su_create_test("decimal_without_digits", test_decimal_without_digits);
  su_add_test(group, decimal_without_digits);

  smb_ut_test *exponent_without_digits = su_create_test("exponent_without_digits", test_exponent_without_digits);
  su_add_test(group, exponent_without_digits);

  smb_ut_test *exponent_sign_without_digits = su_create_test("exponent_sign_without_digits", test_exponent_sign_without_digits);
  su_add_test(group, exponent_sign_without_digits);

  smb_ut_test *negative_zero = su_create_test("negative_zero", test_negative_zero);
  su_add_test(group, negative_zero);

  smb_ut_test *zero_exp = su_create_test("zero_exp", test_zero_exp);
  su_add_test(group, zero_exp);

  smb_ut_test *double_digit_decimal = su_create_test("double_digit_decimal", test_double_digit_decimal);
  su_add_test(group, double_digit_decimal);

  smb_ut_test *double_digit_exp = su_create_test("double_digit_exp", test_double_digit_exp);
  su_add_test(group, double_digit_exp);

  su_run_group(group);
  su_delete_group(group);
}
