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
 
// parse integer from file and return it
// WARN: call ERROR if number is too big to
// fit in long
static long ParseInt(FILE* file,int c)
{
  unsigned long m;
  int sign = 1;
  
  if (c == '+')
    c = fgetc(file);
  else if (c == '-') {
    c = fgetc(file);
    sign = -1;
  }

  m = 0;
  for(;c != EOF && isdigit(c); c = fgetc(file)) {
    int d = c - '0';
    if (((sign > 0) && (m > (LONG_MAX - d) / 10UL)) ||
        ((sign < 0) && (m > (LONG_MAX + 1UL - d) / 10UL)))
      ERROR("do not support big number yet");
    m = m*10 + d;
  }

  if (c != EOF)
    ungetc(c,file);

  return sign > 0? m:-m;
}

// parse number from file and return it
// parameter c must contain the first digit
static Token NextNumber(FILE* file,int c)
{
  long m = ParseInt(file,c);

  Token t;
  if ((c = fgetc(file)) == '.') {
    float f,g;
    c = fgetc(file);
    f = (float)ParseInt(file,c);
    long power = 10;
    g = f;
    while ((g/=10) > 1)
      power *= 10;
    t.type = Token::kFloat;
    t.data.f = m + f/power;
    return t;
  } else if (c != EOF) {
    ungetc(c,file);
  }

  t.type = Token::kInt;
  t.data.i = m;
  return t;
}

// parse string as format "..." and return it
// assume the first '"' has already been parsed
static Token NextString(FILE* file)
{
  long nbuf = 50;
  int i = 0;
  char* buf = static_cast<char*>(ALLOC(nbuf));
  int c;
  
  while((c = fgetc(file)) != EOF && c != '"') {
    if (c == '\\') { // support escape char '\'
      if ((c = fgetc(file)) == EOF)
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
  return isalnum(c) || strchr("?+-*/_",c);
}

// parse symbol and return it
// the first char is in c
static Token NextSymbol(FILE* file,int c)
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
  } while(IsSymbol(c = fgetc(file)));
  
  if (c != EOF)
    ungetc(c,file);

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
  while(isblank(c = fgetc(file_)) || (c=='\r') || (c=='\n'))
    ;
  if (c == EOF)
    return Token(Token::kNone);
  else if (isdigit(c) || (c == '-'))
    return NextNumber(file_,c);
  else if (c == '\"')
    return NextString(file_);
  else if (c == '(')
    return Token(Token::kLParen);
  else if (c == ')')
    return Token(Token::kRParen);
  else if (c == '.')
    return Token(Token::kDot);
  else
    return NextSymbol(file_,c);
}

} // namespace sscheme

