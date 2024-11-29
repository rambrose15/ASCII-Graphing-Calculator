#ifndef __FORMULA__H__
#define __FORMULA__H__

#include <map>
#include <string>
#include <set>

#include "token.h"
#include "parser.h"

enum FormulaType{
  CONSTANT, PARAMETER, EQUATION, FUNCTION
};

class Formula{
  std::set<char> dependencies; // The set of all other formulas this is defined in terms of
  char name;
  std::string formulaString;

  std::vector<Token> tokenList;
  Parser::ParseTree parseTree;

  virtual char getFreeVar() { return 0; }
  virtual bool isFreeVar(char c) { return false; }

  void tokenize(const std::map<char,std::pair<int,FormulaType>>& varMap, 
    const std::map<std::string,FormulaType>& preDefs);

  void getDependencies();

  public: 

  Formula(char name, std::string formulaString): name{name}, formulaString{formulaString} {}

  const Parser::ParseTree& parse(Parser parser);

  char getName() { return name; }
};

class Constant : public Formula {
  public:
  Constant(char name, std::string formulaString): Formula{name, formulaString} {}
};

class Parameter : public Formula {
  public:
  Parameter(char name, std::string formulaString): Formula{name, formulaString} {}
};

class Expression : public Formula {
  bool isFreeVar(char c) override { return getName() == c; }
  char getFreeVar() override { return getName(); }

  public:

  Expression(char name, std::string formulaString): Formula{name, formulaString} {}
};

class Function : public Formula {
  char freeVar;
  bool isFreeVar(char c) override { return freeVar == c; }
  char getFreeVar() override { return freeVar; }

  public:

  Function(char name, std::string formulaString, char freeVar): Formula{name, formulaString}, freeVar{freeVar} {}
};

#endif