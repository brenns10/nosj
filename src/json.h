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

#include <stddef.h>
#include <stdio.h>
#include <stdbool.h>
#include <wchar.h>

/**
   @brief Enumeration for all possible types of JSON values.

   The JSON spec lists each of these as their own type.  So, I internally
   recognize them as such.  An argument could be made for having JSON_TRUE and
   JSON_FALSE be the same type (some sort of boolean type).  But I don't
   honestly think that would make things that much clearer or simpler.
 */
enum json_type {
  JSON_OBJECT=0,
  JSON_ARRAY,
  JSON_NUMBER,
  JSON_STRING,
  JSON_TRUE,
  JSON_FALSE,
  JSON_NULL
};

/**
   @brief Represents a JSON "token".

   This isn't a token in the sense of lexical analysis.  JSON is simple enough
   that I don't really need to tokenize before parsing.  Each "token" is a JSON
   value (object, array, string, number, etc).  Tokens are stored in an array.
 */
struct json_token {

  /**
     @brief Type of the token.
   */
  enum json_type type;
  /**
     @brief Index of the first character of the token in the string.
   */
  size_t start;
  /**
     @brief Index of the last character of the token in the string.
   */
  size_t end;
  /**
     @brief For tokens that can have a length, this is the length!

     More specifically, it is:
     - For arrays, the number of elements.
     - For objects, the number of key, value pairs.
     - For strings, the number of Unicode code points.
   */
  size_t length;
  /**
     @brief Index of the first "child" value.

     Child values are either items in a list, keys in a dictionary, or the
     values associated with a key.
   */
  size_t child;
  /**
     @brief Index of the next value in the sequence.
   */
  size_t next;
};

/**
   @brief Errors that could be encountered in JSON parsing.
 */
enum json_error {
  /**
    @brief No error!
   */
  JSONERR_NO_ERROR,
  /**
     @brief An error was encountered while parsing a number.
   */
  JSONERR_INVALID_NUMBER,
  /**
     @brief The string ended unexpectedly.
   */
  JSONERR_PREMATURE_EOF,
  /**
     @brief Parser encountered a token that was not expected.
   */
  JSONERR_UNEXPECTED_TOKEN,
  /**
     @brief Parser encountered an invalid surrogate pair.
   */
  JSONERR_INVALID_SURROGATE,
  /**
     @brief Parser did not encounter an expected token.

     This error has an argument (e.g. expected ':').
   */
  JSONERR_EXPECTED_TOKEN,
};

/**
   @brief Data structure that contains parser state.

   We pass and return copies of this struct throughout this parser.  I like this
   just because it makes things a bit more transparent, and a little less
   "mystery pointer operations".
 */
struct json_parser {
  /**
     @brief The index of the next "unhandled" character.
   */
  size_t textidx;
  /**
     @brief The index of the next slot to stick a token in our array.
   */
  size_t tokenidx;
  /**
     @brief Error code from the parser, if any.
   */
  enum json_error error;
  /**
     @brief Argument to the error code.
   */
  size_t errorarg;
};

/**
   @brief Array mapping JSON type to a string representation of that type.
 */
char *json_type_str[JSON_NULL+1];

/**
   @brief Array mapping error to printf format string.
 */
char *json_error_str[JSONERR_EXPECTED_TOKEN+1];

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
   @returns A parser result.
 */
struct json_parser json_parse(wchar_t *json, struct json_token *arr, size_t n);

/**
   @brief Print a list of JSON tokens.

   This is mostly for diagnostics.  After you've parsed JSON, you may want to
   know whether you did it right.  This function prints out the contents of a
   buffer of JSON tokens so that you can see the parse results.

   @param arr The array of tokens to print.
   @param n The number of tokens in the buffer.
 */
void json_print(struct json_token *arr, size_t n);

/**
   @brief Print out the error message for a parser error.
   @param f File to print to.
   @param p Parser return struct.
 */
void json_print_error(FILE *f, struct json_parser p);

/**
   @brief Return whether or not a string matches a token string.
   @param json The original JSON buffer.
   @param tokens The parsed tokens.
   @param index The index of the string token.
   @param other The other string to compare to.
   @return True if they are equal, false otherwise.
 */
bool json_string_match(const wchar_t *json, const struct json_token *tokens,
                       size_t index, const wchar_t *other);

/**
   @brief Load a string into a buffer.
   @param json The original JSON buffer.
   @param tokens The parsed tokens.
   @param index The index of the string token.
   @param buffer The buffer to load the string into.

   The buffer MUST NOT be null.  It must point to an already allocated buffer,
   of at least size `tokens[index].length + 1` (room for the text and a NULL
   character).
 */
void json_string_load(const wchar_t *json, const struct json_token *tokens,
                      size_t index, wchar_t *buffer);

#endif // SMB_JSON
