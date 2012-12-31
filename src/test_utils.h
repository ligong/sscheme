#ifndef SSCHEME_SRC_TEST_UTILS_H_
#define SSCHEME_SRC_TEST_UTILS_H_

#define EXPECT_SYMEQ(sym_str,x)  {                      \
    EXPECT_TRUE(x.IsSymbol());                          \
    EXPECT_EQ(Symbol::New(sym_str),x.Symbol()); }

#define EXPECT_INTEQ(i,x)  {                    \
    EXPECT_TRUE(x.IsInt());                     \
    EXPECT_EQ(i,x.Int()); }

#define EXPECT_FLOATEQ(f,x) {                   \
    EXPECT_TRUE(x.IsFloat());                   \
    EXPECT_EQ(i,x.Float()); }

#define EXPECT_LISTEQ(list,x) {                 \
    EXPECT_TRUE(x.IsList());                    \
    EXPECT_TRUE(EQUAL(list,x));}                \

#define EXPECT_BOOLFALSE(x)                     \
  EXPECT_TRUE(x == Data::f);

#define EXPECT_BOOLTRUE(x)                      \
  EXPECT_TRUE(x != Data::f);

#define SYM(x) (Memory::NewSymbol(Symbol::New(x)))
#define INT(x) (Memory::NewInt(x))


#endif
