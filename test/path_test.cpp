//  path_test program  -------------------------------------------------------//

//  Copyright Beman Dawes 2002.
//  Use, modification, and distribution is subject to the Boost Software
//  License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

//  See library home page at http://www.boost.org/libs/filesystem

#include <boost/filesystem/operations.hpp>
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

namespace
{
  int errors;

  std::string platform( BOOST_PLATFORM );

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

  void check_normalize()
  {
    PATH_CHECK( path("foo/").normalize(), "foo/" );
    PATH_CHECK( path("f/").normalize(), "f/" );
    PATH_CHECK( path( "/foo" ).normalize(), "/foo" );
    PATH_CHECK( path( "foo/bar" ).normalize(), "foo/bar" );
    PATH_CHECK( path("..").normalize(), ".." );
    PATH_CHECK( path("../..").normalize(), "../.." );
    PATH_CHECK( path("/..").normalize(), "/.." );
    PATH_CHECK( path("/../..").normalize(), "/../.." );
    PATH_CHECK( path("../foo").normalize(), "../foo" );
    PATH_CHECK( path("foo/..").normalize(), "." );
    PATH_CHECK( path("foo/../").normalize(), "./" );
    PATH_CHECK( (path("foo") / "..").normalize() , "." );
    PATH_CHECK( path("foo/...").normalize(), "foo/..." );
    PATH_CHECK( path("foo/.../").normalize(), "foo/.../" );
    PATH_CHECK( path("foo/..bar").normalize(), "foo/..bar" );
    PATH_CHECK( path("../f").normalize(), "../f" );
    PATH_CHECK( path("/../f").normalize(), "/../f" );
    PATH_CHECK( path("f/..").normalize(), "." );
    PATH_CHECK( (path("f") / "..").normalize() , "." );
    PATH_CHECK( path("foo/../..").normalize(), ".." );
    PATH_CHECK( path("foo/../../").normalize(), "../" );
    PATH_CHECK( path("foo/../../..").normalize(), "../.." );
    PATH_CHECK( path("foo/../../../").normalize(), "../../" );
    PATH_CHECK( path("foo/../bar").normalize(), "bar" );
    PATH_CHECK( path("foo/../bar/").normalize(), "bar/" );
    PATH_CHECK( path("foo/bar/..").normalize(), "foo" );
    PATH_CHECK( path("foo/bar/../").normalize(), "foo/" );
    PATH_CHECK( path("foo/bar/../..").normalize(), "." );
    PATH_CHECK( path("foo/bar/../../").normalize(), "./" );
    PATH_CHECK( path("foo/bar/../blah").normalize(), "foo/blah" );
    PATH_CHECK( path("f/../b").normalize(), "b" );
    PATH_CHECK( path("f/b/..").normalize(), "f" );
    PATH_CHECK( path("f/b/../").normalize(), "f/" );
    PATH_CHECK( path("f/b/../a").normalize(), "f/a" );
    PATH_CHECK( path("foo/bar/blah/../..").normalize(), "foo" );
    PATH_CHECK( path("foo/bar/blah/../../bletch").normalize(), "foo/bletch" );
    PATH_CHECK( path( "//net" ).normalize(), "//net" );
    PATH_CHECK( path( "//net/" ).normalize(), "//net/" );
    PATH_CHECK( path( "//..net" ).normalize(), "//..net" );
    PATH_CHECK( path( "//net/.." ).normalize(), "//net" );
    PATH_CHECK( path( "//net/foo" ).normalize(), "//net/foo" );
    PATH_CHECK( path( "//net/foo/" ).normalize(), "//net/foo/" );
    PATH_CHECK( path( "//net/foo/.." ).normalize(), "//net" );
    PATH_CHECK( path( "//net/foo/../" ).normalize(), "//net/" );
    if ( platform == "Windows" )
    {
      PATH_CHECK( path( "c:.." ).normalize(), "c:.." );
      PATH_CHECK( path( "c:foo/.." ).normalize(), "c:" );

      // should the results of this one be "c:./"? 
      PATH_CHECK( path( "c:foo/../" ).normalize(), "c:" );

      PATH_CHECK( path( "c:/foo/.." ).normalize(), "c:/" );
      PATH_CHECK( path( "c:/foo/../" ).normalize(), "c:/" );
      PATH_CHECK( path( "c:/.." ).normalize(), "c:/.." );
      PATH_CHECK( path( "c:/../" ).normalize(), "c:/../" );
      PATH_CHECK( path( "c:/../.." ).normalize(), "c:/../.." );
      PATH_CHECK( path( "c:/../../" ).normalize(), "c:/../../" );
      PATH_CHECK( path( "c:/../foo" ).normalize(), "c:/../foo" );
      PATH_CHECK( path( "c:/../foo/" ).normalize(), "c:/../foo/" );
      PATH_CHECK( path( "c:/../../foo" ).normalize(), "c:/../../foo" );
      PATH_CHECK( path( "c:/../../foo/" ).normalize(), "c:/../../foo/" );
      PATH_CHECK( path( "c:/..foo" ).normalize(), "c:/..foo" );
    }
    else // POSIX
    {
      PATH_CHECK( path( "c:.." ).normalize(), "c:.." );
      PATH_CHECK( path( "c:foo/.." ).normalize(), "." );
      PATH_CHECK( path( "c:foo/../" ).normalize(), "./" );
      PATH_CHECK( path( "c:/foo/.." ).normalize(), "c:" );
      PATH_CHECK( path( "c:/foo/../" ).normalize(), "c:/" );
      PATH_CHECK( path( "c:/.." ).normalize(), "." );
      PATH_CHECK( path( "c:/../" ).normalize(), "./" );
      PATH_CHECK( path( "c:/../.." ).normalize(), ".." );
      PATH_CHECK( path( "c:/../../" ).normalize(), "../" );
      PATH_CHECK( path( "c:/../foo" ).normalize(), "foo" );
      PATH_CHECK( path( "c:/../foo/" ).normalize(), "foo/" );
      PATH_CHECK( path( "c:/../../foo" ).normalize(), "../foo" );
      PATH_CHECK( path( "c:/../../foo/" ).normalize(), "../foo/" );
      PATH_CHECK( path( "c:/..foo" ).normalize(), "c:/..foo" );
    }
  }

} // unnamed namespace

int test_main( int, char*[] )
{
  // The choice of platform is make at runtime rather than compile-time
  // so that compile errors for all platforms will be detected even though
  // only the current platform is runtime tested.
  platform = ( platform == "Win32" || platform == "Win64" || platform == "Cygwin" )
               ? "Windows"
               : "POSIX";
  std::cout << "Platform is " << platform << '\n';

  path p1( "fe/fi/fo/fum" );
  path p2( p1 );
  path p3;
  BOOST_CHECK( p1.string() != p3.string() );
  p3 = p2;
  BOOST_CHECK( p1.string() == p3.string() );

  path p4( "foobar" );
  BOOST_CHECK( p4.string() == "foobar" );
  p4 = p4; // self-assignment
  BOOST_CHECK( p4.string() == "foobar" );

  check_throw( "foo//bar" );

  BOOST_CHECK( path("foo\\bar").string()
    == (platform=="Windows" ? "foo/bar" : "foo\\bar") );
  BOOST_CHECK( path("foo\\bar").leaf()
    == (platform=="Windows" ? "bar" : "foo\\bar") );
  
  // These verify various overloads don't cause compiler errors
  fs::exists( "foo" );
  fs::exists( std::string( "foo" ) );
  fs::exists( p1 );
  fs::exists( "foo" / p1 );
  fs::exists( std::string( "foo" ) / p1 );

  try { fs::is_directory("should-throw-exception"); }
  catch ( const fs::filesystem_error & ex )
  {
    std::cout << ex.what() << "\n";
  }

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

  PATH_CHECK( "foo/", "foo/" );
  PATH_CHECK( "f/", "f/" );
  PATH_CHECK( "foo/..", "foo/.." );
  PATH_CHECK( "foo/../", "foo/../" );
  PATH_CHECK( "foo/bar/../..", "foo/bar/../.." );
  PATH_CHECK( "foo/bar/../../", "foo/bar/../../" );
  PATH_CHECK( path("") / "foo", "foo" );
  PATH_CHECK( path("") / "foo/", "foo/" );
  PATH_CHECK( path("foo") / "", "foo" );
  PATH_CHECK( path( "/" ), "/" );
  PATH_CHECK( path( "/" ) / "", "/" );
  PATH_CHECK( path( "/f" ), "/f" );

  PATH_CHECK( "/foo", "/foo" );
  PATH_CHECK( path("") / "/foo", "/foo" );
  PATH_CHECK( path("/foo") / "", "/foo" );

  check_normalize();

  PATH_CHECK( "foo/bar", "foo/bar" );
  PATH_CHECK( path("foo") / path("bar"), "foo/bar" ); // path arg
  PATH_CHECK( path("foo") / "bar", "foo/bar" );       // const char * arg
  PATH_CHECK( path("foo") / path("woo/bar").leaf(), "foo/bar" ); // const std::string & arg
  PATH_CHECK( "foo" / path("bar"), "foo/bar" );

  PATH_CHECK( "a/b", "a/b" );  // probe for length effects
  PATH_CHECK( path("a") / "b", "a/b" );

  PATH_CHECK( "..", ".." );
  PATH_CHECK( path("..") / "", ".." );
  PATH_CHECK( path("") / "..", ".." );

  PATH_CHECK( "../..", "../.." );
  PATH_CHECK( path("..") / ".." , "../.." );

  PATH_CHECK( "/..", "/.." );
  PATH_CHECK( path("/") / ".." , "/.." );

  PATH_CHECK( "/../..", "/../.." );
  PATH_CHECK( path("/..") / ".." , "/../.." );

  PATH_CHECK( "../foo", "../foo" );
  PATH_CHECK( path("..") / "foo" , "../foo" );

  PATH_CHECK( "foo/..", "foo/.." );
  PATH_CHECK( path("foo") / ".." , "foo/.." );
  PATH_CHECK( path( "foo/..bar"), "foo/..bar" );

  PATH_CHECK( "../f", "../f" );
  PATH_CHECK( path("..") / "f" , "../f" );

  PATH_CHECK( "/../f", "/../f" );
  PATH_CHECK( path("/..") / "f" , "/../f" );

  PATH_CHECK( "f/..", "f/.." );
  PATH_CHECK( path("f") / ".." , "f/.." );

  PATH_CHECK( "foo/../..", "foo/../.." );
  PATH_CHECK( path("foo") / ".." / ".." , "foo/../.." );

  PATH_CHECK( "foo/../../..", "foo/../../.." );
  PATH_CHECK( path("foo") / ".." / ".." / ".." , "foo/../../.." );

  PATH_CHECK( "foo/../bar", "foo/../bar" );
  PATH_CHECK( path("foo") / ".." / "bar" , "foo/../bar" );

  PATH_CHECK( "foo/bar/..", "foo/bar/.." );
  PATH_CHECK( path("foo") / "bar" / ".." , "foo/bar/.." );

  PATH_CHECK( "foo/bar/../..", "foo/bar/../.." );
  PATH_CHECK( path("foo") / "bar" / ".." / "..", "foo/bar/../.." );

  PATH_CHECK( "foo/bar/../blah", "foo/bar/../blah" );
  PATH_CHECK( path("foo") / "bar" / ".." / "blah", "foo/bar/../blah" );

  PATH_CHECK( "f/../b", "f/../b" );
  PATH_CHECK( path("f") / ".." / "b" , "f/../b" );

  PATH_CHECK( "f/b/..", "f/b/.." );
  PATH_CHECK( path("f") / "b" / ".." , "f/b/.." );

  PATH_CHECK( "f/b/../a", "f/b/../a" );
  PATH_CHECK( path("f") / "b" / ".." / "a", "f/b/../a" );

  PATH_CHECK( "foo/bar/blah/../..", "foo/bar/blah/../.." );
  PATH_CHECK( path("foo") / "bar" / "blah" / ".." / "..", "foo/bar/blah/../.." );

  PATH_CHECK( "foo/bar/blah/../../bletch", "foo/bar/blah/../../bletch" );
  PATH_CHECK( path("foo") / "bar" / "blah" / ".." / ".." / "bletch", "foo/bar/blah/../../bletch" );

  PATH_CHECK( "...", "..." );
  PATH_CHECK( "....", "...." );
  PATH_CHECK( "foo/...", "foo/..." );
  PATH_CHECK( "abc.", "abc." );
  PATH_CHECK( "abc..", "abc.." );
  PATH_CHECK( "foo/abc.", "foo/abc." );
  PATH_CHECK( "foo/abc..", "foo/abc.." );

  PATH_CHECK( path(".abc"), ".abc" );
  PATH_CHECK( "a.c", "a.c" );
  PATH_CHECK( path("..abc"), "..abc" );
  PATH_CHECK( "a..c", "a..c" );
  PATH_CHECK( path("foo/.abc"), "foo/.abc" );
  PATH_CHECK( "foo/a.c", "foo/a.c" );
  PATH_CHECK( path("foo/..abc"), "foo/..abc" );
  PATH_CHECK( "foo/a..c", "foo/a..c" );

  PATH_CHECK( ".", "." );
  PATH_CHECK( path("") / ".", "." );
  PATH_CHECK( "./foo", "./foo" );
  PATH_CHECK( path(".") / "foo", "./foo" );
  PATH_CHECK( "./..", "./.." );
  PATH_CHECK( path(".") / "..", "./.." );
  PATH_CHECK( "./../foo", "./../foo" );
  PATH_CHECK( "foo/.", "foo/." );
  PATH_CHECK( path("foo") / ".", "foo/." );
  PATH_CHECK( "../.", "../." );
  PATH_CHECK( path("..") / ".", "../." );
  PATH_CHECK( "./.", "./." );
  PATH_CHECK( path(".") / ".", "./." );
  PATH_CHECK( "././.", "././." );
  PATH_CHECK( path(".") / "." / ".", "././." );
  PATH_CHECK( "./foo/.", "./foo/." );
  PATH_CHECK( path(".") / "foo" / ".", "./foo/." );
  PATH_CHECK( "foo/./bar", "foo/./bar" );
  PATH_CHECK( path("foo") / "." / "bar", "foo/./bar" );
  PATH_CHECK( "foo/./.", "foo/./." );
  PATH_CHECK( path("foo") / "." / ".", "foo/./." );
  PATH_CHECK( "foo/./..", "foo/./.." );
  PATH_CHECK( path("foo") / "." / "..", "foo/./.." );
  PATH_CHECK( "foo/./../bar", "foo/./../bar" );
  PATH_CHECK( "foo/../.", "foo/../." );
  PATH_CHECK( path(".") / "." / "..", "././.." );
  PATH_CHECK( "././..", "././.." );
  PATH_CHECK( path(".") / "." / "..", "././.." );
  PATH_CHECK( "./../.", "./../." );
  PATH_CHECK( path(".") / ".." / ".", "./../." );
  PATH_CHECK( ".././.", ".././." );
  PATH_CHECK( path("..") / "." / ".", ".././." );

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
  p = path( "/</>" );
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

#if 0

  if ( platform == "Windows" )
  {
    PATH_CHECK( path( "\\" ), "/" );
    PATH_CHECK( path( "\\f" ), "/f" );
    PATH_CHECK( path( "\\foo" ), "/foo" );
    PATH_CHECK( path( "foo\\bar" ), "foo/bar" );
    PATH_CHECK( path( "foo bar" ), "foo bar" );
    PATH_CHECK( path( "c:" ), "c:" );
    PATH_CHECK( path( "c:/" ), "c:/" );
    PATH_CHECK( path( "c:." ), "c:" );
    PATH_CHECK( path( "c:./foo" ), "c:foo" );
    PATH_CHECK( path( "c:.\\foo" ), "c:foo" );
    PATH_CHECK( path( "c:.." ), "c:.." );
    PATH_CHECK( path( "c:/." ), "c:/" );
    PATH_CHECK( path( "c:/.." ), "c:/" );
    PATH_CHECK( path( "c:/../" ), "c:/" );
    PATH_CHECK( path( "c:\\..\\" ), "c:/" );
    PATH_CHECK( path( "c:/../.." ), "c:/" );
    PATH_CHECK( path( "c:/../foo" ), "c:/foo" );
    PATH_CHECK( path( "c:\\..\\foo" ), "c:/foo" );
    PATH_CHECK( path( "c:../foo" ), "c:../foo" );
    PATH_CHECK( path( "c:..\\foo" ), "c:../foo" );
    PATH_CHECK( path( "c:/../../foo" ), "c:/foo" );
    PATH_CHECK( path( "c:\\..\\..\\foo" ), "c:/foo" );
    PATH_CHECK( path( "c:foo/.." ), "c:foo/.." );
    PATH_CHECK( path( "c:/foo/.." ), "c:/foo/.." );
    PATH_CHECK( path( "c:/..foo" ), "c:/..foo" );
    PATH_CHECK( path( "c:foo" ), "c:foo" );
    PATH_CHECK( path( "c:/foo" ), "c:/foo" );
    PATH_CHECK( path( "//share" ), "//share" );
    PATH_CHECK( path( "//share/" ), "//share/" );
    PATH_CHECK( path( "//share/foo" ), "//share/foo" );
    PATH_CHECK( path( "\\\\share" ), "//share" );
    PATH_CHECK( path( "\\\\share\\" ), "//share/" );
    PATH_CHECK( path( "\\\\share\\foo" ), "//share/foo" );
    PATH_CHECK( path( "c:/foo" ), "c:/foo" );
    PATH_CHECK( path( "prn:" ), "prn:" );

    p = path( "c:" );
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

    p = path( "c:foo" );
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
 
    p = path( "c:/" );
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

    p = path( "c:.." );
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

    p = path( "c:/foo" );
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

    p = path( "//share" );
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
    p = path( "//share/" );
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

    p = path( "//share/foo" );
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

    p = path( "prn:" );
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

    itr_ck = path( "c:" );
    BOOST_CHECK( *itr_ck.begin() == std::string( "c:" ) );
    BOOST_CHECK( next( itr_ck.begin() ) == itr_ck.end() );
    BOOST_CHECK( prior( itr_ck.end() ) == itr_ck.begin() );
    BOOST_CHECK( *prior( itr_ck.end() ) == std::string( "c:" ) );

    itr_ck = path( "c:/" );
    BOOST_CHECK( *itr_ck.begin() == std::string( "c:" ) );
    BOOST_CHECK( *next( itr_ck.begin() ) == std::string( "/" ) );
    BOOST_CHECK( next( next( itr_ck.begin() )) == itr_ck.end() );
    BOOST_CHECK( prior( prior( itr_ck.end() )) == itr_ck.begin() );
    BOOST_CHECK( *prior( itr_ck.end() ) == std::string( "/" ) );
    BOOST_CHECK( *prior( prior( itr_ck.end() )) == std::string( "c:" ) );

    itr_ck = path( "c:foo" );
    BOOST_CHECK( *itr_ck.begin() == std::string( "c:" ) );
    BOOST_CHECK( *next( itr_ck.begin() ) == std::string( "foo" ) );
    BOOST_CHECK( next(next( itr_ck.begin() )) == itr_ck.end() );
    BOOST_CHECK( prior(prior( itr_ck.end() )) == itr_ck.begin() );
    BOOST_CHECK( *prior( itr_ck.end() ) == std::string( "foo" ) );
    BOOST_CHECK( *prior(prior( itr_ck.end() )) == std::string( "c:" ) );

    itr_ck = path( "c:/foo" );
    BOOST_CHECK( *itr_ck.begin() == std::string( "c:" ) );
    BOOST_CHECK( *next( itr_ck.begin() ) == std::string( "/" ) );
    BOOST_CHECK( *next( next( itr_ck.begin() )) == std::string( "foo" ) );
    BOOST_CHECK( next( next( next( itr_ck.begin() ))) == itr_ck.end() );
    BOOST_CHECK( prior( prior( prior( itr_ck.end() ))) == itr_ck.begin() );
    BOOST_CHECK( *prior( itr_ck.end() ) == std::string( "foo" ) );
    BOOST_CHECK( *prior( prior( itr_ck.end() )) == std::string( "/" ) );
    BOOST_CHECK( *prior( prior( prior( itr_ck.end() ))) == std::string( "c:" ) );

    itr_ck = path( "//share" );
    BOOST_CHECK( *itr_ck.begin() == std::string( "//share" ) );
    BOOST_CHECK( next( itr_ck.begin() ) == itr_ck.end() );
    BOOST_CHECK( prior( itr_ck.end() ) == itr_ck.begin() );
    BOOST_CHECK( *prior( itr_ck.end() ) == std::string( "//share" ) );

    itr_ck = path( "//share/" );
    BOOST_CHECK( *itr_ck.begin() == std::string( "//share" ) );
    BOOST_CHECK( *next( itr_ck.begin() ) == std::string( "/" ) );
    BOOST_CHECK( next(next( itr_ck.begin() )) == itr_ck.end() );
    BOOST_CHECK( prior(prior( itr_ck.end() )) == itr_ck.begin() );
    BOOST_CHECK( *prior( itr_ck.end() ) == std::string( "/" ) );
    BOOST_CHECK( *prior(prior( itr_ck.end() )) == std::string( "//share" ) );

    itr_ck = path( "//share/foo" );
    BOOST_CHECK( *itr_ck.begin() == std::string( "//share" ) );
    BOOST_CHECK( *next( itr_ck.begin() ) == std::string( "/" ) );
    BOOST_CHECK( *next(next( itr_ck.begin() )) == std::string( "foo" ) );
    BOOST_CHECK( next(next(next( itr_ck.begin() ))) == itr_ck.end() );
    BOOST_CHECK( prior(prior(prior( itr_ck.end() ))) == itr_ck.begin() );
    BOOST_CHECK( *prior( itr_ck.end() ) == std::string( "foo" ) );
    BOOST_CHECK( *prior(prior( itr_ck.end() )) == std::string( "/" ) );
    BOOST_CHECK( *prior(prior(prior( itr_ck.end() ))) == std::string( "//share" ) );

    itr_ck = path( "prn:" );
    BOOST_CHECK( *itr_ck.begin() == std::string( "prn:" ) );
    BOOST_CHECK( next( itr_ck.begin() ) == itr_ck.end() );
    BOOST_CHECK( prior( itr_ck.end() ) == itr_ck.begin() );
    BOOST_CHECK( *prior( itr_ck.end() ) == std::string( "prn:" ) );
  } // Windows

  else
  { // POSIX
    p = path( "/usr/local/bin:/usr/bin:/bin" );
    BOOST_CHECK( p.native_file_string() == "/usr/local/bin:/usr/bin:/bin" );
  } // POSIX
#endif
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
