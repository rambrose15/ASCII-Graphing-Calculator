#ifndef __BIG_NUM_H__
#define __BIG_NUM_H__

#include <vector>
#include <string>


class BigNum {
    // Represents number in base 1,000,000
    // As such, each digit is in the range 0-999,999
    // Largest digit at back, smallest digit at front
    std::vector<int> digits;
    size_t len;
    bool negative;
    BigNum(std::vector<int> digits, bool negative);
    
    // Helper functions for the real operation implementations
    std::vector<int> addUnsigned(const BigNum& other) const;
    std::vector<int> subUnsigned(const BigNum& other) const;
    BigNum modulo(const BigNum& other) const;
    BigNum divideUnsigned(BigNum dividend, const BigNum& divisor, bool quotient) const;
    BigNum findGCD(const BigNum& a, const BigNum& b) const;

    public:

    BigNum(std::string digitString = "0");

    bool operator==(const BigNum& other) const;
    bool operator<(const BigNum& other) const;

    BigNum operator+(const BigNum& other) const;
    BigNum operator-(const BigNum& other) const;
    BigNum operator-() const;
    BigNum operator*(const BigNum& other) const;
    BigNum operator/(const BigNum& other) const;
    BigNum operator^(const BigNum& other) const;
    
    void gcdReduce(BigNum& other);
    BigNum abs() const;

    bool isNegative() const { return negative; }

    std::string getDecimal() const;
};

#endif
