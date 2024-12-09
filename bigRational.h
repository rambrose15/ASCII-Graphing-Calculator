#ifndef __BIG_RATIONAL_H__
#define __BIG_RATIONAL_H__

#include <vector>
#include <string>

class BigNum {
    std::vector<int> digits; // Largest digit at back, smallest digit at front
    size_t len;
    bool negative;
    BigNum(std::vector<int> digits, bool negative);
    
    // Helper functions for the real operation implementations
    std::vector<int> addUnsigned(const BigNum& other) const;
    std::vector<int> subUnsigned(const BigNum& other) const;
    BigNum modulo(const BigNum& other) const;
    void quotient(const BigNum& other);
    BigNum abs() const;
    BigNum divideUnsigned(BigNum dividend, const BigNum& divisor, bool quotient) const;

    public:

    BigNum(std::string digitString = "0");

    bool operator==(const BigNum& other) const;
    bool operator<(const BigNum& other) const;

    BigNum operator+(const BigNum& other) const;
    BigNum operator-(const BigNum& other) const;
    BigNum operator-() const;
    BigNum operator*(const BigNum& other) const;
    BigNum operator^(const BigNum& other) const;

    void gcdReduce(BigNum& other);
    private: BigNum findGCD(const BigNum& a, const BigNum& b) const;
    public:

    bool isNegative() const { return negative; }

    std::string getDecimal() const;
};

class BigRational{
    
    BigNum numerator, denominator;
    
    void reduce();

    BigRational modulo(BigRational num, const BigRational& modder) const;
    BigRational computeSin(const BigRational& num) const;
    BigRational computeCos(const BigRational& num) const;
    BigRational abs() const;

    public:

    BigRational(const std::string& decimalString = "0"); // Ctor to get BigRational from a string eg. 132.10210

    BigRational(BigNum numer, BigNum denom): numerator{numer}, denominator{denom} {
        reduce(); // Constructor which directly accepts the numerator & denominator
    }

    BigRational operator+(const BigRational& other) const{
        return BigRational{(numerator * other.denominator) + (other.numerator * denominator), 
                denominator * other.denominator};
    }

    BigRational operator-(const BigRational& other) const{
        return BigRational{(numerator * other.denominator) - (other.numerator * denominator), 
                denominator * other.denominator};
    }

    BigRational operator-() const{
        return BigRational{- numerator, denominator};
    }

    BigRational operator*(const BigRational& other) const{
        return BigRational{numerator * other.numerator, denominator * other.denominator};
    }

    BigRational operator/(const BigRational& other) const{
        return BigRational{numerator * other.denominator, denominator * other.numerator};
    }

    // Currently only works for integer powers
    BigRational operator^(const BigRational& other) const{ 
        if (!other.isNegative()) return BigRational(numerator ^ other.numerator, denominator ^ other.numerator);
        else return BigRational(denominator ^ (-other.numerator), numerator ^ (-other.numerator));
    }

    bool operator<(const BigRational& other) const{
        return numerator * other.denominator < other.numerator * denominator;
    }

    bool isNegative() const { return numerator.isNegative() ^ denominator.isNegative(); }
    
    BigRational getSin() const;
    BigRational getCos() const;

    std::string getDecimal() const {
        return numerator.getDecimal() + " | " + denominator.getDecimal();
    }
    std::string getNumeratorDecimal() const {
        return numerator.getDecimal();
    }
    std::string getDenominatorDecimal() const {
        return denominator.getDecimal();
    }
};

#endif
