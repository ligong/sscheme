#ifndef SSCHEME_SRC_TOKEN_H_
#define SSCHEME_SRC_TOKEN_H_

#include <stdio.h>

#include "utils.h"


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

  TokenStream(FILE* file): file_(file) { }
  Token Next();
  
 private:
  FILE* file_;
};

} // namespace sscheme

#endif
