#include <vector>
#include <set>
#include <map>
#include <utility>
#include <string>
#include <cctype>
#include <chrono>
#include <ctime>
#include <math.h>

#include "formula.h"

using std::vector, std::set, std::pair, std::map, std::string;

bool Formula::tokenize(const map<char,FormulaType>& varMap, const map<string,FormulaType>& preDefs){
  tokenList = {};
  // Processes the formulaString into a sequence of tokens
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
              if (def->second == FUNCTION) tokenList.push_back(Token{FNAME, def->first});
              else tokenList.push_back(Token{VNAME, def->first});
              ind += def->first.length()-1;
              resolved = true;
              break;
            }
          }
          // Check against user-defined functions and variables
          if (!resolved) for (auto var = varMap.begin(); var != varMap.end(); var++){
            if (formulaString[ind] == var->first){
              if (var->second == FUNCTION) tokenList.push_back(Token{FNAME, string{var->first}});
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
        if (!resolved) return false;
    }
  }
  
  // Get dependency and parameters list from the newly created token list
  for (Token t : tokenList){
    if (t.type == FNAME || t.type == VNAME){
      if (preDefs.find(t.lexeme) == preDefs.end() && t.lexeme != string{getFreeVar()}) { // If not pre-defined and not the free variable
        dependencies.insert(t.lexeme[0]);
        if (varMap.at(t.lexeme[0]) == PARAMETER) parameters.insert(t.lexeme[0]);
      }
    }
  }

  return true;
}

const Parser::ParseTree& Formula::parse(Parser parser){
  parseTree = parser.parseTokens(tokenList);
  return parseTree;
}

const Parser::ParseTree& Formula::getParseTree() { return parseTree; }

vector<Colour> Formula::getColouring(){
  vector<Colour> result;
  vector<int> parens, curls, sqrs;
  Colour nextColour;
  for (int ind = 0, n = tokenList.size(); ind < n; ind++){
    TokenType tt = tokenList[ind].type;
    switch (tt){
      case NUM: nextColour = GREEN; break;
      case VNAME: nextColour = CYAN; break;
      case FNAME: nextColour = BLUE; break;
      case PLUS: case NEG: case MD: case EXPON: case INEQOP:
        nextColour = YELLOW; break;
      case LPAR: case LSQR: case LCURL:
        if (tokenList[ind].type == LPAR) parens.push_back(ind);
        else if (tokenList[ind].type == LSQR) sqrs.push_back(ind);
        else curls.push_back(ind);
        nextColour = RED; break;
      case RPAR: case RSQR: case RCURL:
        if ((tt==RPAR && parens.empty()) || (tt==RSQR && sqrs.empty()) || (tt==RCURL && curls.empty()))
          nextColour = RED;
        else {
          nextColour = WHITE;
          if (tt == RPAR) {
            result[parens.back()] = WHITE; parens.pop_back();
          } else if (tt == RSQR) {
            result[sqrs.back()] = WHITE; sqrs.pop_back();
          } else {
            result[curls.back()] = WHITE; curls.pop_back();
          }
        } break;
      default: nextColour = WHITE; break;
    }
    vector<Colour> nextColours(tokenList[ind].lexeme.length(), nextColour);
    result.insert(result.end(), nextColours.begin(), nextColours.end());
  }
  return result;
}

FormulaError Constant::checkValidity() {
  upToDate = false;
  if (!getParameters().empty()) return CONSTANT_VIOLATION;
  return NONE;
}

FormulaError Parameter::checkValidity() {
  defined = false;
  upToDate = false;
  if (!getParameters().empty()) return PARAMETER_VIOLATION;
  return NONE;
}

BigRational Parameter::getValue(){ 
  if (!upToDate) computeValue();
  return currentValue;
}

void Parameter::updateValues(const BigRational& newL, const BigRational& newR, const BigRational& newSpeed){
  lBound = newL; rBound = newR; speed = newSpeed;
  upToDate = false;
  defined = true;
}

void Parameter::advanceStep(int stepSize){
  currentStep = (currentStep + stepSize) % MAX_STEP;
  upToDate = false;
}

void Parameter::computeValue(){
  currentValue = lBound + ((rBound - lBound) * BigRational(std::to_string((double)currentStep / (double)MAX_STEP)));
  upToDate = true;
}

void Parameter::reset(){
  currentStep = 0;
  currentValue = lBound;
  upToDate = true;
}

FormulaError Expression::checkValidity() {
  return NONE;
}

FormulaError Function::checkValidity() {
  return NONE;
}
