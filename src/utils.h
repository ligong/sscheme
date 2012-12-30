#ifndef SSCHEME_SRC_UTILS_H_
#define SSCHEME_SRC_UTILS_H_

#include "stdio.h"
#include "assert.h"

#define NELEMS(x) (int(sizeof((x)) / sizeof((x[0]))))

namespace sscheme {

bool ParseInt(const char* str, long* n);

#define ERROR(...) ((void)(fprintf(stderr,__VA_ARGS__),\
                           fprintf(stderr,"\n"),\
                           assert(0),0))


void* Mem_alloc(long nbytes, const char *file, int line);
void* Mem_calloc(long count, long nbytes, const char* file, int line);
void* Mem_resize(void* ptr, long nbytes, const char* file, int line);
void  Mem_free(void* ptr, const char* file, int line);


#define ALLOC(nbytes) \
  Mem_alloc((nbytes),__FILE__,__LINE__)

#define CALLOC(count,nbytes) \
  Mem_calloc((count),(nbytes),__FILE__,__LINE__)

#define RESIZE(ptr,nbytes) \
  ((ptr) = static_cast<typeof(ptr)>(Mem_resize((ptr),(nbytes),__FILE__,__LINE__)))

#define NEW(p) ((p) = static_cast<typeof(p)>(ALLOC((long)sizeof(*(p)))))
#define NEW0(p) ((p) = static_cast<typeof(p)(CALLOC(1,(long)sizeof(*(p)))))

#define FREE(ptr) \
  ((void)(Mem_free((ptr), __FILE__,__LINE__),\
          (ptr) = 0))


}

#endif
