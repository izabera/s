#pragma once
#include <stddef.h>

// vector with small buffer optimization

#define v(t, s, name, ...) \
  union { \
    struct { \
      size_t size:54, on_heap:1, capacity:6, flag1:1, flag2:1, flag3:1; \
      t *ptr; \
    }; \
    struct { \
      size_t filler; \
      t buf[(s & (s-1)) == 0 ? s : (size_t)1 << (64 - __builtin_clzl(s))]; /* next power of 2 */ \
    }; \
  } name __attribute__((cleanup(v_free))) = { .buf = { __VA_ARGS__ } }; \
  name.size = sizeof((__typeof__(name.buf[0])[]){ 0, __VA_ARGS__ })/sizeof(name.buf[0]) - 1; \
  name.capacity = sizeof(name.buf)/sizeof(name.buf[0])

#define v_size(v) v.size
#define v_capacity(v) (v.on_heap ? (size_t)1 << v.capacity : sizeof(v.buf)/sizeof(v.buf[0]))

#define v_data(v) (v.on_heap ? v.ptr : v.buf) // always a contiguous buffer

#define v_elemsize(v) sizeof(v.buf[0])
#define v_pos(v,n) v_data(v)[n] // lvalue

__attribute__((nonnull)) void v_free(void*);
__attribute__((nonnull)) void __v_reserve(void*, size_t, size_t, size_t);
#define v_reserve(v,n) __v_reserve(&v,n,v_elemsize(v),v_capacity(v))
__attribute__((nonnull)) void __v_push_back(void * restrict, void * restrict, size_t, size_t);
#define v_push_back(v,e) __v_push_back(&v,&(__typeof__(v.buf[0])[]) { e },v_elemsize(v),v_capacity(v))

#define v_pop_back(v) (void)(v.size -= 1)
