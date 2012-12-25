#include "utils.h"
#include <limits.h>
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>

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

void* Mem_alloc(long nbytes, const char *file, int line)
{
  void* ptr;
  assert(nbytes > 0);
  ptr = malloc(nbytes);
  if (ptr == NULL) {
    printf("%s,%d:",file,line);
    ERROR("out of memory");
  }
  return ptr;
}
  
void* Mem_calloc(long count, long nbytes, const char* file, int line)
{
  void* ptr;
  assert(count > 0);
  assert(nbytes > 0);
  ptr = calloc(count, nbytes);
  if (ptr == NULL) {
    printf("%s,%d:",file,line);
    ERROR("out of memory");
  }
  return ptr;
}
    
void* Mem_resize(void* ptr, long nbytes, const char* file, int line)
{
  assert(ptr);
  assert(nbytes > 0);
  ptr = realloc(ptr, nbytes);
  if (ptr == NULL) {
    printf("%s,%d:",file,line);
    ERROR("out of memory");
  }
  return ptr;
}

void  Mem_free(void* ptr, const char* file, int line)
{
  if (ptr)
    free(ptr);
}

} // namespace sscheme

