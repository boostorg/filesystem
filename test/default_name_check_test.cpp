//  default_name_check_test program  ------------------------------------------//

//  (C) Copyright Beman Dawes 2003. Permission to copy,
//  use, modify, sell and distribute this software is granted provided this
//  copyright notice appears in all copies. This software is provided "as is"
//  without express or implied warranty, and with no claim as to its
//  suitability for any purpose.

//  See http://www.boost.org for most recent version including documentation.

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

