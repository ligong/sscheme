#ifndef SSCHEME_SYMBOL_H_
#define SSCHEME_SYMBOL_H_

#include <string>

namespace sscheme {

class Symbol
{
 public:

  
  static const char* New(const char* str);
  static const char* New(const std::string& str);
  static const char* New(const char* bytes, int len);

};

} // namespace sscheme

#endif
