/***************************************************************************/ /**

   @file         util.c

   @author       Stephen Brennan

   @date         Created Wednesday,  9 December 2015

   @brief        JSON manipulation utilities.

   @copyright    Copyright (c) 2015, Stephen Brennan.  Released under the
 Revised BSD License.  See LICENSE.txt for details.

 *******************************************************************************/

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "nosj.h"

void json_easy_init(struct json_easy *easy, const char *input)
{
	easy->input = input;
	easy->input_len = strlen(input);
	easy->tokens = NULL;
	easy->tokens_len = 0;
}

int json_easy_parse(struct json_easy *easy)
{
	struct json_parser p;

	/* Already parsed? */
	if (easy->tokens)
		return 0;

	p = json_parse(easy->input, NULL, 0);
	if (p.error != JSON_OK)
		return p.error;

	easy->tokens_len = p.tokenidx;
	easy->tokens = calloc(p.tokenidx, sizeof(*easy->tokens));
	p = json_parse(easy->input, easy->tokens, easy->tokens_len);

	/* This should be impossible, but catch it anyway */
	if (p.error != JSON_OK) {
		free(easy->tokens);
		easy->tokens = NULL;
		easy->tokens_len = 0;
		return p.error;
	}
	return 0;
}

void json_easy_destroy(struct json_easy *easy)
{
	free(easy->tokens);
}

int json_easy_string_get(struct json_easy *easy, uint32_t index, char **out)
{
	char *buf = malloc(easy->tokens[index].length + 1);
	int rv = json_easy_string_load(easy, index, buf);
	if (rv != JSON_OK) {
		free(buf);
		return rv;
	}
	*out = buf;
	return JSON_OK;
}

int json_object_get(const char *json, const struct json_token *tokens,
                    uint32_t index, const char *key, uint32_t *ret)
{
	if (tokens[index].type != JSON_OBJECT)
		return JSONERR_TYPE;

	index++; /* First key has index one greater than object */

	while (index != 0) {
		int rv;
		bool match;

		rv = json_string_match(json, tokens, index, key, &match);
		assert(rv == JSON_OK);
		if (match) {
			/* Value has index one greater than key */
			*ret = index + 1;
			return JSON_OK;
		}
		index = tokens[index].next;
	}

	return JSONERR_LOOKUP;
}

int json_array_get(const char *json, const struct json_token *tokens,
                   uint32_t index, uint32_t array_index, uint32_t *result)
{
	(void)json;

	if (tokens[index].type != JSON_ARRAY)
		return JSONERR_TYPE;
	if (array_index >= tokens[index].length)
		return JSONERR_INDEX;

	index++; /* First element has index one greater than array */
	while (array_index--) {
		index = tokens[index].next;
	}
	*result = index;
	return JSON_OK;
}

int json_number_get(const char *json, const struct json_token *tokens,
                    uint32_t index, double *number)
{
	if (tokens[index].type != JSON_NUMBER)
		return JSONERR_TYPE;
	/*
	 * At this point, we've validated the syntax of the float. If sscanf()
	 * fails, that is highly unexpected and worth an assertion.
	 */
	assert(sscanf(json + tokens[index].start, "%lf", number) == 1);
	return JSON_OK;
}

int json_number_getint(const char *json, const struct json_token *tokens,
                       uint32_t index, int64_t *number)
{
	if (tokens[index].type != JSON_NUMBER)
		return JSONERR_TYPE;
	char *end;
	int64_t val = strtoll(json + tokens[index].start, &end, 10);
	if (end != json + tokens[index].length) {
		return JSONERR_NOT_INT;
	}
	*number = val;
	return JSON_OK;
}

int json_number_getuint(const char *json, const struct json_token *tokens,
                        uint32_t index, uint64_t *number)
{
	if (tokens[index].type != JSON_NUMBER)
		return JSONERR_TYPE;
	char *end;
	/* Fail negative numbers, since strtoull doesn't */
	if (json[tokens[index].start] == '-')
		return JSONERR_NOT_INT;
	uint64_t val = strtoull(json + tokens[index].start, &end, 10);
	if (end != json + tokens[index].length)
		return JSONERR_NOT_INT;
	*number = val;
	return JSON_OK;
}

/**
 * Lookup key within the parsed json_token buffer. Note that in this case, "key"
 * is not simply an object key. It can express a dotted notation for traversing
 * JSON objects:
 *
 *   keyname.nextkey[123].blah
 */
int json_lookup(const char *json, const struct json_token *arr, uint32_t tok,
                const char *key, uint32_t *result)
{
	uint32_t start = 0, i = 0;
	int state = 0;
	int ret = JSON_OK;
	long index;
	char *keymut = strdup(key);
	char *endptr;
	char c;

	for (i = 0;; i++) {
		c = keymut[i];
		if (state == 0 && (c == '.' || c == '[' || c == '\0')) {
			if (i == 0 && c == '[') {
				/* Exception: allow the string to start with
				 * [indexing] */
				state = 1;
				start = 1;
				continue;
			}
			if (arr[tok].type != JSON_OBJECT) {
				tok = 0;
				ret = JSONERR_TYPE;
				goto out;
			}
			keymut[i] = '\0';
			ret = json_object_get(json, arr, tok, &keymut[start],
			                      &tok);
			if (tok == 0)
				goto out;
			start = i + 1;
			if (c == '[')
				state = 1;
			else if (c == '\0')
				goto out;
			// else if (c == '.') stay in state 0
		} else if (state == 0) {
			// do nothing
		} else if (state == 1 && c == ']') {
			if (arr[tok].type != JSON_ARRAY) {
				ret = JSONERR_TYPE;
				goto out;
			}
			keymut[i] = '\0';
			index = strtol(&keymut[start], &endptr, 10);
			if (endptr != &keymut[i]) {
				/* This should actually be covered by the case
				 * below, but it's extra precaution. */
				ret = JSONERR_BAD_EXPR;
				goto out;
			}
			if (index < 0) {
				ret = JSONERR_INDEX;
				goto out;
			}
			ret = json_array_get(json, arr, tok, (size_t)index,
			                     &tok);
			if (ret != JSON_OK)
				goto out;
			i += 1;
			start = i + 1;
			if (keymut[i] == '.') {
				state = 0;
			} else if (keymut[i] == '[') {
				state = 1;
			} else if (keymut[i] == '\0') {
				goto out;
			} else {
				ret = JSONERR_BAD_EXPR;
				goto out;
			}
		} else if (state == 1 && (c < '0' || c > '9')) {
			/* We also check the index values above, but
			 * Doing it preemptively here allows us to give
			 * a more accurate error index.
			 */
			ret = JSONERR_BAD_EXPR;
			goto out;
		}
	}

out:
	free(keymut);
	if (ret == JSON_OK) {
		assert(tok != 0);
		*result = tok;
	} else {
		/* For errors, we can give an exact index. */
		*result = i;
	}
	return ret;
}

void json_lookup_error(FILE *f, const char *expr, int err, uint32_t index)
{
	fprintf(f, "error in lookup expression:\n");
	fprintf(f, "  %s\n", expr);
	for (uint32_t i = 0; i < index + 2; i++)
		fputc(' ', f);
	fputc('^', f);
	fprintf(f, "\n%s\n", json_strerror(err));
}
