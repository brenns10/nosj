/***************************************************************************//**

  @file         util.c

  @author       Stephen Brennan

  @date         Created Wednesday,  9 December 2015

  @brief        JSON manipulation utilities.

  @copyright    Copyright (c) 2015, Stephen Brennan.  Released under the Revised
                BSD License.  See LICENSE.txt for details.

*******************************************************************************/

#include "json.h"

size_t json_object_get(const wchar_t *json, const struct json_token *tokens,
                       size_t index, const wchar_t *key)
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
