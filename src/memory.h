#ifndef SSCHEME_SRC_MEMORY_H_
#define SSCHEME_SRC_MEMORY_H_

#include <assert.h>
#include "utils.h"
#include "symbol.h"
#include <string.h>

// Scheme's memory model
namespace sscheme {

// scheme data 
// is all represented by typed pointer
struct Data
{
  enum TYPE {kInt,kBigNumber,kFloat,kString,kSymbol,kPair,
             kNull,kNone,kEndList,kInvalid} type;
  union {
    int i;                   // integer
    float f;                 // float
    char* str;               // pointer to string
    const char* sym;         // pointer to symbol
    int p;                   // pointer to pair, refer to class Memory
  } data;

  Data(): type(kInvalid) { }
  explicit Data(TYPE t): type(t) { }
  
  explicit Data(TYPE t,int x): type(t)  {
    if (t == kInt) data.i = x;
    else if (t == kPair) data.p = x;
    else ERROR("Wong type to Data(TYPE,int) call");
  }

  explicit Data(TYPE t,const char* s): type(t) {
    if (t == kString) {
      if ((data.str = strdup(s)) == NULL)
        ERROR("out of memory");
    } else if (t == kSymbol) data.sym = s;
    else ERROR("Wrong type to Data(TYPE,const char*)");
  }
  
  explicit Data(float ff): type(kFloat) { data.f = ff; }


  ~Data();
  Data(const Data&);
  Data& operator=(const Data&);

  bool IsNull() { return type == kNull; }
  bool IsInt() { return type == kInt; }
  bool IsFloat() { return type == kFloat; }
  bool IsSymbol() { return type == kSymbol; }
  bool IsString() { return type == kString; }
  bool IsPair() { return type == kPair; }
  bool IsAtom() {return !IsPair();}
  bool IsList() {return IsNull() || IsPair();}

  int Int() {assert(IsInt()); return data.i;}
  float Float() {assert(IsFloat()); return data.f;}
  char* String() {assert(IsString()); return data.str;}
  const char* Symbol() {assert(IsSymbol()); return data.sym;}

  static Data null;
  static Data none;
  static Data end_list;
  
};

// REQUIRE: all evaluated scheme value
// must be put into following registers
// Thus,the garbage collector can track all
// used memory.
// it is unchecked run-time error to create new
// scheme value before storing to following register
struct Machine
{
  Data exp;   // expression to be evaluated
  Data unev;  // unevaluated parameters
  Data env;   // current environment
  Data val;   // evaluated value
  Data arg1;  // accumulate evaluated parameters
  Data proc;  // operator
  Data stack;
  
  void Push(Data x);
  Data Pop();
  bool IsStackEmpty();
};

extern Machine g_machine;

class Memory
{
  
 public:
  
  static Data NewInt(int i);
  static Data NewString(const char* s);
  static Data NewSymbol(const char* sym);
  static Data NewFloat(float f);
  static Data NewPair(Data x, Data y);

  static Data Car(Data pair);
  static Data Cdr(Data pair);
  static void SetCar(Data pair, Data x);
  static void SetCdr(Data pair, Data x);
  static Data First(Data list);
  static Data Rest(Data list);
  static Data Nth(Data list, int n);
  static Data ReverseList(Data list);

  // assert data is valid
  static void Assert(Data);

  static void Init(int size = 5 * 1024 * 1024);

 private:
  
  static Data* car_;
  static Data* cdr_;
  static Data* car1_;
  static Data* cdr1_;
  static Data* car2_;
  static Data* cdr2_;

  static int free_;
  static int size_;

  static void GarbageCollect();
  
};

#define CONS(x,y) (Memory::NewPair(x,y))
#define CAR(x) (Memory::Car(x))
#define CDR(x) (Memory::Cdr(x))
#define CAAR(x) (CAR(CAR(x)))
#define CADR(x) (CAR(CDR(x)))
#define CADDR(x) (CADR(CDR(x)))
#define CAADR(x) (CAR(CADR(x)))
#define FIRST(x) (Memory::First(x))
#define REST(x) (Memory::Rest(x))
#define NTH(x,n) (Memory::Nth((x),(n)))
#define SETCAR(pair,x) (Memory::SetCar((pair),(x)))
#define SETCDR(pair,x) (Memory::SetCdr((pair),(x)))

void Initialize(int mem_size);

} // namespace sscheme

#endif
