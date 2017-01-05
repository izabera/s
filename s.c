#include <string.h>
#include <stdlib.h>
#include "s.h"

/*static inline size_t nextpow2(unsigned);*/
static inline int ilog2(int);
static inline int ilog10(int);


s *s_newlen(s *x, const void *p, size_t len) {
  *x = (s) { 0 };
  if (len > 15) {
    x->capacity = ilog2(len) + 1;
    x->size = len;
    x->is_pointer = 1;
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

s *s_new(s *x, const void *p) {
  *x = s_literal_empty();
  size_t len = strlen(p) + 1;
  if (len > 16) {
    x->capacity = ilog2(len) + 1;
    x->size = len - 1;
    x->is_pointer = 1;
    x->ptr = malloc((size_t)1 << x->capacity);
    memcpy(x->ptr, p, len);
  }
  else {
    memcpy(x->data, p, len);
    x->space_left = 15 - (len-1);
  }
  return x;
}

/*size_t nextpow2(unsigned num) { return (size_t)1 << (32-__builtin_clz(num)); }*/

s *s_cat(s *a, const s *b) {
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

s *s_concat(s *string, const s *prefix, const s *suffix) {
  size_t pres = s_size(prefix),
         sufs = s_size(suffix),
         size = s_size(string),
         capacity = s_capacity(string);

  char *pre = s_data(prefix),
       *suf = s_data(suffix),
       *data = s_data(string);

  if (size + pres + sufs <= capacity) {
    memmove(data + pres, data, size);
    memcpy(data, pre, pres);
    memcpy(data + pres + size, suf, sufs+1);
    string->space_left = 15 - (size + pres + sufs);
  }
  else {
    s tmps = { .space_left = 15 };
    s_grow(&tmps, size + pres + sufs);
    char *tmpdata = s_data(&tmps);
    memcpy(tmpdata + pres, data, size);
    memcpy(tmpdata, pre, pres);
    memcpy(tmpdata + pres + size, suf, sufs+1);
    s_free(string);
    *string = tmps;
    string->size = size + pres + sufs;
  }
  return string;
}

s *s_grow(s *x, size_t len) {
  if (len <= s_capacity(x)) return x;
  len = ilog2(len) + 1;
  if (s_is_pointer(x))
    x->ptr = realloc(x->ptr, (size_t)1 << len);
  else {
    char buf[16];
    memcpy(buf, x->data, 16);
    x->ptr = malloc((size_t)1 << len);
    memcpy(x->ptr, buf, 16);
  }
  x->is_pointer = 1;
  x->capacity = len;
  return x;
}

static inline int ilog2(int n) { return 32 - __builtin_clz(n) - 1; }
static inline int ilog10(int n) {
  const int pow10[] = {
    // note: pow[0] == 0
    0, 10, 100, 1000,
    10000, 100000, 1000000,
    10000000, 100000000, 1000000000,
  };
  // https://graphics.stanford.edu/%7Eseander/bithacks.html#IntegerLog10
  // https://github.com/miloyip/itoa-benchmark/blob/master/src/countdecimaldigit.h
  int temp = (32 - __builtin_clz(n | 1)) * 1233 >> 12;
  return temp - (n < pow10[temp]) + 1;
}

s *s_itos(s *x, int n) {
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

int s_stoi(const s *x) {
  char *ptr = s_data(x);
  int neg = ptr[0] == '-', num = 0, len = s_size(x);
  if (neg) ptr++, len--;

  switch (len) {
    case 10: num += 1000000000 * (ptr[len-10] - '0');
    case  9: num +=  100000000 * (ptr[len- 9] - '0');
    case  8: num +=   10000000 * (ptr[len- 8] - '0');
    case  7: num +=    1000000 * (ptr[len- 7] - '0');
    case  6: num +=     100000 * (ptr[len- 6] - '0');
    case  5: num +=      10000 * (ptr[len- 5] - '0');
    case  4: num +=       1000 * (ptr[len- 4] - '0');
    case  3: num +=        100 * (ptr[len- 3] - '0');
    case  2: num +=         10 * (ptr[len- 2] - '0');
    case  1: num +=          1 * (ptr[len- 1] - '0');
  }
  return neg ? -num : num;
}

s *s_trim(s *x, const char *trimset) {
  if (!trimset[0]) return x;
  char *dataptr = s_data(x), *orig = dataptr;

  // this is similar to strspn/strpbrk but it works on binary data
  unsigned char mask[32] = { 0 };

#define checkbit(byte) (mask[(unsigned char)byte/8] &  1 << (unsigned char)byte % 8)
#define   setbit(byte) (mask[(unsigned char)byte/8] |= 1 << (unsigned char)byte % 8)
  size_t i, slen = s_size(x), trimlen = strlen(trimset);

  for (i = 0; i < trimlen;    i++) setbit(trimset[i]);
  for (i = 0; i <    slen;    i++) if (!checkbit(dataptr[i])) break;
  for (     ;    slen > 0; slen--) if (!checkbit(dataptr[slen-1])) break;
  dataptr += i;
  slen -= i;

  // people reserved space to have a buffer on the heap
  // *don't* free it!  just reuse it, don't shrink to in place if < 16 bytes

  memmove(orig, dataptr, slen);
  // don't dirty memory unless it's needed
  if (orig[slen]) orig[slen] = 0;

  if (s_is_pointer(x)) x->size = slen;
  else x->space_left = 15 - slen;
  return x;
}
