#include "src/atom.h"
#include "gtest/gtest.h"

using sscheme::Atom;



TEST(Atom, New) {

  const char* a = Atom::New(56);
  const char* b = Atom::New(56);

  EXPECT_EQ(a,b);
  EXPECT_TRUE(strcmp(a,"56") == 0);

  const char* c = Atom::New(-56);
  EXPECT_TRUE(strcmp(c,"-56") == 0);
  EXPECT_EQ(Atom::Length(c),3);

  const char* d = Atom::New("hello world");
  const char* e = Atom::New("hello world!");
  const char* f = Atom::New("hello world!");
  EXPECT_TRUE(d != e && e == f);

}
