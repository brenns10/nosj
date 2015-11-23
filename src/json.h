/***************************************************************************//**

  @file         json.h

  @author       Stephen Brennan

  @date         Created Sunday, 22 November 2015

  @brief        JSON parsing!

  @copyright    Copyright (c) 2015, Stephen Brennan.  Released under the Revised
                BSD License.  See LICENSE.txt for details.

*******************************************************************************/

#ifndef SMB_JSON
#define SMB_JSON

enum json_type {
  JSON_OBJECT=0,
  JSON_ARRAY,
  JSON_NUMBER,
  JSON_STRING,
  JSON_TRUE,
  JSON_FALSE,
  JSON_NULL
};

struct json_token {
  enum json_type type;
  size_t start;
  size_t end;
  size_t child;
  size_t next;
};

/**
   @brief Array mapping JSON type to a string representation of that type.
 */
char *json_type_str[JSON_NULL+1];

/**
   @brief Parse JSON into tokens.

   This function simply tokenizes JSON.  That is, it identifies the location of
   each JSON entity: objects, arrays, strings, numbers, booleans, and null.  It
   tokenizes into an already allocated buffer of tokens, so that no memory
   allocation takes place.  This means that you should pre-allocate a buffer.
   In order to know what size buffer to allocate, you can call this function
   with arr=NULL, and it will return the number of tokens it would have parsed.

   @param json The text buffer to parse.
   @param arr A buffer to put the tokens in.  May be null.
   @param n The number of slots in the arr buffer.
   @returns The number of tokens parsed.
 */
size_t json_parse(char *json, struct json_token *arr, size_t n);

/**
   @brief Print a list of JSON tokens.

   This is mostly for diagnostics.  After you've parsed JSON, you may want to
   know whether you did it right.  This function prints out the contents of a
   buffer of JSON tokens so that you can see the parse results.

   @param arr The array of tokens to print.
   @param n The number of tokens in the buffer.
 */
void json_print(struct json_token *arr, size_t n);

#endif // SMB_JSON
