/*
 * test/easy.c: Test the "json_easy" API
 */

#include <unity.h>

#include "nosj.h"
#include "twitapi.h"

void setUp(void)
{
	// set stuff up here
}

void tearDown(void)
{
	// clean stuff up here
}

static void test_basic_access(void)
{
	struct json_easy *easy = json_easy_new(twitapi_json);
	size_t index;
	double number;

	TEST_ASSERT(!json_easy_parse(easy));
	TEST_ASSERT(!json_easy_object_get(easy, 0, "retweet_count", &index));
	TEST_ASSERT(!json_easy_number_get(easy, index, &number));
	TEST_ASSERT_EQUAL(66, number);
	json_easy_free(easy);
}

static void test_easy_string(void)
{
	struct json_easy *easy = json_easy_new(twitapi_json);
	size_t index;
	char *string;

	TEST_ASSERT(!json_easy_parse(easy));
	TEST_ASSERT(!json_easy_lookup(easy, 0, "user.name", &index));
	TEST_ASSERT(!json_easy_string_get(easy, index, &string));
	TEST_ASSERT_EQUAL_STRING("Twitter API", string);
	free(string);
	json_easy_free(easy);
}

static void test_parse_fail(void)
{
	struct json_easy *easy = json_easy_new("[1, 2");
	int result = json_easy_parse(easy);
	TEST_ASSERT_EQUAL(JSONERR_MISSING_COMMA, result);
	TEST_ASSERT_EQUAL_STRING("expected comma between elements",
	                         json_strerror(result));
	json_easy_free(easy);
}

int main(void)
{
	UNITY_BEGIN();

	RUN_TEST(test_basic_access);
	RUN_TEST(test_easy_string);
	RUN_TEST(test_parse_fail);

	return UNITY_END();
}
