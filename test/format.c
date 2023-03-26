/* format.c - test formatting */
#include <stdio.h>
#include <unistd.h>
#include <unity.h>

#include "nosj.h"

FILE *output;
int fdw, fdr;
char buf[4096];

void setUp(void)
{
	int pipes[2];
	pipe(pipes);
	fdr = pipes[0];
	fdw = pipes[1];
	output = fdopen(fdw, "w");
}

void tearDown(void)
{
	if (output) {
		fclose(output);
		close(fdr);
	}
	output = NULL;
	fdw = fdr = 0;
}

static char *format(char *data)
{
	struct json_parser p = json_parse(data, NULL, 0);
	TEST_ASSERT(p.error == JSON_OK);
	struct json_token *tok = calloc(p.tokenidx, sizeof(*tok));
	p = json_parse(data, tok, p.tokenidx);
	json_format(data, tok, p.tokenidx, 0, output);
	fflush(output);
	int count = read(fdr, buf, sizeof(buf));
	buf[count] = '\0';
	return buf;
}

static void test_empty_object(void)
{
	char *res = format("{}");
	TEST_ASSERT_EQUAL_STRING("{}\n", res);
}

static void test_empty_array(void)
{
	char *res = format("[]");
	TEST_ASSERT_EQUAL_STRING("[]\n", res);
}

static void test_simple_object(void)
{
	char *res = format("{\"foo\": 5, \"bar\": 6}");
	TEST_ASSERT_EQUAL_STRING("{\n  \"foo\": 5,\n  \"bar\": 6\n}\n", res);
}

static void test_simple_array(void)
{
	char *res = format("[1, 2, 3]");
	TEST_ASSERT_EQUAL_STRING("[\n  1,\n  2,\n  3\n]\n", res);
}

static void test_complex_nesting(void)
{
	// clang-format off
	char *res = format(
		"{"
		  "\"foo\": {"
		    "\"bar\": 5,"
		    "\"hello\": \"world\""
		  "},"
		  "\"baz\": ["
		    "[true],"
		    "[true, false],"
		    "[null],"
		    "[],"
		    "{},"
		  "]"
		"}"
	);
	const char *expected =
		"{\n"
		"  \"foo\": {\n"
		"    \"bar\": 5,\n"
		"    \"hello\": \"world\"\n"
		"  },\n"
		"  \"baz\": [\n"
		"    [\n"
		"      true\n"
		"    ],\n"
		"    [\n"
		"      true,\n"
		"      false\n"
		"    ],\n"
		"    [\n"
		"      null\n"
		"    ],\n"
		"    [],\n"
		"    {}\n"
		"  ]\n"
		"}\n";
	// clang-format on
	printf("%s", expected);
	printf("%s", res);
	TEST_ASSERT_EQUAL_STRING(expected, res);
}

int main(void)
{
	UNITY_BEGIN();
	RUN_TEST(test_empty_object);
	RUN_TEST(test_empty_array);
	RUN_TEST(test_simple_object);
	RUN_TEST(test_simple_array);
	RUN_TEST(test_complex_nesting);
	return UNITY_END();
}
