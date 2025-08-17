#ifndef _MSG_H
#define _MSG_H
#include <stddef.h>
#define errmsg(fmt, ...) \
  fprintf(stderr, /*"\033[31m"*/ fmt /*"\033[0m"*/, ##__VA_ARGS__);
#endif
