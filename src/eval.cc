#include "eval.h"
#include "memory.h"
#include "printer.h"
#include "reader.h"

namespace sscheme {

static void EnvironmentSetVar(Data var, Data val, Data env);
static void EnvironmentDefineVar(Data var, Data val, Data env);
static Data EvalOperands();
static bool IsEnvironmentEmpty(Data env);
static Data EnvironmentFrame(Data env);
static Data FrameVars(Data frame);
static Data FrameVals(Data frame);
static Data EnclosingEnvironment(Data env);
static Data ExtendEnvironment(Data parameters, Data operands, Data env);
static Data LookupEnvironment(Data var, Data env);

static bool IsSelfEval(Data exp)
{
  return (exp.IsNumber() || exp.IsString()) || exp.IsNull();
}

static bool IsVariable(Data exp)
{
  return exp.IsSymbol();
}

// check if list starts with symbol 
static inline bool StartWith(Data list, const char* symbol)
{
  return (list.IsPair() &&
          FIRST(list).IsSymbol() &&
          FIRST(list).Symbol() == symbol);
}
    
// quote expression
static bool IsQuote(Data exp)
{
  static const char* quote = Symbol::New("quote");
  return StartWith(exp,quote);
}

static Data QuoteText(Data exp)
{
  return SECOND(exp);
}

// lambda expression
static bool IsLambda(Data exp)
{
  static const char* lambda = Symbol::New("lambda");
  return StartWith(exp,lambda);
}

static Data LambdaParameters(Data exp)
{
  return SECOND(exp);
}

static Data LambdaBody(Data exp)
{
  return REST(REST(exp));
}

static Data MakeProcedure(Data parameters, Data body, Data env)
{
  return LIST(parameters,body,env);
}

static Data ProcedureParameters(Data proc)
{
  return FIRST(proc);
}

static Data ProcedureBody(Data proc)
{
  return SECOND(proc);
}

static Data ProcedureEnvironment(Data proc)
{
  return THIRD(proc);
}

// assign expression
static bool IsAssign(Data exp)
{
  static const char* kwset = Symbol::New("set!");
  return StartWith(exp,kwset);
}

static Data AssignVar(Data exp)
{
  return SECOND(exp);
}

static Data AssignValue(Data exp)
{
  return THIRD(exp);
}

static Data EvalAssign()
{
  Data& exp = g_machine.exp;
  Data& env = g_machine.env;
  Data& val = g_machine.val;
  Data& unev = g_machine.unev;
  
  unev = AssignVar(exp);
  g_machine.Push(env);
  exp = AssignValue(exp);
  val = Eval();
  env = g_machine.Pop();
  EnvironmentSetVar(unev,val,env);
  
  return val;
}

static Data MakeLambda(Data args, Data body)
{
  static Data lambda = Memory::NewSymbol(Symbol::New("lambda"));
  Data& unev = g_machine.unev;
  g_machine.Push(unev);
  unev = CONS(args,body);
  unev = CONS(lambda,unev);
  Data result = unev;
  unev = g_machine.Pop();
  return result;
}

static bool IsDefine(Data exp)
{
  static const char* define = Symbol::New("define");
  return StartWith(exp,define);
}


static Data DefineVar(Data exp)
{
  Data second = SECOND(exp);
  if (second.IsPair()) 
    return FIRST(second);
  else
    return second;
}

static Data DefineValue(Data exp)
{
  Data second = SECOND(exp);

  if (second.IsPair())
    return MakeLambda(REST(second),REST(REST(exp)));
  else
    return THIRD(exp);
}

static Data EvalDefine()
{
  Data& exp = g_machine.exp;
  Data& env = g_machine.env;
  Data& val = g_machine.val;
  Data& unev = g_machine.unev;

  unev = DefineVar(exp);
  g_machine.Push(env);
  exp = DefineValue(exp);
  val = Eval();
  env = g_machine.Pop();
  EnvironmentDefineVar(unev,val,env);
  return val;
}

static bool IsSingleton(Data list)
{
  return list.IsPair() && CDR(list).IsNull();
}

static bool IsLastAction(Data exp)
{
  return IsSingleton(exp);
}

static Data EvalSequence()
{
  Data& exp = g_machine.exp;
  Data& env = g_machine.env;
  Data& unev = g_machine.unev;

  if (exp.IsNull()) {
    return exp;
  } else {
    unev = exp;
    while (!IsLastAction(unev)) {
      g_machine.Push(unev);
      g_machine.Push(env);
      exp = FIRST(unev);
      Eval();
      env = g_machine.Pop();
      unev = g_machine.Pop();
      unev = REST(unev);
    }
    exp = FIRST(unev);
    
    // modern compiler should recognize tail recursive call
    return Eval();  
  }
}

static Data CondClauses(Data exp)
{
  return REST(exp);
}

static Data FirstClause(Data clauses)
{
  return FIRST(clauses);
}

static Data RestClauses(Data clauses)
{
  return REST(clauses);
}

static bool IsBegin(Data exp)
{
  static const char* begin = Symbol::New("begin");
  return StartWith(exp,begin);
}

static Data BeginActions(Data exp)
{
  return REST(exp);
}

static Data MakeBegin(Data actions)
{
  static Data begin = Memory::NewSymbol(Symbol::New("begin"));
  return CONS(begin,actions);
}

static Data MakeIf(Data cond, Data consequence, Data alternative)
{
  static Data kwif = Memory::NewSymbol(Symbol::New("if"));

  return LIST(kwif,cond,consequence,alternative);
}

static bool IsIf(Data exp)
{
  static const char* kwif = Symbol::New("if");
  return StartWith(exp,kwif);
}



static Data IfCond(Data exp)
{
  return SECOND(exp);
}

static Data IfConsequence(Data exp)
{
  return THIRD(exp);
}

static Data IfAlternative(Data exp)
{
  return FOURTH(exp);
}

static bool IsCond(Data exp)
{
  static const char* cond = Symbol::New("cond");
  return StartWith(exp,cond);
}

static Data ClauseCond(Data clause)
{
  return FIRST(clause);
}

static Data ClauseActions(Data clause)
{
  return REST(clause);
}

static Data ExpandCondClauses(Data clauses)
{
  Data& exp = g_machine.exp;
  Data& unev  = g_machine.unev;
  Data& arg1 = g_machine.arg1;

  static Data else1 = Memory::NewSymbol(Symbol::New("else"));
  
  exp = clauses;
  if (exp.IsNull()) {
    return Data::null;
  } else {
    unev = FirstClause(exp);
    if (ClauseCond(unev) == else1)
      return MakeBegin(ClauseActions(unev));
    else {
      g_machine.Push(unev);
      exp = ExpandCondClauses(RestClauses(exp));
      unev = g_machine.Pop();
      arg1 = MakeBegin(ClauseActions(unev));
      unev = ClauseCond(unev);
      return MakeIf(unev,arg1, exp);
    }
  }
}

// Convert cond expression to if expression
static Data CondToIf()
{
  Data& exp = g_machine.exp;
  return exp = ExpandCondClauses(CondClauses(exp));
}

static bool IsTrue(Data val)
{
  return !val.IsNull();
}

static Data EvalIf()
{
  Data& exp = g_machine.exp;
  Data& env = g_machine.env;
  Data& val = g_machine.val;
  
  g_machine.Push(exp);
  g_machine.Push(env);
  
  exp = IfCond(exp);
  val = Eval();

  env = g_machine.Pop();
  exp = g_machine.Pop();

  if (IsTrue(val)) {
    exp = IfConsequence(exp);
  } else
    exp = IfAlternative(exp);
  
  return Eval();
}

static bool IsApp(Data exp)
{
  return exp.IsPair();
}

static Data EvalApp()
{
  Data& exp  = g_machine.exp;
  Data& env  = g_machine.env;
  Data& proc = g_machine.proc;
  Data& arg1 = g_machine.arg1;
  
  g_machine.Push(exp);
  g_machine.Push(env);

  // evaluate operator into proc
  exp = FIRST(exp); 
  proc = Eval();
  
  env = g_machine.Pop();
  exp = g_machine.Pop();

  // evaluate operands into arg1
  exp = REST(exp);
  // do not save env,since apply does not need it
  g_machine.Push(proc);
  arg1 = EvalOperands();
  proc = g_machine.Pop();

  // apply operator in proc to operands in arg1
  return Apply();
}

static Data g_empty_environment;

// Look up var in current environment
// If not found, return Data::none
static Data LookupEnvironment(Data var, Data env)
{
  
  while (!IsEnvironmentEmpty(env)) {
    Data frame = EnvironmentFrame(env);
    Data vars = FrameVars(frame);
    Data vals = FrameVals(frame);
    
    while (!vars.IsNull()) {
      if (var == FIRST(vars)) {
        return FIRST(vals);
      }
      vars = REST(vars);
      vals = REST(vals);
      assert((vars.IsNull() && vals.IsNull()) ||
             (!vars.IsNull() && !vals.IsNull()));
    }
    env = EnclosingEnvironment(env);
  }

  return Data::none;
}
  

static Data ExtendEnvironment(Data parameters, Data operands, Data env)
{
  g_machine.Push(env);
  Data frame = CONS(parameters,operands);
  env = g_machine.Pop();
  return CONS(frame,env);
}

static Data EnvironmentFrame(Data env)
{
  return FIRST(env);
}

static Data EnclosingEnvironment(Data env)
{
  return REST(env);
}

static Data FrameVars(Data frame)
{
  return CAR(frame);
}

static Data FrameVals(Data frame)
{
  return CDR(frame);
}

static bool IsEnvironmentEmpty(Data env)
{
  return env.IsNull();
}

static void EnvironmentSetVar(Data var, Data val, Data env)
{
  while (!IsEnvironmentEmpty(env)) {
    
    Data frame = EnvironmentFrame(env);
    Data vars = FrameVars(frame);
    Data vals = FrameVals(frame);

    while (!vars.IsNull()) {
      if (var == FIRST(vars)) {
        SETCAR(vals,val);
        return;
      }
      vars = REST(vars);
      vals = REST(vals);
      assert((vars.IsNull() && vals.IsNull()) ||
             (!vars.IsNull() && !vals.IsNull()));
    }
    env = EnclosingEnvironment(env);
  }

  assert(var.IsSymbol());
  ERROR("Unknown variable:%s",var.Symbol());
}

static void FrameSet(Data vars, Data vals, Data frame)
{
  SETCAR(frame,vars);
  SETCDR(frame,vals);
}

static void EnvironmentDefineVar(Data var, Data val, Data env)
{
  assert(!IsEnvironmentEmpty(env));

  // utilize g_machine's register to meet 
  // garbage collection requirement
  Data& frame = g_machine.exp;
  Data& vars = g_machine.unev;
  Data& vals = g_machine.arg1;
  Data& new_val = g_machine.proc;
  
  frame = EnvironmentFrame(env);
  vars = FrameVars(frame);
  vals = FrameVals(frame);
  new_val = val;

  while (!vars.IsNull()) {
    if (var == FIRST(vars)) {
      SETCAR(vals,new_val);
      return;
    }
    vars = REST(vars);
    vals = REST(vals);
    assert((vars.IsNull() && vals.IsNull()) ||
           (!vars.IsNull() && !vals.IsNull()));
  }
  
  vals = CONS(new_val,vals);
  vars = CONS(var,vars);
  FrameSet(vars,vals,frame);
}

static bool IsLastOperand(Data operands)
{
  return IsSingleton(operands);
}

static Data EvalOperands()
{
  Data& exp  = g_machine.exp;
  Data& arg1 = g_machine.arg1;
  Data& env  = g_machine.env;
  Data& val  = g_machine.val;

  if (exp.IsNull()) {
    return Data::null;
  } else {
    
    arg1 = Data::null;
    
    while (!IsLastOperand(exp)) {
      g_machine.Push(env);
      g_machine.Push(arg1);
      g_machine.Push(exp);
      exp = FIRST(exp);
      val = Eval();
      exp = g_machine.Pop();
      arg1 = g_machine.Pop();
      env = g_machine.Pop();
      arg1 = CONS(val,arg1);
      exp = REST(exp);
    }

    // tail expression in list
    g_machine.Push(arg1);
    exp = FIRST(exp);
    val = Eval();
    arg1 = g_machine.Pop();
    arg1 = CONS(val,arg1);
    arg1 = Memory::ReverseList(arg1);
    return arg1;
  }
}

// retrieve number from arg, multiply sign and accumulate to d or f based on its type
// if it is float, set is_float

static void AddNum(Data arg, int sign, long* d, float* f, bool* is_float)
{
  if (arg.IsInt())
    *d += sign*arg.Int();
  else if (arg.IsFloat()) {
    *f += sign*arg.Float();
    *is_float = true;
  } else {
    ERROR("Wrong data type in arithmatic operation");
  }
}


static void MulNum(Data arg, long* d, float* f, bool* is_float)
{
  if (arg.IsInt())
    *d *= arg.Int();
  else if (arg.IsFloat()) {
    *f *= arg.Float();
    *is_float = true;
  } else {
    ERROR("Wrong data type in arithmatic operation");
  }
}

static void DivNum(Data arg,float* f)
{
  if (arg.IsInt())
    *f /= arg.Int();
  else if (arg.IsFloat()) {
    *f /= arg.Float();
  } else {
    ERROR("Wrong data type in arithmatic operation");
  }
}

static Data PrimAdd(Data args)
{
  bool is_float = false;
  float f = 0;
  long i = 0;

  while(!args.IsNull()) {
    AddNum(FIRST(args),1,&i,&f,&is_float);
    args = REST(args);
  }

  if (is_float)
    return Memory::NewFloat(f+i);
  else
    return Memory::NewInt(i);
}

static Data PrimSub(Data args)
{
  bool is_float = false;
  float f = 0;
  long i = 0;

  if (args.IsNull() || !REST(REST(args)).IsNull())
    ERROR("sub operation: parameters wrong");
  if (REST(args).IsNull()) {
    AddNum(FIRST(args),-1,&i,&f,&is_float);
  } else {
    AddNum(FIRST(args),1,&i,&f,&is_float);
    AddNum(SECOND(args),-1,&i,&f,&is_float);
  }

  if (is_float)
    return Memory::NewFloat(f+i);
  else
    return Memory::NewInt(i);
}

static Data PrimMul(Data args)
{
  bool is_float = false;
  float f = 1;
  long i = 1;

  while(!args.IsNull()) {
    MulNum(FIRST(args),&i,&f,&is_float);
    args = REST(args);
  }

  if (is_float)
    return Memory::NewFloat(f*i);
  else
    return Memory::NewInt(i);
}

static Data PrimDiv(Data args)
{
  float f = 1;
  long i = 1;
  bool is_float;

  if (args.IsNull() || !REST(REST(args)).IsNull())
    ERROR("div operation: parameters wrong");
  if (REST(args).IsNull()) {
    DivNum(FIRST(args),&f);
  } else {
    MulNum(FIRST(args),&i,&f,&is_float);
    f *= i;
    DivNum(SECOND(args),&f);
  }

  return Memory::NewFloat(f);
}

static Data PrimEq(Data args)
{
  if (LENGTH(args) != 2)
    ERROR("eq operation: parameters wrong");
  
  if (FIRST(args) == SECOND(args))
    return Data::t;
  else
    return Data::f;
}


static Data PrimStringEqP(Data args)
{
  if (LENGTH(args) != 2 ||
      !FIRST(args).IsString() || !SECOND(args).IsString())
    ERROR("string= operation: parameters wrong");
  if (strcmp(FIRST(args).String(),SECOND(args).String()) == 0)
    return Data::t;
  else
    return Data::f;
}

static Data PrimAtomP(Data args)
{
  if (LENGTH(args) != 1)
    ERROR("atom? operation: parameters wrong");
  
  if (FIRST(args).IsAtom())
    return Data::t;
  else
    return Data::f;
}

static Data PrimPairP(Data args)
{
  if (LENGTH(args) != 1)
    ERROR("pair? operation: parameters wrong");
  if (FIRST(args).IsPair())
    return Data::t;
  else
    return Data::f;
}

static Data PrimStringP(Data args)
{
  if (LENGTH(args) != 1)
    ERROR("string? operation: parameters wrong");
  if (FIRST(args).IsString())
    return Data::t;
  else
    return Data::f;
}

static Data PrimNumberP(Data args)
{
  if (LENGTH(args) != 1)
    ERROR("number? operation: parameters wrong");
  if (FIRST(args).IsNumber())
    return Data::t;
  else
    return Data::f;
}

void InitialEnvironment()
{
  Data& vars = g_machine.exp;
  Data& vals = g_machine.unev;

  g_empty_environment = Data::null;
  
#define S(x) Memory::NewSymbol(Symbol::New(x))
#define P(x) Memory::NewProc(x)
  vars = LIST(S("+"),S("-"),S("*"),S("/"),
              S("eq"),S("="),
              S("string?"),S("string=?"),S("number?"),S("atom?"),
              S("pair?"));
           
  vals = LIST(P(PrimAdd),P(PrimSub),P(PrimMul),P(PrimDiv),
              P(PrimEq),P(PrimEq),
              P(PrimStringP),P(PrimStringEqP),
              P(PrimNumberP),P(PrimAtomP), P(PrimPairP));
  g_machine.env = ExtendEnvironment(vars, vals, g_empty_environment);
}

Data Apply()
{
  Data& proc = g_machine.proc;
  Data& arg1 = g_machine.arg1;
  Data& unev = g_machine.unev;
  Data& exp = g_machine.exp;
  Data& env = g_machine.env;
  
  if (proc.IsPrimApp())
    return proc.PrimApp()(arg1);
  else {
    exp = ProcedureBody(proc);
    unev = ProcedureParameters(proc);
    env = ProcedureEnvironment(proc);
    env = ExtendEnvironment(unev,arg1,env);
    return EvalSequence();
  }
}

// evaluate g_machine.exp under g_machine.env
// To meet garbage collector's requirement,
// the implementation use g_machine's register
// to store all the intermediate result.

Data Eval()
{
  Data& exp = g_machine.exp;
  Data& env = g_machine.env;
  Data& val = g_machine.val;
  
  if (IsSelfEval(exp))
    val = exp;
  else if (IsVariable(exp)) {
    val = LookupEnvironment(exp,env);
    if (val == Data::none)
      ERROR("undefined var");
  } else if (IsQuote(exp))
    val = QuoteText(exp);
  else if (IsLambda(exp))
    val = MakeProcedure(LambdaParameters(exp),
                        LambdaBody(exp),
                        env);
  else if (IsAssign(exp))
    val = EvalAssign();
  else if (IsDefine(exp))
    val = EvalDefine();
  else if (IsIf(exp))
    val = EvalIf();
  else if (IsBegin(exp)) {
    exp = BeginActions(exp);
    val = EvalSequence();
  } else if (IsCond(exp)) {
    exp = CondToIf();
    val = Eval();
  } else if (IsApp(exp))
    val = EvalApp();
  else
    ERROR("Wrong s-expression");
  
  return val;
}

Data Eval(const char* str)
{
  Data& exp = g_machine.exp;
  exp = Read(str);
  return Eval();
}

} // namespace sscheme
