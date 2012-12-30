#include <errno.h>
#include <stdlib.h>

#include "memory.h"
#include "eval.h"
#include "reader.h"
#include "printer.h"

namespace sscheme {

// repl(read evaluate print loop)
void Repl(char* name, FILE* file)
{

  for(;;) {
    if (!name)
      printf("> ");
    TokenStream ts(file);
    g_machine.exp = Read(ts);
    if (g_machine.exp.type == Data::kNone)
      break;
    Data value = Eval();
    if (!name) {
      printf("Value: ");
      Print(value);
      printf("\n");
    }
  }
}

} // namespace sscheme

int main(int argc, char* argv[])
{

  sscheme::Initialize(5*1024*1024);
  sscheme::InitialEnvironment();
  
  for(int i = 1; i < argc; i++) {
    ifstream ifs(argv[i]);
    if (!ifs.good()) {
      fprintf(stderr,"%s fail to open %s(%s)\n", argv[0], argv[i], strerror(errno));
      return EXIT_FAILURE;
    } else
      sscheme::Repl(argv[i],file);
  }
  if (argc == 1) sscheme::Repl(NULL,stdin);

  return EXIT_SUCCESS;
}
