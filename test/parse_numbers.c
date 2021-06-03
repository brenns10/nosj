/***************************************************************************/ /**

   @file         parse_numbers.c

   @author       Stephen Brennan

   @date         Created Tuesday, 24 November 2015

   @brief        Test number parsing.

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

static void test_single_digit(void)
{
	char input[] = "0";
	size_t ntok = 1;
	struct json_token tokens[ntok];
	struct json_parser p = json_parse(input, tokens, ntok);
	TEST_ASSERT(p.error == JSONERR_NO_ERROR);
	TEST_ASSERT(p.tokenidx == ntok);
	TEST_ASSERT(p.textidx == sizeof(input) / sizeof(char) - 1);
	TEST_ASSERT(json_number_get(input, tokens, 0) == 0.0);
}

static void test_multiple_digit(void)
{
	char input[] = "12";
	size_t ntok = 1;
	struct json_token tokens[ntok];
	struct json_parser p = json_parse(input, tokens, ntok);
	TEST_ASSERT(p.error == JSONERR_NO_ERROR);
	TEST_ASSERT(p.tokenidx == ntok);
	TEST_ASSERT(p.textidx == sizeof(input) / sizeof(char) - 1);
	TEST_ASSERT(json_number_get(input, tokens, 0) == 12.0);
}

static void test_starts_with_zero(void)
{
	/*
	  It would be nicer if this input actually failed.  But the way the
	  current state machine works is that if it receives incorrect while in
	  an accepting state, it accepts and leaves the next input to be parsed.
	  Unfortunately, this means that something like this will be accepted,
	  and the 1 will go unparsed.  (TODO) In the future, I should address
	  the way this works (would produce nicer error messages), but for now
	  this is expected behavior.
	 */
	char input[] = "01";
	struct json_parser p = json_parse(input, NULL, 0);
	TEST_ASSERT(p.error == JSONERR_NO_ERROR);
	TEST_ASSERT(p.tokenidx == 1);
	TEST_ASSERT(p.textidx == 1); // ONLY PARSED ONE CHARACTER
}

static void test_decimal(void)
{
	char input[] = "1.1";
	size_t ntok = 1;
	struct json_token tokens[ntok];
	struct json_parser p = json_parse(input, tokens, ntok);
	TEST_ASSERT(p.error == JSONERR_NO_ERROR);
	TEST_ASSERT(p.tokenidx == ntok);
	TEST_ASSERT(p.textidx == sizeof(input) / sizeof(char) - 1);
	TEST_ASSERT(json_number_get(input, tokens, 0) == 1.1);
}

static void test_zero_decimal(void)
{
	char input[] = "0.1";
	size_t ntok = 1;
	struct json_token tokens[ntok];
	struct json_parser p = json_parse(input, tokens, ntok);
	TEST_ASSERT(p.error == JSONERR_NO_ERROR);
	TEST_ASSERT(p.tokenidx == ntok);
	TEST_ASSERT(p.textidx == sizeof(input) / sizeof(char) - 1);
	TEST_ASSERT(json_number_get(input, tokens, 0) == 0.1);
}

static void test_negative_sign(void)
{
	char input[] = "-1";
	size_t ntok = 1;
	struct json_token tokens[ntok];
	struct json_parser p = json_parse(input, tokens, ntok);
	TEST_ASSERT(p.error == JSONERR_NO_ERROR);
	TEST_ASSERT(p.tokenidx == ntok);
	TEST_ASSERT(p.textidx == sizeof(input) / sizeof(char) - 1);
	TEST_ASSERT(json_number_get(input, tokens, 0) == -1.0);
}

static void test_exponent_upper(void)
{
	char input[] = "1E5";
	size_t ntok = 1;
	struct json_token tokens[ntok];
	struct json_parser p = json_parse(input, tokens, ntok);
	TEST_ASSERT(p.error == JSONERR_NO_ERROR);
	TEST_ASSERT(p.tokenidx == ntok);
	TEST_ASSERT(p.textidx == sizeof(input) / sizeof(char) - 1);
	TEST_ASSERT(json_number_get(input, tokens, 0) == 1e5);
}

static void test_exponent_lower(void)
{
	char input[] = "1e5";
	size_t ntok = 1;
	struct json_token tokens[ntok];
	struct json_parser p = json_parse(input, tokens, ntok);
	TEST_ASSERT(p.error == JSONERR_NO_ERROR);
	TEST_ASSERT(p.tokenidx == ntok);
	TEST_ASSERT(p.textidx == sizeof(input) / sizeof(char) - 1);
	TEST_ASSERT(json_number_get(input, tokens, 0) == 1e5);
}

static void test_exponent_plus(void)
{
	char input[] = "1e+5";
	size_t ntok = 1;
	struct json_token tokens[ntok];
	struct json_parser p = json_parse(input, tokens, ntok);
	TEST_ASSERT(p.error == JSONERR_NO_ERROR);
	TEST_ASSERT(p.tokenidx == ntok);
	TEST_ASSERT(p.textidx == sizeof(input) / sizeof(char) - 1);
	TEST_ASSERT(json_number_get(input, tokens, 0) == 1e5);
}

static void test_exponent_minus(void)
{
	char input[] = "1e-5";
	size_t ntok = 1;
	struct json_token tokens[ntok];
	struct json_parser p = json_parse(input, tokens, ntok);
	TEST_ASSERT(p.error == JSONERR_NO_ERROR);
	TEST_ASSERT(p.tokenidx == ntok);
	TEST_ASSERT(p.textidx == sizeof(input) / sizeof(char) - 1);
	TEST_ASSERT(json_number_get(input, tokens, 0) == 1e-5);
}

static void test_sign_decimal_exponent(void)
{
	char input[] = "-1.5e+5";
	size_t ntok = 1;
	struct json_token tokens[ntok];
	struct json_parser p = json_parse(input, tokens, ntok);
	TEST_ASSERT(p.error == JSONERR_NO_ERROR);
	TEST_ASSERT(p.tokenidx == ntok);
	TEST_ASSERT(p.textidx == sizeof(input) / sizeof(char) - 1);
	TEST_ASSERT(json_number_get(input, tokens, 0) == -1.5e5);
}

static void test_sign_alone(void)
{
	char input[] = "-";
	struct json_parser p = json_parse(input, NULL, 0);
	TEST_ASSERT(p.error == JSONERR_INVALID_NUMBER);
}

static void test_decimal_without_digits(void)
{
	char input[] = "1.";
	struct json_parser p = json_parse(input, NULL, 0);
	TEST_ASSERT(p.error == JSONERR_INVALID_NUMBER);
}

static void test_exponent_without_digits(void)
{
	char input[] = "1e";
	struct json_parser p = json_parse(input, NULL, 0);
	TEST_ASSERT(p.error == JSONERR_INVALID_NUMBER);
}

static void test_exponent_sign_without_digits(void)
{
	char input[] = "1e+";
	struct json_parser p = json_parse(input, NULL, 0);
	TEST_ASSERT(p.error == JSONERR_INVALID_NUMBER);
}

static void test_negative_zero(void)
{
	char input[] = "-0"; // believe it or not, this is valid JSON.
	size_t ntok = 1;
	struct json_token tokens[ntok];
	struct json_parser p = json_parse(input, tokens, ntok);
	TEST_ASSERT(p.error == JSONERR_NO_ERROR);
	TEST_ASSERT(p.tokenidx == ntok);
	TEST_ASSERT(p.textidx == sizeof(input) / sizeof(char) - 1);
	TEST_ASSERT(json_number_get(input, tokens, 0) == 0.0);
}

static void test_zero_exp(void)
{
	char input[] = "0e5"; // again, doesn't make sense, but is valid
	size_t ntok = 1;
	struct json_token tokens[ntok];
	struct json_parser p = json_parse(input, tokens, ntok);
	TEST_ASSERT(p.error == JSONERR_NO_ERROR);
	TEST_ASSERT(p.tokenidx == ntok);
	TEST_ASSERT(p.textidx == sizeof(input) / sizeof(char) - 1);
	TEST_ASSERT(json_number_get(input, tokens, 0) == 0.0);
}

static void test_double_digit_decimal(void)
{
	char input[] = "1.23";
	size_t ntok = 1;
	struct json_token tokens[ntok];
	struct json_parser p = json_parse(input, tokens, ntok);
	TEST_ASSERT(p.error == JSONERR_NO_ERROR);
	TEST_ASSERT(p.tokenidx == ntok);
	TEST_ASSERT(p.textidx == sizeof(input) / sizeof(char) - 1);
	TEST_ASSERT(json_number_get(input, tokens, 0) == 1.23);
}

static void test_double_digit_exp(void)
{
	char input[] = "1e23";
	size_t ntok = 1;
	struct json_token tokens[ntok];
	struct json_parser p = json_parse(input, tokens, ntok);
	TEST_ASSERT(p.error == JSONERR_NO_ERROR);
	TEST_ASSERT(p.tokenidx == ntok);
	TEST_ASSERT(p.textidx == sizeof(input) / sizeof(char) - 1);
	TEST_ASSERT(json_number_get(input, tokens, 0) == 1e23);
}

int main(void)
{
	UNITY_BEGIN();

	RUN_TEST(test_single_digit);
	RUN_TEST(test_multiple_digit);
	RUN_TEST(test_starts_with_zero);
	RUN_TEST(test_decimal);
	RUN_TEST(test_zero_decimal);
	RUN_TEST(test_negative_sign);
	RUN_TEST(test_exponent_upper);
	RUN_TEST(test_exponent_lower);
	RUN_TEST(test_exponent_plus);
	RUN_TEST(test_exponent_minus);
	RUN_TEST(test_sign_decimal_exponent);
	RUN_TEST(test_sign_alone);
	RUN_TEST(test_decimal_without_digits);
	RUN_TEST(test_exponent_without_digits);
	RUN_TEST(test_exponent_sign_without_digits);
	RUN_TEST(test_negative_zero);
	RUN_TEST(test_zero_exp);
	RUN_TEST(test_double_digit_decimal);
	RUN_TEST(test_double_digit_exp);

	return UNITY_END();
}
