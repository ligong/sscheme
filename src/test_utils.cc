#include "src/utils.h"
#include "gtest/gtest.h"

#include <limits.h>
using sscheme::ParseInt;

TEST(utils, ParseInt) {

  long n;
  bool r;

  r = ParseInt("12345",&n);
  EXPECT_TRUE(r);
  EXPECT_EQ(12345,n);

  r = ParseInt("0",&n);
  EXPECT_TRUE(r);
  EXPECT_EQ(0,n);

  r = ParseInt("-0",&n);
  EXPECT_TRUE(r);
  EXPECT_EQ(0,n);

  r = ParseInt("-1",&n);
  EXPECT_TRUE(r);
  EXPECT_EQ(-1,n);
  
  r = ParseInt("-1024",&n);
  EXPECT_TRUE(r);
  EXPECT_EQ(-1024,n);

  r = ParseInt("",&n);
  EXPECT_TRUE(r);
  EXPECT_EQ(0,n);

  if (sizeof(long) == 8) {
    r = ParseInt("9223372036854775807",&n);
    EXPECT_TRUE(r);
    EXPECT_EQ(9223372036854775807,n);
    
    r = ParseInt("9223372036854775808",&n);
    EXPECT_FALSE(r);
    
    r = ParseInt("-9223372036854775808",&n);
    EXPECT_TRUE(r);
    EXPECT_TRUE(LONG_MIN == n);

    r = ParseInt("-9223372036854775809",&n);
    EXPECT_FALSE(r);
  }
  
}
