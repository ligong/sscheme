#include "eval.h"

namespace sscheme {
// evaluate s-expression in env environment
Data Eval(Data sexp, Data env)
{
  return sexp;
}

// apply procedure proc to parameters
Data Apply(Data proc,Data parameters)
{
  return Data::null;
}


// bool IsSelfEval(const SExp& x);

// bool IsSelfEval(const SExp& x)
// {
//   return (x.type == kInt   || x.type == kBigNumber ||
//           x.type == kFloat || x.type == kString);
// }

// bool IsVariable(const SExp& x)
// {
//   return x.type == kSymbol;
// }

// Data LookUpVariable(const Data& var, const Data& env, const Memory* mem)
// {
//   for(;!IsNull(env);env = EnvironmentEnclosure(env)) {
//     const Data frame = EnvironmentFrame(env);
//     for(;!IsNull(frame);frame = Rest(frame)) {
//       const Data kv = First(frame);
//       if (Eq(var,Car(kv)))
//         return Cdr(kv);
//     }
//   }
//   return kNULL;
// }

// bool IsQuote(const SExp& x)
// {
//   return IsPair(x) && Eq(First(x),Symbol("quote"));
// }
  
// Data QuoteText(const SExp& x)
// {
//   return Rest(x);
// }

// bool IsLambda(const SExp& x)
// {
//   return IsPair(x) && Eq(First(x),Symbol("lambda"));
// }

// SExp LambdaParameters(const SExp& x)
// {
//   return Second(x);
// }

// SExp LambdaBody(const SExp& x)
// {
//   return Thrid(x);
// }

// Data MakeProcedure(const SExp& parameters,
//                    const SExp& body,
//                    const SExp& env,
//                    Memory* mem)
// {
//   return List(mem,parameters,body,env);
// }
                           
// bool IsAssign(const SExp& x)
// {
//   return false;
// }
// SExp AssignVariable(const SExp& x)
// {
//   return InvalidValue;
// }

// SExp AssignValue(const SExp& x)
// {
//   return InvalidValue;
// }

// bool IsDefine(const SExp& x)
// {
//   return false;
// }

// SExp DefineVariable(const SExp& x)
// {
//   return InvalidValue;
// }

// SExp DefineValue(const SExp& x)
// {
//   return InvalidValue;
// }

// bool IsCond(const SExp& x)
// {
//   return false;
// }
// SExp CondToIf(const SExp& x)
// {
//   return InvalidValue;
// }

// bool IsApp(const SExp& x)
// {
//   return InvalidValue;
// }

// Data EvalApp(const SExp& x)
// {
//   return InvalidValue;
// }



// Data Machine::Eval()
// {
//   if (IsSelfEval(exp))
//     val = exp;
//   else if (IsVariable(exp))
//     val = LookupVariable(exp,env);
//   else if (IsQuote(exp))
//     val = QuoteText(exp);
//   else if (IsLambda(exp))
//     val = MakeProcedure(LambdaParameters(exp),
//                         LambdaBody(exp),
//                         env,mem);
//   else if (IsAssign(exp))
//     val = EvalAssign(exp,env);
//   else if (IsDefinition(exp))
//     val = EvalDefinition(exp,env);
//   else if (IsCond(exp)) {
//     exp = CondToIf(exp);
//     val = Eval();
//   }
//   else if (IsApp(exp))
//     val = EvalApp(exp,env);
//   else
//     Error("Wrong s-expression");

//   return val;
// }

} // namespace sscheme
