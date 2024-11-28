#include "formula.cc"
#include <iostream>

int main(){
  string s;
  map<char,pair<int,VarType>> varMap;
  map<string,VarType> predefMap;
  Parser parser;
  while(getline(std::cin, s)){
    try{
      Formula f;
      f.updateString(s);
      f.tokenize(varMap, predefMap);
      parser.parseTokens(f.getTokens());
      std::cout << "Parse succeeded" << std::endl;
    } catch (string s){
      std::cout << s << std::endl;
    }
  }
}