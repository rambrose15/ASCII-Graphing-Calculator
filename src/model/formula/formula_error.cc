#include "formula_error.h"

std::string formulaErrorString(FormulaError es){
  {
    switch (es){
      case PREFIX: return "Invalid prefix";
      case TOKENIZATION: return "Use of invalid tokens";
      case PARSE: return "Parse failed";
      case CONSTANT_VIOLATION: return "Constant expression cannot be paramaterized";
      case PARAMETER_VIOLATION: return "Parameters canno be paramterized";
      case EXPRESSION_VIOLATION: return "Expression violation";
      case FUNCTION_VIOLATION: return "Function violation";
      case INVALID_DEPENDENCY: return "Depends on an invalid formula";
      case UNRESOLVABLE_DEPENDENCY: return "Unresolvable dependencies";
      case DNE_DEPENDENCY: return "Dependency does not exist";
      default: return "";
    }
  }
}
