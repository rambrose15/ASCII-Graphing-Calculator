#ifndef __BIG_RATIONAL_H__
#define __BIG_RATIONAL_H__

#include "big_num.h"
#include <vector>
#include <string>


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

    BigRational operator^(const BigRational& other) const{ 
        BigNum power = other.numerator;
        power = power / other.denominator;
        if (!power.isNegative()) return BigRational(numerator ^ power, denominator ^ power);
        else return BigRational(denominator ^ (-power), numerator ^ (-power));
    }

    bool operator<(const BigRational& other) const{
        return numerator * other.denominator < other.numerator * denominator;
    }

    bool isNegative() const { return numerator.isNegative() ^ denominator.isNegative(); }
    
    BigRational getSin() const;
    BigRational getCos() const;

    std::string getRationalForm() const {
        return numerator.getDecimal() + " | " + denominator.getDecimal();
    }
    std::string getDecimalForm(size_t decimalPrecision) const;
    std::string getNumeratorDecimal() const {
        return numerator.getDecimal();
    }
    std::string getDenominatorDecimal() const {
        return denominator.getDecimal();
    }
};

#endif
