#ifndef SMB_JSON
#define SMB_JSON

#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>

/**
 * @brief Enumeration for all possible types of JSON values.
 *
 * An instance of this enum is included in each `struct json_token`, since each
 * token directly represents a JSON value.
 *
 * The JSON spec lists each of these as their own type.  So, NOSJ internally
 * recognizes them as such.  Note that the boolean values true and false
 * actually are each their own type (as is null).
 */
enum json_type {
	JSON_OBJECT = 0,
	JSON_ARRAY,
	JSON_NUMBER,
	JSON_STRING,
	JSON_TRUE,
	JSON_FALSE,
	JSON_NULL
};

/**
 * @brief Represents a JSON "token".
 *
 * A "token" in NOSJ terminology maps directly to a single JSON value.  This
 * struct contains "metadata" that NOSJ can later use to navigate through the
 * JSON structure or load the value.  These tokens are stored in an array, so
 * their relationships (child, next, etc) are simply other indices in the array.
 * JSON values take on a "tree-like" structure, so tokens are stored in a
 * pre-order traversal of this tree.  That is, you have the parent object,
 * followed by each of its children.
 */
struct json_token {

	/**
	 * @brief Type of the token.
	 */
	enum json_type type;
	/**
	 * @brief Index of the first character of the token in the string.
	 */
	size_t start;
	/**
	 * @brief Index of the last character of the token in the string.
	 */
	size_t end;
	/**
	 * @brief For tokens that can have a length, this is the length!
	 *
	 * More specifically, this value represents:
	 * - For arrays, the number of elements.
	 * - For objects, the number of key, value pairs.
	 * - For strings, the number of Unicode code points.
	 */
	size_t length;
	/**
	 * @brief Index of the first "child" value.
	 *
	 * A "child value" can mean slightly different things in different
	 * situations:
	 * - For an array, the child is the first element of the array.
	 * - For an object, the child is the first key of the object.
	 * - For strings that are keys in an object, the child is the value
	 *   corresponding to that key.
	 */
	size_t child;
	/**
	 * @brief Index of the next value in the sequence.
	 *
	 * Within lists, "next" refers to the next value in the list.  Within
	 * objects, the "next" attribute of a key refers to the next key in the
	 * object.
	 */
	size_t next;
};

/**
 * @brief Errors that could be encountered in JSON parsing.
 */
enum json_error {
	/**
	 * @brief No error!
	 */
	JSONERR_NO_ERROR,
	/**
	 * @brief An error was encountered while parsing a number.
	 */
	JSONERR_INVALID_NUMBER,
	/**
	 * @brief The string ended unexpectedly.
	 */
	JSONERR_PREMATURE_EOF,
	/**
	 * @brief Parser encountered a token that was not expected.
	 */
	JSONERR_UNEXPECTED_TOKEN,
	/**
	 * @brief Parser encountered an invalid surrogate pair.
	 */
	JSONERR_INVALID_SURROGATE,
	/**
	 * @brief Parser did not encounter an expected token.
	 *
	 * This error has an argument (e.g. expected ':').
	 */
	JSONERR_EXPECTED_TOKEN,
};

/**
 * @brief A data structure that contains parser state.
 *
 * This struct is the return value of `json_parse()`.  It is also used
 * internally throughout parsing to represent the current state of the parser.
 */
struct json_parser {
	/**
	 * @brief The index of the next "unhandled" character.
	 *
	 * On return from `json_parse()`, this is the index of the first
	 * character that wasn't parsed.  Or, equivalently, this is the number
	 * of input characters parsed.
	 */
	size_t textidx;
	/**
	 * @brief The index of the next slot to stick a token in our array.
	 *
	 * On return from `json_parse()`, this is the first index of the token
	 * array that was not used.  Equivalently, this is the number of tokens
	 * parsed.
	 */
	size_t tokenidx;
	/**
	 * @brief Error code.  This *must* be checked the first time you parse.
	 */
	enum json_error error;
	/**
	 * @brief Argument to the error code.  Useful for printing error
	 * messages.
	 */
	size_t errorarg;
};

/**
 * @brief Parse JSON into tokens.
 *
 * This function simply tokenizes JSON.  That is, it identifies the location of
 * each JSON entity: objects, arrays, strings, numbers, booleans, and null.  It
 * tokenizes into an already allocated buffer of tokens, so that no memory
 * allocation takes place.  This means that you should pre-allocate a buffer.
 * In order to know what size buffer to allocate, you can call this function
 * with arr=NULL, and it will return the number of tokens it would have parsed
 * as part of the `json_parser` return value (``textidx``).
 *
 * @param json The text buffer to parse.
 * @param arr A buffer to put the tokens in.  May be null.
 * @param n The number of slots in the arr buffer.
 * @returns A parser result.
 */
struct json_parser json_parse(char *json, struct json_token *arr, size_t n);

/**
 * @brief Print a list of JSON tokens.
 *
 * This is mostly for diagnostics.  After you've parsed JSON, you may want to
 * know whether you did it right.  This function prints out the contents of a
 * buffer of JSON tokens so that you can see the parse results.
 *
 * @param arr The array of tokens to print.
 * @param n The number of tokens in the buffer.
 */
void json_print(struct json_token *arr, size_t n);

/**
 * @brief Print out a parser error message for a parser error.
 * @param f File to print to.
 * @param p Parser return struct.
 */
void json_print_error(FILE *f, struct json_parser p);

/**
 * @brief Return whether or not a string matches a token string.
 * @param json The original JSON buffer.
 * @param tokens The parsed tokens.
 * @param index The index of the string token.
 * @param other The other string to compare to.
 * @return True if they are equal, false otherwise.
 */
bool json_string_match(const char *json, const struct json_token *tokens,
                       size_t index, const char *other);

/**
 * @brief Load a string into a buffer.
 * @param json The original JSON buffer.
 * @param tokens The parsed tokens.
 * @param index The index of the string token.
 * @param buffer The buffer to load the string into.
 *
 * The buffer MUST NOT be null.  It must point to an already allocated buffer,
 * of at least size `tokens[index].length + 1` (room for the text and a NULL
 * character).
 */
void json_string_load(const char *json, const struct json_token *tokens,
                      size_t index, char *buffer);

/**
 * @brief Return the value associated with a key in a JSON object.
 * @param json The original JSON buffer.
 * @param tokens The parsed token buffer.
 * @param index The index of the JSON object.
 * @param key The key you're searching for.
 * @return the index of the value token, or 0 if not found.
 */
size_t json_object_get(const char *json, const struct json_token *tokens,
                       size_t index, const char *key);

/**
 * @brief Return the value at a certain index within a JSON array.
 * @param json The original JSON buffer.
 * @param tokens The parsed token buffer.
 * @param index The index of the array token within the buffer.
 * @param array_index The index to lookup in the JSON array.
 * @return the index of the value's token, or 0 if not found.
 */
size_t json_array_get(const char *json, const struct json_token *tokens,
                      size_t index, size_t array_index);

/**
 * @brief Return the value of a JSON number token.
 * @param json The original JSON buffer.
 * @param tokens The parsed token buffer.
 * @param index The index of the number in the token buffer.
 * @returns the value as a double-precision float
 */
double json_number_get(const char *json, const struct json_token *tokens,
                       size_t index);

/**
 * @brief Lookup values from JSON array using an expression language
 *
 * The expression language starts relative to the tok parameter (which may be an
 * object or array). Objects keys are traversed by using a ".keyname". Arrays
 * are indexed via "[NUM]". An example of such an expression might be:
 *
 *    "data.entries[5].name"
 *
 * For the most part, this expression language should be familiar to C-like
 * language users. It should be particularly familiar to the way which a JSON
 * object could be traversed in Javascript itself. One "key" difference (ha) is
 * that there are fewer restrictions on object key names. This function accepts
 * any object key which does not contain one of {'.', '[', '\0'}. Even an end
 * bracket is legal, although not recommended.
 *
 * @param json The original JSON text buffer
 * @param arr The parsed tokens array
 * @param index Token which the expression will be evaluated relative to
 * @param key The key, as a JSON-traversing expression
 * @returns 0 when the item is not found (due to any reason, including
 * expression syntax error, or index/key not found), non-0 on success, which
 * indicates the index of the target value.
 */
size_t json_lookup(const char *json, const struct json_token *arr, size_t tok,
                   const char *key);

/**
 * @brief Loop through each value in a JSON array
 *
 * Example:
 * @code
 * size_t elem;
 * json_array_for_each(elem, tokens, 0) {
 *   printf("Element at index %lu\n", elem);
 * }
 * @endcode
 *
 * @param var A variable (size_t) which will contain the index of each token
 * @param tok_arr The JSON token array
 * @param start The index of the JSON array in the token array
 */
#define json_array_for_each(var, tok_arr, start)                               \
	for (var = tok_arr[start].child; var != 0; var = tok_arr[var].next)

#endif // SMB_JSON
