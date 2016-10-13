#include <limits.h>

// public api
typedef struct {
  union {
    char *data;
    char *c_str;
  };
  size_t size;
  union {
    size_t capacity;
    char buf[16];
  };
} s;

static inline size_t s_size(s s) { return s.size; }
static inline char *s_data(s s) { return s.data; }
static inline char *s_c_str(s s) { return s.c_str; }
s s_new(const void *);
s s_newlen(const void *, size_t);
s s_empty(void);
s s_dup(s);
void s_free(s);
s s_catlen(s, const void *, size_t);
s s_cat(s, const void *);
s s_cats(s, s);
s s_growzero(s, size_t);
s s_catprintf(s, const char *, ...);
s s_itos(int);
