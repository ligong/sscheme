#include "gtest/gtest.h"
#include <string.h>
#include <fstream>
#include <string>
#include <stdlib.h>

#include "memory.h"
#include "symbol.h"
#include "eval.h"
#include "reader.h"
#include "token.h"
#include "printer.h"
#include "test_utils.h"

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

  int fib[] = {0,1,1,2,3,5,8,13,21};
  for(int i = 0; i < NELEMS(fib); i++) {
    char buf[100];
    snprintf(buf,sizeof(buf),"(fib %d)",i);
    value = Eval(buf);
    EXPECT_INTEQ(fib[i],value);
  }

  // factorial
  Eval("(define (factorial n) \
          (if (= n 1) 1  (* n (factorial (- n 1)))))");
  value = Eval("(factorial 6)");
  EXPECT_INTEQ(720,value);

  // closure
  // adder
  Eval("(define (adder n) \
                   (lambda (x) (+ n x)))");
  Eval("(define add5 (adder 5))");
  value = Eval("(add5 3)");
  EXPECT_INTEQ(8,value);

  // balance
  Eval("(define (make-account account)                  \
          (define (withdraw n)                          \
            (if (>= account n)                          \
	       (set! account (- account n)))            \
               account)                                 \
          (define (deposit n)                           \
            (set! account (+ account n))                \
             account)                                   \
         (lambda (msg)                                  \
           (cond ((eq? msg (quote withdraw)) withdraw)  \
   	     ((eq? msg (quote deposit)) deposit)        \
	     ((eq? msg (quote account)) account))))");

  Eval("(define acc (make-account 10))");
  value = Eval("((acc (quote withdraw)) 1)");
  EXPECT_INTEQ(9,value);
  value = Eval("((acc (quote deposit)) 20)");
  EXPECT_INTEQ(29,value);
  value = Eval("(acc (quote account))");
  EXPECT_INTEQ(29,value);
  value = Eval("((acc (quote withdraw)) 30)");
  EXPECT_INTEQ(29,value);
  value = Eval("((acc (quote withdraw)) 29)");
  EXPECT_INTEQ(0,value);
}

TEST_F(EvalTest, test_primitive_application)
{
  Data value;
  
  // <
  value = Eval("(< 2 3)");
  EXPECT_BOOLTRUE(value);
  value = Eval("(< 2 2)");
  EXPECT_BOOLFALSE(value);
  value = Eval("(< 3 2)");
  EXPECT_BOOLFALSE(value);

  // <=
  value = Eval("(<= 2 3)");
  EXPECT_BOOLTRUE(value);
  value = Eval("(<= 2 2)");
  EXPECT_BOOLTRUE(value);
  value = Eval("(<= 3 2)");
  EXPECT_BOOLFALSE(value);

  // >
  value = Eval("(> 2 3)");
  EXPECT_BOOLFALSE(value);
  value = Eval("(> 2 2)");
  EXPECT_BOOLFALSE(value);
  value = Eval("(> 3 2)");
  EXPECT_BOOLTRUE(value);

  // >=
  value = Eval("(>= 2 3)");
  EXPECT_BOOLFALSE(value);
  value = Eval("(>= 2 2)");
  EXPECT_BOOLTRUE(value);
  value = Eval("(>= 3 2)");
  EXPECT_BOOLTRUE(value);

  // not
  value = Eval("(not ())");
  EXPECT_BOOLTRUE(value);

  // and
  value = Eval("(and 1 2 3)");
  EXPECT_INTEQ(3,value);
  value = Eval("(and () baz ())");
  EXPECT_BOOLFALSE(value);
  value = Eval("(and)");
  EXPECT_BOOLTRUE(value);
  value = Eval("(and 3 (+ 4 5))");
  EXPECT_INTEQ(9,value);
  value = Eval("(and ())");
  EXPECT_BOOLFALSE(value);
  value = Eval("(and () no error)");
  EXPECT_BOOLFALSE(value);

  // or
  value = Eval("(or)");
  EXPECT_BOOLFALSE(value);
  value = Eval("(or (= 1 2) (+ 3 4) baz)");
  EXPECT_INTEQ(7,value);
  value = Eval("(or (+ 1 1) baz (+ 3 4))");
  EXPECT_INTEQ(2,value);

  // set!
  Eval("(define foo 1)");
  value = Eval("(set! foo (+ 1 2))");
  EXPECT_INTEQ(3,value);
  value = Eval("foo");
  EXPECT_INTEQ(3,value);

  // cons,car,cdr,set-car!,set-cdr!
  Eval("(define foo (cons (+ 1 2) (+ 3 4)))");
  value = Eval("(car foo)");
  EXPECT_INTEQ(3,value);
  value = Eval("(cdr foo)");
  EXPECT_INTEQ(7,value);
  value = Eval("(set-car! foo (+ 5 6))");
  EXPECT_INTEQ(11,value);
  value = Eval("(car foo)");
  EXPECT_INTEQ(11,value);
  value = Eval("(set-cdr! foo (+ 6 8))");
  EXPECT_INTEQ(14,value);
  value = Eval("(cdr foo)");
  EXPECT_INTEQ(14,value);
  
  // list
  value = Eval("(list)");
  EXPECT_TRUE(value.IsNull());
  value = Eval("(list 0 1 (+ 0 1) (+ 1 (+ 0 1)) \
                      (+ (+ 0 1) (+ 1 (+ 0 1))))");
  EXPECT_LISTEQ(value, LIST(INT(0),INT(1),INT(1),INT(2),INT(3)));
}

TEST_F(EvalTest, test_cond)
{
  Data value;

  value = Eval("(cond ((= 1 (+ 0 1)) (quote foo)) \
                      ((= 2 (+ 1 1)) (quote bar)) \
                      (else baz))");
  EXPECT_SYMEQ("foo",value);

  Eval("(define false ())");
  Eval("(define true (quote t))");
  
  value = Eval("(cond (true (quote foo)) \
                      ((= 2 (+ 1 1)) (quote bar)) \
                      (else baz))");

  EXPECT_SYMEQ("foo",value);

  value = Eval("(cond (1 (quote foo)) \
                      ((= 2 (+ 1 1)) (quote bar)) \
                      (else baz))");
  EXPECT_SYMEQ("foo",value);
  
  value = Eval("(cond ((= 1 2) (quote foo)) \
                      ((= 2 (+ 1 1)) (quote bar)) \
                      (else baz))");
  // though baz is not defined, no error should occur
  EXPECT_SYMEQ("bar",value);

  value = Eval("(cond ((= 1 (+ 1 1)) (quote foo)) \
                      ((= 2 (+ 1 2)) (quote bar)) \
                      (else (quote baz)))");
  EXPECT_SYMEQ("baz",value);

  value = Eval("(cond ((= 1 (+ 1 1)) (quote foo)) \
                      (else (quote baz)) \
                      ((= 2 (+ 1 1)) (quote bar)))"); \

  EXPECT_SYMEQ("baz",value);
  
}
  


