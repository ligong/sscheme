#ifndef SSCHEME_SRC_UTILS_H_
#define SSCHEME_SRC_UTILS_H_

#define NELEMS(x) (int(sizeof((x)) / sizeof((x[0]))))

namespace sscheme {

bool ParseInt(const char* str, long* n);

}

#endif
