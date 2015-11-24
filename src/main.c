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
  char *text;
  struct json_token *tokens = NULL;
  struct json_parser p;

  if (argc < 2 || strcmp(argv[1], "-") == 0) {
    f = stdin;
  } else {
    f = fopen(argv[1], "r");
  }

  text = read_file(f);
  p = json_parse(text, tokens, 0);
  tokens = calloc(p.tokenidx, sizeof(struct json_token));
  p = json_parse(text, tokens, p.tokenidx);
  json_print(tokens, p.tokenidx);
  free(text);
  free(tokens);
  return 0;
}

