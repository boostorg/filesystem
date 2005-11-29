//  path_test program  -------------------------------------------------------//

//  Copyright Beman Dawes 2002.
//  Use, modification, and distribution is subject to the Boost Software
//  License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

//  See library home page at http://www.boost.org/libs/filesystem

#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/exception.hpp>
#include <boost/utility.hpp>
#include <iostream>
#include <string>
#include <cstring>
#include <cassert>

namespace fs = boost::filesystem;
using boost::filesystem::path;
using boost::next;
using boost::prior;

#include <boost/test/minimal.hpp>

#define PATH_CHECK( a, b ) check( a, b, __LINE__ )

namespace {
  int errors;

  void check( const fs::path & source,
              const std::string & expected, int line )
  {
    if ( source.string()== expected ) return;

    ++errors;

    std::cout << '(' << line << ") source.string(): \"" << source.string()
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
  std::string platform( BOOST_PLATFORM );

  // The choice of platform is make at runtime rather than compile-time
  // so that compile errors for all platforms will be detected even though
  // only the current platform is runtime tested.
# if defined( BOOST_POSIX )
    platform = "POSIX";
# elif defined( BOOST_WINDOWS )
    platform = "Windows";
# else
    platform = ( platform == "Win32" || platform == "Win64" || platform == "Cygwin" )
               ? "Windows"
               : "POSIX";
# endif
  std::cout << "Platform is " << platform << '\n';

  BOOST_CHECK( path::default_name_check_writable() );
  BOOST_CHECK( path::default_name_check() == fs::portable_name );
  BOOST_CHECK( !path::default_name_check_writable() );
  bool default_name_check_threw = false;
  try { path::default_name_check( fs::no_check ); }
  catch ( const fs::filesystem_error & ) { default_name_check_threw = true; }
  BOOST_CHECK( default_name_check_threw );
  BOOST_CHECK( path::default_name_check() == fs::portable_name );


  path p1( "fe/fi/fo/fum" );
  path p2( p1 );
  path p3;
  BOOST_CHECK( p1.string() != p3.string() );
  p3 = p2;

//  p1.branch_path() = p2; // should fail
//  *p1.begin() = ""; // should fail

  // These verify various overloads don't cause compiler errors
  fs::exists( "foo" );
  fs::exists( std::string( "foo" ) );
  fs::exists( p1 );
  fs::exists( "foo" / p1 );
  fs::exists( std::string( "foo" ) / p1 );

  BOOST_CHECK( p1.string() == p2.string() );
  BOOST_CHECK( p1.string() == p3.string() );
  BOOST_CHECK( path( "foo" ).leaf() == "foo" );
  BOOST_CHECK( path( "foo" ).branch_path().string() == "" );
  BOOST_CHECK( p1.leaf() == "fum" );
  BOOST_CHECK( p1.branch_path().string() == "fe/fi/fo" );
  BOOST_CHECK( path( "" ).empty() == true );
  BOOST_CHECK( path( "foo" ).empty() == false );

  PATH_CHECK( "", "" );

  PATH_CHECK( "foo", "foo" );
  PATH_CHECK( "f", "f" );
  PATH_CHECK( "foo/", "foo" );
  PATH_CHECK( path("foo/").normalize(), "foo" );
  PATH_CHECK( "f/", "f" );
  PATH_CHECK( path("f/").normalize(), "f" );
  PATH_CHECK( path("") / "foo", "foo" );
  PATH_CHECK( path("foo") / "", "foo" );
  PATH_CHECK( path( "/" ), "/" );
  PATH_CHECK( path( "/" ) / "", "/" );
  PATH_CHECK( path( "/f" ), "/f" );
  PATH_CHECK( path( "/foo" ).normalize(), "/foo" );

  PATH_CHECK( "/foo", "/foo" );
  PATH_CHECK( path("") / "/foo", "/foo" );
  PATH_CHECK( path("/foo") / "", "/foo" );

  PATH_CHECK( "foo/", "foo" );
  PATH_CHECK( path("") / "foo/", "foo" );
  PATH_CHECK( path("foo") / "/", "foo" );

  if ( platform == "Windows" )
  {
    PATH_CHECK( path("c:") / "foo", "c:foo" );
    PATH_CHECK( path("c:") / "/foo", "c:/foo" );
  }

  PATH_CHECK( "foo/bar", "foo/bar" );
  PATH_CHECK( path( "foo/bar" ).normalize(), "foo/bar" );
  PATH_CHECK( path("foo") / path("bar"), "foo/bar" ); // path arg
  PATH_CHECK( path("foo") / "bar", "foo/bar" );       // const char * arg
  PATH_CHECK( path("foo") / path("woo/bar").leaf(), "foo/bar" ); // const std::string & arg
  PATH_CHECK( "foo" / path("bar"), "foo/bar" );

  PATH_CHECK( "a/b", "a/b" );  // probe for length effects
  PATH_CHECK( path("a") / "b", "a/b" );

  PATH_CHECK( "..", ".." );
  PATH_CHECK( path("..").normalize(), ".." );
  PATH_CHECK( path("..") / "", ".." );
  PATH_CHECK( path("") / "..", ".." );

  PATH_CHECK( "../..", "../.." );
  PATH_CHECK( path("../..").normalize(), "../.." );
  PATH_CHECK( path("..") / ".." , "../.." );

  PATH_CHECK( "/..", "/" );
  PATH_CHECK( path("/..").normalize(), "/" );
  PATH_CHECK( path("/") / ".." , "/" );

  PATH_CHECK( "/../..", "/" );
  PATH_CHECK( path("/../..").normalize(), "/" );
  PATH_CHECK( path("/..") / ".." , "/" );

  PATH_CHECK( "../foo", "../foo" );
  PATH_CHECK( path("../foo").normalize(), "../foo" );
  PATH_CHECK( path("..") / "foo" , "../foo" );

  PATH_CHECK( "foo/..", "foo/.." );
  PATH_CHECK( path("foo") / ".." , "foo/.." );
  PATH_CHECK( path("foo/..").normalize(), "." );
  PATH_CHECK( (path("foo") / "..").normalize() , "." );
  PATH_CHECK( path( "foo/..bar", fs::no_check ), "foo/..bar" );
  PATH_CHECK( path("foo/..bar", fs::no_check ).normalize(), "foo/..bar" );

  PATH_CHECK( "../f", "../f" );
  PATH_CHECK( path("../f").normalize(), "../f" );
  PATH_CHECK( path("..") / "f" , "../f" );

  PATH_CHECK( "/../f", "/f" );
  PATH_CHECK( path("/../f").normalize(), "/f" );
  PATH_CHECK( path("/..") / "f" , "/f" );

  PATH_CHECK( "f/..", "f/.." );
  PATH_CHECK( path("f") / ".." , "f/.." );
  PATH_CHECK( path("f/..").normalize(), "." );
  PATH_CHECK( (path("f") / "..").normalize() , "." );

  PATH_CHECK( "foo/../..", "foo/../.." );
  PATH_CHECK( path("foo/../..").normalize(), ".." );
  PATH_CHECK( path("foo") / ".." / ".." , "foo/../.." );

  PATH_CHECK( "foo/../../..", "foo/../../.." );
  PATH_CHECK( path("foo/../../..").normalize(), "../.." );
  PATH_CHECK( path("foo") / ".." / ".." / ".." , "foo/../../.." );

  PATH_CHECK( "foo/../bar", "foo/../bar" );
  PATH_CHECK( path("foo/../bar").normalize(), "bar" );
  PATH_CHECK( path("foo") / ".." / "bar" , "foo/../bar" );

  PATH_CHECK( "foo/bar/..", "foo/bar/.." );
  PATH_CHECK( path("foo/bar/..").normalize(), "foo" );
  PATH_CHECK( path("foo") / "bar" / ".." , "foo/bar/.." );

  PATH_CHECK( "foo/bar/../..", "foo/bar/../.." );
  PATH_CHECK( path("foo/bar/../..").normalize(), "." );
  PATH_CHECK( path("foo") / "bar" / ".." / "..", "foo/bar/../.." );

  PATH_CHECK( "foo/bar/../blah", "foo/bar/../blah" );
  PATH_CHECK( path("foo/bar/../blah").normalize(), "foo/blah" );
  PATH_CHECK( path("foo") / "bar" / ".." / "blah", "foo/bar/../blah" );

  PATH_CHECK( "f/../b", "f/../b" );
  PATH_CHECK( path("f/../b").normalize(), "b" );
  PATH_CHECK( path("f") / ".." / "b" , "f/../b" );

  PATH_CHECK( "f/b/..", "f/b/.." );
  PATH_CHECK( path("f/b/..").normalize(), "f" );
  PATH_CHECK( path("f") / "b" / ".." , "f/b/.." );

  PATH_CHECK( "f/b/../a", "f/b/../a" );
  PATH_CHECK( path("f/b/../a").normalize(), "f/a" );
  PATH_CHECK( path("f") / "b" / ".." / "a", "f/b/../a" );

  PATH_CHECK( "foo/bar/blah/../..", "foo/bar/blah/../.." );
  PATH_CHECK( path("foo/bar/blah/../..").normalize(), "foo" );
  PATH_CHECK( path("foo") / "bar" / "blah" / ".." / "..", "foo/bar/blah/../.." );

  PATH_CHECK( "foo/bar/blah/../../bletch", "foo/bar/blah/../../bletch" );
  PATH_CHECK( path("foo/bar/blah/../../bletch").normalize(), "foo/bletch" );
  PATH_CHECK( path("foo") / "bar" / "blah" / ".." / ".." / "bletch", "foo/bar/blah/../../bletch" );

  PATH_CHECK( path("...", fs::portable_posix_name ), "..." );
  PATH_CHECK( path("....", fs::portable_posix_name ), "...." );
  PATH_CHECK( path("foo/...", fs::portable_posix_name ), "foo/..." );
  PATH_CHECK( path("foo/...", fs::portable_posix_name ).normalize(), "foo/..." );
  PATH_CHECK( path("abc.", fs::portable_posix_name ), "abc." );
  PATH_CHECK( path("abc..", fs::portable_posix_name ), "abc.." );
  PATH_CHECK( path("foo/abc.", fs::portable_posix_name ), "foo/abc." );
  PATH_CHECK( path("foo/abc..", fs::portable_posix_name ), "foo/abc.." );

  PATH_CHECK( path(".abc", fs::no_check), ".abc" );
  PATH_CHECK( "a.c", "a.c" );
  PATH_CHECK( path("..abc", fs::no_check), "..abc" );
  PATH_CHECK( "a..c", "a..c" );
  PATH_CHECK( path("foo/.abc", fs::no_check), "foo/.abc" );
  PATH_CHECK( "foo/a.c", "foo/a.c" );
  PATH_CHECK( path("foo/..abc", fs::no_check), "foo/..abc" );
  PATH_CHECK( path("foo/..abc", fs::no_check).normalize(), "foo/..abc" );
  PATH_CHECK( "foo/a..c", "foo/a..c" );

  PATH_CHECK( ".", "." );
  PATH_CHECK( path("") / ".", "." );
  PATH_CHECK( "./foo", "foo" );
  PATH_CHECK( path(".") / "foo", "foo" );
  PATH_CHECK( "./..", ".." );
  PATH_CHECK( path(".") / "..", ".." );
  PATH_CHECK( "./../foo", "../foo" );
  PATH_CHECK( "foo/.", "foo" );
  PATH_CHECK( path("foo") / ".", "foo" );
  PATH_CHECK( "../.", ".." );
  PATH_CHECK( path("..") / ".", ".." );
  PATH_CHECK( "./.", "." );
  PATH_CHECK( path(".") / ".", "." );
  PATH_CHECK( "././.", "." );
  PATH_CHECK( path(".") / "." / ".", "." );
  PATH_CHECK( "./foo/.", "foo" );
  PATH_CHECK( path(".") / "foo" / ".", "foo" );
  PATH_CHECK( "foo/./bar", "foo/bar" );
  PATH_CHECK( path("foo") / "." / "bar", "foo/bar" );
  PATH_CHECK( "foo/./.", "foo" );
  PATH_CHECK( path("foo") / "." / ".", "foo" );
  PATH_CHECK( "foo/./..", "foo/.." );
  PATH_CHECK( path("foo") / "." / "..", "foo/.." );
  PATH_CHECK( "foo/./../bar", "foo/../bar" );
  PATH_CHECK( "foo/../.", "foo/.." );
  PATH_CHECK( path(".") / "." / "..", ".." );
  PATH_CHECK( "././..", ".." );
  PATH_CHECK( path(".") / "." / "..", ".." );
  PATH_CHECK( "./../.", ".." );
  PATH_CHECK( path(".") / ".." / ".", ".." );
  PATH_CHECK( ".././.", ".." );
  PATH_CHECK( path("..") / "." / ".", ".." );

  BOOST_CHECK( path("foo\\bar", fs::no_check).leaf() == "foo\\bar" );
  
  BOOST_CHECK( fs::portable_posix_name(".") );
  BOOST_CHECK( fs::portable_posix_name("..") );
  BOOST_CHECK( fs::portable_posix_name("...") );
  BOOST_CHECK( fs::portable_posix_name("....") );
  BOOST_CHECK( fs::portable_posix_name("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789.-_") );
  BOOST_CHECK( !fs::portable_posix_name("F$O") );

  BOOST_CHECK( fs::portable_name(".") );
  BOOST_CHECK( fs::portable_name("..") );
  BOOST_CHECK( fs::portable_name("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789.-_") );
  BOOST_CHECK( !fs::portable_name("A.") );
  BOOST_CHECK( fs::portable_name("A-") );
  BOOST_CHECK( !fs::portable_name(".A") );
  BOOST_CHECK( !fs::portable_name("-A") );
  BOOST_CHECK( !fs::portable_name("F$O") );


  BOOST_CHECK( fs::portable_file_name(".") );
  BOOST_CHECK( fs::portable_file_name("..") );
  BOOST_CHECK( fs::portable_file_name("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789.-_") );
  BOOST_CHECK( fs::portable_file_name("0123456789.-_") );
  BOOST_CHECK( fs::portable_file_name("1234567890123456789012345678901") );
  BOOST_CHECK( !fs::portable_file_name("a.") );
  BOOST_CHECK( !fs::portable_file_name("a..b") );
  BOOST_CHECK( !fs::portable_file_name("a.bcde") );
  BOOST_CHECK( !fs::portable_file_name("a..cde") );
  BOOST_CHECK( !fs::portable_file_name("a.c.de") );
  BOOST_CHECK( !fs::portable_file_name("a.cd.e") );
  BOOST_CHECK( fs::portable_file_name("a.b") );
  BOOST_CHECK( fs::portable_file_name("a.bc") );
  BOOST_CHECK( fs::portable_file_name("a.bcd") );
  BOOST_CHECK( !fs::portable_file_name("A.") );
  BOOST_CHECK( fs::portable_file_name("A-") );
  BOOST_CHECK( !fs::portable_file_name(".A") );
  BOOST_CHECK( !fs::portable_file_name("-A") );
  BOOST_CHECK( !fs::portable_file_name("F$O") );

  BOOST_CHECK( fs::portable_directory_name(".") );
  BOOST_CHECK( fs::portable_directory_name("..") );
  BOOST_CHECK( fs::portable_directory_name("ABCDEFGHIJKLMNOPQRSTUVWXYZ") );
  BOOST_CHECK( fs::portable_directory_name("abcdefghijklmnopqrstuvwxyz") );
  BOOST_CHECK( fs::portable_directory_name("0123456789-_") );
  BOOST_CHECK( fs::portable_directory_name("1234567890123456789012345678901") );
  BOOST_CHECK( !fs::portable_directory_name("a.") );
  BOOST_CHECK( !fs::portable_directory_name("a.bcde") );
  BOOST_CHECK( !fs::portable_directory_name("a..cde") );
  BOOST_CHECK( !fs::portable_directory_name("a.c.de") );
  BOOST_CHECK( !fs::portable_directory_name("a.cd.e") );
  BOOST_CHECK( !fs::portable_directory_name("a.b") );
  BOOST_CHECK( !fs::portable_directory_name("a.bc") );
  BOOST_CHECK( !fs::portable_directory_name("a.bcd") );
  BOOST_CHECK( !fs::portable_directory_name("A.") );
  BOOST_CHECK( fs::portable_directory_name("A-") );
  BOOST_CHECK( !fs::portable_directory_name(".A") );
  BOOST_CHECK( !fs::portable_directory_name("-A") );
  BOOST_CHECK( !fs::portable_directory_name("F$O") );

  check_throw( "foo//bar" );
  check_throw( "foo\\bar" );
  check_throw( " " );
  check_throw( " foo" );
  check_throw( "foo " );
  check_throw( ">" );
  check_throw( "<" );
  check_throw( ":" );
  check_throw( "\"" );
  check_throw( "|" );

  check_throw( "c:" );
  check_throw( "c:/" );
  check_throw( "//share" );
  check_throw( "prn:" );

  path itr_ck( "/foo/bar" );
  path::iterator itr( itr_ck.begin() );
  BOOST_CHECK( *itr == std::string( "/" ) );
  BOOST_CHECK( *++itr == std::string( "foo" ) );
  BOOST_CHECK( *++itr == std::string( "bar" ) );
  BOOST_CHECK( ++itr == itr_ck.end() );
  BOOST_CHECK( *--itr == std::string( "bar" ) );
  BOOST_CHECK( *--itr == std::string( "foo" ) );
  BOOST_CHECK( *--itr == std::string( "/" ) );

  itr_ck = "";
  BOOST_CHECK( itr_ck.begin() == itr_ck.end() );

  itr_ck = path( "/" );
  BOOST_CHECK( *itr_ck.begin() == std::string( "/" ) );
  BOOST_CHECK( next(itr_ck.begin()) == itr_ck.end() );
  BOOST_CHECK( *prior(itr_ck.end()) == std::string( "/" ) );
  BOOST_CHECK( prior(itr_ck.end()) == itr_ck.begin() );

  itr_ck = path( "/foo" );
  BOOST_CHECK( *itr_ck.begin() == std::string( "/" ) );
  BOOST_CHECK( *next( itr_ck.begin() ) == std::string( "foo" ) );
  BOOST_CHECK( next(next( itr_ck.begin() )) == itr_ck.end() );
  BOOST_CHECK( next( itr_ck.begin() ) == prior( itr_ck.end() ) );
  BOOST_CHECK( *prior( itr_ck.end() ) == std::string( "foo" ) );
  BOOST_CHECK( *prior(prior( itr_ck.end() )) == std::string( "/" ) );
  BOOST_CHECK( prior(prior( itr_ck.end() )) == itr_ck.begin() );

  itr_ck = "foo";
  BOOST_CHECK( *itr_ck.begin() == std::string( "foo" ) );
  BOOST_CHECK( next( itr_ck.begin() ) == itr_ck.end() );
  BOOST_CHECK( *prior( itr_ck.end() ) == std::string( "foo" ) );
  BOOST_CHECK( prior( itr_ck.end() ) == itr_ck.begin() );

  path p;

  p = "";
  BOOST_CHECK( p.relative_path().string() == "" );
  BOOST_CHECK( p.branch_path().string() == "" );
  BOOST_CHECK( p.leaf() == "" );
  BOOST_CHECK( p.root_name() == "" );
  BOOST_CHECK( p.root_directory() == "" );
  BOOST_CHECK( p.root_path().string() == "" );
  BOOST_CHECK( !p.has_root_path() );
  BOOST_CHECK( !p.has_root_name() );
  BOOST_CHECK( !p.has_root_directory() );
  BOOST_CHECK( !p.has_relative_path() );
  BOOST_CHECK( !p.has_leaf() );
  BOOST_CHECK( !p.has_branch_path() );
  BOOST_CHECK( !p.is_complete() );

  p = "/";
  BOOST_CHECK( p.relative_path().string() == "" );
  BOOST_CHECK( p.branch_path().string() == "" );
  BOOST_CHECK( p.leaf() == "/" );
  BOOST_CHECK( p.root_name() == "" );
  BOOST_CHECK( p.root_directory() == "/" );
  BOOST_CHECK( p.root_path().string() == "/" );
  BOOST_CHECK( p.has_root_path() );
  BOOST_CHECK( !p.has_root_name() );
  BOOST_CHECK( p.has_root_directory() );
  BOOST_CHECK( !p.has_relative_path() );
  BOOST_CHECK( p.has_leaf() );
  BOOST_CHECK( !p.has_branch_path() );
  if ( platform == "POSIX" )
    BOOST_CHECK( p.is_complete() );
  else
    BOOST_CHECK( !p.is_complete() );

  p = ".";
  BOOST_CHECK( p.relative_path().string() == "." );
  BOOST_CHECK( p.branch_path().string() == "" );
  BOOST_CHECK( p.leaf() == "." );
  BOOST_CHECK( p.root_name() == "" );
  BOOST_CHECK( p.root_directory() == "" );
  BOOST_CHECK( p.root_path().string() == "" );
  BOOST_CHECK( !p.has_root_path() );
  BOOST_CHECK( !p.has_root_name() );
  BOOST_CHECK( !p.has_root_directory() );
  BOOST_CHECK( p.has_relative_path() );
  BOOST_CHECK( p.has_leaf() );
  BOOST_CHECK( !p.has_branch_path() );
  BOOST_CHECK( !p.is_complete() );

  p = "..";
  BOOST_CHECK( p.relative_path().string() == ".." );
  BOOST_CHECK( p.branch_path().string() == "" );
  BOOST_CHECK( p.leaf() == ".." );
  BOOST_CHECK( p.root_name() == "" );
  BOOST_CHECK( p.root_directory() == "" );
  BOOST_CHECK( p.root_path().string() == "" );
  BOOST_CHECK( !p.has_root_path() );
  BOOST_CHECK( !p.has_root_name() );
  BOOST_CHECK( !p.has_root_directory() );
  BOOST_CHECK( p.has_relative_path() );
  BOOST_CHECK( p.has_leaf() );
  BOOST_CHECK( !p.has_branch_path() );
  BOOST_CHECK( !p.is_complete() );

  p = "foo";
  BOOST_CHECK( p.relative_path().string() == "foo" );
  BOOST_CHECK( p.branch_path().string() == "" );
  BOOST_CHECK( p.leaf() == "foo" );
  BOOST_CHECK( p.root_name() == "" );
  BOOST_CHECK( p.root_directory() == "" );
  BOOST_CHECK( p.root_path().string() == "" );
  BOOST_CHECK( !p.has_root_path() );
  BOOST_CHECK( !p.has_root_name() );
  BOOST_CHECK( !p.has_root_directory() );
  BOOST_CHECK( p.has_relative_path() );
  BOOST_CHECK( p.has_leaf() );
  BOOST_CHECK( !p.has_branch_path() );
  BOOST_CHECK( !p.is_complete() );

  p = "/foo";
  BOOST_CHECK( p.relative_path().string() == "foo" );
  BOOST_CHECK( p.branch_path().string() == "/" );
  BOOST_CHECK( p.leaf() == "foo" );
  BOOST_CHECK( p.root_name() == "" );
  BOOST_CHECK( p.root_directory() == "/" );
  BOOST_CHECK( p.root_path().string() == "/" );
  BOOST_CHECK( p.has_root_path() );
  BOOST_CHECK( !p.has_root_name() );
  BOOST_CHECK( p.has_root_directory() );
  BOOST_CHECK( p.has_relative_path() );
  BOOST_CHECK( p.has_leaf() );
  BOOST_CHECK( p.has_branch_path() );
  if ( platform == "POSIX" )
    BOOST_CHECK( p.is_complete() );
  else
    BOOST_CHECK( !p.is_complete() );

  p = "foo/bar";
  BOOST_CHECK( p.relative_path().string() == "foo/bar" );
  BOOST_CHECK( p.branch_path().string() == "foo" );
  BOOST_CHECK( p.leaf() == "bar" );
  BOOST_CHECK( p.root_name() == "" );
  BOOST_CHECK( p.root_directory() == "" );
  BOOST_CHECK( p.root_path().string() == "" );
  BOOST_CHECK( !p.has_root_path() );
  BOOST_CHECK( !p.has_root_name() );
  BOOST_CHECK( !p.has_root_directory() );
  BOOST_CHECK( p.has_relative_path() );
  BOOST_CHECK( p.has_leaf() );
  BOOST_CHECK( p.has_branch_path() );
  BOOST_CHECK( !p.is_complete() );

  p = "../foo";
  BOOST_CHECK( p.relative_path().string() == "../foo" );
  BOOST_CHECK( p.branch_path().string() == ".." );
  BOOST_CHECK( p.leaf() == "foo" );
  BOOST_CHECK( p.root_name() == "" );
  BOOST_CHECK( p.root_directory() == "" );
  BOOST_CHECK( p.root_path().string() == "" );
  BOOST_CHECK( !p.has_root_path() );
  BOOST_CHECK( !p.has_root_name() );
  BOOST_CHECK( !p.has_root_directory() );
  BOOST_CHECK( p.has_relative_path() );
  BOOST_CHECK( p.has_leaf() );
  BOOST_CHECK( p.has_branch_path() );
  BOOST_CHECK( !p.is_complete() );

  p = "/foo/bar";
  BOOST_CHECK( p.relative_path().string() == "foo/bar" );
  BOOST_CHECK( p.branch_path().string() == "/foo" );
  BOOST_CHECK( p.leaf() == "bar" );
  BOOST_CHECK( p.root_name() == "" );
  BOOST_CHECK( p.root_directory() == "/" );
  BOOST_CHECK( p.root_path().string() == "/" );
  BOOST_CHECK( p.has_root_path() );
  BOOST_CHECK( !p.has_root_name() );
  BOOST_CHECK( p.has_root_directory() );
  BOOST_CHECK( p.has_relative_path() );
  BOOST_CHECK( p.has_leaf() );
  BOOST_CHECK( p.has_branch_path() );
  if ( platform == "POSIX" )
    BOOST_CHECK( p.is_complete() );
  else
    BOOST_CHECK( !p.is_complete() );

  // decomposition and query functions must work even for paths which
  // do not pass the default name_check 
  p = path( "/</>", fs::no_check );
  BOOST_CHECK( p.relative_path().string() == "</>" );
  BOOST_CHECK( p.branch_path().string() == "/<" );
  BOOST_CHECK( p.leaf() == ">" );
  BOOST_CHECK( p.root_name() == "" );
  BOOST_CHECK( p.root_directory() == "/" );
  BOOST_CHECK( p.root_path().string() == "/" );
  BOOST_CHECK( p.has_root_path() );
  BOOST_CHECK( !p.has_root_name() );
  BOOST_CHECK( p.has_root_directory() );
  BOOST_CHECK( p.has_relative_path() );
  BOOST_CHECK( p.has_leaf() );
  BOOST_CHECK( p.has_branch_path() );
  if ( platform == "POSIX" )
    BOOST_CHECK( p.is_complete() );
  else
    BOOST_CHECK( !p.is_complete() );

  if ( platform == "Windows" )
  {
    PATH_CHECK( path( "\\", fs::native ), "/" );
    PATH_CHECK( path( "\\f", fs::native ), "/f" );
    PATH_CHECK( path( "\\foo", fs::native ), "/foo" );
    PATH_CHECK( path( "foo\\bar", fs::native ), "foo/bar" );
    PATH_CHECK( path( "foo bar", fs::native ), "foo bar" );
    PATH_CHECK( path( "c:", fs::native ), "c:" );
    PATH_CHECK( path( "c:/", fs::native ), "c:/" );
    PATH_CHECK( path( "c:.", fs::native ), "c:" );
    PATH_CHECK( path( "c:./foo", fs::native ), "c:foo" );
    PATH_CHECK( path( "c:.\\foo", fs::native ), "c:foo" );
    PATH_CHECK( path( "c:..", fs::native ), "c:.." );
    PATH_CHECK( path( "c:..", fs::native ).normalize(), "c:.." );
    PATH_CHECK( path( "c:/.", fs::native ), "c:/" );
    PATH_CHECK( path( "c:/..", fs::native ), "c:/" );
    PATH_CHECK( path( "c:/..", fs::native ).normalize(), "c:/" );
    PATH_CHECK( path( "c:/../", fs::native ), "c:/" );
    PATH_CHECK( path( "c:\\..\\", fs::native ), "c:/" );
    PATH_CHECK( path( "c:/../", fs::native ).normalize(), "c:/" );
    PATH_CHECK( path( "c:/../..", fs::native ), "c:/" );
    PATH_CHECK( path( "c:/../..", fs::native ).normalize(), "c:/" );
    PATH_CHECK( path( "c:/../foo", fs::native ), "c:/foo" );
    PATH_CHECK( path( "c:\\..\\foo", fs::native ), "c:/foo" );
    PATH_CHECK( path( "c:../foo", fs::native ), "c:../foo" );
    PATH_CHECK( path( "c:..\\foo", fs::native ), "c:../foo" );
    PATH_CHECK( path( "c:/../foo", fs::native ).normalize(), "c:/foo" );
    PATH_CHECK( path( "c:/../../foo", fs::native ), "c:/foo" );
    PATH_CHECK( path( "c:\\..\\..\\foo", fs::native ), "c:/foo" );
    PATH_CHECK( path( "c:/../../foo", fs::native ).normalize(), "c:/foo" );
    PATH_CHECK( path( "c:foo/..", fs::native ), "c:foo/.." );
    PATH_CHECK( path( "c:foo/..", fs::native ).normalize(), "c:" );
    PATH_CHECK( path( "c:/foo/..", fs::native ), "c:/foo/.." );
    PATH_CHECK( path( "c:/foo/..", fs::native ).normalize(), "c:/" );
    PATH_CHECK( path( "c:/..foo", fs::native ), "c:/..foo" );
    PATH_CHECK( path( "c:/..foo", fs::native ).normalize(), "c:/..foo" );
    PATH_CHECK( path( "c:foo", fs::native ), "c:foo" );
    PATH_CHECK( path( "c:/foo", fs::native ), "c:/foo" );
    PATH_CHECK( path( "c++", fs::native ), "c++" );
    PATH_CHECK( path( "//share", fs::native ), "//share" );
    PATH_CHECK( path( "//share/", fs::native ), "//share/" );
    PATH_CHECK( path( "//share/foo", fs::native ), "//share/foo" );
    PATH_CHECK( path( "\\\\share", fs::native ), "//share" );
    PATH_CHECK( path( "\\\\share\\", fs::native ), "//share/" );
    PATH_CHECK( path( "\\\\share\\foo", fs::native ), "//share/foo" );
    PATH_CHECK( path( "c:/foo", fs::native ), "c:/foo" );
    PATH_CHECK( path( "prn:", fs::native ), "prn:" );

    p = path( "c:", fs::native );
    BOOST_CHECK( p.relative_path().string() == "" );
    BOOST_CHECK( p.branch_path().string() == "" );
    BOOST_CHECK( p.leaf() == "c:" );
    BOOST_CHECK( p.root_name() == "c:" );
    BOOST_CHECK( p.root_directory() == "" );
    BOOST_CHECK( p.root_path().string() == "c:" );
    BOOST_CHECK( p.has_root_path() );
    BOOST_CHECK( p.has_root_name() );
    BOOST_CHECK( !p.has_root_directory() );
    BOOST_CHECK( !p.has_relative_path() );
    BOOST_CHECK( p.has_leaf() );
    BOOST_CHECK( !p.has_branch_path() );
    BOOST_CHECK( !p.is_complete() );

    p = path( "c:foo", fs::native );
    BOOST_CHECK( p.relative_path().string() == "foo" );
    BOOST_CHECK( p.branch_path().string() == "c:" );
    BOOST_CHECK( p.leaf() == "foo" );
    BOOST_CHECK( p.root_name() == "c:" );
    BOOST_CHECK( p.root_directory() == "" );
    BOOST_CHECK( p.root_path().string() == "c:" );
    BOOST_CHECK( p.has_root_path() );
    BOOST_CHECK( p.has_root_name() );
    BOOST_CHECK( !p.has_root_directory() );
    BOOST_CHECK( p.has_relative_path() );
    BOOST_CHECK( p.has_leaf() );
    BOOST_CHECK( p.has_branch_path() );
    BOOST_CHECK( !p.is_complete() );
 
    p = path( "c:/", fs::native );
    BOOST_CHECK( p.relative_path().string() == "" );
    BOOST_CHECK( p.branch_path().string() == "c:" );
    BOOST_CHECK( p.leaf() == "/" );
    BOOST_CHECK( p.root_name() == "c:" );
    BOOST_CHECK( p.root_directory() == "/" );
    BOOST_CHECK( p.root_path().string() == "c:/" );
    BOOST_CHECK( p.has_root_path() );
    BOOST_CHECK( p.has_root_name() );
    BOOST_CHECK( p.has_root_directory() );
    BOOST_CHECK( !p.has_relative_path() );
    BOOST_CHECK( p.has_leaf() );
    BOOST_CHECK( p.has_branch_path() );
    BOOST_CHECK( p.is_complete() );

    p = path( "c:..", fs::native );
    BOOST_CHECK( p.relative_path().string() == ".." );
    BOOST_CHECK( p.branch_path().string() == "c:" );
    BOOST_CHECK( p.leaf() == ".." );
    BOOST_CHECK( p.root_name() == "c:" );
    BOOST_CHECK( p.root_directory() == "" );
    BOOST_CHECK( p.root_path().string() == "c:" );
    BOOST_CHECK( p.has_root_path() );
    BOOST_CHECK( p.has_root_name() );
    BOOST_CHECK( !p.has_root_directory() );
    BOOST_CHECK( p.has_relative_path() );
    BOOST_CHECK( p.has_leaf() );
    BOOST_CHECK( p.has_branch_path() );
    BOOST_CHECK( !p.is_complete() );

    p = path( "c:/foo", fs::native );
    BOOST_CHECK( p.relative_path().string() == "foo" );
    BOOST_CHECK( p.branch_path().string() == "c:/" );
    BOOST_CHECK( p.leaf() == "foo" );
    BOOST_CHECK( p.root_name() == "c:" );
    BOOST_CHECK( p.root_directory() == "/" );
    BOOST_CHECK( p.root_path().string() == "c:/" );
    BOOST_CHECK( p.has_root_path() );
    BOOST_CHECK( p.has_root_name() );
    BOOST_CHECK( p.has_root_directory() );
    BOOST_CHECK( p.has_relative_path() );
    BOOST_CHECK( p.has_leaf() );
    BOOST_CHECK( p.has_branch_path() );
    BOOST_CHECK( p.is_complete() );

/* Commented out until the semantics of //share are clearer.

    p = path( "//share", fs::native );
    BOOST_CHECK( p.string() == "//share" );
    BOOST_CHECK( p.relative_path().string() == "" );
    BOOST_CHECK( p.branch_path().string() == "" );
    BOOST_CHECK( p.leaf() == "//share" );
    BOOST_CHECK( p.root_name() == "//share" );
    BOOST_CHECK( p.root_directory() == "/" );
    BOOST_CHECK( p.root_path().string() == "//share/" );
    BOOST_CHECK( p.has_root_path() );
    BOOST_CHECK( p.has_root_name() );
    BOOST_CHECK( !p.has_root_directory() );
    BOOST_CHECK( !p.has_relative_path() );
    BOOST_CHECK( p.has_leaf() );
    BOOST_CHECK( !p.has_branch_path() );
    BOOST_CHECK( !p.is_complete() );
*/
    p = path( "//share/", fs::native );
    BOOST_CHECK( p.relative_path().string() == "" );
    BOOST_CHECK( p.branch_path().string() == "//share" );
    BOOST_CHECK( p.leaf() == "/" );
    BOOST_CHECK( p.root_name() == "//share" );
    BOOST_CHECK( p.root_directory() == "/" );
    BOOST_CHECK( p.root_path().string() == "//share/" );
    BOOST_CHECK( p.has_root_path() );
    BOOST_CHECK( p.has_root_name() );
    BOOST_CHECK( p.has_root_directory() );
    BOOST_CHECK( !p.has_relative_path() );
    BOOST_CHECK( p.has_leaf() );
    BOOST_CHECK( p.has_branch_path() );
    BOOST_CHECK( p.is_complete() );

    p = path( "//share/foo", fs::native );
    BOOST_CHECK( p.relative_path().string() == "foo" );
    BOOST_CHECK( p.branch_path().string() == "//share/" );
    BOOST_CHECK( p.leaf() == "foo" );
    BOOST_CHECK( p.root_name() == "//share" );
    BOOST_CHECK( p.root_directory() == "/" );
    BOOST_CHECK( p.root_path().string() == "//share/" );
    BOOST_CHECK( p.has_root_path() );
    BOOST_CHECK( p.has_root_name() );
    BOOST_CHECK( p.has_root_directory() );
    BOOST_CHECK( p.has_relative_path() );
    BOOST_CHECK( p.has_leaf() );
    BOOST_CHECK( p.has_branch_path() );
    BOOST_CHECK( p.is_complete() );

    p = path( "prn:", fs::native );
    BOOST_CHECK( p.relative_path().string() == "" );
    BOOST_CHECK( p.branch_path().string() == "" );
    BOOST_CHECK( p.leaf() == "prn:" );
    BOOST_CHECK( p.root_name() == "prn:" );
    BOOST_CHECK( p.root_directory() == "" );
    BOOST_CHECK( p.root_path().string() == "prn:" );
    BOOST_CHECK( p.has_root_path() );
    BOOST_CHECK( p.has_root_name() );
    BOOST_CHECK( !p.has_root_directory() );
    BOOST_CHECK( !p.has_relative_path() );
    BOOST_CHECK( p.has_leaf() );
    BOOST_CHECK( !p.has_branch_path() );
    BOOST_CHECK( p.is_complete() );

    itr_ck = path( "c:", fs::native );
    BOOST_CHECK( *itr_ck.begin() == std::string( "c:" ) );
    BOOST_CHECK( next( itr_ck.begin() ) == itr_ck.end() );
    BOOST_CHECK( prior( itr_ck.end() ) == itr_ck.begin() );
    BOOST_CHECK( *prior( itr_ck.end() ) == std::string( "c:" ) );

    itr_ck = path( "c:/", fs::native );
    BOOST_CHECK( *itr_ck.begin() == std::string( "c:" ) );
    BOOST_CHECK( *next( itr_ck.begin() ) == std::string( "/" ) );
    BOOST_CHECK( next( next( itr_ck.begin() )) == itr_ck.end() );
    BOOST_CHECK( prior( prior( itr_ck.end() )) == itr_ck.begin() );
    BOOST_CHECK( *prior( itr_ck.end() ) == std::string( "/" ) );
    BOOST_CHECK( *prior( prior( itr_ck.end() )) == std::string( "c:" ) );

    itr_ck = path( "c:foo", fs::native );
    BOOST_CHECK( *itr_ck.begin() == std::string( "c:" ) );
    BOOST_CHECK( *next( itr_ck.begin() ) == std::string( "foo" ) );
    BOOST_CHECK( next(next( itr_ck.begin() )) == itr_ck.end() );
    BOOST_CHECK( prior(prior( itr_ck.end() )) == itr_ck.begin() );
    BOOST_CHECK( *prior( itr_ck.end() ) == std::string( "foo" ) );
    BOOST_CHECK( *prior(prior( itr_ck.end() )) == std::string( "c:" ) );

    itr_ck = path( "c:/foo", fs::native );
    BOOST_CHECK( *itr_ck.begin() == std::string( "c:" ) );
    BOOST_CHECK( *next( itr_ck.begin() ) == std::string( "/" ) );
    BOOST_CHECK( *next( next( itr_ck.begin() )) == std::string( "foo" ) );
    BOOST_CHECK( next( next( next( itr_ck.begin() ))) == itr_ck.end() );
    BOOST_CHECK( prior( prior( prior( itr_ck.end() ))) == itr_ck.begin() );
    BOOST_CHECK( *prior( itr_ck.end() ) == std::string( "foo" ) );
    BOOST_CHECK( *prior( prior( itr_ck.end() )) == std::string( "/" ) );
    BOOST_CHECK( *prior( prior( prior( itr_ck.end() ))) == std::string( "c:" ) );

    itr_ck = path( "//share", fs::native );
    BOOST_CHECK( *itr_ck.begin() == std::string( "//share" ) );
    BOOST_CHECK( next( itr_ck.begin() ) == itr_ck.end() );
    BOOST_CHECK( prior( itr_ck.end() ) == itr_ck.begin() );
    BOOST_CHECK( *prior( itr_ck.end() ) == std::string( "//share" ) );

    itr_ck = path( "//share/", fs::native );
    BOOST_CHECK( *itr_ck.begin() == std::string( "//share" ) );
    BOOST_CHECK( *next( itr_ck.begin() ) == std::string( "/" ) );
    BOOST_CHECK( next(next( itr_ck.begin() )) == itr_ck.end() );
    BOOST_CHECK( prior(prior( itr_ck.end() )) == itr_ck.begin() );
    BOOST_CHECK( *prior( itr_ck.end() ) == std::string( "/" ) );
    BOOST_CHECK( *prior(prior( itr_ck.end() )) == std::string( "//share" ) );

    itr_ck = path( "//share/foo", fs::native );
    BOOST_CHECK( *itr_ck.begin() == std::string( "//share" ) );
    BOOST_CHECK( *next( itr_ck.begin() ) == std::string( "/" ) );
    BOOST_CHECK( *next(next( itr_ck.begin() )) == std::string( "foo" ) );
    BOOST_CHECK( next(next(next( itr_ck.begin() ))) == itr_ck.end() );
    BOOST_CHECK( prior(prior(prior( itr_ck.end() ))) == itr_ck.begin() );
    BOOST_CHECK( *prior( itr_ck.end() ) == std::string( "foo" ) );
    BOOST_CHECK( *prior(prior( itr_ck.end() )) == std::string( "/" ) );
    BOOST_CHECK( *prior(prior(prior( itr_ck.end() ))) == std::string( "//share" ) );

    itr_ck = path( "prn:", fs::native );
    BOOST_CHECK( *itr_ck.begin() == std::string( "prn:" ) );
    BOOST_CHECK( next( itr_ck.begin() ) == itr_ck.end() );
    BOOST_CHECK( prior( itr_ck.end() ) == itr_ck.begin() );
    BOOST_CHECK( *prior( itr_ck.end() ) == std::string( "prn:" ) );
  } // Windows

  else
  { // POSIX
    p = path( "/usr/local/bin:/usr/bin:/bin", fs::no_check );
    BOOST_CHECK( p.native_file_string() == "/usr/local/bin:/usr/bin:/bin" );
  } // POSIX

  // test relational operators
                                                             
  path e, e2;
  path a( "a" );
  path a2( "a" );
  path b( "b" );

  // probe operator <
  BOOST_CHECK( !(e < e2) );
  BOOST_CHECK( e < a );
  BOOST_CHECK( a < b );
  BOOST_CHECK( !(a < a2) );

  // reality check character set is as expected
  BOOST_CHECK( std::string("a.b") < std::string("a/b") );
  // verify compare is actually lexicographical
  BOOST_CHECK( path("a/b") < path("a.b") );

  // make sure the derivative operators also work
  BOOST_CHECK( a == a2 );
  BOOST_CHECK( a != b );
  BOOST_CHECK( a <= b );
  BOOST_CHECK( a <= a2 );
  BOOST_CHECK( b >= a );
  BOOST_CHECK( a2 >= a );

//  std::cout << errors << " errors detected\n";
  
  return errors;
}
