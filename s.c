#include <string.h>
#include <stdlib.h>
#include "s.h"

static inline size_t nextpow2(size_t);
static inline int ilog2(int);
static inline int ilog10(int);



s s_new(const void *p) {
  s s = { .size = strlen(p) };
  if (s.size+1 > sizeof(s.buf)) {
    s.capacity = nextpow2(s.size+1);
    s.data = malloc(s.capacity);
  }
  else s.data = s.buf;
  memcpy(s.data, p, s.size+1);
  return s;
}

s s_newlen(const void *p, size_t len) {
  s s = { .size = len };
  if (s.size > sizeof(s.buf)) {
    s.capacity = nextpow2(s.size);
    s.data = malloc(s.capacity);
  }
  else s.data = s.buf;
  memcpy(s.data, p, s.size);
  return s;
}

size_t nextpow2(size_t num) {
  // oh gawd it's so portable
  return (size_t)1 << (CHAR_BIT*sizeof(size_t)-__builtin_clz(num));
}

s s_empty(void) {
  s s_null = { .data = s_null.buf, .size = 0 };
  return s_null;
}

s s_dup(s old) { return s_newlen(old.data, old.size); }
void s_free(s s) { if (s.size > sizeof(s.buf)) free(s.data); }

s s_catlen(s s, const void *p, size_t len) {
  if (s.size + len > sizeof(s.buf) || s.size + len > s.capacity) {
    // can't overwrite capacity and can't realloc!
    size_t cap = nextpow2(s.size + len);
    char *p = malloc(cap);
    memcpy(p, s.data, s.size);
    s.capacity = cap;
    s.data = p;
  }
  memcpy(s.data + s.size, p, len);
  s.size += len;
  return s;
}

s s_cat(s s, const void *p) { return s_catlen(s, p, strlen(p)+1); }
s s_cats(s s1, s s2) { return s_catlen(s1, s2.data, s2.size); }

s s_growzero(s s, size_t len) {
  len++; // tell antirez that this is one confusing api
  if (len > sizeof(s.buf) && s.capacity <= len) {
    size_t cap = nextpow2(s.size + len);
    char *p = malloc(cap);
    memcpy(p, s.data, s.size);
    s.capacity = cap;
    s.data = p;
  }
  return s;
}


s sdscatprintf(s s, const char *fmt, ...) {
  // stub
  return s;
}



static inline int ilog2(int n) { return 64 - __builtin_clz(n) - 1; }
static inline int ilog10(int n) {
  // https://graphics.stanford.edu/%7Eseander/bithacks.html#IntegerLog10
  // works in gcc/clang/probably icc
  int temp = (CHAR_BIT*sizeof(int) - __builtin_clz(n | 1)) * 1233 >> 12;
  int pow10[] = {
    0, 10, 100, 1000,
    10000, 100000, 1000000,
    10000000, 100000000, 1000000000,
  };
  return temp - (n < pow10[temp]) + 1;
}


s s_itos(int n) {
  int neg = n < 0;
  if (neg) n = -n;
  s s = { .data = s.buf + ilog10(n) + neg };
  if (neg) s.buf[0] = '-';
  *s.data = 0;

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
    *--s.data = digits[2*pos+1];
    *--s.data = digits[2*pos  ];
  }
  if (n >= 10) {
    *--s.data = digits[2*n+1];
    *--s.data = digits[2*n  ];
  }
  else
    *--s.data = '0' + n;
  s.size = strlen(s.data);
  return s;
}
