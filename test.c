#include <stdio.h>
#include <string.h>
#include <limits.h>
#include "s.h"

int main() {
  s s1, s2;
  // itos converts ints to strings and they will always stay on the stack
  s_itos(&s1, 12345);
  s_itos(&s2, INT_MIN+1);
  printf("s1: <%s> size: %zu heap: %c  s2: <%s> size: %zu heap: %c\n",
      s_data(&s1), s_size(&s1), s_is_pointer(&s1) ? 'y' : 'n',
      s_data(&s2), s_size(&s2), s_is_pointer(&s2) ? 'y' : 'n');

  // s3 dies at scope exit
  s_autofreed s s3;
  // this string is too long to fit in 15 bytes and is allocated on the heap
  s_newlen(&s3, "hello world hello world", 20);
  printf("s3: <%s> size: %zu capacity: %zu heap: %c\n",
      s_data(&s3), s_size(&s3), s_capacity(&s3), s_is_pointer(&s3) ? 'y' : 'n');

  s_free(&s1);
  s_new(&s1, "0123456789");
  printf("s1: <%s> size: %zu heap: %c\n",
      s_data(&s1), s_size(&s1), s_is_pointer(&s1) ? 'y' : 'n');

  s_cat(&s1, s_small_temporary("foo bar baz bat"));
  printf("s1: <%s> size: %zu heap: %c\n", s_data(&s1), s_size(&s1), s_is_pointer(&s1) ? 'y' : 'n');

  s_free(&s1);
  s_free(&s2);
  s_free(&s2);
  // freeing twice is not an error

  s string;
  string = *s_small_temporary("1234567890");
  printf("%s %zu ->%d\n", s_data(&string), s_size(&string), s_stoi(&string));
  string = *s_small_temporary("123456789");
  printf("%s %zu ->%d\n", s_data(&string), s_size(&string), s_stoi(&string));
  string = *s_small_temporary("12345678");
  printf("%s %zu ->%d\n", s_data(&string), s_size(&string), s_stoi(&string));
  string = *s_small_temporary("1234567");
  printf("%s %zu ->%d\n", s_data(&string), s_size(&string), s_stoi(&string));
  string = *s_small_temporary("123456");
  printf("%s %zu ->%d\n", s_data(&string), s_size(&string), s_stoi(&string));
  string = *s_small_temporary("12345");
  printf("%s %zu ->%d\n", s_data(&string), s_size(&string), s_stoi(&string));
  string = *s_small_temporary("1234");
  printf("%s %zu ->%d\n", s_data(&string), s_size(&string), s_stoi(&string));
  string = *s_small_temporary("123");
  printf("%s %zu ->%d\n", s_data(&string), s_size(&string), s_stoi(&string));
  string = *s_small_temporary("12");
  printf("%s %zu ->%d\n", s_data(&string), s_size(&string), s_stoi(&string));
  string = *s_small_temporary("1");
  printf("%s %zu ->%d\n", s_data(&string), s_size(&string), s_stoi(&string));
  string = *s_small_temporary("-1234567890");
  printf("%s %zu ->%d\n", s_data(&string), s_size(&string), s_stoi(&string));
  string = *s_small_temporary("-123456789");
  printf("%s %zu ->%d\n", s_data(&string), s_size(&string), s_stoi(&string));
  string = *s_small_temporary("-12345678");
  printf("%s %zu ->%d\n", s_data(&string), s_size(&string), s_stoi(&string));
  string = *s_small_temporary("-1234567");
  printf("%s %zu ->%d\n", s_data(&string), s_size(&string), s_stoi(&string));
  string = *s_small_temporary("-123456");
  printf("%s %zu ->%d\n", s_data(&string), s_size(&string), s_stoi(&string));
  string = *s_small_temporary("-12345");
  printf("%s %zu ->%d\n", s_data(&string), s_size(&string), s_stoi(&string));
  string = *s_small_temporary("-1234");
  printf("%s %zu ->%d\n", s_data(&string), s_size(&string), s_stoi(&string));
  string = *s_small_temporary("-123");
  printf("%s %zu ->%d\n", s_data(&string), s_size(&string), s_stoi(&string));
  string = *s_small_temporary("-12");
  printf("%s %zu ->%d\n", s_data(&string), s_size(&string), s_stoi(&string));
  string = *s_small_temporary("-1");
  printf("%s %zu ->%d\n", s_data(&string), s_size(&string), s_stoi(&string));

  string = *s_small_temporary("  foobarbaz  \n\n");
  s_trim(&string, "\n ");
  printf("%zu <%s>\n", s_size(&string), s_data(&string));
  return 0;
}
