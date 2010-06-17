#include <boost/io/quote_manip.hpp>
#include <iostream>
#include <sstream>
#include <cassert>

using boost::io::quote;
using boost::io::unquote;

int main()
{

  std::cout << quote(std::string("foo\\bar, \" *")) << std::endl;
  std::cout << quote("foo\\bar, \" *") << std::endl;
  std::cout << quote("foo & bar, \" *", '&') << std::endl;
  std::cout << quote("foo & bar, * ", '&', '*') << std::endl;

  std::wcout << quote(L"foo$bar, \" *", L'$') << std::endl;

  std::string non_const_string("non_const_string");
  std::cout << quote(non_const_string) << '\n';

  std::stringstream ss;

  const std::string expected("foo\\bar, \" *");
  std::string actual;

  ss << quote(expected);
  ss >> unquote(actual);

  std::cout << "round trip tests...\n";
  std::cout << "  expected--: " << expected << '\n';
  std::cout << "  actual----: " << actual << '\n';

  assert(expected == actual);

  // these should fail to compile because the arguments are non-const:
  //   ss >> unquote(expected);
  //   ss >> unquote("foo");

  return 0;
}
