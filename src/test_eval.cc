#include "gtest/gtest.h"
#include <string.h>
#include <fstream>

#include "memory.h"
#include "symbol.h"
#include "eval.h"
#include "reader.h"
#include "token.h"

#include "printer.h"

using namespace sscheme;


class EvalTest : public ::testing::Test {
 protected:
  virtual void SetUp() {
    sscheme::Initialize(5*1024*1024);
    sscheme::InitialEnvironment();
  }
  // virtual void TearDown() {}
};


TEST_F(EvalTest, test_atom) {
  
  Data& exp = g_machine.exp;

  // number
  exp = Read("1");
  Data result = Eval();
  EXPECT_TRUE(result.IsInt());
  EXPECT_EQ(1,result.Int());

  // string
  exp = Read("\"hello scheme\"");
  result = Eval();
  EXPECT_TRUE(result.IsString());
  EXPECT_STREQ("hello scheme",result.String());

  // symbol
  exp = Read("+");
  result = Eval();
  EXPECT_TRUE(result.IsPrimApp());

  // empty test
  exp = Read("()");
  result = Eval();
  EXPECT_EQ(Data::null, result);

}

#define EXPECT_SYMEQ(sym_str,x)  {                      \
    EXPECT_TRUE(x.IsSymbol());                          \
    EXPECT_EQ(Symbol::New(sym_str),x.Symbol()); }

#define EXPECT_INTEQ(i,x)  {                    \
    EXPECT_TRUE(x.IsInt());                     \
    EXPECT_EQ(i,x.Int()); }

#define EXPECT_FLOATEQ(f,x)   {                 \
    EXPECT_TRUE(x.IsFloat());                   \
    EXPECT_EQ(i,x.Float()); }

#define EXPECT_LISTEQ(list,x)   {               \
    EXPECT_TRUE(x.IsList());                    \
    EXPECT_TRUE(EQUAL(list,x)); }



#define SYM(x) (Memory::NewSymbol(Symbol::New(x)))

TEST_F(EvalTest, test_quote)
{
  Data& exp = g_machine.exp;

  // quote symbol
  exp = Read("(quote foo)");
  Data result = Eval();
  EXPECT_SYMEQ("foo",result);

  // quote number
  exp = Read("(quote 1)");
  result = Eval();
  EXPECT_INTEQ(1,result);
  
  // embeded list
  exp = Read("(quote (foo (bar)))");
  result = Eval();
  EXPECT_LISTEQ(result,
                LIST(SYM("foo"),LIST(SYM("bar"))));
  
}

TEST_F(EvalTest, test_define)
{
  Data& exp = g_machine.exp;

  exp = Read("(define foo 5)");
  Data value = Eval();
  EXPECT_INTEQ(5,value);

  exp = Read("foo");
  value = Eval();
  EXPECT_INTEQ(5,value);
}

TEST_F(EvalTest, test_assign)
{
  Data& exp = g_machine.exp;

  exp = Read("(define foo 5)");
  Data value = Eval();
  EXPECT_INTEQ(5,value);
  exp = Read("(set! foo 6)");
  Eval();
  exp = Read("foo");
  value = Eval();
  EXPECT_INTEQ(6,value);

  exp = Read("(set! foo (quote bar))");
  Eval();
  exp = Read("foo");
  value = Eval();
  EXPECT_SYMEQ("bar",value);
}

TEST_F(EvalTest, test_if)
{
  Data value;

  value = Eval("(if (quote foo) 3 5)");
  EXPECT_INTEQ(3,value);

  value = Eval("(if () 3 5)");
  EXPECT_INTEQ(5,value);
  
}


TEST_F(EvalTest, test_lambda)
{
  Data value;

  // simple function application
  value = Eval("((lambda (x y) (+ x y)) 2 3)");
  EXPECT_INTEQ(5,value);

  // define function
  value = Eval("(define (add x y) (+ x y))");
  value = Eval("(add 2 3)");
  EXPECT_INTEQ(5,value);
  
  // recursive fib
  value = Eval("(define (fib n) \
                   (if (= n 0) \
                       0 \
                       (if (= n 1) \
                        1 \
                        (+ (fib (- n 1)) (fib (- n 2))))))");
  value = Eval("(fib 6)");
  EXPECT_INTEQ(8,value);

  // nested define
  // iter fib
  value = Eval("(define (fib n) \
                   (define (iter a b k) \
                      (if (= k n)  \
                          a  \
                          (iter b (+ a b) (+ 1 k)))) \
                   (iter 0 1 0))");
  Print(SECOND(value));
  value = Eval("(fib 0)");
  EXPECT_INTEQ(8,value);
    
}
  




