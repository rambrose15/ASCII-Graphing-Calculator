#include <iostream>
#include <string>
#include "../../bigRational.cc"
using std::string, std::cin, std::cout, std::endl;

void unitTests();

int main(int argc, char* args[]){
  unitTests();
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

void unitTests(){
  (BigNum("-123456789101112131415") + BigNum("123456789101112131400")).getDecimal();
  /*assert(BigNum("01234567890783740287390237902374").getDecimal() == "1234567890783740287390237902374");
  assert(BigNum("-000000010525555555").getDecimal() == "-10525555555");
  assert((BigNum("560123456") + BigNum("960123456")).getDecimal() == "1520246912");
  assert((BigNum("-1234567891011121314") - BigNum("-11234567891011121314")).getDecimal() == "10000000000000000000");
  assert((BigNum("4") - BigNum("100")).getDecimal() == "-96");
  assert((BigNum("111111111111") * BigNum("-1234567891011")).getDecimal() == "-137174210112196159123221");
  assert((BigNum("1234567891011") ^ BigNum("3")).getDecimal() == "1881676376411925699615487319217434331");
  assert((-BigNum("-222222222222")).getDecimal() == "222222222222");
  BigNum b1("293391909323"), b2("12954895625");
  b1.gcdReduce(b2);
  assert((b1.getDecimal() + "|" + b2.getDecimal()) == "33984931|1500625");*/
}
