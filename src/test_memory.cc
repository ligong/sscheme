#include "gtest/gtest.h"
#include "memory.h"
#include "symbol.h"

using namespace sscheme;

TEST(Memory, Data) {

  EXPECT_TRUE(Data::null.IsNull());

  Data x(Data::kInt,10);
  EXPECT_TRUE(x.IsInt());
  EXPECT_EQ(10,x.Int());

  Data y(Data::kSymbol, Symbol::New("test"));
  EXPECT_TRUE(y.IsSymbol());
  EXPECT_EQ(Symbol::New("test"),y.data.sym);

  Data z(Data::kString,"test");
  EXPECT_TRUE(z.IsString());
  EXPECT_TRUE(strcmp(z.String(),"test") == 0);

  Data f(11.5);
  z = f;
  EXPECT_TRUE(f.IsFloat());
  EXPECT_TRUE(abs(f.data.f - 11.5) < 0.000001);
}

TEST(Memory, Memory) {
  Initialize(10*1024*1024);
    
  Data x = Memory::NewInt(10);
  EXPECT_TRUE(x.IsInt());
  EXPECT_EQ(10,x.Int());

  x = Memory::NewFloat(3.1415);
  EXPECT_TRUE(x.IsFloat());
  EXPECT_TRUE(abs(x.Float() - 3.1415) < 0.000001);
  
  Data y = Memory::NewString("foobar");
  EXPECT_TRUE(y.IsString());
  EXPECT_TRUE(strcmp(y.String(),"foobar") == 0);

  Data z = Memory::NewSymbol(Symbol::New("foo"));
  EXPECT_TRUE(z.IsSymbol());
  EXPECT_EQ(Symbol::New("foo"),z.Symbol());

  Data a = Memory::NewPair(x,y);
  EXPECT_TRUE(a.IsPair());
  Data b = CAR(a);
  EXPECT_TRUE(b.IsFloat());
  EXPECT_TRUE(abs(b.Float() - 3.1415) < 0.000001);
  Data c = CDR(a);
  EXPECT_TRUE(y.IsString());
  EXPECT_TRUE(strcmp(y.String(),"foobar") == 0);

  SETCAR(a,c);
  SETCDR(a,b);
  EXPECT_TRUE(a.IsPair());
  b = CDR(a);
  EXPECT_TRUE(b.IsFloat());
  EXPECT_TRUE(abs(b.Float() - 3.1415) < 0.000001);
  c = CAR(a);
  EXPECT_TRUE(y.IsString());
  EXPECT_TRUE(strcmp(y.String(),"foobar") == 0);



}

TEST(Machine, test_list)
{
  int v[] = {0,1,2,3,4,5,6,7,8,9,10,11};
  Data list = Data::null;
  for(int i = 0; i < NELEMS(v); i++)
    list = Memory::NewPair(Memory::NewInt(v[i]),list);
  for(int i = 0; i < NELEMS(v); i++)
    EXPECT_EQ(v[i], NTH(list,NELEMS(v) - i).Int());

  list = Memory::ReverseList(list);

  for(int i = 0; i < NELEMS(v); i++)
    EXPECT_EQ(v[i], NTH(list,i+1).Int());

#define I(x) Memory::NewInt(x)
  list = LIST(I(0),I(1),I(2),I(3),I(4));
  for(int i = 0; i < 5; i++)
    EXPECT_EQ(i, NTH(list,i+1).Int());
}

TEST(Machine, test_stack)
{
  int v[] = {0,1,2,3,4,5,6,7,8,9,10,11};
  for(int i = 0; i < NELEMS(v); i++) {
    Data x = Memory::NewInt(v[i]);
    g_machine.Push(x);
  }
  for(int i = 0; i < NELEMS(v); i++) {
    Data x = g_machine.Pop();
    EXPECT_EQ(v[NELEMS(v)-i-1],x.Int());
  }
    
}

TEST(Memory, test_equal) {

#define I(x) Memory::NewInt(x)
#define STR(x) Memory::NewString(x)  
  EXPECT_TRUE(EQUAL(I(1),I(1)));
  EXPECT_TRUE(EQUAL(STR("foo"),STR("foo")));
  EXPECT_TRUE(EQUAL(LIST(I(1),LIST(I(2))),
                    LIST(I(1),LIST(I(2)))));
  EXPECT_TRUE(!EQUAL(LIST(I(1),LIST(I(2))),
                     LIST(I(1),LIST(I(3)))));

}
