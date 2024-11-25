#include <string>
#include <variant>
#include <memory>
#include <vector>
#include <utility>
using std::string, std::variant, std::unique_ptr, std::vector, std::pair;

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
  START_STATE // Initial state which is decomposed
};

struct TokenTree{
  TokenType type;
  vector<variant<unique_ptr<TokenTree>,unique_ptr<Token>>> tree;
};

struct Token{
  TokenType type;
  string lexeme;
};

class Tokenizer{
  using TokenList = vector<TokenType>;
  vector<pair<TokenType,TokenList>> transformationRules = {
    { START_STATE, TokenList{ EXPR }},
    { START_STATE, TokenList{ EXPR, LCURL, INEQ, RCURL }},
    { START_STATE, TokenList{ LSQR, EXPR, COMMA, EXPR, RSQR }},
    { START_STATE, TokenList{ LSQR, EXPR, COMMA, EXPR, RSQR, EXPR }},
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

    public:

    TokenTree generateParseTree(const vector<Token>& tokenString){
      // Parse function
    }
};
