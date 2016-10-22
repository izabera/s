#include "v.h"
#include <stdlib.h>
#include <string.h>

__attribute__((nonnull))
void v_free(void *p) {
  struct {
    size_t size:54, on_heap:1, capacity:6, flag1:1, flag2:1, flag3:1;
    void *ptr;
  } *s = p;
  if (s->on_heap) free(s->ptr);
}

__attribute__((nonnull))
void __v_reserve(void *vec, size_t n, size_t elemsize, size_t capacity) {
  union {
    struct {
      size_t size:54, on_heap:1, capacity:6, flag1:1, flag2:1, flag3:1;
      void *ptr;
    };
    struct {
      size_t filler;
      char buf[];
    };
  } *v = vec;

  if (n > capacity) {
    if (v->on_heap) {
      v->ptr = realloc(v->ptr, elemsize *
          /* ilog2 */
          (size_t)1 << (v->capacity = 64 - __builtin_clzl(n) - ((n & (n-1)) == 0)));
    }
    else {
      void *tmp = malloc(elemsize *
          (size_t)1 << (v->capacity = 64 - __builtin_clzl(n) - ((n & (n-1)) == 0)));
      memcpy(tmp, v->buf, elemsize * v->size);
      v->ptr = tmp;
      v->on_heap = 1;
    }
  }
}

__attribute__((nonnull))
void __v_push_back(void * restrict vec, void * restrict e, size_t elemsize, size_t capacity) {
  union {
    struct {
      size_t size:54, on_heap:1, capacity:6, flag1:1, flag2:1, flag3:1;
      char *ptr;
    };
    struct {
      size_t filler;
      char buf[];
    };
  } *v = vec;

  if (v->on_heap) {
    if (v->size == capacity) {
      v->ptr = realloc(v->ptr, elemsize * (size_t)1 << ++v->capacity);
    }
    memcpy(&v->ptr[v->size++ * elemsize], e, elemsize);
  }
  else {
    if (v->size == capacity) {
      void* tmp = malloc(elemsize * (size_t)1 << (v->capacity = capacity + 1));
      memcpy(tmp, v->buf, elemsize * v->size);
      v->ptr = tmp;
      v->on_heap = 1;
      memcpy(&v->ptr[v->size++ * elemsize], e, elemsize);
    }
    else memcpy(&v->buf[v->size++ * elemsize], e, elemsize);
  }
}



#ifdef testvector
#include <stdio.h>
int main () {
  v(float, 3, vec1);
  v(int, 2, vec2, 13, 42);

  printf("v_size(vec1):%zu        v_size(vec2):%zu\n", v_size(vec1), v_size(vec2));
  printf("v_pos(vec2,0):%d      v_pos(vec2,1):%d\n", v_pos(vec2,0), v_pos(vec2,1));
  printf("v_capacity(vec1):%zu    v_capacity(vec2):%zu\n", v_capacity(vec1), v_capacity(vec2));
  printf("sizeof(vec1):%zu       sizeof(vec2):%zu\n", sizeof(vec1), sizeof(vec2));

  v_push_back(vec2, 88);
  printf("v_size(vec1):%zu        v_size(vec2):%zu\n", v_size(vec1), v_size(vec2));
  printf("v_elemsize(vec1):%zu    v_elemsize(vec2):%zu\n", v_elemsize(vec1), v_elemsize(vec2));
  v_reserve(vec2, 100);
  printf("v_capacity(vec1):%zu    v_capacity(vec2):%zu\n", v_capacity(vec1), v_capacity(vec2));
  printf("v_pos(vec2,0):%d      v_pos(vec2,1):%d      v_pos(vec2,2):%d\n",
      v_pos(vec2,0), v_pos(vec2,1), v_pos(vec2,2));

#define display(v) for (size_t i = 0; i < v_size(v); i++) printf("%.2f ", v_pos(v, i)); puts(v.on_heap ? "heap" : "stack")
  display(vec1);
  v_push_back(vec1, 0.0); display(vec1);
  v_push_back(vec1, 1.1); display(vec1);
  v_push_back(vec1, 2.2); display(vec1);
  v_push_back(vec1, 3.3); display(vec1);
  v_push_back(vec1, 4.4); display(vec1);
  v_push_back(vec1, 5.5); display(vec1);
  v_push_back(vec1, 6.6); display(vec1);
  v_push_back(vec1, 7.7); display(vec1);
  v_push_back(vec1, 8.8); display(vec1);
  v_push_back(vec1, 9.9); display(vec1);
  v_pop_back(vec1); display(vec1);
  v_pop_back(vec1); display(vec1);
  v_pop_back(vec1); display(vec1);
  v_pop_back(vec1); display(vec1);
  v_pop_back(vec1); display(vec1);
  v_pop_back(vec1); display(vec1);
  v_pop_back(vec1); display(vec1);
  v_pop_back(vec1); display(vec1);
  v_pop_back(vec1); display(vec1);
  v_pop_back(vec1); display(vec1);
}
#endif
