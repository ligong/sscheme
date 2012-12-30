#include <errno.h>
#include <stdlib.h>
#include <fstream>
#include <iostream>

#include "memory.h"
#include "eval.h"
#include "reader.h"
#include "printer.h"

namespace sscheme {

// repl(read evaluate print loop)
void Repl(char* name, std::istream* ifs)
{

  for(;;) {
    if (!name)
      printf("> ");
    TokenStream ts(ifs);
    g_machine.exp = Read(ts);
    if (g_machine.exp.type == Data::kNone)
      break;
    RestoreGlobalEnvironment();
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
    std::ifstream ifs(argv[i]);
    if (!ifs.good()) {
      fprintf(stderr,"%s fail to open %s(%s)\n", argv[0], argv[i], strerror(errno));
      return EXIT_FAILURE;
    } else
      sscheme::Repl(argv[i],&ifs);
  }
  if (argc == 1) sscheme::Repl(NULL,&std::cin);

  return EXIT_SUCCESS;
}
