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
    if (!depthFirstSearch(dep, states, sortedList)){
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
    if (errorStatus[iter->first] != PREFIX){
      iter->second->tokenize(nameTypeMapping, preDefs);
    }
  }

  // Determine parameters
  vector<int> order = circularCheck();
  for (int i = 0, n = order.size(); i < n; i++){
    if (errorStatus[i] != NONE) continue;
    for (int j = i+1; j < n; j++){
      if (errorStatus[i] != NONE) continue;
      if (formulaSet[j]->getDependencies().count(formulaSet[i]->getName())){
        for (auto p : formulaSet[i]->getParameters()){
          formulaSet[j]->addParameter(p);
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
      } else {
        try{
          formulaSet[ind]->parse(parser);
        } catch (ParseError){
          errorStatus[ind] = PARSE;
        }
      }
    }
  }
}
