#ifndef SSCHEME_SRC_EVAL_H_
#define SSCHEME_SRC_EVAL_H_

#include "memory.h"

namespace sscheme {

// evaluate s-expression in env environment
Data Eval(Data sexp, Data env);

// apply procedure proc to parameters
Data Apply(Data proc,Data parameters);

} // namespace sscheme

#endif
