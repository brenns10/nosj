/***************************************************************************//**

  @file         string.c

  @author       Stephen Brennan

  @date         Created Tuesday,  8 December 2015

  @brief        Parsing strings.

  @copyright    Copyright (c) 2015, Stephen Brennan.  Released under the Revised
                BSD License.  See LICENSE.txt for details.

  This file contains the string parser.  It is designed to be independent of
  what you're parsing the string for.  That is, it can be used in all these
  situations:

  - Recognizing string tokens when doing the initial tokenizing.
  - Comparing string tokens against other strings.
  - Loading string tokens into actual strings.

*******************************************************************************/

#include <stdbool.h>
#include <assert.h>

#include "json.h"
#include "json_private.h"

/*******************************************************************************

                      Private Declarations for the Parser

*******************************************************************************/

// forward declaration of struct parser_arg
struct parser_arg;

/**
   @brief A function that is called for every parsed character.
   @param a The parser arguments.  Mostly for reference.
   @param out The next parsed character in the string.
   @param data Any data the setter might need.
 */
typedef void (*output_setter)(struct parser_arg *a, wchar_t out, void *data);

/**
   @brief States of the parser.
 */
enum parser_st {
  START, INSTRING, ESCAPE, END, UESC0, UESC1, UESC2, UESC3
};

/**
   @brief All the variables the parser needs to do its job.
 */
struct parser_arg {
  /**
     @brief The state of the parser.
   */
  enum parser_st state;
  /**
     @brief Input text.
   */
  const wchar_t *text;
  /**
     @brief Current index of the text we're parsing.
   */
  size_t textidx;
  /**
     @brief Function to call for every character we parse.
   */
  output_setter setter;
  /**
     @brief Argument to go to the output setting function.
   */
  void *setter_arg;
  /**
     @brief Index in which to put the next output character.
   */
  size_t outidx;
  /**
     @brief Previously parsed unicode escape character.

     This is used due to the fact that JSON only does 2-byte Unicode escapes.
     In order to escape characters beyond the BMP (besides just putting them in
     literally), you have to do the UTF-16 surrogate pair.  What a pain.
   */
  wchar_t prev;
  /**
     @brief Unicode escape character we are currently parsing.
   */
  wchar_t curr;
  /**
     @brief Any error we want to report.
   */
  enum json_error error;
};

/*******************************************************************************

                                Helper Functions

*******************************************************************************/

/**
   @brief Return true if c is a valid character to come after a backslash.
 */
static bool json_isescape(wchar_t c)
{
  switch (c) {
  case L'\"':
  case L'\\':
  case L'/':
  case L'b':
  case L'f':
  case L'n':
  case L'r':
  case L't':
  case L'u':
    return true;
  default:
    return false;
  }
}

/**
   @brief Return true if c is a valid hexadecimal digit for JSON.

   Although there is an iswxdigit function in the C standard library, it allows
   for other hexadecimal other than just 0-9, a-f, A-F (depending on locale).
   The JSON spec explicitly states that these are the only hex characters it
   accepts, so I've written my own to explicitly cover only those.
 */
static unsigned char json_xdigit(wchar_t c) {
  if (L'0' <= c && c <= L'9') {
    return (unsigned char) (c - L'0');
  } else if (L'a' <= c && c <= L'f') {
    return (unsigned char) (10 + c - L'a');
  } else if (L'A' <= c && c <= L'F') {
    return (unsigned char) (10 + c - L'A');
  } else {
    return 0xFF;
  }
}

/**
   @brief Register the output character.
   @param a Parser data.
   @param out The output character.

   Always "flushes" the previous character so that in case a surrogate pair
   doesn't happen, the original first character is preserved.  After that, it
   calls the output setter and increments the index.
 */
static void set_output(struct parser_arg *a, wchar_t out) {
  // don't forget to flush the "buffered" potential surrogate pair
  if (a->prev != 0) {
    wchar_t prev = a->prev;
    a->prev = 0;
    set_output(a, prev);
  }

  if (a->setter != NULL) {
    a->setter(a, out, a->setter_arg);
  }

  a->outidx++;
}

/*******************************************************************************

                                Parser Functions

*******************************************************************************/

/**
   @brief Called by the parser when it is in the START state.
   @param a Parser data.
   @param wc Character.
 */
static void json_string_start(struct parser_arg *a, wchar_t wc)
{
  if (wc == L'"') {
    a->state = INSTRING;
  } else {
    a->state = END;
    a->error = JSONERR_UNEXPECTED_TOKEN;
    a->textidx--;
  }
}

/**
   @brief Called by the parser when it is in the INSTRING state.
   @param a Parser data.
   @param wc Character.
 */
static void json_string_instring(struct parser_arg *a, wchar_t wc)
{
  if (wc == L'\\') {
    a->state = ESCAPE;
  } else if (wc == L'"') {
    a->state = END;
  } else if (wc == L'\0') {
    a->state = END;
    a->error = JSONERR_PREMATURE_EOF;
    a->textidx--;
  } else {
    set_output(a, wc);
  }
}

/**
   @brief Called by the parser when it is in the ESCAPE state.
   @param a Parser data.
   @param wc Character.
 */
static void json_string_escape(struct parser_arg *a, wchar_t wc)
{
  if (wc == L'\0') {
    a->state = END;
    a->error = JSONERR_PREMATURE_EOF;
    a->textidx--;
  } else if (wc == L'u') {
    a->state = UESC0;
  } else if (json_isescape(wc)) {
    a->state = INSTRING;
  } else {
    a->state = END;
    a->error = JSONERR_UNEXPECTED_TOKEN;
    a->textidx--;
  }
}

/**
   @brief Called by the parser when it is in one of the UESC states.
   @param a Parser data.
   @param wc Character.
 */
static void json_string_uesc(struct parser_arg *a, wchar_t wc)
{
  if (wc == L'\0') {
    a->state = END;
    a->error = JSONERR_PREMATURE_EOF;
    a->textidx--;
  } else if (json_xdigit(wc) == 0xFF) {
    a->state = END;
    a->error = JSONERR_UNEXPECTED_TOKEN;
    a->textidx--;
  } else {
    a->curr = a->curr << 4;
    a->curr |= json_xdigit(wc);
    if (a->state < UESC3) {
      // continue reading all the input
      a->state += 1;
    } else {
      // time to "publish" our unicode escape
      if (a->prev == 0) {
        // if there was no "prev", that means this might be the start of a
        // surrogate pair.  Check for that!
        if (0xD800 <= wc && wc <= 0xDFFF) {
          // yup, it's a surrogate pair!
          a->prev = a->curr;
        } else {
          // nope, keep going
          set_output(a, a->curr);
        }
      } else {
        // there was a previous starting surrogate
        if (0xD800 <= wc && wc <= 0xDFFF) {
          // and this is also a surrogate
          a->curr &= 0x03FF; // clear upper bits; keep lower 10
          a->curr |= (a->prev & 0x03FF) << 10;
          a->prev = 0;
          set_output(a, a->curr);
        } else {
          // not a legal surrogate to match previous surrogate.  We'll just push
          // the previous code point right along anyway.  May want to error
          // someday.
          set_output(a, a->curr);
        }
      }
      a->state = INSTRING;
      a->curr = 0;
    }
  }
}

/**
   @brief Parses JSON strings, in a very generic manner.
   @param text Input text.
   @param idx Starting index of the string.
   @param setter Function to call with each character.
   @param setarg Argument to give to the setter function.
 */
static struct parser_arg json_string(const wchar_t *text, size_t idx,
                                     output_setter setter, void *setarg)
{
  wchar_t wc;
  struct parser_arg a = {
    .state = START,
    .text = text,
    .textidx = idx,
    .outidx = 0,
    .setter = setter,
    .setter_arg = setarg,
    .prev = 0,
    .curr = 0,
    .error = JSONERR_NO_ERROR
  };

  while (a.state != END) {
    wc = a.text[a.textidx];
    switch (a.state) {
    case START:
      json_string_start(&a, wc);
      break;
    case INSTRING:
      json_string_instring(&a, wc);
      break;
    case ESCAPE:
      json_string_escape(&a, wc);
      break;
    case UESC0:
    case UESC1:
    case UESC2:
    case UESC3:
      json_string_uesc(&a, wc);
      break;
    case END:
      // never happens
      assert(false);
      break;
    }
    a.textidx++;
  }
  return a;
}

/*******************************************************************************

                          Application-Specific Parsers

*******************************************************************************/

/**
   @brief Parse a string literal.
   @param text The text we're parsing.
   @param arr The token buffer.
   @param maxtoken The length of the token buffer.
   @param p The parser state.
   @returns Parser state after parsing the string.
 */
struct json_parser json_parse_string(wchar_t *text, struct json_token *arr,
                                     size_t maxtoken, struct json_parser p)
{
  struct json_token tok;
  struct parser_arg a;

  tok.type = JSON_STRING;
  tok.start = p.textidx;

  a = json_string(text, p.textidx, NULL, NULL);

  tok.end = a.textidx - 1;
  tok.child = 0;
  tok.next = 0;
  json_settoken(arr, tok, p, maxtoken);

  p.error = a.error;
  p.tokenidx++;
  p.textidx = a.textidx;
  return p;
}
