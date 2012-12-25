#ifndef SSCHEME_SRC_READER_H_
#define SSCHEME_SRC_READER_H_

#include "memory.h"
#include "token.h"


namespace sscheme {

Data Read(TokenStream& ts);

} // namespace sscheme
    

#endif
