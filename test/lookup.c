#include <stdlib.h>
#include <unity.h>

#include "nosj.h"
#include "twitapi.h"

char *j = NULL;
struct json_token *t = NULL;

void setUp(void)
{
	struct json_parser p;
	if (!t) {
		j = (char *)twitapi_json;
		p = json_parse(j, NULL, 0);
		TEST_ASSERT(p.error == JSONERR_NO_ERROR);
		t = calloc(p.tokenidx, sizeof(*t));
		p = json_parse(j, t, p.tokenidx);
		TEST_ASSERT(p.error == JSONERR_NO_ERROR);
	}
}

void tearDown(void)
{
	// clean stuff up here
}

static void test_lookup_single(void)
{
	size_t r = json_lookup(j, t, 0, "favorited");
	TEST_ASSERT_NOT_EQUAL(0, r);
	TEST_ASSERT_EQUAL(JSON_FALSE, t[r].type);
}

static void test_lookup_key_not_exist(void)
{
	size_t r = json_lookup(j, t, 0, "user.foobar");
	TEST_ASSERT_EQUAL(0, r);
}

static void test_lookup_array(void)
{
	size_t r = json_lookup(j, t, 0, "contributors[0]");
	TEST_ASSERT_NOT_EQUAL_INT(0, r);
	TEST_ASSERT_EQUAL_INT(JSON_NUMBER, t[r].type);
	TEST_ASSERT_EQUAL_FLOAT(14927800.0, json_number_get(j, t, r));
}

static void test_lookup_big_chain(void)
{
	size_t r = json_lookup(j, t, 0, "user.entities.url.urls[0].indices[1]");
	TEST_ASSERT_NOT_EQUAL_INT(0, r);
	TEST_ASSERT_EQUAL_INT(JSON_NUMBER, t[r].type);
	TEST_ASSERT_EQUAL_FLOAT(22.0, json_number_get(j, t, r));
}

static void test_lookup_two_index(void)
{
	/* no nested lists in twitapi.json :( */
	char *lj = "[0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, [0, 2, 4, 6, 8]]";
	struct json_token *lt = NULL;
	struct json_parser p = json_parse(lj, NULL, 0);
	TEST_ASSERT_EQUAL_INT(p.error, JSONERR_NO_ERROR);
	lt = calloc(p.tokenidx, sizeof(*lt));
	p = json_parse(lj, lt, p.tokenidx);
	TEST_ASSERT_EQUAL_INT(p.error, JSONERR_NO_ERROR);
	size_t r = json_lookup(lj, lt, 0, "[11][2]");
	TEST_ASSERT_NOT_EQUAL_INT(0, r);
	TEST_ASSERT_EQUAL_INT(JSON_NUMBER, lt[r].type);
	TEST_ASSERT_EQUAL_FLOAT(4.0, json_number_get(lj, lt, r));
	free(lt);
}

static void test_lookup_dot_not_object(void)
{
	size_t r = json_lookup(j, t, 0, "retweeted.yes");
	TEST_ASSERT_EQUAL_INT(0, r);
}

static void test_lookup_index_not_array(void)
{
	size_t r = json_lookup(j, t, 0, "entities[1]");
	TEST_ASSERT_EQUAL_INT(0, r);
}

static void test_lookup_non_integer_index(void)
{
	size_t r = json_lookup(j, t, 0, "entities.urls[abc]");
	TEST_ASSERT_EQUAL_INT(0, r);
}

static void test_lookup_invalid_after_index(void)
{
	size_t r = json_lookup(j, t, 0, "user.entities.url.urls[0]indices");
	TEST_ASSERT_EQUAL_INT(0, r);
}

static void test_lookup_invalid_index(void)
{
	size_t r = json_lookup(j, t, 0, "entities.urls[2]");
	TEST_ASSERT_EQUAL_INT(0, r);
}

int main(void)
{
	UNITY_BEGIN();
	RUN_TEST(test_lookup_single);
	RUN_TEST(test_lookup_key_not_exist);
	RUN_TEST(test_lookup_array);
	RUN_TEST(test_lookup_big_chain);
	RUN_TEST(test_lookup_two_index);
	RUN_TEST(test_lookup_dot_not_object);
	RUN_TEST(test_lookup_index_not_array);
	RUN_TEST(test_lookup_non_integer_index);
	RUN_TEST(test_lookup_invalid_after_index);
	RUN_TEST(test_lookup_invalid_index);
	return UNITY_END();
}
