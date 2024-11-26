#include <utility>
#include <deque>
#include <iterator>
#include "token.cc"
using std::deque, std::pair;

using ParseNode = variant<unique_ptr<TokenTree>,unique_ptr<Token>>;
using TokenList = deque<TokenType>;

class Parser{
  using ParseTree = pair<bool, vector<ParseNode>>;
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

    ParseTree generateParseTree(TokenList structure, const vector<Token>& tokenSeq, size_t head, size_t tail){
      
      if (structure.empty() || head == tail){
        return ParseTree{structure.empty() && head == tail, vector<ParseNode>{}};
      }
      else if (structure.front() == tokenSeq[head].type){
        structure.pop_front();
        ParseTree rest = generateParseTree(structure, tokenSeq, head+1, tail);
        rest.second.insert(rest.second.begin(), unique_ptr<Token>(new Token(tokenSeq[head])));
        return rest;
      }
      else for (auto rule : transformationRules){
        if (rule.first != structure.front()) continue;
        for (size_t splitPoint = head+rule.second.size(); splitPoint <= tail - structure.size()+1; splitPoint++){
          ParseTree leftTree = generateParseTree(rule.second, tokenSeq, head, splitPoint);
          if (leftTree.first){
            ParseTree rightTree = generateParseTree(TokenList(structure.begin()+1,structure.end()), tokenSeq, splitPoint, tail);
            if (rightTree.first){
              leftTree.second.insert(leftTree.second.end(), std::make_move_iterator(rightTree.second.begin()), std::make_move_iterator(rightTree.second.end()));
              return leftTree;
            }
          }
        }
      }
      return ParseTree{false, vector<ParseNode>{}};
    }

    public:

    ParseNode parseTokens(const vector<Token>& tokenSeq){
      ParseTree parseTree = generateParseTree(deque<TokenType>{START_STATE}, tokenSeq, 0, tokenSeq.size());
      if (!parseTree.first) throw string{"Unable to parse"};
      return std::move(parseTree.second.front());
    }
};
