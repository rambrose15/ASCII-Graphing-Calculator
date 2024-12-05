#ifndef __FORMULA_LIST_H__
#define __FORMULA_LIST_H__

#include <string>
#include <map>
#include <memory>
#include <variant>

#include "formula.h"
#include "parser.h"
#include "formulaError.h"
#include "bigRational.h"

class ComputeError{};

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

  BigRational computeValueFromTree(const Parser::ParseTree& tree, const BigRational& varInput = BigRational("0"), char freeVar = 0);

  public:

  void updateFormula(int index, const std::string& fullFormula);
  
  FormulaError getErrorStatus(size_t index) { return errorStatus[index]; }

  BigRational computeValue(int index, const BigRational& varInput = BigRational("0"));
};

#endif
