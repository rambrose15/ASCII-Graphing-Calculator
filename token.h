#ifndef __TOKEN_H__
#define __TOKEN_H__

#include <string>
#include <variant>
#include <memory>
#include <vector>

enum TokenType{
  LPAR, RPAR, // ( )
  LCURL, RCURL, // { }
  LSQR, RSQR, // [ ]
  COMMA, // ,
  NUM, // Number 
  FNAME, // Function name (user or pre defined)
  VNAME, // Variable name (user or pre defined)
  PNAME, // Parameter name
  PLUS, NEG, // Plus/Minus
  MD, // Divide/Multiply
  EXPON, // Exponentiation
  INEQOP, // Inequality sign: > < >= <=
  EXPR, // General arithmetic expression
  TERM, FACTOR, POWER, UNSIGNED, // Allow for order of operations, left associativity
  INEQ, // Inequality expression
  START // Initial state which is decomposed
};

struct Token{
  TokenType type; // Always a terminal token type
  std::string lexeme; // The characters it represents
};

struct TokenTree{
  TokenType type; // Always a non-terminal token type

  // This serves as a sequence of terminal tokens and token tree rooted at non-terminal tokens
  // This will be of the form of one of the transformation rules for the above TokenType
  std::vector<std::variant<std::unique_ptr<TokenTree>,std::unique_ptr<Token>>> tree;
};

#endif