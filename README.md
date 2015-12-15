NOSJ ![Build Status](https://travis-ci.org/brenns10/nosj.svg?branch=master)
====

*NOSJ is a library for parsing JSON in C.*

Motivation
----------

While JSON parsing comes naturally in many languages, C is not one of them.  C
lacks the built in data structures and garbage collection that makes parsing
JSON so simple in other languages.  Some C JSON implementations try to solve
this problem by implementing data structures and garbage collection for you.
This is problematic because most of the time, you don't want all that extra
machinery -- you just want to get some data out of an object.

Other JSON implementations swing too far in the other direction: they parse out
the structure of the JSON, but don't give you any information beyond the
locations of these structures in the string.  While this is simple and avoids
most of the memory management issues, it leaves the library user with only a
slightly smaller problem.  You can't look up a key in a JSON object when all you
have is a list of tokens.

This library aims to hit a sweet spot in between these two extremes.  Here are
some goals for the library:

- Keep things simple - only parse into memory the values the user needs.
- Traditional C design - leave the decisions of memory allocation up to the
  library user
    - For instance, check out the `json_parse()` function.  It takes an input
      `char *` buffer, an output token buffer, and the size of the output
      buffer.  If you pass `NULL` for the output buffer, it procedes as though
      it had a buffer and returns the number of tokens it encountered, so you
      can allocate the right size output buffer.  This follows the design of
      several C standard functions like `wcstombs()`.
- Good testing and documentation (otherwise, why make a library?).
- Good error handling!

Status
------

- [x] Implement parsing JSON input into tokens.
- [x] Simple tests for parsing values (i.e. accept/reject).
    - [x] Strings
    - [x] Arrays
    - [x] Objects
    - [x] Numbers
    - [x] True/False/Null
- [x] Tests for the parsed tokens (i.e. relationships, indices, etc).
- [ ] Loading JSON values into memory for inspection.
    - [x] Simple values (True/False/Null/Numbers)
    - [x] Loading strings
    - [ ] Loading arrays
    - [ ] Loading objects
    - [x] Lookup for objects 
    - [x] Indexing for arrays
- [x] API Documentation.

Setup
-----

If you'd like to try it out, follow the commands below:

    $ git clone git@github.com:brenns10/nosj.git
    $ cd nosj
    $ git submodule init
    $ git submodule update
    $ make
    $ bin/release/main twitapi.json

Above, `twitapi.json` is an JSON
[example](https://dev.twitter.com/rest/reference/get/statuses/show/%3Aid) from
the Twitter API Documentation.  It contains a deeply nested structure with all
sorts of data types and stuff.  The main driver program parses it and prints out
every single "token" from it.  Then, it queries the root object for the key
"text", which does exist in that particular example.  Finally, it prints out the
token and loads the corresponding string, and prints that.  Here is some example
output:

```
$ bin/release/main twitapi.json
000: object	0000-3065,	length=21,	child=1,	next=0
001: string	0004-0016,	length=11,	child=2,	next=3
002:   null	0019-0022,	length=0,	child=0,	next=0
003: string	0027-0037,	length=9,	child=4,	next=5
004:  false	0040-0044,	length=0,	child=0,	next=0
005: string	0049-0059,	length=9,	child=6,	next=7
...  ...  ...  ...  ...  ...  ...  ...  ...  ...  ...
162:   null	3060-3063,	length=0,	child=0,	next=0
Searching for key "text" in the base object.
Found key "text".
000: string	0692-0795,	length=102,	child=0,	next=0
Value: "Along with our new #Twitterbird, we've also updated our Display Guidelines: https://t.co/Ed4omjYs  ^JC"
```

You can also run the tests:

    $ make test
