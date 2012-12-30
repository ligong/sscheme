#include "gtest/gtest.h"

#include <stdio.h>

#include "token.h"
#include "symbol.h"
#include <fstream>

using namespace sscheme;

TEST(TokenStream, next) {

  std::ifstream ifs("src/test_token.txt");
  TokenStream ts(&ifs);

  Token t = ts.Next();
  EXPECT_EQ(t.type, Token::kInt);
  EXPECT_EQ(t.data.i,12345);

  t = ts.Next();
  EXPECT_EQ(t.type,Token::kSymbol);
  EXPECT_EQ(t.data.sym,Symbol::New("foo"));

  t = ts.Next();
  EXPECT_EQ(t.type,Token::kString);
  EXPECT_TRUE(strcmp(t.data.str,"test test") == 0); 

  t = ts.Next();
  EXPECT_EQ(t.type,Token::kString);
  EXPECT_TRUE(strcmp(t.data.str,"") == 0);   // test empty string

  t = ts.Next();
  EXPECT_EQ(t.type,Token::kString);
  EXPECT_TRUE(strcmp(t.data.str,"foo\\bar") == 0); // test escape char

  t = ts.Next();
  EXPECT_EQ(t.type,Token::kFloat);
  EXPECT_TRUE(abs(t.data.f - 3.14159) < 0.00001);

  t = ts.Next();
  EXPECT_TRUE(t.type == Token::kNone);

}



