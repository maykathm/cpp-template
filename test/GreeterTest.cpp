
#include <boost/test/unit_test.hpp>
#include "cpptemplate/Greeter.h"
#include <numeric>

BOOST_AUTO_TEST_SUITE(greeter_test)

  BOOST_AUTO_TEST_CASE(say_hi_test) {
    cpptemplate::Greeter greeter;
    std::string name("name");
    auto hi = greeter.sayHi(name);
    BOOST_CHECK_EQUAL("Hi, name", hi);
  }

  BOOST_AUTO_TEST_CASE(copy_test) {
    cpptemplate::Greeter greeter;
    std::vector<float> vector(1000);
    std::iota(begin(vector), end(vector), 0);
    auto copied = greeter.copy(vector);
    auto copiedIterator = begin(copied);
    for (auto& vecElem : vector) {
      if (vecElem != *copiedIterator)
        BOOST_CHECK_EQUAL(vecElem, *copiedIterator);
      copiedIterator++;
    }
  }

BOOST_AUTO_TEST_SUITE_END()