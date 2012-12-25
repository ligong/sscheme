#ifndef SSCHEME_ATOM_H_
#define SSCHEME_ATOM_H_

#include <string>

namespace sscheme {

class Atom
{
 public:
  static const char* New(const char* bytes, int len);
  static const char* New(const char* str);
  static const char* New(const std::string& str);
  static const char* New(long n);
  static int Length(const char* atom);
};

} // namespace ccii

#endif
