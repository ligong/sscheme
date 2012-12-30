#include "printer.h"

namespace sscheme {

void PrintRest(Data x);

void PrintAtom(Data x)
{
  if (x.IsInt())
    fprintf(stderr,"%d",x.Int());
  else if (x.IsFloat())
    fprintf(stderr,"%f",x.Float());
  else if (x.IsString())
    fprintf(stderr,"\"%s\"",x.String());
  else if (x.IsSymbol())
    fprintf(stderr,"%s",x.Symbol());
  else if (x.IsNull())
    fprintf(stderr,"NIL");
  else
    assert(0);
}


void Print(Data x)
{
  if (x.IsAtom()) {
    PrintAtom(x);
  } else {
    fprintf(stderr,"(");
    Print(FIRST(x));
    PrintRest(REST(x));
    fprintf(stderr,")");
  }
}

void PrintRest(Data x)
{
  if (x.IsNull())
    return;
  else if (x.IsAtom()) {
    fprintf(stderr," . ");
    Print(x);
  } else {
    fprintf(stderr," ");
    Print(FIRST(x));
    // wish modern compiler support tail recursive
    PrintRest(REST(x));
  }
}

} // namespace sscheme
