/***************************************************************************/ /**

   @file         util.c

   @author       Stephen Brennan

   @date         Created Wednesday,  9 December 2015

   @brief        JSON manipulation utilities.

   @copyright    Copyright (c) 2015, Stephen Brennan.  Released under the
 Revised BSD License.  See LICENSE.txt for details.

 *******************************************************************************/

#include <stdio.h>

#include "nosj.h"

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

	if (index >= tokens[index].length) {
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
