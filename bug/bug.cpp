//  filesystem/bug/bug.cpp

#include <boost/detail/lightweight_test_report.hpp>

int test_main(int, char*[])          // note name
{
  BOOST_TEST(2 + 2 == 6);
  BOOST_TEST_EQ(2 + 2, 6);

  return ::boost::report_errors();   // required
}

//  Copyright Beman Dawes 2014
//  Distributed under the Boost Software License, Version 1.0.
//  www.boost.org/LICENSE_1_0.txt
