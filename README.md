NOSJ
====

NOSJ is a library for parsing JSON in C.  It's in pretty heavy development for
the core features.  Right now the tokenizing works, but it doesn't parse in the
actual values of the JSON structures.  If you'd like to try it out, you can
build it and run:

    $ git clone git@github.com:brenns10/nosj.git
    $ cd nosj
    $ git submodule init
    $ git submodule update
    $ make
    $ emacs test.json # make your own test file
    $ bin/release/main test.json

For an example input like this:

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

You might get an output like this:

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
