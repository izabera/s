#pragma once
#include <stddef.h>
#include <stdlib.h>

// public api
typedef union {
  union {
    // allow strings up to 15 bytes to stay on the stack
    // use the last byte as a null terminator and to store flags
    // much like fbstring
    char data[16];

    struct {
      unsigned char filler1[15],
                    // how many free bytes in this stack allocated string
                    // same idea as fbstring
                    space_left:4,
                    // if it's on heap, this is set to 1
                    is_on_heap:1,
                    flag1:1,
                    flag2:1,
                    flag3:1;
    };

    // heap allocated
    struct {
      char *ptr;
      // supports strings up to 2^54 -1 bytes
      size_t size : 54,
      // capacity is always a power of 2 -1
             capacity : 6;
      // the last 4 bits are important flags
    };
  };
} s;

#define an0 __attribute__((nonnull))
#define an0si an0 static inline
// query
an0si int s_is_on_heap(s* s) { return s->is_on_heap; }
an0si size_t s_size(s* s) { return s_is_on_heap(s) ? s->size : 15 - s->space_left; }
an0si char *s_data(s* s) { return s_is_on_heap(s) ? s->ptr : s->data; }
an0si int s_empty(s* s) { return s_is_on_heap(s) ? s->size == 0 : s->space_left == 15; }
an0si size_t s_capacity(s* s) { return s_is_on_heap(s) ? (size_t)1 << s->capacity - 1 : 15; }

// manipulation
an0 void s_new(s*, const void *);
an0 void s_newlen(s*, const void *, size_t);
an0si void s_newempty(s* x) { *x = (s) { .space_left = 15 }; }
an0si void s_dup(s* dest, s* src) { s_newlen(dest, src->data, src->size); }
an0si void s_free(s* s) { if (s_is_on_heap(s)) free(s_data(s)); }
an0 void s_catlen(s*, const void *, size_t);
an0 void s_cat(s*, const void *);
an0 void s_cats(s*, s*);
an0 void s_growzero(s*, size_t);
an0 void s_catprintf(s*, const char *, ...);
an0 void s_itos(s*, int);
#undef an0si
#undef an0
