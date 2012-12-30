#include "token.h"

#include <stdio.h>
#include <string.h>
#include <limits.h>

#include "utils.h"
#include "symbol.h"

namespace sscheme {

Token& Token::operator=(const Token& that)
{
  if (this != &that) {
    if (type == kString)
      FREE(data.str);
    type = that.type;
    if (that.type == kString) {
      if ((data.str = strdup(that.data.str)) == NULL)
        ERROR("out of memory");
    } else {
      data = that.data;
    }
  }
  return *this;
}
 
// parse integer from file and return it,
// set the number of analyzed digits in *n
// WARN: call ERROR if number is too big to
// fit in long
static long ParseInt(std::istream* is,int c,int* n)
{
  unsigned long m;
  int nn;
  int sign = 1;
  
  if (c == '+')
    c = is->get();
  else if (c == '-') {
    c = is->get();
    sign = -1;
  }

  nn = 0;
  
  m = 0;
  for(;c != EOF && isdigit(c); c = is->get()) {
    int d = c - '0';
    if (((sign > 0) && (m > (LONG_MAX - d) / 10UL)) ||
        ((sign < 0) && (m > (LONG_MAX + 1UL - d) / 10UL)))
      ERROR("do not support big number yet");
    m = m*10 + d;
    ++nn;
  }

  if (c != EOF)
    is->unget();

  *n = nn;
  
  return sign > 0? m:-m;
}

// parse number from file and return it
// parameter c must contain the first digit
static Token NextNumber(std::istream* is,int c)
{
  int n;
  long m = ParseInt(is,c,&n);

  Token t;
  if ((c = is->get()) == '.') {
    float f;
    c = is->get();
    f = (float)ParseInt(is,c,&n);
    long power = 10;
     while (--n > 0)
      power *= 10;
    t.type = Token::kFloat;
    t.data.f = m + f/power;
    return t;
  } else if (c != EOF) {
    is->unget();
  }

  t.type = Token::kInt;
  t.data.i = m;
  return t;
}

// parse string as format "..." and return it
// assume the first '"' has already been parsed
static Token NextString(std::istream* is)
{
  long nbuf = 50;
  int i = 0;
  char* buf = static_cast<char*>(ALLOC(nbuf));
  int c;
  
  while((c = is->get()) != EOF && c != '"') {
    if (c == '\\') { // support escape char '\'
      if ((c = is->get()) == EOF)
        ERROR("'\' in string must follow a char");
    }

    if (i >= nbuf-1 ) { // ensure buffer large enough
      nbuf <<= 1;
      RESIZE(buf,nbuf);
    }

    buf[i++] = c;
  }
  
  if (c == EOF)
    ERROR("Unbalanced '\"' in string");
  
  buf[i++] = '\0';

  RESIZE(buf,i);
  Token t(Token::kString);
  t.data.str = buf;
  return t;
}

static int IsSymbol(int c)
{
  return isalnum(c) || strchr("?+-*/_!=<>",c);
}

// parse symbol and return it
// the first char is in c
static Token NextSymbol(std::istream* is,int c)
{
  long nbuf = 50;
  char* buf = static_cast<char*>(ALLOC(nbuf));
  int i = 0;


  assert(IsSymbol(c));
  
  do {
    if (i >= nbuf-1 ) { // ensure buffer large enough
      nbuf <<= 1;
      RESIZE(buf,nbuf);
    }
    buf[i++] = c;
  } while(IsSymbol(c = is->get()));
  
  if (c != EOF)
    is->unget();

  const char* sym = Symbol::New(buf,i);
  FREE(buf);

  Token t(Token::kSymbol);
  t.data.sym = sym;
  
  return t;
}

Token TokenStream::Next()
{
  int c;

  // skip blanks
  while(isblank(c = is_->get()) || (c=='\r') || (c=='\n'))
    ;
  if (c == EOF)
    return Token(Token::kNone);
  else if (isdigit(c) || (c == '-')) {
    if (c == '-') {
      int d = is_->get();
      if (d != EOF)
        is_->unget();
      if (isdigit(d) || d == '.') {
        return NextNumber(is_,c);
      } else
        return NextSymbol(is_,c);
    } else {
      return NextNumber(is_,c);
    }
  } else if (c == '\"')
    return NextString(is_);
  else if (c == '(')
    return Token(Token::kLParen);
  else if (c == ')')
    return Token(Token::kRParen);
  else if (c == '.')
    return Token(Token::kDot);
  else
    return NextSymbol(is_,c);
}

} // namespace sscheme


