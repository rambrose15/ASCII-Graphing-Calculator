#include <string>
#include <map>
#include <memory>
#include <vector>
#include <typeinfo>

#include "formulaList.h"

using std::string, std::map, std::unique_ptr, std::vector;

bool FormulaList::isValidName(char c){
  return ('a' <= c && c <= 'z') || ('A' <= c && c <= 'Z'); 
}

vector<int> FormulaList::circularCheck(){

  // 0: untouched, 1: in processing, 2: successful, 3: failed
  map<int,int> states;
  vector<int> sortedFormulaList;

  for (auto iter = formulaSet.begin(); iter != formulaSet.end(); ++iter){
    states[iter->first] = 0; // Intialize states to 0
  } 
  
  // Executes the sorting algorithm
  for (auto iter = formulaSet.begin(); iter != formulaSet.end(); ++iter){
    if (states[iter->first] != 0) continue;
    depthFirstSearch(iter->first, states, sortedFormulaList);
  }
  // Updates the error states of formulas with unresolvable dependencies
  for (auto iter = states.begin(); iter != states.end(); ++iter){
    if (iter->second == 3){ // If in failed state
      errorStatus[iter->first] = UNRESOLVABLE_DEPENDENCY;
    }
  }
  
  return sortedFormulaList;
}

bool FormulaList::depthFirstSearch(int vertex, map<int,int>& states, vector<int>& sortedList){
  if (states[vertex] == 1 || states[vertex] == 3) return false;
  else if (states[vertex] == 2) return true;
  
  states[vertex] = 1;
  for (auto dep : formulaSet[vertex]->getDependencies()){
    if (!depthFirstSearch(nameIndexMapping[dep], states, sortedList)){
      states[vertex] = 3;
      return false;
    }
  }
  sortedList.push_back(vertex);
  states[vertex] = 2;
  return true;
}

void FormulaList::createFormula(int index, const string& fullFormula){
  // Splits the formula string into the parts before and after the = sign
  string prefix, formulaString;
  for (size_t ind = 0, n = fullFormula.length(); ind < n; ind++){
    if (fullFormula[ind] == '='){
      formulaString = string(fullFormula.begin() + ind + 1, fullFormula.end());
      break;
    }
    else if (!std::isspace(fullFormula[ind])) prefix += fullFormula[ind];
  }

  errorStatus[index] = NONE;
  colourMapping[index] = WHITE;

  // Verifies the validity of the prefix and updates name/type based on it
  if (prefix.length() == 1){
    if (isValidName(prefix[0])){
      nameIndexMapping[prefix[0]] = index;
      if (prefix[0] == 'x' || prefix[0] == 'y') {
        formulaSet[index] = unique_ptr<Formula>(new Expression(prefix[0], formulaString));
        nameTypeMapping[prefix[0]] = EXPRESSION;
      }
      else if (formulaString.find('[') != string::npos) {
        formulaSet[index] = unique_ptr<Formula>(new Parameter(prefix[0], formulaString));
        nameTypeMapping[prefix[0]] = PARAMETER;
      }
      else {
        formulaSet[index] = unique_ptr<Formula>(new Constant(prefix[0], formulaString));
        nameTypeMapping[prefix[0]] = CONSTANT;
      }
      return;
    }
  }
  else if (prefix.length() == 4){
    if (isValidName(prefix[0]) && prefix[1] == '(' && (prefix[2] == 'x' || prefix[2] == 'y') && prefix[3] == ')'){
      nameIndexMapping[prefix[0]] = index;
      formulaSet[index] = unique_ptr<Formula>(new Function(prefix[0], formulaString, prefix[2]));
      nameTypeMapping[prefix[0]] = FUNCTION;
      return;
    }
  }

  errorStatus[index] = PREFIX;
}

void FormulaList::updateFormula(int index, const string& fullFormula){
  formulaSet.erase(index);
  nameIndexMapping.erase(index);
  createFormula(index, fullFormula);
  if (errorStatus[index] != NONE) return;
  formulaSet[index]->tokenize(nameTypeMapping, preDefs);

  for (auto iter = formulaSet.begin(); iter != formulaSet.end(); ++iter){
    // Update formula tokenizations with new variable changes
    if (iter->first == index) continue;
    errorStatus[iter->first] = NONE; 
    if (errorStatus[iter->first] != PREFIX){
      iter->second->tokenize(nameTypeMapping, preDefs);
    }
  }

  // Determine parameters
  vector<int> order = circularCheck();
  for (int i = 0, n = order.size(); i < n; i++){
    if (errorStatus[order[i]] != NONE) continue;
    for (int j = i+1; j < n; j++){
      if (errorStatus[order[j]] != NONE) continue;
      if (formulaSet[order[j]]->getDependencies().count(formulaSet[order[i]]->getName())){
        for (auto p : formulaSet[order[i]]->getParameters()){
          formulaSet[order[j]]->addParameter(p);
        }
      }
    }
  }

  // Check that formulas follow parameterization rules
  for (auto iter = formulaSet.begin(); iter != formulaSet.end(); ++iter){
    if (errorStatus[iter->first] != NONE) continue;
    errorStatus[iter->first] = iter->second->checkValidity();
  }

  // Check that all expressions are dependent on valid formulas and parse them
  for (auto ind : order){
    if (errorStatus[ind] != NONE) continue;
    for (auto d : formulaSet[ind]->getDependencies()){
      if (errorStatus[nameIndexMapping[d]] != NONE){
        errorStatus[ind] = INVALID_DEPENDENCY;
        break;
      } 
    }
    if (errorStatus[ind] == NONE){
      try{
        formulaSet[ind]->parse(parser);
      } catch (ParseError){
        errorStatus[ind] = PARSE;
      }
    }
  }
}

BigRational FormulaList::computeValueFromTree(const Parser::ParseTree& tree, const BigRational& varInput, char freeVar){
  if (std::holds_alternative<unique_ptr<Token>>(tree)){
    Token* tk = std::get<unique_ptr<Token>>(tree).get();
    switch(tk->type){
      case NUM:
        return BigRational(tk->lexeme);
      case VNAME:
        if (string{freeVar} == tk->lexeme) return varInput;
        else return computeValue(nameIndexMapping[tk->lexeme[0]]);
      default:
        throw ComputeError{};
    }
  } else{
    TokenTree* tt = std::get<unique_ptr<TokenTree>>(tree).get();
    switch(tt->type){
      case EXPR: case TERM: case FACTOR:
        if (tt->tree.size() == 1) return computeValueFromTree(tt->tree[0], varInput, freeVar);
        else {
          Token* op = std::get<unique_ptr<Token>>(tt->tree[1]).get();
          BigRational lSide = computeValueFromTree(tt->tree[0], varInput, freeVar);
          BigRational rSide = computeValueFromTree(tt->tree[2], varInput, freeVar);
          switch (op->type){
            case PLUS: return lSide + rSide;
            case NEG: return lSide - rSide;
            case MD:
              if (op->lexeme == "*") return lSide * rSide;
              else if (op->lexeme == "/") return lSide / rSide;
            case EXPON: return lSide ^ rSide;
            default: throw ComputeError{};
          }
        }
      case POWER:
        if (tt->tree.size() == 1) computeValueFromTree(tt->tree[0], varInput, freeVar);
        else if (tt->tree.size() == 2){
          return - computeValueFromTree(tt->tree[1], varInput, freeVar);
        }
        else throw ComputeError{};
      case UNSIGNED:
        if (tt->tree.size() == 1) computeValueFromTree(tt->tree[0], varInput, freeVar);
        else if (tt->tree.size() == 3) computeValueFromTree(tt->tree[1], varInput, freeVar);
        else if (tt->tree.size() == 4){
          char func = std::get<unique_ptr<Token>>(tt->tree[0]).get()->lexeme[0];
          return computeValue(nameIndexMapping[func], computeValueFromTree(tt->tree[2], varInput, freeVar));
        }
        else throw ComputeError{};
      case START:
        return computeValueFromTree(tt->tree[0], varInput, freeVar);
      default:
        throw ComputeError{};
    }
  }
}

BigRational FormulaList::computeValue(int index, const BigRational& varInput){
  FormulaType fType = nameTypeMapping[formulaSet[index]->getName()];
  if (fType == CONSTANT){
    Constant *c = dynamic_cast<Constant*>(formulaSet[index].get());
    if (c->isUpdated()) return c->getValue();
    else{
      BigRational newValue = computeValueFromTree(c->getParseTree());
      c->updateValue(newValue);
      return newValue;
    }
  } else if (fType == PARAMETER){
    Parameter* p = dynamic_cast<Parameter*>(formulaSet[index].get());
    if (p->isUpdated()) p->getValue();
    else{
      vector<Parser::ParseTree*> subTrees = parser.getComponents(p->getParseTree());
      if (subTrees.size() == 2 || subTrees.size() == 3){
        BigRational lBound = computeValueFromTree(*subTrees[0]);
        BigRational rBound = computeValueFromTree(*subTrees[1]);
        if (subTrees.size() == 2) p->updateValues(lBound, rBound);
        else{
          BigRational speed = computeValueFromTree(*subTrees[2]);
          p->updateValues(lBound, rBound, speed);
        }
      } else throw ComputeError{};
      return p->getValue();
    }
  } else if (fType == EXPRESSION){
    Expression* exp = dynamic_cast<Expression*>(formulaSet[index].get());
    return computeValueFromTree(exp->getParseTree(), varInput, exp->getFreeVar());
  } else if (fType == FUNCTION){
    Function* exp = dynamic_cast<Function*>(formulaSet[index].get());
    return computeValueFromTree(exp->getParseTree(), varInput, exp->getFreeVar());
  }
  throw ComputeError{};
}

GraphPackage FormulaList::getGraphs(int rLen, int cLen, BigRational coordXL, BigRational coordXR, 
  BigRational coordYL, BigRational coordYR){
    GraphPackage result;
    for (auto formula = formulaSet.begin(); formula != formulaSet.end(); formula++){
      FormulaType fType = nameTypeMapping[formula->second->getName()];
      if (fType != FUNCTION && fType != EXPRESSION) continue;
      string graph; vector<int> positions;
      bool xfunc = formula->second->getFreeVar() == 'x';
      int maxInd = (xfunc ? cLen : rLen);
      BigRational prevVal;
      bool validPrev = false;
      for (int ind = 0; ind <= maxInd; ++ind){
        BigRational nextVal;
        bool validNext = true;
        bool outofBounds = false;
        try{
          if (xfunc) nextVal = computeValue(formula->first, 
            coordXL + ((coordXR - coordXL) * BigRational(std::to_string((double)ind/(double)maxInd))));
          else nextVal = computeValue(formula->first, 
            coordYL + ((coordYR - coordYL) * BigRational(std::to_string((double)ind/(double)maxInd))));
        } catch(ComputeError){
          validNext = false;
        }
        if (validNext){
          if (xfunc && (nextVal < coordYL || coordYR < nextVal)) outofBounds = true;
          else if (!xfunc && (nextVal < coordXL || coordXR < nextVal)) outofBounds = true;
        }
        int pos = 0; char ch = ' ';
        int ceilingIndex = (xfunc ? rLen : cLen);
        if (validPrev && validNext){
          BigRational halfsq = (xfunc ? coordYR - coordYL : coordXR - coordXL) / BigRational(std::to_string(ceilingIndex*2));
          for (; pos < ceilingIndex && 
          (xfunc ? coordYL : coordXL)+(BigRational(std::to_string((pos+1)*2))*halfsq) < prevVal; ++pos);
          if (prevVal < nextVal){
            if (nextVal < prevVal + halfsq) ch = '.';
            else ch = '/';
          } else{
            if (prevVal < nextVal + halfsq) ch = '.';
            else ch = '\\';
          }
        }
        if (ind != 0){
          graph += ch; positions.push_back(pos);
        }
        if (validNext && !outofBounds) prevVal = nextVal;
        validPrev = validNext && !outofBounds;
      }
      if (xfunc) {
        result.xStrings.push_back(graph);
        result.xFuncIndices.push_back(formula->first);
        result.xFuncPositions.push_back(positions);
      } else {
        result.yStrings.push_back(graph);
        result.yFuncIndices.push_back(formula->first);
        result.yFuncPositions.push_back(positions);
      }
    }
    return result;
}