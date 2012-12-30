#include "reader.h"

namespace sscheme {

Data ReadList(TokenStream& ts);

Data Read(TokenStream& ts)
{
  Token t = ts.Next();
  Data x;

  if (t.type == Token::kNone) {
    x = Data::none;
  } else if (t.type == Token::kInt) {
    x = Memory::NewInt(t.data.i);
  } else if (t.type == Token::kString) {
    x = Memory::NewString(t.data.str);
  } else if (t.type == Token::kFloat) {
    x = Memory::NewFloat(t.data.f);
  } else if (t.type == Token::kSymbol) {
    x = Memory::NewSymbol(t.data.sym);
  } else if (t.type == Token::kLParen) {
    x = ReadList(ts);
  } else if (t.type == Token::kRParen) {
    x = Data::end_list;
  } else {
    ERROR("invalid s-expression");
    x = Data::none;
  }
  
  return x;
}


Data ReadList(TokenStream& ts)
{
  Data list = Data::null;
  Data sub_sexp;

  while((sub_sexp = Read(ts)).type != Data::kNone &&
        sub_sexp.type != Data::kEndList) {
    list = CONS(sub_sexp,list);
  }

  if (sub_sexp.type == Data::kNone)
    ERROR("Unbalanced parenthese");

  return Memory::ReverseList(list);
}

Data Read(const char* str)
{
  TokenStream ts(str);
  return Read(ts);
}


} // namespace sscheme

