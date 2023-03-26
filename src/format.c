/* format.c: human friendly JSON formatter routines */
#include <stdint.h>

#include "json_private.h"

static void print_indent(int amt, FILE *f)
{
	for (int i = 0; i < amt; i++)
		fputs("  ", f);
}

static void push(uint32_t val, uint32_t **stack, uint32_t *len, uint32_t *cap)
{
	if (*len == *cap) {
		*cap *= 2;
		*stack = reallocarray(*stack, *cap, sizeof(val));
	}
	(*stack)[*len] = val;
	*len += 1;
}

void json_format(const char *json, const struct json_token *arr, uint32_t len,
                 uint32_t start, FILE *f)
{
	uint32_t *nest;
	uint32_t nest_len = 0;
	uint32_t nest_cap = 32;

	nest = calloc(nest_cap, sizeof(*nest));

	for (uint32_t ix = start; ix < len;) {
		switch (arr[ix].type) {
		case JSON_NUMBER:
			printf("NUMBER: %u len, %.*s\n", arr[ix].length,
			       arr[ix].length, &json[arr[ix].start]);
			fprintf(f, "%.*s", arr[ix].length,
			        &json[arr[ix].start]);
			break;
		case JSON_TRUE:
			fputs("true", f);
			break;
		case JSON_FALSE:
			fputs("false", f);
			break;
		case JSON_NULL:
			fputs("null", f);
			break;
		case JSON_STRING:
			fputc('"', f);
			json_string_print(json, arr, ix, f, true);
			fputc('"', f);
			break;
		case JSON_ARRAY:
			fputc('[', f);
			if (arr[ix].length) {
				fputc('\n', f);
				push(ix, &nest, &nest_len, &nest_cap);
				print_indent(nest_len, f);
				ix += 1;
				continue;
			} else {
				fputc(']', f);
			}
			break;
		case JSON_OBJECT:
			fputc('{', f);
			if (arr[ix].length) {
				fputc('\n', f);
				push(ix, &nest, &nest_len, &nest_cap);
				ix += 1;
				/* We need to print the first key */
				print_indent(nest_len, f);
				fputc('"', f);
				json_string_print(json, arr, ix, f, true);
				fputs("\": ", f);
				ix += 1;
				continue;
			} else {
				fputc('}', f);
			}
			break;
		}
	up_parent:
		if (nest_len == 0)
			break;
		uint32_t pix = nest[nest_len - 1];
		if (arr[pix].type == JSON_ARRAY) {
			if (arr[ix].next) {
				fputs(",\n", f);
				print_indent(nest_len, f);
				ix = arr[ix].next;
			} else {
				fputs("\n", f);
				print_indent(nest_len - 1, f);
				fputs("]", f);
				nest_len -= 1;
				ix = pix;
				goto up_parent;
			}
		} else if (arr[pix].type == JSON_OBJECT) {
			if (arr[ix - 1].next) {
				fputs(",\n", f);
				print_indent(nest_len, f);
				ix = arr[ix - 1].next;
				fputc('"', f);
				json_string_print(json, arr, ix, f, true);
				fputs("\": ", f);
				ix += 1;
			} else {
				fputc('\n', f);
				print_indent(nest_len - 1, f);
				fputc('}', f);
				nest_len -= 1;
				ix = pix;
				goto up_parent;
			}
		}
	}
	fputc('\n', f);
	free(nest);
}
