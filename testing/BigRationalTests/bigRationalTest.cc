#include <iostream>
#include <string>
#include "../../bigRational.cc"
using std::string, std::cin, std::cout, std::endl;

int main(int argc, char* args[]){
  string s, t, op;
  int testNum = 1;
  while (cin >> s){
    BigRational b1(s);
    if (argc > 1){
      b1.getDecimal();
      continue;
    }
    cin >> op >> s;
    BigRational b2(s);
    BigRational b3("0");
    if (op == "*") b3 = b1 * b2;
    else if (op == "+") b3 = b1 + b2;
    else if (op == "-") b3 = b1 - b2;
    else if (op == "/") b3 = b1 / b2;
    else if (op == "^") b3 = b1 ^ b2;
    else std::cerr << "Unrecognized symbol\n";
    cin >> s >> t;
    if (b3.getDecimal() == s + " | " + t) cout << "\033[0;32mTest " << testNum << " Passed\033[0;0m\n";
    else cout << "\033[0;91mTest " << testNum << " Failed.\033[0;0m Output was: " << b3.getDecimal() 
              << " Input was: " + s + " | " + t << endl;
    testNum++;
  }
}
