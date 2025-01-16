#ifndef __FORMULA_LIST_H__
#define __FORMULA_LIST_H__

#include <string>
#include <map>
#include <set>
#include <memory>
#include <variant>

#include "formula.h"
#include "parser.h"
#include "formula_error.h"
#include "../numeric/big_rational.h"
#include "../../utility/colour.h"

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

  bool computeIneq(const Parser::ParseTree&, const BigRational& varInput, char freeVar);

  bool assessIneq(const BigRational& leftVal, const BigRational& rightVal, Token* op);

  std::pair<std::string,std::vector<int>> getSingleGraph(int rLen, int cLen, const BigRational& coordXL, const BigRational& coordXR, 
    const BigRational& coordYL, const BigRational& coordYR, int funcInd);

  BigRational computeValueFromTree(const Parser::ParseTree& tree, const BigRational& varInput = BigRational("0"), char freeVar = 0);

  public:

  void updateFormula(int index, const std::string& fullFormula);

  Colour getColour(int index);
  bool setColour(int index, Colour newColour); // Return is whether such an index is valid
  std::vector<Colour> getFormulaColouring(int index);

  FormulaError getErrorStatus(int index);

  bool isConstant(int index);
  bool isParameter(int index);
  bool isFunction(int index);
  bool isExpression(int index);

  BigRational computeValue(int index, const BigRational& varInput = BigRational("0"));

  GraphPackage getGraphs(int rLen, int cLen, const BigRational& coordXL, const BigRational& coordXR, 
  const BigRational& coordYL, const BigRational& coordYR);

  void updateParameters(const std::set<int>& activeParams, int stepSize);
  void updateParameterized(int rLen, int cLen, const BigRational& coordXL, const BigRational& coordXR, 
    const BigRational& coordYL, const BigRational& coordYR, GraphPackage& gp, const std::set<int>& activeParams);

  void resetParams();
  void setParam(int index, const BigRational& newValue);

  void clear();
};

#endif
