#include "memory.h"

#include <stdlib.h>
#include <stdio.h>
#include "string.h"

namespace sscheme {

Machine g_machine;

Data::~Data()
{
  if (type == kString) {
    free(data.str);
    data.str = NULL;
  }
}

Data::Data(const Data& x)
{
  type = x.type;
  if (x.type == kString) {
    if ((data.str = strdup(x.data.str)) == NULL)
      ERROR("out of memory");
  } else {
    data = x.data;
  }
}

Data& Data::operator=(const Data& x)
{
  if (this == &x)
    return *this;

  if (type == kString) {
    free(data.str);
    data.str = NULL;
  }

  type = x.type;
  if (x.type == kString) {
    if ((data.str = strdup(x.data.str)) == NULL)
      ERROR("out of memory");
  } else {
    data = x.data;
  }
  return *this;
}

Data& Data::operator=(const Data& x);

Data Data::null(Data::kNull);
Data Data::none(Data::kNone);
Data Data::end_list(Data::kEndList);
Data Data::f(Data::kNull);
Data Data::t = Memory::NewSymbol(Symbol::New("#t"));
Data Data::ok = Memory::NewSymbol(Symbol::New("ok"));

bool operator==(const Data& x, const Data& y)
{
  if (x.type != y.type || x.type == Data::kInvalid)
    return false;

  if (x.type == Data::kInt)
    return x.data.i == y.data.i;
  else if (x.type == Data::kFloat)
    return x.data.f == y.data.f;
  else if (x.type == Data::kString)
    return x.data.str == y.data.str;
  else if (x.type == Data::kSymbol)
    return x.data.sym == y.data.sym;
  else if (x.type == Data::kPair)
    return x.data.p == y.data.p;
  else if (x.type == Data::kPrimProc)
    return x.data.proc == y.data.proc;
  else
    return true;
}

bool operator!=(const Data& x, const Data& y)
{
  return !(x==y);
}

Data* Memory::car_;
Data* Memory::cdr_;
Data* Memory::car1_;
Data* Memory::cdr1_;
Data* Memory::car2_;
Data* Memory::cdr2_;
int   Memory::free_;
int   Memory::size_;
Data  Memory::tmp_list_;
Data  Memory::tmp_x;
Data  Memory::tmp_y;

int Memory::new_free_;
Data* Memory::new_car_;
Data* Memory::new_cdr_;


Data Memory::NewInt(int i)
{
  return Data(Data::kInt, i);
}

Data Memory::NewString(const char* s)
{
  // [fixme] copy is simple but inefficient
  return Data(Data::kString,s);
}

Data Memory::NewSymbol(const char* sym)
{
  return Data(Data::kSymbol,sym);
}

Data Memory::NewFloat(float f)
{
  return Data(f);
}

Data Memory::NewProc(Data::PrimProc proc)
{
  return Data(proc);
}

Data Memory::NewPair(const Data& x, const Data& y)
{
  if (free_ >= size_) {
    tmp_x = x;
    tmp_y = y;
    GarbageCollect();
    // GarbageCollect should ERROR if out of memory
    if (free_ >= size_)
      ERROR("Garbage collect fail: out of memory");
    car_[free_] = tmp_x;
    cdr_[free_] = tmp_y;
    tmp_x = tmp_y = Data::null;
  } else {
    car_[free_] = x;
    cdr_[free_] = y;
  }
  return Data(Data::kPair,free_++);
}

void Memory::Assert(Data x)
{
  assert(x.type != Data::kInvalid);
  if (x.IsPair() && !x.IsBrokenHeart()) {
    if (!(x.data.p >= 0 && x.data.p < free_) )
      printf("fail");
    assert(x.data.p >= 0 && x.data.p < free_);
  }
}
    
Data Memory::Car(const Data& pair)
{
  Assert(pair);

  return Memory::car_[pair.data.p];
}

Data Memory::Cdr(const Data& pair)
{
  Assert(pair);
  return cdr_[pair.data.p];
}

Data Memory::First(Data list)
{
  Assert(list);
  assert(list.IsList());
  
  if (list.IsNull())
    return list;
  else
    return Car(list);
}

Data Memory::Rest(Data list)
{
  Assert(list);
  assert(list.IsList());
  if (list.IsNull())
    return list;
  else
    return Cdr(list);
}

Data Memory::Nth(Data list, int n)
{
  while(--n > 0)
    list = Rest(list);
  return First(list);
}

Data Memory::ReverseList(Data list)
{
  assert(list.IsList());
  Assert(list);
  
  Data newlist = Data::null;

  while (!list.IsNull()) {
    Data next = Rest(list);
    cdr_[list.data.p] = newlist;
    newlist = list;
    list = next;
  }
  return newlist;
}

void Memory::SetCar(Data pair, const Data& x)
{
  Assert(pair);
  assert(pair.IsPair());
  car_[pair.data.p] = x;
}

void Memory::SetCdr(Data pair, const Data& x)
{
  Assert(pair);
  assert(pair.IsPair());
  cdr_[pair.data.p] = x;
}

void Memory::Init(int size)
{
  assert(size > 0);

  FREE(car1_);
  FREE(cdr1_);
  FREE(car2_);
  FREE(cdr2_);
  
  car1_ = static_cast<Data*>(ALLOC(size * sizeof(Data)));
  cdr1_ = static_cast<Data*>(ALLOC(size * sizeof(Data)));
  car2_ = static_cast<Data*>(ALLOC(size * sizeof(Data)));
  cdr2_ = static_cast<Data*>(ALLOC(size * sizeof(Data)));

  car_ = car1_;
  cdr_ = cdr1_;
  
  size_ = size;
  free_ = 0;

  tmp_list_ = tmp_x = tmp_y = Data::null;
  
}

Data Memory::List(Data v[], int n)
{
  tmp_list_ = Data::null;

  for(int i = 0; i < n; i++) {
    tmp_list_ = Memory::NewPair(v[i],tmp_list_);
  }

  Data list = tmp_list_;
  tmp_list_ = Data::null;

  return ReverseList(list);
}

int Memory::Length(Data list)
{
  assert(list.IsList());

  int n = 0;
  while(!list.IsNull()) {
    list = REST(list);
    ++n;
  }
  return n;
}

bool Memory::Equal(Data x, Data y)
{
  if (x.IsAtom() && y.IsAtom()) {
    if (x.IsString() && y.IsString()) {
      return strcmp(x.String(),y.String()) == 0;
    } else {
      return x == y;
    }
  } else if (x.IsPair() && y.IsPair()) {
    return (Equal(FIRST(x),FIRST(y)) &&
            Equal(REST(x),REST(y)));
  } else
    return false;
}
      

void Initialize(int mem_size)
{
  Memory::Init(mem_size);

  for(int i = 0; i < int(g_machine.stack.size()); i++)
    g_machine.stack[i] = Data::null;
  g_machine.stack.clear();

  Data* registers[] = {&g_machine.exp, &g_machine.unev, &g_machine.env,
                       &g_machine.val, &g_machine.arg1, &g_machine.proc};

  
  for(int i = 0; i < NELEMS(registers); i++)
    *registers[i] = Data::null;
}

void Machine::Push(Data& x)
{
  stack.push_back(x);
}

Data Machine::Pop()
{
  assert(stack.size() > 0);
  Data d = stack.back();
  stack.pop_back();
  return d;
}

bool Machine::IsStackEmpty()
{
  return stack.size() == 0;
}

Data Memory::Migrate(Data old)
{
  if (!old.IsPair())
    return old;
  else if (CAR(old).IsBrokenHeart())
    return CDR(old);
  else {
    if (new_free_ >= size_)
      ERROR("Garbage collect failure: out of memory in migrate");
    new_car_[new_free_] = CAR(old);
    new_cdr_[new_free_] = CDR(old);
    Data new_pair = Data(Data::kPair,new_free_);
    // set brokenheart
    SETCAR(old,Data(Data::kBrokenHeart));
    SETCDR(old,new_pair);
    ++new_free_;
    return new_pair;
  }
}

int Memory::FreeSize()
{
  return size_ - free_;
}


void Memory::GarbageCollect()
{
  // Initialize variables used by Migrate
  new_free_ = 0;
  if (car_ == car1_) {
    new_car_ = car2_;
    new_cdr_ = cdr2_;
  } else {
    new_car_ = car1_;
    new_cdr_ = cdr1_;
  }

  Data* registers[] = {&g_machine.exp, &g_machine.unev, &g_machine.env,
                       &g_machine.val, &g_machine.arg1, &g_machine.proc,
                       &tmp_list_, &tmp_x, &tmp_y};
  
  
  for(int i = 0; i < NELEMS(registers); i++)
    *registers[i] = Migrate(*registers[i]);
  
  for(int i = 0; i < (int)g_machine.stack.size(); i++) 
    g_machine.stack[i] = Migrate(g_machine.stack[i]);

  for(int scan = 0; scan < new_free_; scan++) {
    new_car_[scan] = Migrate(new_car_[scan]);
    new_cdr_[scan] = Migrate(new_cdr_[scan]);
  }

  // swap old memory region and new one
  car_ = new_car_;
  cdr_ = new_cdr_;
  free_ = new_free_;
}
  
} // namespace sscheme


