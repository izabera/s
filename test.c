#include <stdio.h>
#include <string.h>
#include "s.h"

int main() {
  s s1, s2;
  s_itos(&s1, 12345);
  s_itos(&s2, -12345);
  printf("<%p> <%p>\n", s_data(&s1), s_data(&s2));
  printf("<%s> <%s>\n", s_data(&s1), s_data(&s2));
  printf("<%zu> <%zu>\n", s_size(&s1), s_size(&s2));
  s_newlen(&s1, "hello world hello world", 20);
  printf("<%s> <%zu> <%zu>\n", s_data(&s1), s_size(&s1), s_capacity(&s1));
  s_free(&s1);
  s_new(&s1, "foo bar baz");
  printf("%s\n", s_data(&s1));
  s_free(&s1);
  s_new(&s1, "foo bar baz bat 1234567890");
  printf("%s\n", s_data(&s1));
  s_free(&s1);
}
