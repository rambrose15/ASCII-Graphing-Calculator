#ifndef __FORMULA_H__
#define __FORMULA_H__

#include <map>
#include <string>
#include <set>
#include <cctype>
#include <chrono>

#include "token.h"
#include "parser.h"
#include "formulaError.h"
#include "bigRational.h"
#include "colour.h"

enum FormulaType{
  CONSTANT, PARAMETER, EXPRESSION, FUNCTION
};

class Formula{
  std::set<char> dependencies; // The set of all other formulas this is defined in terms of
  std::set<char> parameters; // Set of parameters which this is defined in terms of
  char name;
  std::string formulaString;

  std::vector<Token> tokenList;
  Parser::ParseTree parseTree;

  virtual bool isFreeVar(char c) { return false; }

  public: 

  Formula(char name, std::string formulaString): name{name}, formulaString{formulaString} {}

  virtual ~Formula() = default;

  virtual char getFreeVar() { return 0; }

  std::vector<Colour> getColouring();

  virtual FormulaError checkValidity() = 0;

  bool tokenize(const std::map<char,FormulaType>& varMap, 
    const std::map<std::string,FormulaType>& preDefs);

  const Parser::ParseTree& parse(Parser parser);

  const Parser::ParseTree& getParseTree();

  char getName() const { return name; }

  const std::set<char>& getDependencies() const { return dependencies; }
  const std::set<char>& getParameters() const { return parameters; }

  bool dependsOn (char varName) const { return dependencies.count(varName) != 0; }

  void addParameter(char newParam) { parameters.insert(newParam); }
};

class Constant : public Formula {
  BigRational value;
  bool upToDate;
  
  public:
  
  bool isUpdated() { return upToDate; }
  const BigRational& getValue() { return value; }
  void updateValue(const BigRational& newValue){
    value = newValue;
    upToDate = true;
  }
  Constant(char name, std::string formulaString): Formula{name, formulaString}, upToDate{false} {}
  FormulaError checkValidity() override;
};

class Parameter : public Formula {
  BigRational lBound, rBound, speed;
  BigRational currentValue;
  std::chrono::time_point<std::chrono::system_clock> timeStart;
  bool upToDate = false;
  bool defined = false;
  int currentStep = 0;
  inline static const int MAX_STEP = 5;

  void computeValue();

  public:
  
  Parameter(char name, std::string formulaString): Formula{name, formulaString} {}
  void updateValues(const BigRational& newL, const BigRational& newR, const BigRational& newSpeed = BigRational("1"));
  void setValue(const BigRational& newValue);
  bool isDefined() { return defined; }
  BigRational getValue();
  void reset();
  void advanceStep(int stepSize);
  FormulaError checkValidity() override;
};

class Expression : public Formula {
  bool isFreeVar(char c) override { return c == getFreeVar(); }

  public:

  Expression(char name, std::string formulaString): Formula{name, formulaString} {}

  char getFreeVar() override { return (getName() == 'x' ? 'y' : 'x'); }
  FormulaError checkValidity() override;
};

class Function : public Formula {
  char freeVar;
  bool isFreeVar(char c) override { return freeVar == c; }

  public:

  Function(char name, std::string formulaString, char freeVar): Formula{name, formulaString}, freeVar{freeVar} {}
  
  char getFreeVar() override { return freeVar; }
  FormulaError checkValidity() override;
};

#endif
