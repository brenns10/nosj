# CHANGELOG

## v2.2.1 -- 2022-05-25

- Fix getuint

## v2.2.0 -- 2022-05-23

- Add `json_{easy_,}number_get{u,}int` functions for accessing numbers as
  integers.

## v2.1.1 -- 2022-03-25

- Remove forgotten print statement.

## v2.1.0 -- 2022-03-25

- The macro `json_easy_for_each()` has been fixed.
- Added `json_string_print()` function to print strings escaped, or not.
- Added `json_format()` function to format JSON nicely.

## v2.0.0 -- 2022-11-15

Multiple Major API Changes:

Accessors and lookups now do type checking and may return errors!

- To achieve this, every `json_*_get()` function now returns an int (actually,
  an `enum json_error` but we cast is as an int for easier typing). The actual
  return value is now an out parameter.
- We've added several error classes to deal with lookup failures, indexes out of
  range, type errors, and bad lookup expressions.
- Got rid of `json_easy_strerror()` in favor of one unified one.

Compressed the JSON token from 48 bytes (!) down to 16. We did this by:

- Removing the end and child fields: they had no practical purpose.
- Making the integer fields 4-byte (uint32_t) - parsing anything over 4GiB is
  out of scope for this library. Honestly, it seems like it would be possible,
  but you'd need to manually chunk it up. In any case, there's no practical use
  case for 64-bit indexing.

## v1.1.0, v1.1.1 -- 2022-11-14

Add `json_easy` API: provisional.

## v1.0.0 -- 2021-09-03

First stable release.

- Unicode parsing is now correct, and the API uses char rather than wchar_t
- Add `json_array_for_each()` macro
- Updated documentation

## v0.1.0 -- 2021-06-03

Initial versioned release.

- Switch to meson, use Unity for testing
- Add `json_lookup()` for expression language lookup

## v0.0.1 -- Prehistory

No history before this except in git. No versions.
