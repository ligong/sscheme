#ifndef SSCHEME_SYMBOL_H_
#define SSCHEME_SYMBOL_H_

#include <string>

namespace sscheme {

// Symbol is represented by const char*.
// It is efficient in storage and compare
class Symbol
{
 public:

  static const char* New(const char* str);
  static const char* New(const std::string& str);
  static const char* New(const char* bytes, int len);

};

} // namespace sscheme

#endif
