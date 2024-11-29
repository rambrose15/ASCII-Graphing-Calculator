#include <string>

#include "formula.h"

using std::string;

class FormulaList{

  bool isValidName(char c){
    return ('a' <= c && c <= 'z') || ('A' <= c && c <= 'Z'); 
  }

  public:

  void updateFormula(int index, string fullFormula){
    // Splits the formula string into the parts before and after the = sign
    string prefix, formulaString;
    for (size_t ind = 0, n = fullFormula.length(); ind < n; ind++){
      if (fullFormula[ind] == '='){
        formulaString = string(fullFormula.begin() + ind + 1, fullFormula.end());
        break;
      }
      else if (!std::isspace(fullFormula[ind])) prefix += fullFormula[ind];
    }

    Formula* newFormula;

    // Verifies the validity of the prefix and updates name/type based on it
    if (prefix.length() == 1){
      if (isValidName(prefix[0])){
        if (prefix[0] == 'x' || prefix[0] == 'y') {
          newFormula = new Expression(prefix[0], formulaString);
        }
        else if (formulaString.find('[') != string::npos) {
          newFormula = new Parameter(prefix[0], formulaString);
        }
        else {
          newFormula = new Constant(prefix[0], formulaString);
        }
        return;
      }
    }
    else if (prefix.length() == 4){
      if (isValidName(prefix[0]) && prefix[1] == '(' && (prefix[2] == 'x' || prefix[2] == 'y') && prefix[3] == ')'){
        newFormula = new Function(prefix[0], formulaString, prefix[2]);
        return;
      }
    }

    throw string{"Invalid prefix"};
  }
};