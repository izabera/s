#include <string.h>
#include <stdlib.h>
#include "s.h"

static inline size_t nextpow2(unsigned);
static inline int ilog2(int);
static inline int ilog10(int);


s* s_newlen(s *x, const void *p, size_t len) {
  *x = (s) { 0 };
  if (len > 15) {
    x->capacity = ilog2(len) + 1;
    x->size = len;
    x->is_on_heap = 1;
    x->ptr = malloc((size_t)1 << x->capacity);
    memcpy(x->ptr, p, len);
    x->ptr[len] = 0;
  }
  else {
    memcpy(x->data, p, len);
    x->space_left = 15 - len;
  }
  return x;
}

s* s_new(s *x, const void *p) {
  *x = s_literal_empty();
  size_t len = strlen(p) + 1;
  if (len > 16) {
    x->capacity = ilog2(len) + 1;
    x->size = len - 1;
    x->is_on_heap = 1;
    x->ptr = malloc((size_t)1 << x->capacity);
    memcpy(x->ptr, p, len);
  }
  else {
    memcpy(x->data, p, len);
    x->space_left = 15 - (len-1);
  }
  return x;
}

size_t nextpow2(unsigned num) { return (size_t)1 << (32-__builtin_clz(num)); }

s* s_cat(s* a, const s* b) {
  size_t sizea = s_size(a), sizeb = s_size(b);
  if (sizea + sizeb > 15) {
    if (s_capacity(a) < sizea + sizeb + 1)
      s_grow(a, sizea + sizeb + 1);
    memcpy(a->ptr + sizea, s_data(b), sizeb+1);
    a->size = sizea + sizeb;
    a->ptr[a->size] = 0;
  }
  else {
    memcpy(a->data + sizea, b->data, sizeb+1);
    a->space_left = 15 - (sizea + sizeb);
  }
  return a;
}

s* s_grow(s* x, size_t len) {
  if (len <= s_capacity(x)) return x;
  len = ilog2(len) + 1;
  if (s_is_on_heap(x))
    x->ptr = realloc(x->ptr, (size_t)1 << len);
  else {
    char buf[16];
    memcpy(buf, x->data, 16);
    x->ptr = malloc((size_t)1 << len);
    memcpy(x->ptr, buf, 16);
  }
  x->is_on_heap = 1;
  x->capacity = len;
  return x;
}

static inline int ilog2(int n) { return 32 - __builtin_clz(n) - 1; }
static inline int ilog10(int n) {
  // https://graphics.stanford.edu/%7Eseander/bithacks.html#IntegerLog10
  // works in gcc/clang/probably icc
  int temp = (32 - __builtin_clz(n | 1)) * 1233 >> 12;
  int pow10[] = {
    0, 10, 100, 1000,
    10000, 100000, 1000000,
    10000000, 100000000, 1000000000,
  };
  return temp - (n < pow10[temp]) + 1;
}

s* s_itos(s *x, int n) {
  int neg = n < 0;
  if (neg) n = -n;
  *x = (s) { 0 };
  int len = ilog10(n) + neg;
  char *ptr = x->data + len;
  if (neg) x->data[0] = '-';

  const char *digits =
    "00" "01" "02" "03" "04" "05" "06" "07" "08" "09"
    "10" "11" "12" "13" "14" "15" "16" "17" "18" "19"
    "20" "21" "22" "23" "24" "25" "26" "27" "28" "29"
    "30" "31" "32" "33" "34" "35" "36" "37" "38" "39"
    "40" "41" "42" "43" "44" "45" "46" "47" "48" "49"
    "50" "51" "52" "53" "54" "55" "56" "57" "58" "59"
    "60" "61" "62" "63" "64" "65" "66" "67" "68" "69"
    "70" "71" "72" "73" "74" "75" "76" "77" "78" "79"
    "80" "81" "82" "83" "84" "85" "86" "87" "88" "89"
    "90" "91" "92" "93" "94" "95" "96" "97" "98" "99";

  while (n >= 100) {
    int pos = n % 100;
    n /= 100;
    *--ptr = digits[2*pos+1];
    *--ptr = digits[2*pos  ];
  }
  if (n >= 10) {
    *--ptr = digits[2*n+1];
    *--ptr = digits[2*n  ];
  }
  else
    *--ptr = '0' + n;
  x->space_left = 15 - len;
  return x;
}
