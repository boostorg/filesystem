#include <boost/delimit_string.hpp>
#include <iostream>
#include <cassert>

using boost::string::delimit_string;

// Should these two operators be in the global namespace? 
using boost::string::operator<<;
using boost::string::operator>>;

int main()
{
  std::cout << delimit_string(std::string("foo\\bar, \" *")) << std::endl;
  //std::cout << delimit_string("foo & bar, \" *", '&') << std::endl;
  //std::cout << delimit_string("foo & bar, * ", '&', '*') << std::endl;

  std::string non_const_string("non_const_string");
  std::cout << delimit_string(non_const_string) << '\n';

  std::stringstream ss;

  const std::string expected("foo\\bar, \" *");
  std::string actual;

  ss << delimit_string(expected) << '\n';
  ss >> delimit_string<std::string>(actual);

  std::cout << "round trip tests...\n";
  std::cout << "  expected--: " << expected << '\n';
  std::cout << "  actual----: " << actual << '\n';

  assert(expected == actual);

  // each of these should fail to compile because they are const:
  //   ss >> delimit_string(expected);
  //   ss >> delimit_string("foo");

  return 0;
}
