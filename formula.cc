#include <vector>
#include <set>
#include <map>
#include <utility>
#include <string>
#include "token.cc"
using std::vector;
using std::set;
using std::pair;
using std::map;
using std::string;

class Formula;

enum VarType{
  Parameter, Function, Equation, Constant
};

class FormulaList{
  vector<Formula> formulaList;
  map<char,pair<int,VarType>> varMap;
};


class Formula{
  set<char> dependencies; // The set of all other formulas this is defined in terms of
  set<char> parameters; // The set of all parameters this is defined in terms of
  char name;
  char freeVar;
  VarType type;
  string formulaString;
  string prefix;

  vector<Token> tokenList;

  bool isValidName(char c){
    return ('a' <= c && c <= 'z') || ('A' <= c && c <= 'Z'); 
  }

  public:

    pair<char,VarType> updateString(string fullFormula){
      // Splits the formula string into the parts before and after the = sign
      for (size_t ind = 0, n = fullFormula.length(); ind < n; ind++){
        if (fullFormula[ind] == '='){
          prefix = string(fullFormula.begin(), fullFormula.begin() + ind);
          formulaString = string(fullFormula.begin() + ind, fullFormula.end());
        }
      }

      name = freeVar = 0;

      // Verifies the validity of the prefix and updates name/type based on it
      if (prefix.length() == 1){
        if (isValidName(prefix[0])){
          if (prefix[0] == 'x' || prefix[0] == 'y') {
            type = Equation;
            freeVar = prefix[0];
          }
          else if (formulaString.find('[') != string::npos) {
            type = Parameter;
            name = prefix[0];
          }
          else {
            type = Constant;
            name = prefix[0];
          }
          return {name,type};
        }
      }
      else if (prefix.length() == 4){
        if (isValidName(prefix[0]) && prefix[1] == '(' && (prefix[2] == 'x' || prefix[2] == 'y') && prefix[3] == ')'){
          name = prefix[0];
          type = Function;
          freeVar = prefix[2];
          return {name,type};
        }
      }

      throw("Invalid prefix");
    }

    void tokenize(const map<char,pair<int,VarType>>& varMap, const map<string,VarType>& preDefs){
      for (size_t ind = 0, n = formulaString.length(); ind < n; ind++){
        switch(formulaString[ind]){
          case '(': tokenList.push_back(Token{LPAR, "("}); break;
          case ')': tokenList.push_back(Token{RPAR, ")"}); break;
          case '{': tokenList.push_back(Token{LCURL, "{"}); break;
          case '}': tokenList.push_back(Token{RCURL, "}"}); break;
          case '[': tokenList.push_back(Token{LSQR, "["}); break;
          case ']': tokenList.push_back(Token{RSQR, "]"}); break;
          case ',': tokenList.push_back(Token{COMMA, ","}); break;
          case '+': tokenList.push_back(Token{PLUS, "+"}); break;
          case '-': tokenList.push_back(Token{NEG, "-"}); break;
          case '*': case '/':
            tokenList.push_back(Token{MD, string{formulaString[ind]}}); break;
          case '^': tokenList.push_back(Token{EXPON, "^"}); break;
          case '>': case '<': 
            if (ind < n-1 && formulaString[ind+1] == '='){
              tokenList.push_back(Token{INEQOP, formulaString.substr(ind, 2)});
              ind++;
            }
            else tokenList.push_back(Token{INEQOP, string{formulaString[ind]}});
            break;
          default:
            bool resolved = true;
            if (('0' <= formulaString[ind] && formulaString[ind] <= '9') || formulaString[ind] == '.'){
              size_t startInd = ind;
              bool seenDecimal = false;
              while (('0' <= formulaString[ind]&& formulaString[ind] <= '9') || formulaString[ind] == '.'){
                if (formulaString[ind] == '.'){
                  if (seenDecimal) {
                    resolved = false;
                    break;
                  }
                  seenDecimal = true;
                }
                ind++;
              }
              if (resolved) tokenList.push_back(Token{NUM, formulaString.substr(startInd, ind - startInd)});
            } 
            else{
              resolved = false;
              // Check first against pre-defined functions list
              for (auto def = preDefs.begin(); def != preDefs.end(); def++){
                if (def->first.length() + ind <= n && def->first == formulaString.substr(ind, def->first.length())){
                  if (def->second == Function) tokenList.push_back(Token{FNAME, def->first});
                  else tokenList.push_back(Token{VNAME, def->first});
                  ind += def->first.length()-1;
                  resolved = true;
                  break;
                }
              }
              // Check against user-defined functions and variables
              if (resolved) for (auto var = varMap.begin(); var != varMap.end(); var++){
                if (formulaString[ind] == var->first){
                  if (var->second.second == Function) tokenList.push_back(Token{FNAME, string{var->first}});
                  else tokenList.push_back(Token{VNAME, string{var->first}});
                  resolved = true;
                  break;
                }
              }
            }
            if (!resolved) throw "Unable to parse at " + std::to_string(ind);
        }
      }
    }
};
