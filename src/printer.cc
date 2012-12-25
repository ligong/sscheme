#include "printer.h"

namespace sscheme {

void PrintRest(Data x);

void PrintAtom(Data x)
{
  if (x.IsInt())
    printf("%d",x.Int());
  else if (x.IsFloat())
    printf("%f",x.Float());
  else if (x.IsString())
    printf("\"%s\"",x.String());
  else if (x.IsSymbol())
    printf("%s",x.Symbol());
  else if (x.IsNull())
    printf("NIL");
  else
    assert(0);
}


void Print(Data x)
{
  if (x.IsAtom()) {
    PrintAtom(x);
  } else {
    printf("(");
    Print(FIRST(x));
    PrintRest(REST(x));
    printf(")");
  }
}

void PrintRest(Data x)
{
  if (x.IsNull())
    return;
  else if (x.IsAtom()) {
    printf(" . ");
    Print(x);
  } else {
    printf(" ");
    Print(FIRST(x));
    // wish modern compiler support tail recursive
    PrintRest(REST(x));
  }
}

} // namespace sscheme
