#ifndef SSCHEME_SRC_TOKEN_H_
#define SSCHEME_SRC_TOKEN_H_

#include <stdio.h>

#include "utils.h"
#include <fstream>
#include <istream>
#include <iostream>
#include <sstream>

namespace sscheme {

struct Token
{

  enum TYPE {kInt,kFloat,kString,kSymbol,kLParen,kRParen,
             kDot,kNone} type;
  union {
    long i;
    float f;
    char* str;
    const char* sym;
  } data;

  explicit Token(TYPE t): type(t) { }
  Token() { }
  ~Token() {if (type == kString) FREE(data.str);}
  Token& operator=(const Token& that);
};

class TokenStream
{
 public:

  TokenStream(std::ifstream* ifs): ifs_(ifs), is_(ifs_) { }
  TokenStream(std::istream* is): is_(is) { } 
  TokenStream(const char* str): ss_(str), is_(&ss_) { } 
  Token Next();
  
 private:
  std::ifstream* ifs_;
  std::stringstream ss_;
  std::istream* is_;
};

} // namespace sscheme

#endif
