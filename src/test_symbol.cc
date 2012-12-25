#include "src/symbol.h"
#include "gtest/gtest.h"
#include <string>

using sscheme::Symbol;

TEST(Symbol, New) {

  const char* d = Symbol::New("hello world");
  const char* e = Symbol::New("hello world!");
  const char* f = Symbol::New("hello world!");
  const char* g = Symbol::New(std::string("hello world!"));
  EXPECT_TRUE(d != e && e == f && e == g);

}
