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

Data Data::null(Data::kNull);
Data Data::none(Data::kNone);
Data Data::end_list(Data::kEndList);


Data* Memory::car_;
Data* Memory::cdr_;
Data* Memory::car1_;
Data* Memory::cdr1_;
Data* Memory::car2_;
Data* Memory::cdr2_;
int   Memory::free_;
int   Memory::size_;


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

Data Memory::NewPair(Data x, Data y)
{
  if (free_ >= size_)
    GarbageCollect();
  
  // GarbageCollect should ERROR if out of memory
  assert(free_ < size_); 

  car_[free_] = x;
  cdr_[free_] = y;
  return Data(Data::kPair,free_++);
}

void Memory::Assert(Data x)
{
  assert(x.type != Data::kInvalid &&
         (x.type != Data::kPair ||
          (x.data.p >= 0 && x.data.p < free_)));
}

Data Memory::Car(Data pair)
{
  Assert(pair);

  return Memory::car_[pair.data.p];
}

Data Memory::Cdr(Data pair)
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

void Memory::SetCar(Data pair, Data x)
{
  Assert(pair);
  assert(pair.IsPair());
  car_[pair.data.p] = x;
}

void Memory::SetCdr(Data pair, Data x)
{
  Assert(pair);
  assert(pair.IsPair());
  cdr_[pair.data.p] = x;
}


void Memory::Init(int size)
{
  assert(size > 0);
  
  car1_ = static_cast<Data*>(ALLOC(size * sizeof(Data)));
  cdr1_ = static_cast<Data*>(ALLOC(size * sizeof(Data)));
  car2_ = static_cast<Data*>(ALLOC(size * sizeof(Data)));
  cdr2_ = static_cast<Data*>(ALLOC(size * sizeof(Data)));

  car_ = car1_;
  cdr_ = cdr1_;
  
  size_ = size;
  free_ = 0;
  
}

void Initialize(int mem_size)
{
  Memory::Init(mem_size);

  g_machine.stack = Data::null;
  g_machine.env = Data::null;

}

void Machine::Push(Data x)
{
  stack = Memory::NewPair(x,stack);
}

Data Machine::Pop()
{
  assert(!IsStackEmpty());
  Data d = FIRST(stack);
  stack = REST(stack);
  return d;
}

bool Machine::IsStackEmpty()
{
  return stack.IsNull();
}

void Memory::GarbageCollect()
{

}

} // namespace sscheme
