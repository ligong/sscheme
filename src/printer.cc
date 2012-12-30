#include "printer.h"

namespace sscheme {

void PrintRest(Data x);

void PrintAtom(Data x)
{
  if (x.IsInt())
    fprintf(stdout,"%d",x.Int());
  else if (x.IsFloat())
    fprintf(stdout,"%f",x.Float());
  else if (x.IsString())
    fprintf(stdout,"\"%s\"",x.String());
  else if (x.IsSymbol())
    fprintf(stdout,"%s",x.Symbol());
  else if (x.IsNull())
    fprintf(stdout,"NIL");
  else
    assert(0);
}


void Print(Data x)
{
  if (x.IsAtom()) {
    PrintAtom(x);
  } else {
    fprintf(stdout,"(");
    Print(FIRST(x));
    PrintRest(REST(x));
    fprintf(stdout,")");
  }
}

void PrintRest(Data x)
{
  if (x.IsNull())
    return;
  else if (x.IsAtom()) {
    fprintf(stdout," . ");
    Print(x);
  } else {
    fprintf(stdout," ");
    Print(FIRST(x));
    // wish modern compiler support tail recursive
    PrintRest(REST(x));
  }
}

} // namespace sscheme
