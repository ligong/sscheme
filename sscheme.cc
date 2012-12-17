
void Repl(char* name, FILE* file)
{
  Environment env;
  
  // repl(read evaluate print loop)
  for(;;) {
    if (!name)
      printf(">");
    Exp* exp = Exp::Read(file);
    if (exp == NULL)
      break;
    Value val = exp->Eval(&env);
    if (!name)
      val->Print();
    delete exp;
  }
}

// opaque expression
class Exp
{
 public:
  
  virtual Value Eval(Environment*) = 0;
  virtual ~Exp() { }
  
  static Exp* Read();
  
 private:
  // Disallow explict/copy/assignment constructor
  Exp();
  Exp(const Exp&);
  void operator=(const Exp&);
};

class SelfEvalExp: public Exp
{
 public:
  SelfEvalExp(Value value):  value_(value) { }
  virtual Value Eval(Environment*) { return value_; }
  
 private:
  Value value_;
};

class VariableExp: public Exp
{
 public:
  VariableExp(Variable var): var_(var) { }
  virtual Value Eval(Environment*);
 private:
  Variable var_;
};

Value VariableExp::Eval(Environment* env)
{
  assert(env);
  return env->Lookup(var_);
}

class QuoteExp: public Exp
{
 public:
  QuoteExp(Exp* exp): quote_text_(exp) { }
  virtual Value Eval(Environment*) {return Value(text_);}
 private:
  Exp* text_;
};

class LambdaExp: public Exp
{
 public:
  LambdaExp(Arguments plist, Code code; Environment*);
  virtual Value Eval(Environment*);
 private:
  Arguments plist_;
  Code code_;
  Environment* env_;
};

class IfExp: public Exp
{
 public:
  virtual Value Eval(Environment*);
};

class CondExp: public Exp
{
 public:
  virtual Value Eval(Environment*);
};

class AssignExp: public Exp
{
 public:
  virutal Value Eval(Environment*);
};

class DefineExp: public Exp
{
 public:
  virtual Value Eval(Environment*);
};

class BeginExp: public Exp
{
 public:
  virtual Value Eval(Environment*);
};

class AppExp: public Exp
{
 public:
  virtual Value Eval(Environment*);
};
  

int main(int argc, char* argv[])
{

  for(int i = 1; i < argc; i++) {
    FILE* file = fopen(argv[i],"r");
    if (!file) {
      fprintf(stderr,"%s fail to open %s(%s)\n", argv[0], argv[i], strerr(errno));
      return EXIT_FAILURE;
    } else {
      Repl(argv[i],file);
      fclose(file);
    }
  }
  if (argc == 1) repl(NULL,stdin);

  return EXIT_SUCCESS;
}
