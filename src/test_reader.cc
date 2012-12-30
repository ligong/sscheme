#include <assert.h>

#include "gtest/gtest.h"
#include "reader.h"
#include "memory.h"
#include "token.h"
#include "stdio.h"
#include <fstream>

using namespace sscheme;

TEST(Reader, read) {

  std::ifstream ifs("src/test_reader.txt");
  assert(ifs.good());
  TokenStream ts(&ifs);

  Data x = Read(ts);
  EXPECT_TRUE(x.IsPair());
  EXPECT_TRUE(CAR(x).IsInt());
  EXPECT_EQ(1,CAR(x).Int());

  EXPECT_TRUE(CDR(x).IsPair());
  EXPECT_TRUE(CAADR(x).IsInt());
  EXPECT_EQ(2,CAADR(x).Int());
  EXPECT_TRUE(CADR(CADR(x)).IsInt());
  EXPECT_EQ(3,CADR(CADR(x)).Int());

  x = Read(ts);

  EXPECT_TRUE(FIRST(x).Symbol());
  EXPECT_EQ(Symbol::New("a"),FIRST(x).Symbol());
  EXPECT_EQ(123,NTH(x,2).Int());

  Data y = NTH(x,3);
  EXPECT_TRUE(NTH(y,1).IsFloat());
  EXPECT_TRUE(abs(3.14 - NTH(y,1).Float()) < 0.000001);
  EXPECT_EQ(Symbol::New("pi"),NTH(y,2).Symbol());
  EXPECT_TRUE(NTH(y,3).IsNull());

  EXPECT_TRUE(NTH(x,4).IsSymbol());
  EXPECT_EQ(Symbol::New("d"),NTH(x,4).Symbol());
  
  EXPECT_EQ(Symbol::New("e"),NTH(x,5).Symbol());
  EXPECT_EQ(12,NTH(x,6).Int());
  EXPECT_TRUE(abs(5.6 - NTH(x,7).Float()) < 0.000001);

}


