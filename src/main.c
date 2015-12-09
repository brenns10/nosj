/***************************************************************************//**

  @file         main.c

  @author       Stephen Brennan

  @date         Created Monday, 23 November 2015

  @brief        Driver

  @copyright    Copyright (c) 2015, Stephen Brennan.  Released under the Revised
                BSD License.  See LICENSE.txt for details.

*******************************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "libstephen/str.h"
#include "json.h"

int main(int argc, char *argv[])
{
  FILE *f;
  wchar_t *text;
  struct json_token *tokens = NULL;
  struct json_parser p;
  int returncode = 0;

  if (argc < 2 || strcmp(argv[1], "-") == 0) {
    f = stdin;
  } else {
    f = fopen(argv[1], "r");
  }

  // Read the whole contents of the file.
  text = read_filew(f);

  // Parse the first time to get the number of tokens.
  p = json_parse(text, tokens, 0);
  if (p.error != JSONERR_NO_ERROR) {
    json_print_error(stderr, p);
    returncode = 1;
    goto cleanup_text;
  }

  // Then allocate and parse to save the tokens.
  tokens = calloc(p.tokenidx, sizeof(struct json_token));
  p = json_parse(text, tokens, p.tokenidx);

  // Finally, print.
  json_print(tokens, p.tokenidx);

  // A demonstration of getting tokens out of an object.
  if (p.tokenidx > 0 && tokens[0].type == JSON_OBJECT) {
    printf("Searching for key \"text\" in the base object.\n");
    size_t value = json_object_get(text, tokens, 0, L"text");
    if (value != 0) {
      printf("Found key \"text\".\n");
      json_print(tokens + value, 1);
      wchar_t *string = calloc(sizeof(wchar_t), tokens[value].length + 1);
      json_string_load(text, tokens, value, string);
      printf("Value: \"%ls\"\n", string);
      free(string);
    } else {
      printf("Key \"text\" not found in base object.\n");
    }
  }

  free(tokens);
 cleanup_text:
  free(text);
  return returncode;
}

