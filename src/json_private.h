/***************************************************************************/ /**

   @file         json_private.h

   @author       Stephen Brennan

   @date         Created Tuesday,  8 December 2015

   @brief        Declarations of internal functions for JSON parsing.

   @copyright    Copyright (c) 2015, Stephen Brennan.  Released under the
 Revised BSD License.  See LICENSE.txt for details.

 *******************************************************************************/

#ifndef SMB_JSON_PRIVATE_H
#define SMB_JSON_PRIVATE_H

#include "nosj.h"

/**
   @brief Array mapping JSON type to a string representation of that type.
 */
extern char *json_type_str[JSON_NULL + 1];

/**
   @brief Array mapping error to printf format string.
 */
extern char *json_error_str[_LAST_JSONERR];

void json_settoken(struct json_token *arr, struct json_token tok,
                   struct json_parser p, uint32_t maxtoken);
struct json_parser json_parse_string(const char *text, struct json_token *arr,
                                     uint32_t maxtoken, struct json_parser p);

#endif // SMB_JSON_PRIVATE_H
