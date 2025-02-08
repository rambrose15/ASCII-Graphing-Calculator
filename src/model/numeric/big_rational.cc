#include "big_rational.h"

#define TWOPI BigRational("6.283185")
#define HALFPI BigRational("1.570796")
#define FULLPI BigRational("3.141593")

using std::vector, std::string;


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
    BigNum num = (numerator * BigNum("1" + string(decimalPrecision, '0'))) / denominator;
    string result = num.abs().getDecimal();
    if (decimalPrecision > 0 && result.length() > decimalPrecision) result.insert(result.end()-decimalPrecision, '.');
    else if (decimalPrecision > 0 && result.length() <= decimalPrecision) {
        result = "0." + string(decimalPrecision-result.length(), '0') + result;
    }
    return result;
}
