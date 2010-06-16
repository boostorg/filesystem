#include <boost/delimit_string.hpp>
#include <iostream>
#include <sstream>
#include <cassert>

using boost::delimit;
using boost::undelimit;

int main()
{
  std::cout << delimit("foo\\bar, \" *") << std::endl;
  std::cout << delimit("foo & bar, \" *", '&') << std::endl;
  std::cout << delimit("foo & bar, * ", '&', '*') << std::endl;

  std::wcout << delimit(L"foo$bar, \" *", L'$') << std::endl;

  std::string non_const_string("non_const_string");
  std::cout << delimit(non_const_string) << '\n';

  std::stringstream ss;

  const std::string expected("foo\\bar, \" *");
  std::string actual;

  ss << delimit(expected);
  ss >> undelimit(actual);

  std::cout << "round trip tests...\n";
  std::cout << "  expected--: " << expected << '\n';
  std::cout << "  actual----: " << actual << '\n';

  assert(expected == actual);

  // each of these should fail to compile because they are const:
  //   ss >> undelimit(expected);
  //   ss >> undelimit("foo");

  return 0;
}
