#include "parser.h"

using std::deque, std::pair, std::variant, std::unique_ptr, std::vector, std::string;

Parser::ParseForest Parser::generateParseTree(TokenList structure, const vector<Token>& tokenSeq, size_t head, size_t tail){
  if (structure == deque<TokenType>{LPAR, EXPR, RPAR} && head == 0 && tail == 3){
    ;
  }
  if (memo.count({structure, {head, tail}})){
    return ParseForest{false, vector<ParseTree>{}};
  }
  memo.insert({structure, {head, tail}});
  if (structure.empty() || head == tail){
    if (structure.empty() && head == tail) memo.erase({structure, {head, tail}});
    return ParseForest{structure.empty() && head == tail, vector<ParseTree>{}};
  }
  else if (structure.front() == tokenSeq[head].type){
    ParseForest rest = generateParseTree(TokenList(structure.begin()+1,structure.end()), tokenSeq, head+1, tail);
    rest.second.insert(rest.second.begin(), unique_ptr<Token>(new Token(tokenSeq[head])));
    if (rest.first) memo.erase({structure, {head, tail}});
    return rest;
  }
  else for (auto rule : transformationRules){
    if (rule.first != structure.front()) continue;
    for (size_t splitPoint = (structure.size()-1 ? head+rule.second.size() : std::max(head+rule.second.size(),tail)); 
        splitPoint <= tail - structure.size()+1; splitPoint++){
      ParseForest leftTree = generateParseTree(rule.second, tokenSeq, head, splitPoint);
      if (leftTree.first){
        ParseForest rightTree = generateParseTree(TokenList(structure.begin()+1,structure.end()), tokenSeq, splitPoint, tail);
        if (rightTree.first){
          ParseForest fullTree{true, vector<ParseTree>{}};
          fullTree.second.insert(fullTree.second.end(), unique_ptr<TokenTree>{new TokenTree{structure.front(), std::move(leftTree.second)}});
          fullTree.second.insert(fullTree.second.end(), std::make_move_iterator(rightTree.second.begin()), std::make_move_iterator(rightTree.second.end()));
          memo.erase({structure, {head, tail}});
          return fullTree;
        }
      }
    }
  }
  return ParseForest{false, vector<ParseTree>{}};
}

Parser::ParseTree Parser::parseTokens(const vector<Token>& tokenSeq){
  memo.clear();
  ParseForest parseTree = generateParseTree(deque<TokenType>{START}, tokenSeq, 0, tokenSeq.size());
  if (!parseTree.first) throw ParseError{};
  return std::move(parseTree.second.front());
}

vector<Parser::ParseTree*> Parser::getComponents(const ParseTree& pt){
  vector<ParseTree*> returnVal; 
  if (std::holds_alternative<unique_ptr<Token>>(pt)) return vector<ParseTree*>{};
  TokenTree* tt = std::get<unique_ptr<TokenTree>>(pt).get();
  for (auto iter = tt->tree.begin(); iter != tt->tree.end(); iter++){
    if (std::holds_alternative<unique_ptr<Token>>(*iter)) continue;
    returnVal.push_back(&(*iter));
  }
  return returnVal;
}
