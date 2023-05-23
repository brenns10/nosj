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
	uint32_t ntok = 1;
	struct json_token tokens[ntok];
	struct json_parser p = json_parse(input, tokens, ntok);
	double number;
	int64_t dst_int;
	uint64_t dst_uint;
	TEST_ASSERT(p.error == JSON_OK);
	TEST_ASSERT(p.tokenidx == ntok);
	TEST_ASSERT(p.textidx == sizeof(input) / sizeof(char) - 1);
	TEST_ASSERT(!json_number_get(input, tokens, 0, &number));
	TEST_ASSERT_EQUAL(0.0, number);
	TEST_ASSERT(!json_number_getint(input, tokens, 0, &dst_int));
	TEST_ASSERT_EQUAL(0LL, dst_int);
	TEST_ASSERT(!json_number_getuint(input, tokens, 0, &dst_uint));
	TEST_ASSERT_EQUAL(0ULL, dst_uint);
}

static void test_multiple_digit(void)
{
	char input[] = "12";
	uint32_t ntok = 1;
	struct json_token tokens[ntok];
	struct json_parser p = json_parse(input, tokens, ntok);
	double number;
	int64_t dst_int;
	uint64_t dst_uint;
	TEST_ASSERT(p.error == JSON_OK);
	TEST_ASSERT(p.tokenidx == ntok);
	TEST_ASSERT(p.textidx == sizeof(input) / sizeof(char) - 1);
	TEST_ASSERT(!json_number_get(input, tokens, 0, &number));
	TEST_ASSERT_EQUAL(12.0, number);
	TEST_ASSERT(!json_number_getint(input, tokens, 0, &dst_int));
	TEST_ASSERT_EQUAL(12LL, dst_int);
	TEST_ASSERT(!json_number_getuint(input, tokens, 0, &dst_uint));
	TEST_ASSERT_EQUAL(12ULL, dst_uint);
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
	TEST_ASSERT(p.error == JSON_OK);
	TEST_ASSERT(p.tokenidx == 1);
	TEST_ASSERT(p.textidx == 1); // ONLY PARSED ONE CHARACTER
}

static void test_decimal(void)
{
	char input[] = "1.1";
	uint32_t ntok = 1;
	struct json_token tokens[ntok];
	struct json_parser p = json_parse(input, tokens, ntok);
	double number;
	int64_t dst_int;
	uint64_t dst_uint;
	TEST_ASSERT(p.error == JSON_OK);
	TEST_ASSERT(p.tokenidx == ntok);
	TEST_ASSERT(p.textidx == sizeof(input) / sizeof(char) - 1);
	TEST_ASSERT(!json_number_get(input, tokens, 0, &number));
	TEST_ASSERT_EQUAL(1.1, number);
	TEST_ASSERT(JSONERR_NOT_INT ==
	            json_number_getint(input, tokens, 0, &dst_int));
	TEST_ASSERT(JSONERR_NOT_INT ==
	            json_number_getuint(input, tokens, 0, &dst_uint));
}

static void test_zero_decimal(void)
{
	char input[] = "0.1";
	uint32_t ntok = 1;
	struct json_token tokens[ntok];
	struct json_parser p = json_parse(input, tokens, ntok);
	double number;
	int64_t dst_int;
	uint64_t dst_uint;
	TEST_ASSERT(p.error == JSON_OK);
	TEST_ASSERT(p.tokenidx == ntok);
	TEST_ASSERT(p.textidx == sizeof(input) / sizeof(char) - 1);
	TEST_ASSERT(!json_number_get(input, tokens, 0, &number));
	TEST_ASSERT_EQUAL(0.1, number);
	TEST_ASSERT(JSONERR_NOT_INT ==
	            json_number_getint(input, tokens, 0, &dst_int));
	TEST_ASSERT(JSONERR_NOT_INT ==
	            json_number_getuint(input, tokens, 0, &dst_uint));
}

static void test_negative_sign(void)
{
	char input[] = "-123";
	uint32_t ntok = 1;
	struct json_token tokens[ntok];
	struct json_parser p = json_parse(input, tokens, ntok);
	double number;
	int64_t dst_int;
	uint64_t dst_uint;
	TEST_ASSERT(p.error == JSON_OK);
	TEST_ASSERT(p.tokenidx == ntok);
	TEST_ASSERT(p.textidx == sizeof(input) / sizeof(char) - 1);
	TEST_ASSERT(!json_number_get(input, tokens, 0, &number));
	TEST_ASSERT_EQUAL(-123.0, number);
	TEST_ASSERT(!json_number_getint(input, tokens, 0, &dst_int));
	TEST_ASSERT_EQUAL(-123LL, dst_int);
	TEST_ASSERT(JSONERR_NOT_INT ==
	            json_number_getuint(input, tokens, 0, &dst_uint));
}

static void test_exponent_upper(void)
{
	char input[] = "1E5";
	uint32_t ntok = 1;
	struct json_token tokens[ntok];
	struct json_parser p = json_parse(input, tokens, ntok);
	double number;
	int64_t dst_int;
	uint64_t dst_uint;
	TEST_ASSERT(p.error == JSON_OK);
	TEST_ASSERT(p.tokenidx == ntok);
	TEST_ASSERT(p.textidx == sizeof(input) / sizeof(char) - 1);
	TEST_ASSERT(!json_number_get(input, tokens, 0, &number));
	TEST_ASSERT_EQUAL(1e5, number);
	TEST_ASSERT(JSONERR_NOT_INT ==
	            json_number_getint(input, tokens, 0, &dst_int));
	TEST_ASSERT(JSONERR_NOT_INT ==
	            json_number_getuint(input, tokens, 0, &dst_uint));
}

static void test_exponent_lower(void)
{
	char input[] = "1e5";
	uint32_t ntok = 1;
	struct json_token tokens[ntok];
	struct json_parser p = json_parse(input, tokens, ntok);
	double number;
	int64_t dst_int;
	uint64_t dst_uint;
	TEST_ASSERT(p.error == JSON_OK);
	TEST_ASSERT(p.tokenidx == ntok);
	TEST_ASSERT(p.textidx == sizeof(input) / sizeof(char) - 1);
	TEST_ASSERT(!json_number_get(input, tokens, 0, &number));
	TEST_ASSERT_EQUAL(1e5, number);
	TEST_ASSERT(JSONERR_NOT_INT ==
	            json_number_getint(input, tokens, 0, &dst_int));
	TEST_ASSERT(JSONERR_NOT_INT ==
	            json_number_getuint(input, tokens, 0, &dst_uint));
}

static void test_exponent_plus(void)
{
	char input[] = "1e+5";
	uint32_t ntok = 1;
	struct json_token tokens[ntok];
	struct json_parser p = json_parse(input, tokens, ntok);
	double number;
	int64_t dst_int;
	uint64_t dst_uint;
	TEST_ASSERT(p.error == JSON_OK);
	TEST_ASSERT(p.tokenidx == ntok);
	TEST_ASSERT(p.textidx == sizeof(input) / sizeof(char) - 1);
	TEST_ASSERT(!json_number_get(input, tokens, 0, &number));
	TEST_ASSERT_EQUAL(1e5, number);
	TEST_ASSERT(JSONERR_NOT_INT ==
	            json_number_getint(input, tokens, 0, &dst_int));
	TEST_ASSERT(JSONERR_NOT_INT ==
	            json_number_getuint(input, tokens, 0, &dst_uint));
}

static void test_exponent_minus(void)
{
	char input[] = "1e-5";
	uint32_t ntok = 1;
	struct json_token tokens[ntok];
	struct json_parser p = json_parse(input, tokens, ntok);
	double number;
	int64_t dst_int;
	uint64_t dst_uint;
	TEST_ASSERT(p.error == JSON_OK);
	TEST_ASSERT(p.tokenidx == ntok);
	TEST_ASSERT(p.textidx == sizeof(input) / sizeof(char) - 1);
	TEST_ASSERT(!json_number_get(input, tokens, 0, &number));
	TEST_ASSERT_EQUAL(1e-5, number);
	TEST_ASSERT(JSONERR_NOT_INT ==
	            json_number_getint(input, tokens, 0, &dst_int));
	TEST_ASSERT(JSONERR_NOT_INT ==
	            json_number_getuint(input, tokens, 0, &dst_uint));
}

static void test_sign_decimal_exponent(void)
{
	char input[] = "-1.5e+5";
	uint32_t ntok = 1;
	struct json_token tokens[ntok];
	struct json_parser p = json_parse(input, tokens, ntok);
	double number;
	int64_t dst_int;
	uint64_t dst_uint;
	TEST_ASSERT(p.error == JSON_OK);
	TEST_ASSERT(p.tokenidx == ntok);
	TEST_ASSERT(p.textidx == sizeof(input) / sizeof(char) - 1);
	TEST_ASSERT(!json_number_get(input, tokens, 0, &number));
	TEST_ASSERT_EQUAL(-1.5e5, number);
	TEST_ASSERT(JSONERR_NOT_INT ==
	            json_number_getint(input, tokens, 0, &dst_int));
	TEST_ASSERT(JSONERR_NOT_INT ==
	            json_number_getuint(input, tokens, 0, &dst_uint));
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
	uint32_t ntok = 1;
	struct json_token tokens[ntok];
	struct json_parser p = json_parse(input, tokens, ntok);
	double number;
	int64_t dst_int;
	uint64_t dst_uint;
	TEST_ASSERT(p.error == JSON_OK);
	TEST_ASSERT(p.tokenidx == ntok);
	TEST_ASSERT(p.textidx == sizeof(input) / sizeof(char) - 1);
	TEST_ASSERT(!json_number_get(input, tokens, 0, &number));
	TEST_ASSERT_EQUAL(0.0, number);
	TEST_ASSERT(!json_number_getint(input, tokens, 0, &dst_int));
	TEST_ASSERT_EQUAL(0LL, dst_int);
	/* Even though this is technically unsigned, it's a negative and we'll
	 * fail */
	TEST_ASSERT(JSONERR_NOT_INT ==
	            json_number_getuint(input, tokens, 0, &dst_uint));
}

static void test_zero_exp(void)
{
	char input[] = "0e5"; // again, doesn't make sense, but is valid
	uint32_t ntok = 1;
	struct json_token tokens[ntok];
	struct json_parser p = json_parse(input, tokens, ntok);
	double number;
	int64_t dst_int;
	uint64_t dst_uint;
	TEST_ASSERT(p.error == JSON_OK);
	TEST_ASSERT(p.tokenidx == ntok);
	TEST_ASSERT(p.textidx == sizeof(input) / sizeof(char) - 1);
	TEST_ASSERT(JSONERR_NOT_INT ==
	            json_number_getint(input, tokens, 0, &dst_int));
	TEST_ASSERT(JSONERR_NOT_INT ==
	            json_number_getuint(input, tokens, 0, &dst_uint));
	TEST_ASSERT(!json_number_get(input, tokens, 0, &number));
	TEST_ASSERT_EQUAL(0.0, number);
}

static void test_double_digit_decimal(void)
{
	char input[] = "1.23";
	uint32_t ntok = 1;
	struct json_token tokens[ntok];
	struct json_parser p = json_parse(input, tokens, ntok);
	double number;
	int64_t dst_int;
	uint64_t dst_uint;
	TEST_ASSERT(p.error == JSON_OK);
	TEST_ASSERT(p.tokenidx == ntok);
	TEST_ASSERT(p.textidx == sizeof(input) / sizeof(char) - 1);
	TEST_ASSERT(JSONERR_NOT_INT ==
	            json_number_getint(input, tokens, 0, &dst_int));
	TEST_ASSERT(JSONERR_NOT_INT ==
	            json_number_getuint(input, tokens, 0, &dst_uint));
	TEST_ASSERT(!json_number_get(input, tokens, 0, &number));
	TEST_ASSERT_EQUAL(1.23, number);
}

static void test_double_digit_exp(void)
{
	char input[] = "1e23";
	uint32_t ntok = 1;
	struct json_token tokens[ntok];
	struct json_parser p = json_parse(input, tokens, ntok);
	double number = 0;
	int64_t dst_int;
	uint64_t dst_uint;
	TEST_ASSERT(p.error == JSON_OK);
	TEST_ASSERT(p.tokenidx == ntok);
	TEST_ASSERT(p.textidx == sizeof(input) / sizeof(char) - 1);
	TEST_ASSERT(JSONERR_NOT_INT ==
	            json_number_getint(input, tokens, 0, &dst_int));
	TEST_ASSERT(JSONERR_NOT_INT ==
	            json_number_getuint(input, tokens, 0, &dst_uint));
	TEST_ASSERT(!json_number_get(input, tokens, 0, &number));
	/* Something with Unity doesn't work with TEST_ASSERT_EQUAL here ... */
	TEST_ASSERT(1e23 == number);
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
