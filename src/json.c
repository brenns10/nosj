/***************************************************************************/ /**

   @file         json.c

   @author       Stephen Brennan

   @date         Created Sunday, 22 November 2015

   @brief        JSON parsing!

   @copyright    Copyright (c) 2015, Stephen Brennan.  Released under the
 Revised BSD License.  See LICENSE.txt for details.

 *******************************************************************************/

#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>

#include "json_private.h"
#include "nosj.h"

// forward declaration of the main parser
static struct json_parser json_parse_rec(const char *text,
                                         struct json_token *arr,
                                         uint32_t maxtoken,
                                         struct json_parser p);

/**
   @brief Return true if c is a whitespace character according to the JSON spec.
 */
static bool json_isspace(char c)
{
	return (c == ' ' || c == '\t' || c == '\r' || c == '\n');
}

/**
   @brief Return true if c could be the beginning of a JSON number.
 */
static bool json_isnumber(char c)
{
	return (c == '-' || ('0' <= c && c <= '9'));
}

/**
   @brief Place a token in the next open slot of arr.

   If arr is null, we do nothing (so that the parser can be called for an
   initial memory estimate).  Otherwise, if we've run past the end of the token
   buffer, return an error.
   @param arr The token buffer.  May be null.
   @param tok The token to add.
   @param p The parser state.
 */
void json_settoken(struct json_token *arr, struct json_token tok,
                   struct json_parser p, uint32_t maxtoken)
{
	if (arr == NULL || p.tokenidx >= maxtoken) {
		return;
	}
	arr[p.tokenidx] = tok;
}

/**
   @brief Set the "next" pointer in a token to be a new value.

   If arr is null, this does nothing.  If we've run past the end of the buffer,
   do nothing.
   @param arr The token buffer.  May be null.
   @param tokidx The index of the token to update.
   @param next New value for next.
 */
static void json_setnext(struct json_token *arr, uint32_t tokidx, size_t next,
                         uint32_t maxtoken)
{
	if (arr == NULL || tokidx >= maxtoken) {
		return;
	}
	struct json_token tok = arr[tokidx];
	tok.next = next;
	arr[tokidx] = tok;
}

/**
   @brief Set the "length" index in a token to be a new value.

   If arr is null, this does nothing.  If we've run past the end of the buffer,
   do nothing.
   @param arr The token buffer.  May be null.
   @param tokidx The index of the token to update.
   @param length New value for end.
 */
static void json_setlength(struct json_token *arr, uint32_t tokidx,
                           size_t length, uint32_t maxtoken)
{
	if (arr == NULL || tokidx >= maxtoken) {
		return;
	}
	struct json_token tok = arr[tokidx];
	tok.length = length;
	arr[tokidx] = tok;
}

/**
   @brief Return the parser state with textidx pointed at the next non-ws char.
   @param text The text we're parsing.
   @param p The current parser state
   @returns The new parser state
 */
static struct json_parser json_skip_whitespace(const char *text,
                                               struct json_parser p)
{
	while (json_isspace(text[p.textidx]) && text[p.textidx] != '\0') {
		p.textidx++;
	}
	return p;
}

/**
   @brief Parse the "true" literal.
   @param text The text we're parsing.
   @param arr The token buffer.
   @param maxtoken The length of the token buffer.
   @param p The parser state.
   @returns Parser state after parsing true.
 */
static struct json_parser json_parse_true(const char *text,
                                          struct json_token *arr,
                                          uint32_t maxtoken,
                                          struct json_parser p)
{
	struct json_token tok;
	tok.type = JSON_TRUE;
	tok.start = p.textidx;
	tok.length = 0;
	tok.next = 0;
	if (strncmp("true", text + p.textidx, 4) == 0) {
		json_settoken(arr, tok, p, maxtoken);
		p.textidx += 4;
		p.tokenidx += 1;
		return p;
	} else {
		p.error = JSONERR_UNEXPECTED_TOKEN;
		return p;
	}
}

/**
   @brief Parse the "false" literal.
   @param text The text we're parsing.
   @param arr The token buffer.
   @param maxtoken The length of the token buffer.
   @param p The parser state.
   @returns Parser state after parsing false.
 */
static struct json_parser json_parse_false(const char *text,
                                           struct json_token *arr,
                                           uint32_t maxtoken,
                                           struct json_parser p)
{
	(void)maxtoken; // unused
	struct json_token tok;
	tok.type = JSON_FALSE;
	tok.start = p.textidx;
	tok.length = 0;
	tok.next = 0;
	if (strncmp("false", text + p.textidx, 5) == 0) {
		json_settoken(arr, tok, p, maxtoken);
		p.textidx += 5;
		p.tokenidx += 1;
		return p;
	} else {
		p.error = JSONERR_UNEXPECTED_TOKEN;
		return p;
	}
}

/**
   @brief Parse the "null" literal.
   @param text The text we're parsing.
   @param arr The token buffer.
   @param maxtoken The length of the token buffer.
   @param p The parser state.
   @returns Parser state after parsing null.
 */
static struct json_parser json_parse_null(const char *text,
                                          struct json_token *arr,
                                          uint32_t maxtoken,
                                          struct json_parser p)
{
	struct json_token tok;
	tok.type = JSON_NULL;
	tok.start = p.textidx;
	tok.length = 0;
	tok.next = 0;
	if (strncmp("null", text + p.textidx, 4) == 0) {
		json_settoken(arr, tok, p, maxtoken);
		p.textidx += 4;
		p.tokenidx += 1;
		return p;
	} else {
		p.error = JSONERR_UNEXPECTED_TOKEN;
		return p;
	}
}

/**
   @brief Parse an array.
   @param text The text we're parsing.
   @param arr The token buffer.
   @param maxtoken The length of the token buffer.
   @param p The parser state.
   @returns Parser state after parsing the array.
 */
static struct json_parser json_parse_array(const char *text,
                                           struct json_token *arr,
                                           uint32_t maxtoken,
                                           struct json_parser p)
{
	uint32_t array_tokenidx = p.tokenidx, prev_tokenidx, curr_tokenidx = 0,
	         length = 0;
	struct json_token tok = {
		.type = JSON_ARRAY,
		.start = p.textidx,
		.length = 0,
		.next = 0,
	};
	json_settoken(arr, tok, p, maxtoken);

	// current char is [, so we need to go past it.
	p.textidx++;
	p.tokenidx++;

	// Skip through whitespace.
	p = json_skip_whitespace(text, p);
	while (text[p.textidx] != ']') {

		if (text[p.textidx] == '\0') {
			p.error = JSONERR_PREMATURE_EOF;
			return p;
		}
		// Parse a value.
		prev_tokenidx = curr_tokenidx;
		curr_tokenidx = p.tokenidx;
		p = json_parse_rec(text, arr, maxtoken, p);
		if (p.error != JSON_OK) {
			return p;
		}

		/* Set the previous token's "next" field to point at this one.
		 */
		if (prev_tokenidx != 0) {
			json_setnext(arr, prev_tokenidx, curr_tokenidx,
			             maxtoken);
		}

		length++;

		// Skip whitespace.
		p = json_skip_whitespace(text, p);
		if (text[p.textidx] == ',') {
			p.textidx++;
			p = json_skip_whitespace(text, p);
		} else if (text[p.textidx] != ']') {
			// If there was no comma, this better be the end of the
			// object.
			p.error = JSONERR_MISSING_COMMA;
			return p;
		}
	}

	// Set the end of the array token to point to the closing bracket, then
	// move it up.
	json_setlength(arr, array_tokenidx, length, maxtoken);
	p.textidx++;
	return p;
}

/**
   @brief Parse an object.
   @param text The text we're parsing.
   @param arr The token buffer.
   @param maxtoken The length of the token buffer.
   @param p The parser state.
   @returns Parser state after parsing the object.
 */
static struct json_parser json_parse_object(const char *text,
                                            struct json_token *arr,
                                            uint32_t maxtoken,
                                            struct json_parser p)
{
	uint32_t object_tokenidx = p.tokenidx, prev_keyidx, curr_keyidx = 0,
	         length = 0;
	struct json_token tok = {
		.type = JSON_OBJECT,
		.start = p.textidx,
		.length = 0,
		.next = 0,
	};
	json_settoken(arr, tok, p, maxtoken);

	// current char is {, so we need to go past it.
	p.textidx++;
	p.tokenidx++;

	// Skip through whitespace.
	p = json_skip_whitespace(text, p);
	while (text[p.textidx] != '}') {
		// Make sure the string didn't end.
		if (text[p.textidx] == '\0') {
			p.error = JSONERR_PREMATURE_EOF;
			return p;
		}

		// Parse a string (key) and value.
		prev_keyidx = curr_keyidx;
		curr_keyidx = p.tokenidx;
		p = json_parse_string(text, arr, maxtoken, p);
		if (p.error != JSON_OK) {
			return p;
		}
		p = json_skip_whitespace(text, p);
		if (text[p.textidx] != ':') {
			p.error = JSONERR_MISSING_COLON;
			return p;
		}
		p.textidx++;
		p = json_parse_rec(text, arr, maxtoken, p);
		if (p.error != JSON_OK) {
			return p;
		}

		/* Set the previous key's "next" field to point to this */
		if (prev_keyidx != 0) {
			// Otherwise set the previous element's next pointer to
			// point to it.
			json_setnext(arr, prev_keyidx, curr_keyidx, maxtoken);
		}

		length++;

		// Skip whitespace.
		p = json_skip_whitespace(text, p);
		if (text[p.textidx] == ',') {
			p.textidx++;
			p = json_skip_whitespace(text, p);
		} else if (text[p.textidx] != '}') {
			// If there was no comma, this better be the end of the
			// object.
			p.error = JSONERR_MISSING_COMMA;
			return p;
		}
	}

	// Set the end of the array token to point to the closing bracket, then
	// move it up.
	json_setlength(arr, object_tokenidx, length, maxtoken);
	p.textidx++;
	return p;
}

char *parse_number_state[] = {
	"START",    "MINUS",          "ZERO",
	"DIGIT",    "DECIMAL",        "DECIMAL_ACCEPT",
	"EXPONENT", "EXPONENT_DIGIT", "EXPONENT_DIGIT_ACCEPT",
	"END"
};

/**
   @brief Parse a string number.
   @param text The text we're parsing.
   @param arr The token buffer.
   @param maxtoken The length of the token buffer.
   @param p The parser state.
   @returns Parser state after parsing the number.
 */
static struct json_parser json_parse_number(const char *text,
                                            struct json_token *arr,
                                            uint32_t maxtoken,
                                            struct json_parser p)
{
	struct json_token tok = { .type = JSON_NUMBER,
		                  .start = p.textidx,
		                  .length = 0, // will become string length
		                  .next = 0 };
	enum state {
		START,
		MINUS,
		ZERO,
		DIGIT,
		DECIMAL,
		DECIMAL_ACCEPT,
		EXPONENT,
		EXPONENT_DIGIT,
		EXPONENT_DIGIT_ACCEPT,
		END
	} state = START;

	/*
	  This function is completely described by this FSM.  States marked by
	  asterisk are accepting.  Unexpected input at accepting states ends the
	  number, and unexpected input at rejecting states causes an error. This
	  state machine is designed to accept any input given by the diagram in
	  the ECMA JSON spec.

	                       -----START-----
	                      /       | (-)   \
	                     /        v        \
	                 (0) | +----MINUS----+ | (1-9)
	                     v v (0)   (1-9) v v
	                  *ZERO*            *DIGIT*--------
	                   |  \ (.)       (.) / |-\ (0-9)  \
	                   |   --->DECIMAL<---              \
	                   |          |                      \
	                   |          v (0-9)  /----\ (0-9)  |
	                   |   *DECIMAL_ACCEPT* ----/        |
	                   |          |                     /
	                   |(e,E)     v (e,E)   (e,E)      /
	                   +-----> EXPONENT <-------------
	                         /        \
	                    (+,-)v        v (0-9)
	            EXPONENT_DIGIT        *EXPONENT_DIGIT_ACCEPT*
	                        \-----------/         \    /(0-9)
	                               (0-9)           \--/
	 */

	// printf("input: %s\n", text + p.textidx);
	while (state != END) {
		char c = text[p.textidx];
		// printf("state: %s\n", parse_number_state[state]);
		switch (state) {
		case START:
			if (c == '0') {
				state = ZERO;
			} else if (c == '-') {
				state = MINUS;
			} else if ('1' <= c && c <= '9') {
				state = DIGIT;
			} else {
				p.error = JSONERR_INVALID_NUMBER;
				state = END; // ERROR
			}
			break;
		case MINUS:
			if (c == '0') {
				state = ZERO;
			} else if ('1' <= c && c <= '9') {
				state = DIGIT;
			} else {
				p.error = JSONERR_INVALID_NUMBER;
				state = END; // ERROR
			}
			break;
		case ZERO:
			if (c == '.') {
				state = DECIMAL;
			} else if (c == 'e' || c == 'E') {
				state = EXPONENT;
			} else {
				state = END;
			}
			break;
		case DIGIT:
			if (c == '.') {
				state = DECIMAL;
			} else if (c == 'e' || c == 'E') {
				state = EXPONENT;
			} else if ('0' <= c && c <= '9') {
				state = DIGIT;
			} else {
				state = END;
			}
			break;
		case DECIMAL:
			if ('0' <= c && c <= '9') {
				state = DECIMAL_ACCEPT;
			} else {
				p.error = JSONERR_INVALID_NUMBER;
				state = END; // ERROR
			}
			break;
		case DECIMAL_ACCEPT:
			if ('0' <= c && c <= '9') {
				state = DECIMAL_ACCEPT;
			} else if (c == 'e' || c == 'E') {
				state = EXPONENT;
			} else {
				state = END;
			}
			break;
		case EXPONENT:
			if (c == '+' || c == '-') {
				state = EXPONENT_DIGIT;
			} else if ('0' <= c && c <= '9') {
				state = EXPONENT_DIGIT_ACCEPT;
			} else {
				p.error = JSONERR_INVALID_NUMBER;
				state = END; // ERROR
			}
			break;
		case EXPONENT_DIGIT:
			if ('0' <= c && c <= '9') {
				state = EXPONENT_DIGIT_ACCEPT;
			} else {
				p.error = JSONERR_INVALID_NUMBER;
				state = END; // ERROR
			}
			break;
		case EXPONENT_DIGIT_ACCEPT:
			if ('0' <= c && c <= '9') {
				state = EXPONENT_DIGIT_ACCEPT;
			} else {
				state = END;
			}
			break;
		case END:
			// never happens
			assert(false);
		}
		p.textidx++;
	}

	p.textidx--; // the character we failed on
	tok.length = p.textidx - tok.start;
	json_settoken(arr, tok, p, maxtoken);
	p.tokenidx++;
	return p;
}

/**
   @brief Parse any JSON value.
   @param text The text we're parsing.
   @param arr The token buffer.
   @param maxtoken The length of the token buffer.
   @param p The parser state.
   @returns Parser state after parsing the value.
 */
static struct json_parser json_parse_rec(const char *text,
                                         struct json_token *arr,
                                         uint32_t maxtoken,
                                         struct json_parser p)
{
	p = json_skip_whitespace(text, p);

	if (text[p.textidx] == '\0') {
		p.error = JSONERR_PREMATURE_EOF;
		return p;
	}

	switch (text[p.textidx]) {
	case '{':
		return json_parse_object(text, arr, maxtoken, p);
	case '[':
		return json_parse_array(text, arr, maxtoken, p);
	case '"':
		return json_parse_string(text, arr, maxtoken, p);
	case 't':
		return json_parse_true(text, arr, maxtoken, p);
	case 'f':
		return json_parse_false(text, arr, maxtoken, p);
	case 'n':
		return json_parse_null(text, arr, maxtoken, p);
	default:
		if (json_isnumber(text[p.textidx])) {
			return json_parse_number(text, arr, maxtoken, p);
		} else {
			p.error = JSONERR_UNEXPECTED_TOKEN;
			return p;
		}
	}
}

char *json_type_str[] = { "object", "array", "number", "string",
	                  "true",   "false", "null" };

char *json_error_str[] = {
	"no error",
	"encountered an invalid numeric literal",
	"string ended prematurely",
	"unexpected token",
	"invalid surrogate pair",
	"expected comma between elements",
	"expected colon between key and value",
	"JSON type does not match expected",
	"the key does not exist in the object",
	"the array index is out of bounds",
	"invalid object lookup syntax",
	"the number provided is not an integer",
};

struct json_parser json_parse(const char *text, struct json_token *arr,
                              uint32_t maxtoken)
{
	struct json_parser parser = { .textidx = 0,
		                      .tokenidx = 0,
		                      .error = JSON_OK };
	return json_parse_rec(text, arr, maxtoken, parser);
}

void json_print(struct json_token *arr, uint32_t n)
{
	uint32_t i;
	for (i = 0; i < n; i++) {
		printf("%03u: "
		       "%6s\t%04u,\tlength=%u,\tnext=%u\n",
		       i, json_type_str[arr[i].type], arr[i].start,
		       arr[i].length, arr[i].next);
	}
}

const char *json_strerror(int err)
{
	if (err < 0)
		err = -err;
	if (err >= _LAST_JSONERR)
		return "Unknown JSON error";
	return json_error_str[err];
}

void json_print_error(FILE *f, struct json_parser p)
{
	fprintf(f, "at character %u: %s\n", p.textidx, json_error_str[p.error]);
}
