#include <string>
#include <map>
#include <memory>
#include <vector>
#include <typeinfo>

#include "formulaList.h"

using std::string, std::map, std::unique_ptr, std::vector, std::set;

bool FormulaList::isValidName(char c){
  return ('a' <= c && c <= 'z') || ('A' <= c && c <= 'Z'); 
}

Colour FormulaList::getColour(int index) { 
  if (colourMapping.find(index) != colourMapping.end()) return colourMapping[index];
  return Colour::WHITE;
}

bool FormulaList::setColour(int index, Colour newColour){
  if (colourMapping.find(index) != colourMapping.end()) {
    colourMapping[index] = newColour;
    return true;
  }
  return false;
}

vector<Colour> FormulaList::getFormulaColouring(int index){
  if (formulaSet.find(index) != formulaSet.end() && errorStatus[index] == NONE) {
    vector<Colour> prefixColouring;
    if (nameTypeMapping[formulaSet[index]->getName()] == FUNCTION){
      prefixColouring = {BLUE, WHITE, CYAN, WHITE, YELLOW};
    } else prefixColouring = {CYAN, YELLOW};
    vector<Colour> formulaColouring = formulaSet[index]->getColouring();
    prefixColouring.insert(prefixColouring.end(), formulaColouring.begin(), formulaColouring.end());
    return prefixColouring;
  }
  else return {};
}

FormulaError FormulaList::getErrorStatus(int index){
  if (errorStatus.find(index) != errorStatus.end()) return errorStatus[index];
  return NONE;
}

bool FormulaList::isParameter(int index){
  return formulaSet.find(index) != formulaSet.end() &&
    nameTypeMapping[formulaSet[index]->getName()] == PARAMETER;
}

bool FormulaList::isFunction(int index){
  return formulaSet.find(index) != formulaSet.end() &&
    nameTypeMapping[formulaSet[index]->getName()] == FUNCTION;
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
  colourMapping.insert({index, Colour::WHITE});

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
  if (formulaSet.find(index) != formulaSet.end()) {
    nameIndexMapping.erase(formulaSet[index]->getName());
    nameTypeMapping.erase(formulaSet[index]->getName());
  }
  formulaSet.erase(index);
  errorStatus.erase(index);
  colourMapping.erase(index);
  if (fullFormula.empty()) return;
  createFormula(index, fullFormula);
  if (errorStatus[index] != NONE) return;
  if (!formulaSet[index]->tokenize(nameTypeMapping, preDefs)){
    errorStatus[index] = TOKENIZATION;
  }

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
        else if (preDefs.find(tk->lexeme) != preDefs.end()) return computePredefinedVar(tk->lexeme);
        else return computeValue(nameIndexMapping[tk->lexeme[0]]);
      default:
        throw ComputeError{};
    }
  } else{
    TokenTree* tt = std::get<unique_ptr<TokenTree>>(tree).get();
    switch(tt->type){
      case EXPR: case TERM: case FACTOR:
        if (tt->tree.size() == 1) return computeValueFromTree(tt->tree[0], varInput, freeVar);
        else if (tt->tree.size() == 2){
          BigRational lSide = computeValueFromTree(tt->tree[0], varInput, freeVar);
          BigRational rSide = computeValueFromTree(tt->tree[1], varInput, freeVar);
          return lSide * rSide;
        }
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
              else throw ComputeError{};
            case EXPON: return lSide ^ rSide;
            default: throw ComputeError{};
          }
        }
      case POWER:
        if (tt->tree.size() == 1) return computeValueFromTree(tt->tree[0], varInput, freeVar);
        else if (tt->tree.size() == 2){
          return - computeValueFromTree(tt->tree[1], varInput, freeVar);
        }
        else throw ComputeError{};
      case UNSIGNED:
        if (tt->tree.size() == 1) return computeValueFromTree(tt->tree[0], varInput, freeVar);
        else if (tt->tree.size() == 3) return computeValueFromTree(tt->tree[1], varInput, freeVar);
        else if (tt->tree.size() == 4){
          string funcName = std::get<unique_ptr<Token>>(tt->tree[0]).get()->lexeme;
          if (preDefs.find(funcName) != preDefs.end()){
            return computePredefinedFunction(funcName, computeValueFromTree(tt->tree[2], varInput, freeVar));
          }
          else return computeValue(nameIndexMapping[funcName[0]], computeValueFromTree(tt->tree[2], varInput, freeVar));
        }
        else throw ComputeError{};
      case START:
        if (tt->tree.size() == 4){ // An inequality is at play
          if (!computeIneq(tt->tree[2], varInput, freeVar)) throw ComputeError{};
        }
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
    if (!p->isDefined()){
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
    }
    return p->getValue();
  } else if (fType == EXPRESSION){
    Expression* exp = dynamic_cast<Expression*>(formulaSet[index].get());
    return computeValueFromTree(exp->getParseTree(), varInput, exp->getFreeVar());
  } else if (fType == FUNCTION){
    Function* exp = dynamic_cast<Function*>(formulaSet[index].get());
    return computeValueFromTree(exp->getParseTree(), varInput, exp->getFreeVar());
  }
  throw ComputeError{};
}

BigRational FormulaList::computePredefinedVar(const string& name){
  if (name == "PI"){
    return BigRational("3.14159");
  } else if (name == "E"){
    return BigRational("2.71828");
  }
  throw ComputeError{};
}

BigRational FormulaList::computePredefinedFunction(const string& name, const BigRational& input){
  if (name == "sin"){
    return input.getSin();
  } else if (name == "cos"){
    return input.getCos();
  } else if (name == "tan"){
    return input.getSin() / input.getCos();
  } else if (name == "csc"){
    return BigRational("1") / input.getSin();
  } else if (name == "sec"){
    return BigRational("1") / input.getCos();
  } else if (name == "cot"){
    return input.getCos() / input.getSin();
  }
  throw ComputeError{};
}

bool FormulaList::computeIneq(const Parser::ParseTree& tree, const BigRational& varInput, char freeVar) {
  if (!std::holds_alternative<unique_ptr<TokenTree>>(tree)) throw ComputeError{};
  TokenTree* tt = std::get<unique_ptr<TokenTree>>(tree).get();
  if (tt->type != INEQ) throw ComputeError{};
  if (!std::holds_alternative<unique_ptr<Token>>(tt->tree[1])) throw ComputeError{};
  BigRational leftValue = computeValueFromTree(tt->tree[0], varInput, freeVar);
  BigRational middleValue = computeValueFromTree(tt->tree[2], varInput, freeVar);
  if (!assessIneq(leftValue, middleValue, std::get<unique_ptr<Token>>(tt->tree[1]).get())) return false;
  if (tt->tree.size() == 5){
    if (!std::holds_alternative<unique_ptr<Token>>(tt->tree[3])) throw ComputeError{};
    BigRational rightValue = computeValueFromTree(tt->tree[4], varInput, freeVar);
    if (!assessIneq(middleValue, rightValue, std::get<unique_ptr<Token>>(tt->tree[3]).get())) return false;
  }
  return true;
}

bool FormulaList::assessIneq(const BigRational& leftVal, const BigRational& rightVal, Token* op){
  if (op->type != INEQOP) throw ComputeError{};
  if (op->lexeme == "<") return leftVal < rightVal;
  else if (op->lexeme == ">") return rightVal < leftVal;
  else if (op->lexeme == "<=") return !(rightVal < leftVal);
  else if (op->lexeme == ">=") return !(leftVal < rightVal);
  return false;
}

GraphPackage FormulaList::getGraphs(int rLen, int cLen, const BigRational& coordXL, const BigRational& coordXR, 
  const BigRational& coordYL, const BigRational& coordYR){
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

std::pair<string,vector<int>> FormulaList::getSingleGraph(int rLen, int cLen, const BigRational& coordXL, const BigRational& coordXR, 
  const BigRational& coordYL, const BigRational& coordYR, int formulaInd){
  Formula* formula = formulaSet[formulaInd].get();
  string graph; vector<int> positions;
  bool xfunc = formula->getFreeVar() == 'x';
  int maxInd = (xfunc ? cLen : rLen);
  BigRational prevVal;
  bool validPrev = false;
  for (int ind = 0; ind <= maxInd; ++ind){
    BigRational nextVal;
    bool validNext = true;
    bool outofBounds = false;
    try{
      if (xfunc) nextVal = computeValue(formulaInd, 
        coordXL + ((coordXR - coordXL) * BigRational(std::to_string((double)ind/(double)maxInd))));
      else nextVal = computeValue(formulaInd, 
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
  return {graph,positions};
}

void FormulaList::updateParameters(const set<int>& activeParams, int stepSize){
  for (auto p : activeParams){
    if (formulaSet.find(p) == formulaSet.end()) continue;
    Parameter* ptr = dynamic_cast<Parameter*>(formulaSet[p].get());
    if (!ptr) continue;
    ptr->advanceStep(stepSize);
  }
}

void FormulaList::updateParameterized(int rLen, int cLen, const BigRational& coordXL, const BigRational& coordXR, 
  const BigRational& coordYL, const BigRational& coordYR, GraphPackage& gp, const set<int>& activeParams){
  for (int i = 0; i < 2; i++){
    vector<int> indices = (i ? gp.yFuncIndices : gp.xFuncIndices);
    for (int j = 0, n = indices.size(); j < n; j++){
      bool needsUpdate = false;
      for (char p : formulaSet[indices[j]]->getParameters()){
        if (activeParams.count(nameIndexMapping[p])){
          needsUpdate = true;
          break;
        }
      }
      if (needsUpdate){
        std::pair<string,vector<int>> newGraph = getSingleGraph(rLen, cLen, coordXL, coordXR, coordYL, coordYR, indices[j]);
        if (i == 0) {
          gp.xStrings[j] = newGraph.first;
          gp.xFuncPositions[j] = newGraph.second;
        } else{
          gp.yStrings[j] = newGraph.first;
          gp.yFuncPositions[j] = newGraph.second;
        }
      }
    }
  }
}

void FormulaList::resetParams(){
  for (auto iter = formulaSet.begin(); iter != formulaSet.end(); ++iter){
    Parameter* p = dynamic_cast<Parameter*>(iter->second.get());
    if (p) p->reset();
  }
}

void FormulaList::setParam(int index, const BigRational& newValue){
  if (formulaSet.find(index) == formulaSet.end()) return;
  Parameter* p = dynamic_cast<Parameter*>(formulaSet[index].get());
  if (p) p->setValue(newValue);
}

void FormulaList::clear(){
  formulaSet.clear();
  nameIndexMapping.clear();
  nameTypeMapping.clear();
  errorStatus.clear();
  colourMapping.clear();
}
