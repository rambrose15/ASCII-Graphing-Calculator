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
#include "view.h"

class ComputeError{};

struct GraphPackage{
  std::vector<std::string> xStrings, yStrings;
  std::vector<std::vector<int>> xFuncPositions, yFuncPositions;
  std::vector<int> xFuncIndices, yFuncIndices;
};

class FormulaList{

  std::map<int,std::unique_ptr<Formula>> formulaSet;
  std::map<char,int> nameIndexMapping;
  std::map<char,FormulaType> nameTypeMapping;
  std::map<int,FormulaError> errorStatus;
  std::map<int,Colour> colourMapping;

  Parser parser;
  const std::map<std::string,FormulaType> preDefs = 
    {{"sin", FUNCTION}, {"cos", FUNCTION}, {"tan", FUNCTION},
     {"csc", FUNCTION}, {"sec", FUNCTION}, {"cot", FUNCTION},
     {"PI", CONSTANT}, {"E", CONSTANT}};

  bool isValidName(char c);

  std::vector<int> circularCheck();

  bool depthFirstSearch(int vertex, std::map<int,int>& states, std::vector<int>& sortedList);
  
  void createFormula(int index, const std::string& fullFormula);

  BigRational computePredefinedVar(const std::string& name);

  BigRational computePredefinedFunction(const std::string& name, const BigRational& input);

  BigRational computeValueFromTree(const Parser::ParseTree& tree, const BigRational& varInput = BigRational("0"), char freeVar = 0);

  public:

  void updateFormula(int index, const std::string& fullFormula);

  void updateColour(int index, Colour colour) { colourMapping[index] = colour; };

  Colour getColour(int index);
  bool setColour(int index, Colour newColour); // Return is whether such an index is valid

  FormulaError getErrorStatus(size_t index) { return errorStatus[index]; }

  BigRational computeValue(int index, const BigRational& varInput = BigRational("0"));

  GraphPackage getGraphs(int rLen, int cLen, BigRational coordXL, BigRational coordXR, 
    BigRational coordYL, BigRational coordYR);
};

#endif
