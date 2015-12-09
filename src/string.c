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
static wchar_t json_escape(wchar_t c)
{
  switch (c) {
  case L'\"':
  case L'\\':
  case L'/':
    return c;
  case L'b':
    return L'\b';
  case L'f':
    return L'\f';
  case L'n':
    return L'\n';
  case L'r':
    return L'\r';
  case L't':
    return L'\t';
  default:
    return L'\0';
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
    a->state = END;
    a->error = JSONERR_INVALID_SURROGATE;
    return;
  }

  if (a->setter != NULL) {
    a->setter(a, out, a->setter_arg);
  }

  a->outidx++;
}

static void set_state(struct parser_arg *a, enum parser_st state)
{
  if (a->state != END) {
    a->state = state;
  }
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
    set_state(a, INSTRING);
  } else {
    set_state(a, END);
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
    set_state(a, ESCAPE);
  } else if (wc == L'"') {
    set_state(a, END);
  } else if (wc == L'\0') {
    set_state(a, END);
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
  wchar_t esc = json_escape(wc);
  if (wc == L'\0') {
    set_state(a, END);
    a->error = JSONERR_PREMATURE_EOF;
    a->textidx--;
  } else if (wc == L'u') {
    set_state(a, UESC0);
  } else if (esc != L'\0') {
    set_state(a, INSTRING);
    set_output(a, esc);
  } else {
    set_state(a, END);
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
    set_state(a, END);
    a->error = JSONERR_PREMATURE_EOF;
    a->textidx--;
  } else if (json_xdigit(wc) == 0xFF) {
    set_state(a, END);
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
        if (0xD800 <= a->curr && a->curr <= 0xDFFF) {
          // yup, it's a surrogate pair!
          a->prev = a->curr;
        } else {
          // nope, keep going
          set_output(a, a->curr);
        }
      } else {
        // there was a previous starting surrogate
        if (0xD800 <= a->curr && a->curr <= 0xDFFF) {
          // and this is also a surrogate
          a->curr &= 0x03FF; // clear upper bits; keep lower 10
          a->curr |= (a->prev & 0x03FF) << 10;
          a->curr += 0x10000; // apparently this needs to happen (?)
          a->prev = 0;
          set_output(a, a->curr);
        } else {
          // not a legal surrogate to match previous surrogate.
          a->state = END;
          a->error = JSONERR_INVALID_SURROGATE;
        }
      }
      set_state(a, INSTRING);
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
  if (a.prev != 0) {
    a.error = JSONERR_INVALID_SURROGATE;
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
  tok.length = a.outidx;
  json_settoken(arr, tok, p, maxtoken);

  p.error = a.error;
  p.tokenidx++;
  p.textidx = a.textidx;
  return p;
}

/**
   @brief Argument passed to setter when we are doing json_string_match().
 */
struct string_compare_arg {
  /**
     @brief String we're comparing to.
   */
  const wchar_t *other;
  /**
     @brief Whether or not the string has evaluated to equal so far.
   */
  bool equal;
};

/**
   @brief This is the "setter" function for json_string_match().
   @param a Parser arguments.
   @param wc Character to set.
   @param arg The struct string_compare_arg.

   This function just compares each output character to the corresponding
   character in the other string.  It stores the result in the arg, which will
   be examined after the fact.
 */
static void json_string_comparator(struct parser_arg *a, wchar_t wc, void *arg)
{
  struct string_compare_arg *ca = arg;
  // we are depending on short-circuit evaluation here :)
  ca->equal = ca->equal && (wc == ca->other[a->outidx]);
}

bool json_string_match(const wchar_t *json, const struct json_token *tokens,
                       size_t index, const wchar_t *other)
{
  struct string_compare_arg ca = {
    .other = other,
    .equal = true,
  };
  struct parser_arg pa = json_string(json, tokens[index].start,
                                     &json_string_comparator, &ca);

  // They are equal if every previous character matches, and the next character
  // in the other string is the null character, signifying the end.
  return ca.equal && (other[pa.outidx] == L'\0');
}
