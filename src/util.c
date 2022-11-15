/***************************************************************************/ /**

   @file         util.c

   @author       Stephen Brennan

   @date         Created Wednesday,  9 December 2015

   @brief        JSON manipulation utilities.

   @copyright    Copyright (c) 2015, Stephen Brennan.  Released under the
 Revised BSD License.  See LICENSE.txt for details.

 *******************************************************************************/

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
	if (p.error != JSONERR_NO_ERROR)
		return -p.error;

	easy->tokens_len = p.tokenidx;
	easy->tokens = calloc(p.tokenidx, sizeof(*easy->tokens));
	p = json_parse(easy->input, easy->tokens, easy->tokens_len);

	/* This should be impossible, but catch it anyway */
	if (p.error != JSONERR_NO_ERROR) {
		free(easy->tokens);
		easy->tokens = NULL;
		easy->tokens_len = 0;
		return -p.error;
	}
	return 0;
}

void json_easy_destroy(struct json_easy *easy)
{
	free(easy->tokens);
}

const char *json_easy_strerror(int err)
{
	return json_strerror(-err);
}

char *json_easy_string_get(struct json_easy *easy, size_t index)
{
	char *buf = malloc(easy->tokens[index].length + 1);
	json_easy_string_load(easy, index, buf);
	return buf;
}

size_t json_object_get(const char *json, const struct json_token *tokens,
                       size_t index, const char *key)
{
	if (tokens[index].type != JSON_OBJECT)
		return 0;

	index = tokens[index].child;

	while (index != 0) {
		if (json_string_match(json, tokens, index, key)) {
			return tokens[index].child;
		}
		index = tokens[index].next;
	}

	return 0;
}

size_t json_array_get(const char *json, const struct json_token *tokens,
                      size_t index, size_t array_index)
{
	(void)json;

	if (array_index >= tokens[index].length) {
		return 0;
	}

	index = tokens[index].child;
	while (array_index--) {
		index = tokens[index].next;
	}

	return index;
}

double json_number_get(const char *json, const struct json_token *tokens,
                       size_t index)
{
	double result;
	sscanf(json + tokens[index].start, "%lf", &result);
	return result;
}

/**
 * Lookup key within the parsed json_token buffer. Note that in this case, "key"
 * is not simply an object key. It can express a dotted notation for traversing
 * JSON objects:
 *
 *   keyname.nextkey[123].blah
 */
size_t json_lookup(const char *json, const struct json_token *arr, size_t tok,
                   const char *key)
{
	size_t start = 0, i = 0;
	int state = 0;
	long index;
	char *keymut = strdup(key);
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
				goto out;
			}
			keymut[i] = '\0';
			// printf("Get key \"%s\"\n", &keymut[start]);
			tok = json_object_get(json, arr, tok, &keymut[start]);
			if (tok == 0) {
				// printf("not found\n");
				goto out;
			}
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
				tok = 0;
				goto out;
			}
			keymut[i] = '\0';
			index = strtol(&keymut[start], NULL, 10);
			// printf("Get ix %ld\n", index);
			tok = json_array_get(json, arr, tok, (size_t)index);
			if (tok == 0) {
				// printf("Not found\n");
				goto out;
			}
			i += 1;
			start = i + 1;
			if (keymut[i] == '.') {
				state = 0;
			} else if (keymut[i] == '[') {
				state = 1;
			} else if (keymut[i] == '\0') {
				goto out;
			} else {
				tok = 0;
				goto out;
			}
		} else if (state == 1 && (c < '0' || c > '9')) {
			// bad character
			tok = 0;
			goto out;
		}
	}

out:
	free(keymut);
	return tok;
}
