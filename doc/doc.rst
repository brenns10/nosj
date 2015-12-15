NOSJ Documentation
==================

Introduction to JSON
--------------------

NOSJ is a C library for parsing data encoded in the JSON (JavaScript Object
Notation) format.  In case you're not familiar, JSON is a simple format for
saving and transferring structured data.  One common use of JSON is in the
responses of many REST APIs (such as the Twitter API), although there are many
other applications.  Here is an example of some JSON data:

.. code:: json

   {
     "coordinates": null,
     "favorited": false,
     "entities": {
       "urls": [
         {
           "expanded_url": "https://dev.twitter.com/terms/display-guidelines",
           "url": "https://t.co/Ed4omjYs",
           "indices": [
             76,
             97
           ],
           "display_url": "dev.twitter.com/terms/display-\u2026"
         }
       ],
       "hashtags": [
         {
           "text": "Twitterbird",
           "indices": [
             19,
             31
           ]
         }
       ],
       "user_mentions": [
   
       ]
     },
   }

This is a truncated selection from a larger example Twitter API response.  From
it, you can see that JSON has the following data types:

- Null (really a placeholder value that means "nothing").
- Booleans, AKA true or false.
- Numbers, which can be integer or have additional precision.
- Strings, which should be capable of representing any sequence of Unicode code
  points.
- Arrays, which are lists of values surrounded by ``[]`` brackets.
- Objects, which map string keys to values of any type.  These are surrounded by
  ``{}`` braces.

A full description of JSON's syntax is beyond the scope of this document, but
you can find JSON standards at http://json.org .

Motivation
----------

JSON offers very few surprises, especially to those familiar with the data
structures typically provided by high level languages.  These languages
typically have no trouble integrating JSON parsing very simply into their
built-in data structures.  Unfortunately, C doesn't have built-in data
structures to match all of these features.  What's worse, C requires manual
memory management, which means that every single bit of memory allocated for
these data structures needs to be freed when it's no longer used.  This can be
very costly in all of the following ways:

- Memory usage: complex JSON requires allocating space, and potentially a lot of
  it, for data structures.
- Memory allocations: memory allocation is not a simple or efficient task.
  Frequent allocations (one for each JSON value or data structure) can be very
  inefficient.
- Programmer time: memory allocations by a library need to be freed at the
  correct time.  It can be difficult for a programmer to grasp the memory
  management strategies of other libraries, and even worse to try to integrate
  those strategies into their own application's strategy.
- Locality: when many data structures are allocated separately, they end up
  sprinkled throughout the memory space of the application.  Chasing pointers
  through these data structures can result in more frequent page faults.

While there are certainly situations where these inefficiencies are acceptable
or necessary, they probably do not make up the majority of use cases for JSON.
Many times, a programmer only wants a few values out of an API response, not the
whole data structure.  NOSJ is a library that aims to satisfy that use case.  It
doesn't do any memory allocation on its own, leaving those decisions up to the
user.  It only loads values into memory when asked to.

NOSJ Basics
-----------

Getting information from JSON with NOSJ is a two step process.  The first step
is parsing.  You do this with the function :c:func:`json_parse()`.  NOSJ needs
to read through the JSON buffer and determine the structure: namely, what types
of objects are where.  It stores this information in an array of :c:func:`struct
json_token <json_token>`.  These "tokens" are not tokens in the sense of lexical
analysis.  Instead, every token represents a JSON value.  So, the following
input would have 3 tokens:

.. code:: json

    {"a": 3}

The first token would be the object.  The second token is the key "a", and the
third token is the number 3.  As you can see, tokens (i.e. JSON values) can be
nested within one another in a tree structure.  NOSJ stores these tokens in an
array using a `pre-order traversal
<https://en.wikipedia.org/wiki/Tree_traversal#Pre-order>`_.  These tokens don't
actually hold any of the JSON values.  They simply represent the structure of
the JSON document, and they hold the indices of each object in the input string.
For all the other NOSJ functions, you need to have both the input string and the
token array (along with the index of a token in the array) to do things.

When NOSJ parses your input, it only parses one complete JSON value.  This value
will always be at the root (index 0) of the token array.  Typically, this JSON
value is going to be something like an object or a list.  From there, you'll
probably want to get items of a particular index (if it's a list) or with a
certain key (if it's an object).  You can do the following operations on your
parsed JSON:

- Index into a JSON list with :c:func:`json_array_get()`.  This will return to
  you the index of the correct token in the token array.  Note that this has to
  go item by item through the list, so it takes ``O(n)`` time.
- Get the value associated with a key from an object with
  :c:func:`json_object_get()`.  Again, this will return the index of the value in
  the token array.  Also, this does a linear search through the keys of the
  object.  While this would be inefficient in general (if you were constantly
  looking up keys in the object), it is cheaper for a few accesses than it is to
  build a hash table or binary tree.  I may add the ability to create a hash table
  in order to improve lookup times, if this functionality becomes necessary.
- If you have the index of a string, you can use :c:func:`json_string_load()` to
  load it (processing all escape sequences properly) into memory.
- If you have the index of a number, you can use :c:func:`json_number_get()` to
  return its value (as a double).

True, false, and null are all represented as separate :c:func:`types
<json_type>` in NOSJ.  These types are stored as metadata in the token array, so
you don't even need a function to load those values into memory.

With these few functions, you can get any information out of a JSON data
structure, all with minimal memory allocation.

Unicode Support
---------------

The JSON specification indicates that the JSON file format is a "sequence of
Unicode code points".  It doesn't specify a particular encoding.  Therefore, the
most obvious choice I could make when implementing the library was to require
all inputs be internally represented using UCS-4 (that is, each character
represented as a Unicode code point in a ``wchar_t``).  It is the responsibility
of the library user to identify (or know ahead of time) the encoding of the
input, and to decode it into an array of ``wchar_t``.  You may want to use
libraries such as ``iconv`` for this task.

The other way you can do this is to use standard C's built-in functions.  Your
system should have a locale configured, which includes a setting for
``LC_CTYPE``.  Mine is configured to ``en_US.UTF-8``.  If the encoding of your
input matches the system's locale setting, you can usually read it directly
using wide characters (see the ``fgetwc()`` family of functions).  If you have a
buffer of ``char`` encoded using the system's locale setting, you can convert it
using the standard ``mbstowcs()`` function.

If the above paragraphs don't make sense to you, you may need to stop and read
up about Unicode and its various encodings.  I would recommend Joel Spolsky's
`excellent <http://www.joelonsoftware.com/articles/Unicode.html>`_ piece on it,
and this `additional <http://kunststube.net/encoding/>`_ piece seems to go into
slightly more detail (although it has a focus on PHP).

Complete Example
----------------

Here is a complete example of how to use NOSJ to parse a JSON file and load
specific parts of it.

.. code:: C

    #include <stdlib.h>
    #include <stdio.h>
    #include <string.h>
    
    #include "libstephen/str.h" // for read_filew()
    #include "nosj.h"
    
    int main(int argc, char *argv[])
    {
      FILE *f;
      wchar_t *text;
      struct json_token *tokens = NULL;
      struct json_parser p;
      int returncode = 0;
    
      // When no filename specified, or "-" specified, use STDIN.  Else, use the
      // specified filename as input.
      if (argc < 2 || strcmp(argv[1], "-") == 0) {
        f = stdin;
      } else {
        f = fopen(argv[1], "r");
      }
    
      // Read the whole contents of the file.  This uses a libstephen helper
      // function.  http://stephen-brennan.com/libstephen/doc/api/str.html
      text = read_filew(f);
    
      // Parse the first time to get the number of tokens.
      p = json_parse(text, tokens, 0);
      if (p.error != JSONERR_NO_ERROR) {
        json_print_error(stderr, p);
        returncode = 1;
        goto cleanup_text;
      }
    
      // Then allocate and parse to save the tokens.
      tokens = calloc(p.tokenidx, sizeof(struct json_token));
      p = json_parse(text, tokens, p.tokenidx);
    
      // Finally, print the entire token array.
      json_print(tokens, p.tokenidx);
    
      // Now, let's look for the key "text" in the root object.
      if (p.tokenidx > 0 && tokens[0].type == JSON_OBJECT) {
        // We can only do this if there is a root value and it's an object.
        printf("Searching for key \"text\" in the base object.\n");
        size_t value = json_object_get(text, tokens, 0, L"text");
    
        if (value != 0) {
          // Non-zero means we successfully found the key!
          printf("Found key \"text\".\n");
          json_print(tokens + value, 1); // print just that one token
    
          if (tokens[value].type == JSON_STRING) {
            // We're expecting this to be a string.  So, let's load it and print it.
            wchar_t *string = calloc(sizeof(wchar_t), tokens[value].length + 1);
            json_string_load(text, tokens, value, string);
            printf("Value: \"%ls\"\n", string);
            free(string);
          } else {
            printf("Value associated with \"text\" was not a string.\n");
          }
        } else {
          printf("Key \"text\" not found in base object.\n");
        }
      }
    
      free(tokens);
     cleanup_text:
      free(text);
      return returncode;
    }
