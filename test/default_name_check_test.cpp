//  default_name_check_test program  ------------------------------------------//

//  Copyright Beman Dawes 2003.
//  See accompanying license for terms and conditions of use.

//  See http://www.boost.org/libs/filesystem for documentation.

#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/exception.hpp>

namespace fs = boost::filesystem;
using boost::filesystem::path;

#include <boost/test/minimal.hpp>

int test_main( int, char*[] )
{
  BOOST_TEST( path::default_name_check_writable() );
  path::default_name_check( fs::no_check );
  BOOST_TEST( !path::default_name_check_writable() );
  BOOST_TEST( path::default_name_check() == fs::no_check );
  return 0;
}

