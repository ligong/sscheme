#ifndef SSCHEME_SRC_UTILS_H_
#define SSCHEME_SRC_UTILS_H_

#include <limits.h>

namespace sscheme {

// parse str to int n
// return true on success
// return false if str is too big
// to fit in int

bool ParseInt(const char* str, long* n)
{
  unsigned long m;
  int sign = 1;

  if (*str == '+')
    str++;
  else if (*str == '-') {
    str++;
    sign = -1;
  }

  m = 0;
  for(;*str != '\0';str++) {
    int d = *str - '0';
    if (((sign > 0) && (m > (LONG_MAX - d) / 10UL)) ||
        ((sign < 0) && (m > (LONG_MAX + 1UL - d) / 10UL)))
      return false;
    m = m*10 + d;
  }

  *n = sign >= 0? m:-m;
  return true;
}

} // namespace sscheme
#endif
