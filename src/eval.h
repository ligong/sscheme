#ifndef SSCHEME_SRC_EVAL_H_
#define SSCHEME_SRC_EVAL_H_

#include "memory.h"

namespace sscheme {

// evaluate s-expression(g_machine.exp) in g_machine.env environment
Data Eval();
Data Eval(const char* str);

// apply g_machine.proc operator to g_machine.arg1 parameters
Data Apply();

void InitialEnvironment();

void PrintEnvVars();

void RestoreGlobalEnvironment();

} // namespace sscheme

#endif
