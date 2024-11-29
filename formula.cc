#include <vector>
#include <set>
#include <map>
#include <utility>
#include <string>
#include <cctype>

#include "formula.h"
#include "parser.h"
#include "bigRational.h"

using std::vector, std::set, std::pair, std::map, std::string;

void Formula::tokenize(const map<char,pair<int,FormulaType>>& varMap, const map<string,FormulaType>& preDefs){
  for (size_t ind = 0, n = formulaString.length(); ind < n; ind++){
    if (isspace(formulaString[ind])) continue;
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
          size_t currentInd = ind;
          bool seenDecimal = false;
          while (('0' <= formulaString[currentInd]&& formulaString[currentInd] <= '9') || formulaString[currentInd] == '.'){
            if (formulaString[currentInd] == '.'){
              if (seenDecimal) {
                resolved = false;
                break;
              }
              seenDecimal = true;
            }
            currentInd++;
          }
          if (resolved) {
            tokenList.push_back(Token{NUM, formulaString.substr(ind, currentInd - ind)});
            ind = currentInd - 1;
          }
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
          if (!resolved) for (auto var = varMap.begin(); var != varMap.end(); var++){
            if (formulaString[ind] == var->first){
              if (var->second.second == Function) tokenList.push_back(Token{FNAME, string{var->first}});
              else tokenList.push_back(Token{VNAME, string{var->first}});
              resolved = true;
              break;
            }
          }

          // Check the free variable
          if (!resolved && isFreeVar(formulaString[ind])){
            tokenList.push_back(Token{VNAME, string{getFreeVar()}});
            resolved = true;
          }
        }
        if (!resolved) throw string{"Unrecognized sequence of characters: "} + formulaString.substr(ind);
    }
  }
  updateDependencies();
}

void Formula::getDependencies(){
  for (Token t : tokenList){
    if (t.type == FNAME || t.type == VNAME){
      dependencies.insert(t.lexeme[0]);
    }
  }
}

const Parser::ParseTree& Formula::parse(Parser parser){
  parseTree = parser.parseTokens(tokenList);
  return parseTree;
}
