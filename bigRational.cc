#include "bigRational.h"
#define TWOPI BigRational("6.283185")
#define HALFPI BigRational("1.570796")
#define FULLPI BigRational("3.141593")
using std::vector, std::string;


BigNum::BigNum(vector<int> digits, bool negative): 
    digits{digits.empty() ? vector<int>{0} : digits}, 
    len{std::max((size_t)1, digits.size())}, 
    negative{digits.empty() || (digits.size() == 1 && digits[0] == 0) ? false : negative}{}
    
// Helper functions for the real operation implementations
vector<int> BigNum::addUnsigned(const BigNum& other) const {
    using ll = long long;
    vector<int> sumDigits{};
    int base = 1000000000;
    size_t ind1 = 0, ind2 = 0;
    int remainder = 0;
    while (ind1 < len || ind2 < other.len || remainder){
        ll subTotal = (ll)(ind1 < len ? digits[ind1] : 0) + 
                    (ll)(ind2 < other.len ? other.digits[ind2] : 0) + (ll)remainder;
        sumDigits.push_back(subTotal % base);
        remainder = subTotal / base;
        ind1++; ind2++;
    }
    return sumDigits;
}

vector<int> BigNum::subUnsigned(const BigNum& other) const {
    using ll = long long;
    vector<int> differenceDigits{};
    int ind1 = 0, ind2 = 0;
    bool debt = false;
    while (ind1 < len || ind2 < other.len){
        ll subTotal = (ll)digits[ind1] - (ll)(ind2 < other.len ? other.digits[ind2] : 0) - (ll)debt;
        debt = subTotal < 0;
        if (debt) subTotal += (ll)1000000000;
        differenceDigits.push_back(subTotal);
        ind1++; ind2++;
    }
    while (!differenceDigits.empty() && differenceDigits.back() == 0) 
        differenceDigits.pop_back();
    return differenceDigits;
}

BigNum BigNum::modulo(const BigNum& other) const {
    return divideUnsigned(*this, other, false);
}

void BigNum::quotient(const BigNum& other) {
    *this = BigNum(divideUnsigned(*this, other, true).digits, negative ^ other.negative);
}

BigNum BigNum::abs() const{
    BigNum result = *this;
    result.negative = false;
    return result;
}

BigNum BigNum::divideUnsigned(BigNum dividend, const BigNum& divisor, bool quotient) const{
    dividend = dividend.abs();
    vector<BigNum> multiples{divisor.abs()};
    vector<BigNum> twoPowers{BigNum("1")};
    while (!(dividend < multiples.back())){
        twoPowers.push_back(twoPowers.back() * BigNum("2"));
        multiples.push_back(multiples.back() * BigNum("2"));
    }
    multiples.pop_back(); twoPowers.pop_back();
    BigNum result("0");
    for (int ind = multiples.size()-1; ind >= 0; ind--){
        if (!(dividend < multiples[ind])){
            dividend = dividend - multiples[ind];
            result = result + twoPowers[ind];
        }
    }
    if (quotient) return result;
    else return dividend;
}

BigNum::BigNum(string digitString) { // Assumes the string has only numerics or a negative in the front
    int n = digitString.length();
    int startInd = 0; // This points to the first relevant character of the digitString
    while (startInd < n && 
        (digitString[startInd] == '0' || digitString[startInd] == '-')) startInd++;
    if (startInd == n){
        digits = vector<int>{0};
        negative = false;
        len = 1;
    }
    else{
        digits = vector<int>((n + 8 - startInd)/9, 0); // The +8 serves to make it a ceiling.
        negative = digitString[0] == '-';
        for (int i = n-1; i >= startInd; i -= 9)
            digits[(n-1-i)/9] = std::stoi(digitString.substr(std::max(startInd,i - 8), std::min(i-startInd+1, 9)));
        while (!digits.empty() && digits.back() == 0) digits.pop_back();
        len = digits.size();
    }
}

bool BigNum::operator==(const BigNum& other) const{
    return negative == other.negative && digits == other.digits;
}

BigNum BigNum::operator+(const BigNum& other) const {
    if (negative && other.negative) return BigNum(addUnsigned(other), true);
    else if (!negative && !other.negative) return BigNum(addUnsigned(other), false);
    if (other.abs() < abs()) return BigNum(subUnsigned(other), negative);
    else return BigNum(other.subUnsigned(*this), other.negative);
}

BigNum BigNum::operator-(const BigNum& other) const{
    if (negative ^ other.negative) return BigNum(addUnsigned(other), negative);
    if (other.abs() < abs()) return BigNum(subUnsigned(other), negative);
    else return BigNum(other.subUnsigned(*this), !other.negative);
}

BigNum BigNum::operator-() const{
    BigNum negation = *this;
    negation.negative = !this->negative;
    return negation;
}

BigNum BigNum::operator*(const BigNum& other) const {
    BigNum productNum(vector<int>{0}, negative ^ other.negative);
    using ll = unsigned long long;
    int base = 1000000000;
    for (int ind1 = 0; ind1 < len; ind1++){
        int remainder = 0;
        vector<int> productDigits(ind1, 0);
        for (int ind2 = 0; ind2 < other.len; ind2++){
            ll singleProduct = (ll)digits[ind1] * (ll)other.digits[ind2] + (ll)remainder;
            productDigits.push_back(singleProduct % base);
            remainder = singleProduct / base;
        }
        if (remainder) productDigits.push_back(remainder);
        productNum = productNum + BigNum(productDigits, negative ^ other.negative);
    }
    return productNum;
}

BigNum BigNum::operator^(const BigNum& other) const{
    BigNum result{vector<int>{1}, false};
    BigNum counter = other;
    while (counter.digits.back() != 0 || counter.len > 1){
        result = result * *this;
        counter = counter - BigNum{vector<int>{1}, false};
    }
    return result;
}

bool BigNum::operator<(const BigNum& other) const{
    if (negative ^ other.negative) return negative;
    else if (len == other.len){
        for (int ind = len-1; ind >= 0; ind--){
            if (digits[ind] < other.digits[ind]) return !negative;
            else if (digits[ind] > other.digits[ind]) return negative;
        }
        return false;
    }
    else return (len < other.len) ^ negative;
}

void BigNum::gcdReduce(BigNum& other){ // Reduces both itself and the argument by their GCD
    if (len == 1 && digits[0] == 0) return;
    if (other.len == 1 && digits[0] == 0) return;
    BigNum gcd = findGCD(*this, other);
    quotient(gcd);
    other.quotient(gcd);
    if (negative && other.negative){
        negative = false;
        other.negative = false;
    }
}

BigNum BigNum::findGCD(const BigNum& a, const BigNum& b) const{
    if (a.len == 1 && a.digits[0] == 0) return b;
    return findGCD(b.modulo(a), a);
}

string BigNum::getDecimal() const {
    string s;
    if (negative) s +=  "-";
    for (int i = len-1; i >= 0; i--) {
        string digitString = std::to_string(digits[i]);
        s += (i != len-1 ? string(9-digitString.length(), '0') : "") + digitString;
    }
    return s;
}
    
void BigRational::reduce(){
    if (numerator == BigNum("0")) denominator = BigNum("1");
    else numerator.gcdReduce(denominator);
    if (denominator.isNegative()){
        denominator = -denominator;
        numerator = -numerator;
    }
}

BigRational BigRational::abs() const{
    if (isNegative()) return -*this;
    else return *this;
}

BigRational::BigRational(const string& decimalString){ // Ctor to get BigRational from a string eg. 132.10210
    int n = decimalString.length();
    vector<char> numeratorString; // The decimalString excluding the decimal place
    int decimalPlaces = -1; // Determines how large the initial denominator must be
    for (int index = 0; index < n; index++){
        if (decimalString[index] == '.') decimalPlaces = 0;
        else {
            numeratorString.push_back(decimalString[index]);
            if (decimalPlaces != -1) ++decimalPlaces;
        }
    }
    numerator = BigNum(string(numeratorString.begin(), numeratorString.end()));
    if (decimalPlaces < 1) denominator = BigNum("1");
    else denominator = BigNum("1" + string(decimalPlaces, '0'));
    reduce();
}

BigRational BigRational::modulo(BigRational num, const BigRational& modder) const{
    num = num.abs();
    vector<BigRational> multiples{modder.abs()};
    vector<BigRational> twoPowers{BigRational("1")};
    while (!(num < multiples.back())){
        twoPowers.push_back(twoPowers.back() * BigRational("2"));
        multiples.push_back(multiples.back() * BigRational("2"));
    }
    multiples.pop_back(); twoPowers.pop_back();
    for (int ind = multiples.size()-1; ind >= 0; ind--){
        if (!(num < multiples[ind])){
            num = num - multiples[ind];
        }
    }
    return num;
}

BigRational BigRational::computeSin(const BigRational& num) const{
    return num - ((num ^ BigRational("3")) / BigRational("6")) 
            + ((num ^ BigRational("5")) / BigRational("120"))
            - ((num ^ BigRational("7")) / BigRational("5040"));
}

BigRational BigRational::computeCos(const BigRational& num) const{
    return BigRational("1") - ((num ^ BigRational("2")) / BigRational("2")) 
            + ((num ^ BigRational("4")) / BigRational("24"))
            - ((num ^ BigRational("6")) / BigRational("720"));
}

BigRational BigRational::getSin() const{
    BigRational input = modulo(*this, TWOPI);
    if (!isNegative()) return computeSin(FULLPI - input);
    else return computeSin(input - FULLPI);
}

BigRational BigRational::getCos() const{
    return -computeCos(FULLPI - modulo(*this, TWOPI));
}

string BigRational::getDecimalForm(size_t decimalPrecision) const {
    BigNum num = numerator * BigNum("1" + string(decimalPrecision, '0'));
    num.quotient(denominator);
    string result = num.getDecimal();
    if (decimalPrecision > 0 && result.length() > decimalPrecision) result.insert(result.end()-decimalPrecision, '.');
    else if (decimalPrecision > 0 && result.length() <= decimalPrecision) {
        result = "0." + string(decimalPrecision-result.length(), '0') + result;
    }
    return result;
}
