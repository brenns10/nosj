#ifndef SMB_JSON
#define SMB_JSON

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
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
	uint32_t start;
	/**
	 * @brief For tokens that can have a length, this is the length!
	 *
	 * More specifically, this value represents:
	 * - For arrays, the number of elements.
	 * - For objects, the number of key, value pairs.
	 * - For strings, the length of the string in bytes.
	 */
	uint32_t length;
	/**
	 * @brief Index of the next value in the sequence.
	 *
	 * Within lists, "next" refers to the next value in the list.  Within
	 * objects, the "next" attribute of a key refers to the next key in the
	 * object.
	 */
	uint32_t next;
};

/**
 * @brief Errors that could be encountered in JSON parsing.
 */
enum json_error {
	/**
	 * @brief No error!
	 */
	JSON_OK = 0,
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
	 * @brief Missing comma between list or object elements
	 */
	JSONERR_MISSING_COMMA,
	/**
	 * @brief Missing colon between object key and value
	 */
	JSONERR_MISSING_COLON,
	/**
	 * @brief Incorrect type in lookup
	 */
	JSONERR_TYPE,
	/**
	 * @brief An object key was not found
	 */
	JSONERR_LOOKUP,
	/**
	 * @brief An array index was out of bounds
	 */
	JSONERR_INDEX,
	/**
	 * @brief An error occurred parsing "lookup" expression
	 */
	JSONERR_BAD_EXPR,
	/**
	 * @brief The number provided is not an integer
	 */
	JSONERR_NOT_INT,

	_LAST_JSONERR,
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
	uint32_t textidx;
	/**
	 * @brief The index of the next slot to stick a token in our array.
	 *
	 * On return from `json_parse()`, this is the first index of the token
	 * array that was not used.  Equivalently, this is the number of tokens
	 * parsed.
	 */
	uint32_t tokenidx;
	/**
	 * @brief Error code.  This *must* be checked the first time you parse.
	 */
	enum json_error error;
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
struct json_parser json_parse(const char *json, struct json_token *arr,
                              uint32_t n);

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
void json_print(struct json_token *arr, uint32_t n);

/**
 * @brief Print out a parser error message for a parser error.
 * This is like json_strerror() but also includes character number.
 * @param f File to print to.
 * @param p Parser return struct.
 */
void json_print_error(FILE *f, struct json_parser p);

/**
 * @brief Return an error string for parser error
 */
const char *json_strerror(int err);

/**
 * @brief Print out an error message after failing json_lookup().
 *
 * This will output a detailed error message highlighting exactly the point in
 * the expression where we failed, and the particular error which we
 * encountered.
 *
 * @param f File to write to, probably stderr
 * @param expr Same expression passed to json_lookup()
 * @param err Error returned by json_lookup()
 * @param index Value of the out param to json_lookup()
 */
void json_lookup_error(FILE *f, const char *expr, int err, uint32_t index);

/**
 * @brief Return whether or not a string matches a token string.
 * @param json The original JSON buffer.
 * @param tokens The parsed tokens.
 * @param index The index of the string token.
 * @param other The other string to compare to.
 * @param match Result of comparison (true if equal)
 * @returns 0 (JSON_OK) on success, or JSONERR_TYPE if it's not a string
 */
int json_string_match(const char *json, const struct json_token *tokens,
                      uint32_t index, const char *other, bool *match);

/**
 * @brief Load a string into a buffer.
 * @param json The original JSON buffer.
 * @param tokens The parsed tokens.
 * @param index The index of the string token.
 * @param buffer The buffer to load the string into.
 * @returns 0 (NO_ERROR) on success, or JSONERR_TYPE if token is invalid
 *
 * The buffer MUST NOT be null.  It must point to an already allocated buffer,
 * of at least size `tokens[index].length + 1` (room for the text and a NULL
 * character).
 */
int json_string_load(const char *json, const struct json_token *tokens,
                     uint32_t index, char *buffer);

/**
 * @brief Print a string to a file, escaped or not
 * @param json The original JSON buffer.
 * @param tokens The parsed tokens.
 * @param index The index of the string token
 * @param f The file to print to
 * @param escaped Whether to escape the string
 *
 * If escaped is true, then the string is printed so that backslashes, quotes,
 * and non-space whitespace (newline, tab, etc) are escaped, in order to be
 * interpreted as valid JSON. Non ascii characters are printed as-is, in the
 * UTF-8 encoding.
 */
int json_string_print(const char *json, const struct json_token *tokens,
                      uint32_t index, FILE *f, bool escaped);

/**
 * @brief Return the value associated with a key in a JSON object.
 * @param json The original JSON buffer.
 * @param tokens The parsed token buffer.
 * @param index The index of the JSON object.
 * @param key The key you're searching for.
 * @param[out] ret The output index of the value token.
 * @returns 0 (NO_ERROR) on success, or JSONERR_TYPE if token is invalid
 */
int json_object_get(const char *json, const struct json_token *tokens,
                    uint32_t index, const char *key, uint32_t *ret);

/**
 * @brief Return the value at a certain index within a JSON array.
 * @param json The original JSON buffer.
 * @param tokens The parsed token buffer.
 * @param index The index of the array token within the buffer.
 * @param array_index The index to lookup in the JSON array.
 * @param[out] result The resulting index
 * @return the index of the value's token, or 0 if not found.
 * @returns 0 (NO_ERROR) on success, or JSONERR_TYPE if token is invalid
 */
int json_array_get(const char *json, const struct json_token *tokens,
                   uint32_t index, uint32_t array_index, uint32_t *result);

/**
 * @brief Return the value of a JSON number token.
 * @param json The original JSON buffer.
 * @param tokens The parsed token buffer.
 * @param index The index of the number in the token buffer.
 * @param number Number to fill result
 * @returns 0 (NO_ERROR) on success, or JSONERR_TYPE if token is invalid
 */
int json_number_get(const char *json, const struct json_token *tokens,
                    uint32_t index, double *number);

/**
 * @brief Return the value of a JSON number token as an integer
 *
 * This does NOT mean that this function parses the number in a general purpose
 * way and then checks whether it is an integer. Instead, we parse the numeric
 * literal as an integer, and return the result, if successful. There are
 * several valid JSON representations of integers which are NOT valid integer
 * representations according to the standard library: "10.0", "1e1" are two
 * examples of representations of the integer 10. This function would not
 * successfully parse either of these values.
 *
 * Thankfully, any practical JSON implementation will output integers in the
 * expected base-10 format, when they are expected to be interpreted as
 * integers. Therefore, this limitation is not a problem.
 *
 * @param json The original JSON buffer.
 * @param tokens The parsed token buffer.
 * @param index The index of the number in the token buffer.
 * @param number Number to fill result
 * @returns 0 (NO_ERROR) on success, or JSONERR_TYPE if token is invalid
 */
int json_number_getint(const char *json, const struct json_token *tokens,
                       uint32_t index, int64_t *number);

/**
 * @brief Return the value of a JSON number token as an unsigned integer
 *
 * This behaves similar to json_number_getuint(), but returns an unsigned
 * integer. It should be noted that one
 *
 * @param json The original JSON buffer.
 * @param tokens The parsed token buffer.
 * @param index The index of the number in the token buffer.
 * @param number Number to fill result
 * @returns 0 (NO_ERROR) on success, or JSONERR_TYPE if token is invalid
 */
int json_number_getuint(const char *json, const struct json_token *tokens,
                        uint32_t index, uint64_t *number);

/**
 * @brief Lookup values from complex JSON obj/arr using an expression language
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
 * On success, the function returns 0, and the index of the specified JSON
 * element is placed into the variable pointed by @a index. On failure, the
 * function returns an error code, and the string index (within the expression)
 * where we encountered the error is stored into the variable ponited by @a
 * index. (This is unusual, all other NOSJ functions leave their out parameters
 * unmodified on error.) You can use these two items to print an informative
 * error message, see json_lookup_error().
 *
 * @param json The original JSON text buffer
 * @param arr The parsed tokens array
 * @param tok Token which the expression will be evaluated relative to
 * @param key The key, as a JSON-traversing expression
 * @param[out] index The index found by the lookup
 * @returns 0 (JSON_OK) on success, or else an error describing what failed.
 * This could be a syntax error in the expression, or it could be an index or
 * lookup error while walking objects and arrays.
 */
int json_lookup(const char *json, const struct json_token *arr, uint32_t tok,
                const char *key, uint32_t *index);

void json_format(const char *json, const struct json_token *arr, uint32_t len,
                 uint32_t start, FILE *f);

/**
 * @brief Loop through each value in a JSON array, or each key in an object
 *
 * For arrays, the var will contain the index of each value. For objects, the
 * var will contain the index of each key. The index of each value will be at
 * var + 1.
 *
 * Example:
 * @code
 * uint32_t elem;
 * json_array_for_each(elem, tokens, 0) {
 *   printf("Element at index %lu\n", elem);
 * }
 * @endcode
 *
 * @param var A variable (uint32_t) which will contain the index of each token
 * @param tok_arr The JSON token array
 * @param start The index of the JSON array in the token array
 */
#define json_array_for_each(var, tok_arr, start)                               \
	for (var = (start) + 1; var != 0; var = tok_arr[var].next)

#define json_for_each(var, tok_arr, start)                                     \
	json_array_for_each(var, tok_arr, start)

struct json_easy {
	const char *input;
	uint32_t input_len;
	struct json_token *tokens;
	uint32_t tokens_len;
};

#define json_easy_for_each(var, jsonp, start)                                  \
	json_array_for_each(var, ((jsonp)->tokens), (start))

void json_easy_init(struct json_easy *, const char *input);
static inline struct json_easy *json_easy_new(const char *input)
{
	struct json_easy *easy = (struct json_easy *)malloc(sizeof(*easy));
	if (easy)
		json_easy_init(easy, input);
	return easy;
}

void json_easy_destroy(struct json_easy *easy);
static inline void json_easy_free(struct json_easy *easy)
{
	json_easy_destroy(easy);
	free(easy);
}

int json_easy_parse(struct json_easy *easy);

/**
 * @brief Return the string at a given index. Returned pointer must be freed.
 */
int json_easy_string_get(struct json_easy *easy, uint32_t index, char **out);

/* Below are just like their non-easy counterparts */
static inline int json_easy_lookup(struct json_easy *easy, uint32_t tok,
                                   const char *key, uint32_t *result)
{
	return json_lookup(easy->input, easy->tokens, tok, key, result);
}
static inline int json_easy_number_get(struct json_easy *easy, uint32_t index,
                                       double *result)
{
	return json_number_get(easy->input, easy->tokens, index, result);
}
static inline int json_easy_number_getint(struct json_easy *easy,
                                          uint32_t index, int64_t *result)
{
	return json_number_getint(easy->input, easy->tokens, index, result);
}
static inline int json_easy_number_getuint(struct json_easy *easy,
                                           uint32_t index, uint64_t *result)
{
	return json_number_getuint(easy->input, easy->tokens, index, result);
}
static inline int json_easy_string_match(struct json_easy *easy, uint32_t index,
                                         const char *other, bool *result)
{
	return json_string_match(easy->input, easy->tokens, index, other,
	                         result);
}

static inline int json_easy_string_load(struct json_easy *easy, uint32_t index,
                                        char *buffer)
{
	return json_string_load(easy->input, easy->tokens, index, buffer);
}
static inline int json_easy_string_print(struct json_easy *easy, uint32_t index,
                                         FILE *f, bool escaped)
{
	return json_string_print(easy->input, easy->tokens, index, f, escaped);
}
static inline int json_easy_object_get(struct json_easy *easy, uint32_t index,
                                       const char *key, uint32_t *out)
{
	return json_object_get(easy->input, easy->tokens, index, key, out);
}
static inline int json_easy_array_get(struct json_easy *easy, uint32_t index,
                                      uint32_t array_index, uint32_t *result)
{
	return json_array_get(easy->input, easy->tokens, index, array_index,
	                      result);
}
static inline void json_easy_format(struct json_easy *easy, uint32_t start,
                                    FILE *f)
{
	json_format(easy->input, easy->tokens, easy->tokens_len, start, f);
}
#endif // SMB_JSON
