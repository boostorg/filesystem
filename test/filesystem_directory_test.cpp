//  Boost filesystem_directory_test.cpp  -------------------------------------//

//  (C) Copyright Beman Dawes 2002. Permission to copy,
//  use, modify, sell and distribute this software is granted provided this
//  copyright notice appears in all copies. This software is provided "as is"
//  without express or implied warranty, and with no claim as to its
//  suitability for any purpose.

//  See http://www.boost.org for most recent version including documentation.

#include "boost/filesystem/directory.hpp"
#include "boost/filesystem/exception.hpp"
namespace fs = boost::filesystem;

#define BOOST_INCLUDE_MAIN
#include <boost/test/test_tools.hpp>

#include <boost/bind.hpp>
using boost::bind;

#include <fstream>
#include <iostream>

//  TODO: This program was written before any of the path functions were
//  available.  Thus it just assumes "/" for a path separator, so will only
//  work for POSIX and Windows-like systems.

namespace
{
  fs::directory_iterator end_itr;

  void create_file( const std::string & path, const std::string & contents )
  {
    std::ofstream f( path.c_str() );
    if ( !f ) throw fs::filesystem_error( 0, "ofstream(): " + path );
    f << contents;
  }

  void verify_file( const std::string & path, const std::string & expected )
  {
    std::ifstream f( path.c_str() );
    if ( !f ) throw fs::filesystem_error( 0, "ifstream(): " + path );
    std::string contents;
    f >> contents;
    if ( contents != expected )
      throw fs::filesystem_error( 0, "verify_file(): " + path
        + " contents \"" + contents
        + "\" != \"" + expected + "\"" );
  }

  template< typename F >
  bool throws_fs_error( F func )
  {
    try { func(); }

    catch ( const fs::filesystem_error & ex )
    {
//      std::cout << ex.what() << "\n";
      return true;
    }

    return false;
  }

} // unnamed namespace

//  test_main  ---------------------------------------------------------------//

int test_main( int, char * [] )
{
  int point = 0;

  std::string dir(  fs::initial_directory() + "/temp_fs_test_directory" );
  
  #ifdef BOOST_WINDOWS
  BOOST_TEST( dir.size() > 1 && dir[1] == ':' ); // verify path includes drive
  #endif

  std::string ng( "no-way, Jose/" );

  fs::remove_all( dir );  // in case residue from prior failed tests
  BOOST_TEST( !fs::exists( dir ) );

  // the bound functions should throw, so throws_fs_error() should return true
  BOOST_TEST( throws_fs_error( bind( fs::is_directory, ng ) ) );
  BOOST_TEST( throws_fs_error( bind( fs::is_directory, dir ) ) );
  BOOST_TEST( throws_fs_error( bind( fs::is_file, dir ) ) );
  BOOST_TEST( throws_fs_error( bind( fs::empty, dir ) ) );

  std::cout << "point " << ++point << std::endl; //1
  fs::create_directory( dir );

  BOOST_TEST( fs::exists( dir ) );
  BOOST_TEST( fs::empty( dir ) );

  BOOST_TEST( fs::is_directory( dir ) );
  BOOST_TEST( !fs::is_file( dir ) );
  // make sure C-style string also works:
  BOOST_TEST( fs::exists( dir.c_str() ) );
  BOOST_TEST( fs::is_directory( dir.c_str() ) );
  BOOST_TEST( !fs::is_file( dir.c_str() ) );

  BOOST_TEST( throws_fs_error( bind( fs::create_directory, dir ) ) );
  BOOST_TEST( throws_fs_error( bind( fs::remove_file, dir ) ) );
  std::cout << "point " << ++point << std::endl;  //2

  std::string d1;
  fs::create_directory( d1 = dir + "/d1" );
  BOOST_TEST( fs::exists( d1 ) );
  BOOST_TEST( fs::is_directory( d1 ) );
  BOOST_TEST( fs::empty( d1 ) );

  // create a second directory named d2, making sure C-style strings
  // work as arguments to several functions
  std::string d2;
  fs::create_directory( (d2 = dir + "/d2" ).c_str() );
  BOOST_TEST( fs::exists( d2.c_str() ) );
  BOOST_TEST( fs::is_directory( d2.c_str() ) );

  // create a file named "f1"
  std::string file_path;
  create_file( file_path = dir + "/f1", "foobar1" );
  BOOST_TEST( fs::exists( file_path ) );
  BOOST_TEST( fs::is_file( file_path ) );
  verify_file( file_path, "foobar1" );
  BOOST_TEST( throws_fs_error( bind( fs::remove_directory, file_path ) ) );
  std::cout << "point " << ++point << std::endl; //3

  // copy_file() tests
  fs::copy_file( file_path, d1 + "/f2" );
  BOOST_TEST( fs::exists( file_path ) );
  BOOST_TEST( fs::exists( d1 + "/f2" ) );
  BOOST_TEST( fs::is_file( d1 + "/f2" ) );
  verify_file( d1 + "/f2", "foobar1" );

  // rename() on file d1/f2 to d2/f3
  fs::rename( d1 + "/f2", d2 + "/f3" );
  BOOST_TEST( !fs::exists( d1 + "/f2" ) );
  BOOST_TEST( !fs::exists( d2 + "/f2" ) );
  BOOST_TEST( fs::exists( d2 + "/f3" ) );
  BOOST_TEST( fs::is_file( d2 + "/f3" ) );
  verify_file( d2 + "/f3", "foobar1" );

  // make sure can't rename() a non-existent file
  BOOST_TEST( throws_fs_error( bind( fs::rename, d1 + "/f2", d2 + "/f4" ) ) );

  // make sure can't rename() to an existent file
  BOOST_TEST( throws_fs_error( bind( fs::rename, dir + "/f1", d2 + "/f3" ) ) );

  // make sure can't rename() to a nonexistent parent directory
  BOOST_TEST( throws_fs_error( bind( fs::rename, dir + "/f1", dir + "/d3/f3" ) ) );

  // rename() on directory
  std::string d3( dir + "/d3" );
  fs::rename( d2, d3 );
  BOOST_TEST( !fs::exists( d2 ) );
  BOOST_TEST( fs::exists( d3 ) );
  BOOST_TEST( fs::is_directory( d3 ) );
  BOOST_TEST( !fs::exists( d2 + "/f3" ) );
  BOOST_TEST( fs::exists( d3 + "/f3" ) );

   // remove_file() tests
  file_path = dir + "/shortlife";
  BOOST_TEST( !fs::exists( file_path ) );
  create_file( file_path, "" );
  BOOST_TEST( fs::exists( file_path ) );
  BOOST_TEST( fs::is_file( file_path ) );
  fs::remove_file( file_path );
  BOOST_TEST( !fs::exists( file_path ) );
  std::cout << "point " << ++point << std::endl;   //4

  // remove_directory() tests
  d1 = dir + "/shortlife_dir";
  BOOST_TEST( !fs::exists( d1 ) );
  fs::create_directory( d1 );
  BOOST_TEST( fs::exists( d1 ) );
  BOOST_TEST( fs::is_directory( d1 ) );
  BOOST_TEST( fs::empty( d1 ) );
  fs::remove_directory( d1 );
  BOOST_TEST( !fs::exists( d1 ) );
  std::cout << "point " << ++point << std::endl;

  // post-test cleanup
  BOOST_TEST( fs::remove_all( dir ) != 0 );
  // above was added just to simplify testing, but it ended up detecting
  // a bug (failure to close an internal search handle). 
  BOOST_TEST( !fs::exists( dir ) );
  BOOST_TEST( fs::remove_all( dir ) == 0 );

  return 0;
} // main

