#ifndef __FORMULA_LIST_H__
#define __FORMULA_LIST_H__

#include <string>
#include <map>
#include <memory>

#include "formula.h"
#include "parser.h"
#include "formulaError.h"

class FormulaList{

  std::map<int,std::unique_ptr<Formula>> formulaSet;
  std::map<char,int> nameIndexMapping;
  std::map<char,FormulaType> nameTypeMapping;
  std::map<int,FormulaError> errorStatus;
  Parser parser;
  const std::map<std::string,FormulaType> preDefs = 
    {{"sin", FUNCTION}, {"cos", FUNCTION}, {"tan", FUNCTION}, {"PI", CONSTANT}};

  bool isValidName(char c);

  std::vector<int> circularCheck();

  bool depthFirstSearch(int vertex, std::map<int,int>& states, std::vector<int>& sortedList);
  
  void createFormula(int index, const std::string& fullFormula);

  public:

  void updateFormula(int index, const std::string& fullFormula);
  
  FormulaError getErrorStatus(size_t index) { return errorStatus[index]; }
};

#endif
