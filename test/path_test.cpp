//  path_test program  -------------------------------------------------------//

//  (C) Copyright Beman Dawes 2002. Permission to copy,
//  use, modify, sell and distribute this software is granted provided this
//  copyright notice appears in all copies. This software is provided "as is"
//  without express or implied warranty, and with no claim as to its
//  suitability for any purpose.

//  See http://www.boost.org for most recent version including documentation.

#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/exception.hpp>
#include <iostream>
#include <cassert>

namespace fs = boost::filesystem;
using boost::filesystem::path;

#define BOOST_INCLUDE_MAIN
#include <boost/test/test_tools.hpp>

namespace {
  int errors;

  void check( const fs::path & source,
              const std::string & expected )
  {
    if ( source.generic_path()== expected ) return;

    ++errors;

    std::cout << "source.generic_path(): \"" << source.generic_path()
              << "\" != expected: \"" << expected
              << "\"" << std::endl;
  }

  void check_throw( const std::string & arg )
  {
    try
    {
      fs::path arg_path( arg );
      ++errors;
      std::cout << "failed to throw with argument \"" << arg
                << "\"" << std::endl;
    }
    catch ( const fs::filesystem_error & /*ex*/ )
    {
//      std::cout << ex.what() << "\n";
    }
  }

} // unnamed namespace

int test_main( int, char*[] )
{

  path p1( "fe/fi/fo/fum" );
  path p2( p1 );
  path p3;
  p3 = p2;

  // These verify various overloads don't cause compiler errors
  fs::exists( "foo" );
  fs::exists( std::string( "foo" ) );
  fs::exists( p1 );
  fs::exists( "foo" << p1 );
  fs::exists( std::string( "foo" ) << p1 );
  fs::exists( fs::check_posix_leaf( "foo" ) );

  BOOST_TEST( p1.generic_path() == p2.generic_path() );
  BOOST_TEST( p1.generic_path() == p3.generic_path() );
  BOOST_TEST( path( "foo" ).leaf() == "foo" );
  BOOST_TEST( path( "foo" ).branch().generic_path() == "" );
  BOOST_TEST( p1.leaf() == "fum" );
  BOOST_TEST( p1.branch().generic_path() == "fe/fi/fo" );
  BOOST_TEST( path( "" ).empty() == true );
  BOOST_TEST( path( "foo" ).empty() == false );

  check( "", "" );

  check( "foo", "foo" );
  check( path("") << "foo", "foo" );
  check( path("foo") << "", "foo" );

  check( "foo/bar", "foo/bar" );
  check( path("foo") << "bar", "foo/bar" );
  check( path("foo") << path("bar"), "foo/bar" );
  check( "foo" << path("bar"), "foo/bar" );

  check( "a/b", "a/b" );  // probe for length effects
  check( path("a") << "b", "a/b" );

  check( "..", ".." );
  check( path("..") << "", ".." );
  check( path("") << "..", ".." );

  check( "../..", "../.." );
  check( path("..") << ".." , "../.." );

  check( "../foo", "../foo" );
  check( path("..") << "foo" , "../foo" );

  check( "foo/..", "" );
  check( path("foo") << ".." , "" );

  check( "../f", "../f" );
  check( path("..") << "f" , "../f" );

  check( "f/..", "" );
  check( path("f") << ".." , "" );

  check( "foo/../..", ".." );
  check( path("foo") << ".." << ".." , ".." );

  check( "foo/../../..", "../.." );
  check( path("foo") << ".." << ".." << ".." , "../.." );

  check( "foo/../bar", "bar" );
  check( path("foo") << ".." << "bar" , "bar" );

  check( "foo/bar/..", "foo" );
  check( path("foo") << "bar" << ".." , "foo" );

  check( "foo/bar/../blah", "foo/blah" );
  check( path("foo") << "bar" << ".." << "blah", "foo/blah" );

  check( "f/../b", "b" );
  check( path("f") << ".." << "b" , "b" );

  check( "f/b/..", "f" );
  check( path("f") << "b" << ".." , "f" );

  check( "f/b/../a", "f/a" );
  check( path("f") << "b" << ".." << "a", "f/a" );

  check( "foo/bar/blah/../..", "foo" );
  check( path("foo") << "bar" << "blah" << ".." << "..", "foo" );

  check( "foo/bar/blah/../../bletch", "foo/bletch" );
  check( path("foo") << "bar" << "blah" << ".." << ".." << "bletch", "foo/bletch" );
  
  BOOST_TEST( fs::posix_name("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789.-_") );
  BOOST_TEST( !fs::posix_name("F$O") );
  BOOST_TEST( !fs::posix_name(".") );
  BOOST_TEST( !fs::boost_file_name("ABCDEFGHIJKLMNOPQRSTUVWXYZ") );
  BOOST_TEST( fs::boost_file_name("abcdefghijklmnopqrstuvwxyz") );
  BOOST_TEST( fs::boost_file_name("0123456789.-_") );
  BOOST_TEST( fs::boost_file_name("1234567890123456789012345678901") );
  BOOST_TEST( !fs::boost_file_name("12345678901234567890123456789012") );
  BOOST_TEST( !fs::boost_file_name("F$O") );
  BOOST_TEST( !fs::boost_file_name(".") );
  BOOST_TEST( !fs::boost_directory_name("ABCDEFGHIJKLMNOPQRSTUVWXYZ") );
  BOOST_TEST( fs::boost_directory_name("abcdefghijklmnopqrstuvwxyz") );
  BOOST_TEST( fs::boost_directory_name("0123456789-_") );
  BOOST_TEST( fs::boost_directory_name("1234567890123456789012345678901") );
  BOOST_TEST( !fs::boost_directory_name("12345678901234567890123456789012") );
  BOOST_TEST( !fs::boost_directory_name("F$O") );

  check_throw( "/" );
  check_throw( "...." );
  check_throw( "/foo" );
  check_throw( "foo/" );
  check_throw( "foo/...." );
  check_throw( "foo//bar" );
  check_throw( "foo\\bar" );
  check_throw( "" );
  check_throw( " " );
  check_throw( " foo" );
  check_throw( "foo " );
  check_throw( ">" );
  check_throw( "<" );
  check_throw( ":" );
  check_throw( "." );
  check_throw( "\"" );
  check_throw( "|" );

  check_throw( "c:" );
  check_throw( "c:/" );
  check_throw( "//share" );
  check_throw( "prn:" );

# ifdef BOOST_WINDOWS

  check( path( "/", path::system_specific ), "/" );
  check( path( "/f", path::system_specific ), "/f" );
  check( path( "/foo", path::system_specific ), "/foo" );
  check( path( "\\", path::system_specific ), "/" );
  check( path( "\\f", path::system_specific ), "/f" );
  check( path( "\\foo", path::system_specific ), "/foo" );
  check( path( "foo\\bar", path::system_specific ), "foo/bar" );
  check( path( "foo bar", path::system_specific ), "foo bar" );
  check( path( "c:", path::system_specific ), "c:" );
  check( path( "c:/", path::system_specific ), "c:/" );
  check( path( "c:foo", path::system_specific ), "c:foo" );
  check( path( "c:/foo", path::system_specific ), "c:/foo" );
  check( path( "//share", path::system_specific ), "//share" );
  check( path( "//share/foo", path::system_specific ), "//share/foo" );
  check( path( "\\\\share", path::system_specific ), "//share" );
  check( path( "\\\\share\\foo", path::system_specific ), "//share/foo" );
  check( path( "c:/foo", path::system_specific ), "c:/foo" );
  check( path( "prn:", path::system_specific ), "prn:" );

  BOOST_TEST( path( "/", path::system_specific ).leaf() == "/" );
  BOOST_TEST( path( "c:", path::system_specific ).leaf() == "c:" );
  BOOST_TEST( path( "c:/", path::system_specific ).leaf() == "c:/" );
  BOOST_TEST( path( "c:foo", path::system_specific ).leaf() == "foo" );
  BOOST_TEST( path( "c:/foo", path::system_specific ).leaf() == "foo" );
  BOOST_TEST( path( "//share", path::system_specific ).leaf() == "//share" );
  BOOST_TEST( path( "//share/foo", path::system_specific ).leaf() == "foo" );

  BOOST_TEST( path( "/", path::system_specific ).branch().generic_path() == "" );
  BOOST_TEST( path( "c:", path::system_specific ).branch().generic_path() == "" );
  BOOST_TEST( path( "c:/", path::system_specific ).branch().generic_path() == "" );
  BOOST_TEST( path( "c:foo", path::system_specific ).branch().generic_path() == "c:" );
  BOOST_TEST( path( "c:/foo", path::system_specific ).branch().generic_path() == "c:/" );
  BOOST_TEST( path( "//share", path::system_specific ).branch().generic_path() == "" );
  BOOST_TEST( path( "//share/foo", path::system_specific ).branch().generic_path() == "//share" );

# endif  // BOOST_WINDOWS

//  std::cout << errors << " errors detected\n";

  return errors;
}