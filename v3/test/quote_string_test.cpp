#include <boost/io/quoted_manip.hpp>
#include <iostream>
#include <sstream>
#include <cassert>

using boost::io::quoted;

int main()
{

  std::cout << quoted(std::string("foo\\bar, \" *")) << std::endl;
  std::cout << quoted("'Jack & Jill'", '&', '\'') << '\n';  // outputs: '&'Jack && Jill&''
  std::wcout << "Wide: " << quoted(L"foo$bar, \" *", L'$') << std::endl;

  std::string non_const_string("non-const string");
  std::cout << quoted(non_const_string) << '\n';

  std::stringstream ss;

  const std::string expected("foo\\bar, \" *");
  std::string actual;

  ss << quoted(expected);
  ss >> quoted(actual);

  std::cout << "round trip tests...\n";
  std::cout << "  expected--: " << expected << '\n';
  std::cout << "  actual----: " << actual << '\n';

  assert(expected == actual);

  ss << "\"abc"; // note missing end delimiter
  ss >> quoted(actual);
  std::cout << '*' << actual << "*\n";

  // these should fail to compile because the arguments are non-const:
  //   ss >> quoted(expected);
  //   ss >> quoted("foo");

  return 0;
}
