NOSJ
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
- [ ] Simple tests for parsing values (i.e. accept/reject).
    - [x] Strings
    - [x] Arrays
    - [x] Objects
    - [x] Numbers
    - [ ] True/False/Null
- [ ] Tests for the parsed tokens (i.e. relationships, indices, etc).
- [ ] Loading JSON values into memory for inspection.
    - [ ] Simple values (True/False/Null/Numbers)
    - [ ] Strings, Arrays, Objects
    - [ ] Implement lookup for objects, indexing for arrays.
    - [ ] Minimize memory allocation!
- [ ] API Documentation.

Setup
-----

If you'd like to try it out, follow the commands below:

    $ git clone git@github.com:brenns10/nosj.git
    $ cd nosj
    $ git submodule init
    $ git submodule update
    $ make
    $ emacs test.json # make your own test file
    $ bin/release/main test.json

Above, `test.json` is some input file, like the example below:

```
["a", true, ["b", "c"], "d", false, null,
 {
     "key": "value",
     "key2": "val\"ue2"
 },
 [], {},
 1,
 0,
 -0,
 -0.0,
 -1.57,
 15.75,
 1e-1,
 1e+1,
 1.2e3,
]
```

The driver program should give output like this:

```
amount: 25
tokens: 0x24512f0
000:  array	0000-0161,	child=1,	next=0
001: string	0001-0003,	child=0,	next=2
002:   true	0006-0009,	child=0,	next=3
003:  array	0012-0021,	child=4,	next=6
004: string	0013-0015,	child=0,	next=5
005: string	0018-0020,	child=0,	next=0
006: string	0024-0026,	child=0,	next=7
007:  false	0029-0033,	child=0,	next=8
008:   null	0036-0039,	child=0,	next=9
009: object	0043-0091,	child=10,	next=14
010: string	0050-0054,	child=11,	next=12
011: string	0057-0063,	child=0,	next=0
012: string	0071-0076,	child=13,	next=0
013: string	0079-0088,	child=0,	next=0
014:  array	0095-0096,	child=0,	next=15
015: object	0099-0100,	child=0,	next=16
016: number	0104-0105,	child=0,	next=17
017: number	0108-0109,	child=0,	next=18
018: number	0112-0114,	child=0,	next=19
019: number	0117-0121,	child=0,	next=20
020: number	0124-0129,	child=0,	next=21
021: number	0132-0137,	child=0,	next=22
022: number	0140-0144,	child=0,	next=23
023: number	0147-0151,	child=0,	next=24
024: number	0154-0159,	child=0,	next=0
```

This output describes each token encountered.  The first number of the line is
the index (within the token buffer) of the token.  The next item is the JSON
type of the token.  The next is the index range of the token in the input
string.  The `child` field is the index of the first "child" of this value (for
lists, this is the first element contained; for objects, this is the first
key).  The `next` field is the index of the next item in a container.  Both of
these fields default to zero.

You can also run the tests:

    $ bin/release/test
