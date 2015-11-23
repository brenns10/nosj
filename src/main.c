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

#include "libstephen/str.h"
#include "json.h"

int main(int argc, char *argv[])
{
  FILE *f;
  char *text;
  struct json_token *tokens = NULL;
  size_t amount = 1000;

  if (argc < 2 || strcmp(argv[1], "-") == 0) {
    f = stdin;
  } else {
    f = fopen(argv[1], "r");
  }

  text = read_file(f);
  amount = json_parse(text, tokens, amount);
  printf("amount: %lu\n", amount);
  tokens = calloc(amount, sizeof(struct json_token));
  printf("tokens: %p\n", tokens);
  json_parse(text, tokens, amount);
  json_print(tokens, amount);
  free(text);
  free(tokens);
  return 0;
}

