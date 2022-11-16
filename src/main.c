/***************************************************************************/ /**

   @file         main.c

   @author       Stephen Brennan

   @date         Created Monday, 23 November 2015

   @brief        Driver

   @copyright    Copyright (c) 2015, Stephen Brennan.  Released under the
 Revised BSD License.  See LICENSE.txt for details.

 *******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "nosj.h"

int main(int argc, char *argv[])
{
	FILE *f;
	char *text;
	struct json_token *tokens = NULL;
	struct json_parser p;
	int returncode = 0;

	// When no filename specified, or "-" specified, use STDIN.  Else, use
	// the specified filename as input.
	if (argc < 2 || strcmp(argv[1], "-") == 0) {
		f = stdin;
	} else {
		f = fopen(argv[1], "r");
	}

#define FS 8192
	// Read the whole contents of the file (hope it's less than 8192 byte)
	text = malloc(FS);
	fread(text, 1, FS, f);

	// Parse the first time to get the number of tokens.
	p = json_parse(text, tokens, 0);
	if (p.error != JSON_OK) {
		json_print_error(stderr, p);
		returncode = 1;
		goto cleanup_text;
	}

	// Then allocate and parse to save the tokens.
	tokens = calloc(p.tokenidx, sizeof(struct json_token));
	p = json_parse(text, tokens, p.tokenidx);

	// Finally, print the entire token array.
	json_print(tokens, p.tokenidx);

	// Now, let's look for the key "text" in the root object.
	if (p.tokenidx > 0 && tokens[0].type == JSON_OBJECT) {
		uint32_t value;
		// We can only do this if there is a root value and it's an
		// object.
		printf("Searching for key \"text\" in the base object.\n");
		int ret = json_object_get(text, tokens, 0, "text", &value);

		if (ret == JSON_OK) {
			// Non-zero means we successfully found the key!
			printf("Found key \"text\".\n");
			json_print(tokens + value,
			           1); // print just that one token

			if (tokens[value].type == JSON_STRING) {
				// We're expecting this to be a string.  So,
				// let's load it and print it.
				char *string = calloc(sizeof(char),
				                      tokens[value].length + 1);
				json_string_load(text, tokens, value, string);
				printf("Value: \"%s\"\n", string);
				free(string);
			} else {
				printf("Value associated with \"text\" was not "
				       "a string.\n");
			}
		} else {
			/* Technically we should check ret, but w/e */
			printf("Key \"text\" not found in base object.\n");
		}
	}

	free(tokens);
cleanup_text:
	free(text);
	return returncode;
}
