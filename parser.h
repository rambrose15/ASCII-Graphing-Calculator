#ifndef __PARSER_H__
#define __PARSER_H__

#include <utility>
#include <deque>
#include <iterator>
#include <variant>
#include <memory>
#include <vector>
#include <string>
#include <set>

#include "token.h"

class ParseError{};

class Parser{
  public:
  using ParseTree = std::variant<std::unique_ptr<TokenTree>,std::unique_ptr<Token>>;
  using ParseForest = std::pair<bool, std::vector<ParseTree>>;
  using TokenList = std::deque<TokenType>;

  private:

  std::set<std::pair<TokenList,std::pair<size_t,size_t>>> memo;

  const std::vector<std::pair<TokenType,TokenList>> transformationRules = {
    { START, TokenList{ EXPR }},
    { START, TokenList{ EXPR, LCURL, INEQ, RCURL }},
    { START, TokenList{ LSQR, EXPR, COMMA, EXPR, RSQR }},
    { START, TokenList{ LSQR, EXPR, COMMA, EXPR, RSQR, EXPR }},
    { EXPR, TokenList{ EXPR, PLUS, TERM }},
    { EXPR, TokenList{ EXPR, NEG, TERM }},
    { EXPR, TokenList{ TERM }},
    { TERM, TokenList{ TERM, MD, FACTOR }},
    { TERM, TokenList{ TERM, FACTOR }},
    { TERM, TokenList{ FACTOR }},
    { FACTOR, TokenList{ FACTOR, EXPON, POWER }},
    { FACTOR, TokenList{ POWER }},
    { POWER, TokenList{ NEG, UNSIGNED }},
    { POWER, TokenList{ UNSIGNED }},
    { UNSIGNED, TokenList{ LPAR, EXPR, RPAR }},
    { UNSIGNED, TokenList{ FNAME, LPAR, EXPR, RPAR }},
    { UNSIGNED, TokenList{ VNAME }},
    { UNSIGNED, TokenList{ NUM }},
    { INEQ, TokenList{ EXPR, INEQOP, EXPR, INEQOP, EXPR }},
    { INEQ, TokenList{ EXPR, INEQOP, EXPR }}
    };

  ParseForest generateParseTree(TokenList structure, const std::vector<Token>& tokenSeq, size_t head, size_t tail);

  public:

  ParseTree parseTokens(const std::vector<Token>& tokenSeq);

  std::vector<ParseTree*> getComponents(const ParseTree& pt);
};

#endif
