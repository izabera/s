s
==

small string optimization in c

features
---

- 16 byte data structure (4 fit in a cache line)
- up to 15 characters + null terminator can be stored in place
- size and capacity are always in place


how it works
---

each `s` is a 16 byte union.
the 16th byte is used as a null terminator and to store some metadata.

longer strings are allocated on the heap.
the union holds a pointer to a malloced area, a size and a capacity.
to fit in 16 bytes, strings are limited to 2^54-1 characters.


how to use
---

add `#include "s.h"` and compile `s.c`

to build the example program, use `gcc-or-clang s.c test.c`

read `s.h` to learn about the api


license
---

isc
