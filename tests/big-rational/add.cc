#include "doctest.h"
#include "numeric/big_rational.h"

TEST_CASE("big rational addition") {
  BigRational a("1111");
  BigRational b("999");
  BigRational c = a + b;
  CHECK(c.getDecimalForm(0) == "2110");
}
