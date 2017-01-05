#pragma once
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

// public api
typedef union {
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
                  is_pointer:1,
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
} s;

#define an0 __attribute__((nonnull))
#define an0si an0 static inline
// query
an0si int s_is_pointer(const s *s) { return s->is_pointer; }
an0si size_t s_size(const s *s) { return s_is_pointer(s) ? s->size : 15 - s->space_left; }
an0si char *s_data(const s *s) { return s_is_pointer(s) ? (char *)s->ptr : (char *)s->data; }
an0si int s_empty(const s *s) { return s_is_pointer(s) ? s->size == 0 : s->space_left == 15; }
an0si size_t s_capacity(const s *s) { return s_is_pointer(s) ? ((size_t)1 << s->capacity) - 1 : 15; }

// manipulation
// all of these return their first argument
an0 s *s_new(s *, const void *);
an0 s *s_newlen(s *, const void *, size_t);
#define s_literal_empty() (s) { .space_left = 15 }
// this leaks if the string is too long but it's very handy for short strings
// "" causes a compile time error if x is not a string literal or too long
// _Static_assert is a declaration, not an expression.  fizzie came up with this hack
#define s_tmp(x) \
  ((void)((struct { _Static_assert(sizeof x <= 16, "it's too big"); int dummy; }){1}), \
   s_new(&s_literal_empty(), "" x))
an0si s *s_newempty(s *x) { *x = s_literal_empty(); return x; }
an0si s *s_dup(s *dest, const s *src) { return s_newlen(dest, s_data(src), s_size(src)); }
an0 s *s_cat(s *, const s *);
an0 s *s_concat(s *, const s *, const s *);
// grow up to that size
an0 s *s_grow(s *, size_t);
// from INT_MIN+1 to INT_MAX
an0 s *s_itos(s *, int);
an0 int s_stoi(const s *);
an0 s *s_trim(s *, const char *);

an0si s *s_free(s *x) { if (s_is_pointer(x)) free(s_data(x)); return s_newempty(x); }

an0si int s_cmp(s *restrict a, s *restrict b) {
  return s_size(a) == s_size(b) ? memcmp(s_data(a), s_data(b), s_size(a)) : s_size(a) > s_size(b);
}

#undef an0si
#undef an0

#define s_autofreed __attribute__((cleanup(s_free)))
