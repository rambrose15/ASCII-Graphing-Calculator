#include "parser.h"

using std::deque, std::pair, std::variant, std::unique_ptr, std::vector, std::string;

Parser::ParseForest Parser::generateParseTree(TokenList structure, const vector<Token>& tokenSeq, size_t head, size_t tail){
  if (structure.empty() || head == tail){
    return ParseForest{structure.empty() && head == tail, vector<ParseTree>{}};
  }
  else if (structure.front() == tokenSeq[head].type){
    structure.pop_front();
    ParseForest rest = generateParseTree(structure, tokenSeq, head+1, tail);
    rest.second.insert(rest.second.begin(), unique_ptr<Token>(new Token(tokenSeq[head])));
    return rest;
  }
  else for (auto rule : transformationRules){
    if (rule.first != structure.front()) continue;
    for (size_t splitPoint = head+rule.second.size(); splitPoint <= tail - structure.size()+1; splitPoint++){
      ParseForest leftTree = generateParseTree(rule.second, tokenSeq, head, splitPoint);
      if (leftTree.first){
        ParseForest rightTree = generateParseTree(TokenList(structure.begin()+1,structure.end()), tokenSeq, splitPoint, tail);
        if (rightTree.first){
          leftTree.second.insert(leftTree.second.end(), std::make_move_iterator(rightTree.second.begin()), std::make_move_iterator(rightTree.second.end()));
          return leftTree;
        }
      }
    }
  }
  return ParseForest{false, vector<ParseTree>{}};
}

Parser::ParseTree Parser::parseTokens(const vector<Token>& tokenSeq){
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
