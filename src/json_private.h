/***************************************************************************//**

  @file         json_private.h

  @author       Stephen Brennan

  @date         Created Tuesday,  8 December 2015

  @brief        Declarations of internal functions for JSON parsing.

  @copyright    Copyright (c) 2015, Stephen Brennan.  Released under the Revised
                BSD License.  See LICENSE.txt for details.

*******************************************************************************/

#ifndef SMB_JSON_PRIVATE_H
#define SMB_JSON_PRIVATE_H

#include "json.h"

void json_settoken(struct json_token *arr, struct json_token tok,
                   struct json_parser p, size_t maxtoken);
struct json_parser json_parse_string(wchar_t *text, struct json_token *arr,
                                     size_t maxtoken, struct json_parser p);


#endif // SMB_JSON_PRIVATE_H
