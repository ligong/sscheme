#include "printer.h"

namespace sscheme {

#ifdef NDEBUG
#define OUTPUT stdout
#else
#define OUTPUT stderr
#endif

void PrintRest(Data x);

void PrintAtom(Data x)
{
  if (x.IsInt())
    fprintf(OUTPUT,"%d",x.Int());
  else if (x.IsFloat())
    fprintf(OUTPUT,"%f",x.Float());
  else if (x.IsString())
    fprintf(OUTPUT,"\"%s\"",x.String());
  else if (x.IsSymbol())
    fprintf(OUTPUT,"%s",x.Symbol());
  else if (x.IsNull())
    fprintf(OUTPUT,"NIL");
  else
    assert(0);
}


void Print(Data x)
{
  if (x.IsAtom()) {
    PrintAtom(x);
  } else {
    fprintf(OUTPUT,"(");
    Print(FIRST(x));
    PrintRest(REST(x));
    fprintf(OUTPUT,")");
  }
}

void PrintRest(Data x)
{
  if (x.IsNull())
    return;
  else if (x.IsAtom()) {
    fprintf(OUTPUT," . ");
    Print(x);
  } else {
    fprintf(OUTPUT," ");
    Print(FIRST(x));
    // wish modern compiler support tail recursive
    PrintRest(REST(x));
  }
}

} // namespace sscheme
