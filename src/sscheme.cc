struct TypedPointer
{
  enum {kInt,kBigNumber,kFloat,kString,kSymbol,kPair,kNULL} type;
  union {
    int i;                   // integer
    float f;                 // float
    char* str;               // pointer to string
    const char* sym;         // pointer to symbol
    char bytes[sizeof long];
    int p;      // cons vectors index, big number is also a list
  };
};

const TypedPointer NoneValue = {kNULL};

// lower level, all scheme data type is represented by TypedPointer
typedef TypedPointer Value;

// scheme program consist of s-expression
// s-expression is also scheme's data
typedef Value SExp;

bool IsSelfEval(const SExp& x);

bool IsSelfEval(const SExp& x)
{
  return (x.type == kInt   || x.type == kBigNumber ||
          x.type == kFloat || x.type == kString);
}

bool IsVariable(const SExp& x)
{
  return x.type == kSymbol;
}

Value LookUpVariable(const Value& var, const Value& env, const Memory* mem)
{
  for(;!IsNull(env);env = EnvironmentEnclosure(env)) {
    const Value frame = EnvironmentFrame(env);
    for(;!IsNull(frame);frame = Rest(frame)) {
      const Value kv = First(frame);
      if (Eq(var,Car(kv)))
        return Cdr(kv);
    }
  }
  return kNULL;
}

bool IsQuote(const SExp& x)
{
  return IsPair(x) && Eq(First(x),Symbol("quote"));
}
  
Value QuoteText(const SExp& x)
{
  return Rest(x);
}

bool IsLambda(const SExp& x)
{
  return IsPair(x) && Eq(First(x),Symbol("lambda"));
}

SExp LambdaParameters(const SExp& x)
{
  return Second(x);
}

SExp LambdaBody(const SExp& x)
{
  return Thrid(x);
}

Value MakeProcedure(const SExp& parameters,
                    const SExp& body,
                    const SExp& env,
                    Memory* mem)
{
  return List(mem,parameters,body,env);
}
                           
bool IsAssign(const SExp& x)
{
  return false;
}
SExp AssignVariable(const SExp& x)
{
  return InvalidValue;
}

SExp AssignValue(const SExp& x)
{
  return InvalidValue;
}

bool IsDefine(const SExp& x)
{
  return false;
}

SExp DefineVariable(const SExp& x)
{
  return InvalidValue;
}

SExp DefineValue(const SExp& x)
{
  return InvalidValue;
}

bool IsCond(const SExp& x)
{
  return false;
}
SExp CondToIf(const SExp& x)
{
  return InvalidValue;
}

bool IsApp(const SExp& x)
{
  return InvalidValue;
}

Value EvalApp(const SExp& x)
{
  return InvalidValue;
}

/*
  read the next s-exp into memory
  return s-exp value
*/
SExp Read(TokenStream& ts, Memory* mem)
{
  assert(mem);
  
  Token t = ts.Next();
  SExp x;
  if (t == Token::kNULL) {
    x.type == SExp::kNULL;
  } else if (t.type == Token::kNum) {
    x.type = SExp::kInt;
    x.i = t.i;
  } else if (t.type() == Token::kString) {
    x.type = SExp::kString;
    x.str = strdup(t.str);
    if (!x.str)
      raise ReadErr("out of memory");
  } else if (t.type() == Token::kFloat) {
    x.type = SExp::kFloat;
    x.f = t.f;
  } else if (t.type() == Token::kSymbol) {
    x.type = SExp::kSymbol;
    x.sym = t.sym;
  } else if (t.type() == Token::kLParen) {
    x = mem->StartList();
    SExp y;
    while((y = Read(ts,mem)) != SExp::kNULL && y != SExp::RParen)
      mem->ListAdd(y,x);
    if (y != SExp::RParen)
      raise ReadErr("lack of )");
    x = mem->EndList(x);
  }
  return x;
}

struct Token
{
  enum {kInt,kBigNumber,kFloat,kString,kSymbol,kPair,kNULL,kLParen,kRParen} type;
  union {
    int i;       // integer
    float f;     // float
    char* str;   // pointer to string
    const char* sym;   // pointer to symbol
    BigNum* bign; 
  };
  Token(type t): type(t) { }
  Token(int ii): type(kInt), i(ii) { }
  Token(float ff): type(kFloat), f(ff) { }
  Token(char* s): str(s) { }
  Token(const char* sy): sym(sy) { }
  Token(BigNum* b): bign(b) { }
};

class TokenStream
{
 public:
  TokenStream(FILE* file): file_(file) { };
  Token Next();
 private:
  FILE* file_;
};


Token TokenStream::Next()
{
  int c;
  char buf[4096];

  // skip blank chars
  while((c = getc(file_)) != EOF && isblank(c))
    ;
  if (c == EOF)
    return Token(Token::kNULL);
  else if (isdigit(c) || (c == '+') || (c == '-'))
    return NextNumber(c);
  else if (c == '\"')
    return NextString();
  else if (c == '(')
    return Token(Token::LParen);
  else if (c == ')')
    return Token(Token::RParen);
  else
    return NextSymbol(c);
}

Token TokenStream::NextNumber()
{
  char buf[4096];
  
  // copy digits to buffer
  int i = 0;
  do {
    buf[i++] = c;
  } while (i < NELEMS(buf)-1 && isdigit(c = getc(file_)));
    
  if (i >= NELEMS(buf)-1)
    raise ReadErr("number too big");

  if (c != EOF)
    ungetc(file_);
    
  buf[i] = '\0';
  int n;
  if (ParseInt(buf,&n))
    return Token(n);
  else if (BigNum *bign = ParseBigNum(buf))
    return Token(bign);
  else
    raise ReadErr("parse big number error");
}


Token TokenStream::NextString()
{
  char buf[4096];

  int c;
  int i;
  for(i = 0; i < NELEMS(buf)-1; i++) {
    c = getc(file_);
    if (c == '"')
      break;
    if (c == '\\') {
      c = getc(file_);
      if (c == EOF)
        raise ReadErr("Escaped char must follow another char");
    }
    buf[i] = c;
  }

  buf[i] = '\0';
  char *s = strdup(buf);
  if (!s)
    raise ReadErr("out of memory");
  return Token(s);
}

bool issymbol(int c)
{
  return isalnum(c) || strchr("+-*/?=",c);
}

Token TokenStream::NextSymbol(int c)
{
  char buf[4096];
  int i;
  for(i = 0; i < NELEMS(buf)-1 && (c = getc(file_)) != EOF && issymbol(c); i++)
    buf[i] = c;
  buf[i++] = '\0';
  return Token(Atom::New(buf));
}

    
class Machine
{
 public:

  Machine();
  
  // Read an s-exp into machine's exp
  Machine* Read(FILE*);
  
  // Evaluate machine's exp and store
  // result into val and return it
  SExp Eval();

 private:
  
  Memory* mem_;
  
  // REQUIRE: all evaluated scheme value
  // must be put into following storage.
  // So,the garbage collector can track all
  // valid memory.
  // it is unchecked run-time error to create new
  // scheme value before storing to following storage
  SExp exp_;   // expression to be evaluated
  SExp unev_;  // unevaluated parameters
  Value env_;   // current environment
  Value val_;   // evaluated value
  Value arg1_;  // accumulate evaluated parameters
  Value proc_;  // operator
  Value stack_; 
};

void Repl(char* name, FILE* file, Machine* machine)
{

  Machine machine;
  // repl(read evaluate print loop)
  for(;;) {
    if (!name)
      printf(">");
    Print(machine->Read(file)->Eval());
  }
}

Value Machine::Eval()
{
  if (IsSelfEval(exp))
    val = exp;
  else if (IsVariable(exp))
    val = LookupVariable(exp,env);
  else if (IsQuote(exp))
    val = QuoteText(exp);
  else if (IsLambda(exp))
    val = MakeProcedure(LambdaParameters(exp),
                        LambdaBody(exp),
                        env,mem);
  else if (IsAssign(exp))
    val = EvalAssign(exp,env);
  else if (IsDefinition(exp))
    val = EvalDefinition(exp,env);
  else if (IsCond(exp)) {
    exp = CondToIf(exp);
    val = Eval();
  }
  else if (IsApp(exp))
    val = EvalApp(exp,env);
  else
    Error("Wrong s-expression");

  return val;
}
    
int main(int argc, char* argv[])
{

  for(int i = 1; i < argc; i++) {
    ifstream ifs(argv[i]);
    if (!ifs.good()) {
      fprintf(stderr,"%s fail to open %s(%s)\n", argv[0], argv[i], strerr(errno));
      return EXIT_FAILURE;
    } else
      Repl(argv[i],ifs);
  }
  if (argc == 1) repl(NULL,stdin);

  return EXIT_SUCCESS;
}
