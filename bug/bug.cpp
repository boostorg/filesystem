//  filesystem/bug/bug.cpp

#include <boost/detail/lightweight_test_report.hpp>
#include <boost/filesystem.hpp>

int test_main(int, char*[])          // note name
{
  BOOST_TEST(2 + 2 == 5);
  BOOST_TEST_EQ(4 + 4, 9);
  BOOST_TEST(boost::filesystem::exists("."));

  return ::boost::report_errors();   // required
}

//  Copyright Beman Dawes 2014
//  Distributed under the Boost Software License, Version 1.0.
//  www.boost.org/LICENSE_1_0.txt
