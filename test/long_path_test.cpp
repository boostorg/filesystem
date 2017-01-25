//  long_path_test.cpp  ----------------------------------------------------------------//

//  Copyright Beman Dawes 2011

//  Distributed under the Boost Software License, Version 1.0.
//  http://www.boost.org/LICENSE_1_0.txt

/*----------------------------------------------------------------------------------------

This is a Windows-only test is to ensure that \\?\ pathnames (i.e. very long paths)
work correctly.

See http://msdn.microsoft.com/en-us/library/aa365247%28v=vs.85%29.aspx

----------------------------------------------------------------------------------------*/

#include <boost/config/warning_disable.hpp>

#include <boost/filesystem.hpp>
#include <string>

using namespace boost::filesystem;

#include <iostream>
#define BOOST_LIGHTWEIGHT_TEST_OSTREAM std::cout
#include <boost/core/lightweight_test.hpp>
#include <boost/detail/lightweight_main.hpp>

namespace
{
}  // unnamed namespace

int cpp_main(int, char*[])
{

  std::string prefix("d:\\temp\\");
  std::cout << "prefix is " << prefix << '\n';

  const std::size_t safe_size
    = 260 - prefix.size() - 100;  // Windows MAX_PATH is 260

  std::string safe_x_string(safe_size, 'x');
  std::string safe_y_string(safe_size, 'y');
  std::string path_escape("\\\\?\\");

  path x_p(prefix + safe_x_string);
  path y_p(path_escape + prefix + safe_x_string + "\\" + safe_y_string);

  std::cout << "x_p.native().size() is " << x_p.native().size() << '\n';
  std::cout << "y_p.native().size() is " << y_p.native().size() << '\n';

  std::cout << "removing y_p" << std::endl;
  remove(y_p);   // remove residue from prior tests
  std::cout << "removing x_p" << std::endl;
  remove(x_p);   // remove residue from prior tests

  std::cout << "creating x_p" << std::endl;
  create_directory(x_p);
  std::cout << "test exists(x_p)" << std::endl;
  BOOST_TEST(exists(x_p));
  std::cout << "creating y_p" << std::endl;
  create_directory(y_p);
  std::cout << "test exists(y_p)" << std::endl;
  BOOST_TEST(exists(y_p));
  std::cout << "retest exists(x_p)" << std::endl;
  BOOST_TEST(exists(x_p));

  return ::boost::report_errors();
}
