#ifndef _STREQ_H
#define _STREQ_H
#include <string.h>
#include <stdbool.h>
#include <stddef.h>
static bool streq(const char *a, const char *b) {
  return a && b && strcmp(a, b) == 0;
}
#endif
