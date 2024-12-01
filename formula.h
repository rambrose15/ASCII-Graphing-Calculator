#ifndef __FORMULA_H__
#define __FORMULA_H__

#include <map>
#include <string>
#include <set>

#include "token.h"
#include "parser.h"
#include "formulaError.h"

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

  virtual char getFreeVar() { return 0; }
  virtual bool isFreeVar(char c) { return false; }

  public: 

  Formula(char name, std::string formulaString): name{name}, formulaString{formulaString} {}

  virtual ~Formula() = default;

  virtual FormulaError checkValidity() const = 0;

  bool tokenize(const std::map<char,FormulaType>& varMap, 
    const std::map<std::string,FormulaType>& preDefs);

  const Parser::ParseTree& parse(Parser parser);

  char getName() const { return name; }

  const std::set<char>& getDependencies() const { return dependencies; }
  const std::set<char>& getParameters() const { return parameters; }

  bool dependsOn (char varName) const { return dependencies.count(varName) != 0; }

  void addParameter(char newParam) { parameters.insert(newParam); }
};

class Constant : public Formula {
  public:
  Constant(char name, std::string formulaString): Formula{name, formulaString} {}
  FormulaError checkValidity() const override;
};

class Parameter : public Formula {
  public:
  Parameter(char name, std::string formulaString): Formula{name, formulaString} {}
  FormulaError checkValidity() const override;
};

class Expression : public Formula {
  bool isFreeVar(char c) override { return getName() == c; }
  char getFreeVar() override { return getName(); }

  public:

  Expression(char name, std::string formulaString): Formula{name, formulaString} {}
  FormulaError checkValidity() const override;
};

class Function : public Formula {
  char freeVar;
  bool isFreeVar(char c) override { return freeVar == c; }
  char getFreeVar() override { return freeVar; }

  public:

  Function(char name, std::string formulaString, char freeVar): Formula{name, formulaString}, freeVar{freeVar} {}
  FormulaError checkValidity() const override;
};

#endif
