//  Boost operations_test.cpp  -----------------------------------------------//

//  (C) Copyright Beman Dawes 2002. Permission to copy,
//  use, modify, sell and distribute this software is granted provided this
//  copyright notice appears in all copies. This software is provided "as is"
//  without express or implied warranty, and with no claim as to its
//  suitability for any purpose.

//  See http://www.boost.org for most recent version including documentation.

#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/exception.hpp>
namespace fs = boost::filesystem;

//#include <boost/test/test_tools.hpp>
#include <boost/test/minimal.hpp>

#include <boost/bind.hpp>
using boost::bind;

#include <fstream>
#include <iostream>
#include <cstring>

namespace
{
  fs::directory_iterator end_itr;

  void create_file( const fs::path & ph, const std::string & contents )
  {
    std::ofstream f( ph.file_path().c_str() );
    if ( !f )
      throw fs::filesystem_error( "ofstream(): " + ph.generic_path() );
    if ( !contents.empty() ) f << contents;
  }

  void verify_file( const fs::path & ph, const std::string & expected )
  {
    std::ifstream f( ph.file_path().c_str() );
    if ( !f )
      throw fs::filesystem_error( "ifstream(): " + ph.generic_path() );
    std::string contents;
    f >> contents;
    if ( contents != expected )
      throw fs::filesystem_error("verify_file(): " + ph.generic_path()
        + " contents \"" + contents
        + "\" != \"" + expected + "\"" );
  }

  template< typename F >
  bool throws_fs_error( F func )
  {
    try { func(); }

    catch ( const fs::filesystem_error & /*ex*/ )
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
  std::cout << "implemenation name: "
            << fs::detail::implementation_name() << "\n";
  std::cout << "initial_directory() is \""
            << fs::initial_directory().generic_path()
            << "\"\n";

  fs::path dir(  fs::initial_directory() << "temp_fs_test_directory" );
  
  if ( std::strcmp( fs::detail::implementation_name(), "Windows" ) == 0 )
  {
    BOOST_TEST( dir.generic_path().size() > 1
      && dir.generic_path()[1] == ':' ); // verify path includes drive
  }

  fs::path ng( " no-way, Jose ", fs::system_specific );

  fs::remove_all( dir );  // in case residue from prior failed tests
  BOOST_TEST( !fs::exists( dir ) );

  // the bound functions should throw, so throws_fs_error() should return true
  BOOST_TEST( throws_fs_error( bind( fs::is_directory, ng ) ) );
  BOOST_TEST( throws_fs_error( bind( fs::is_directory, dir ) ) );
  BOOST_TEST( throws_fs_error( bind( fs::_is_empty, dir ) ) );

  fs::create_directory( dir );

  BOOST_TEST( fs::exists( dir ) );
  BOOST_TEST( fs::_is_empty( dir ) );

  BOOST_TEST( fs::is_directory( dir ) );

  fs::path d1( dir << "d1" );
  fs::create_directory( d1  );
  BOOST_TEST( fs::exists( d1 ) );
  BOOST_TEST( fs::is_directory( d1 ) );
  BOOST_TEST( fs::_is_empty( d1 ) );

  {
    fs::directory_iterator dir_itr( dir );
    BOOST_TEST( dir_itr->leaf() == "d1" );
  }

  // create a second directory named d2
  fs::path d2( dir << "d2" );
  fs::create_directory(d2 );
  BOOST_TEST( fs::exists( d2 ) );
  BOOST_TEST( fs::is_directory( d2 ) );

  {
    fs::directory_iterator dir_itr( dir );
    BOOST_TEST( dir_itr->leaf() == "d1" || dir_itr->leaf() == "d2" );
    if ( dir_itr->leaf() == "d1" )
    {
      // Thomas Witt suggested the ++ to verify a compiler problem workaround
//      BOOST_TEST( (++fs::directory_iterator(dir))->leaf() == "d2" );
      ++dir_itr;
      BOOST_TEST( dir_itr->leaf() == "d2" );
    }
    else
    {
      BOOST_TEST( (++fs::directory_iterator(dir))->leaf() == "d1" );
    }
  }

  // create an empty file named "f0"
  fs::path file_ph( dir << "f0");
  create_file( file_ph, "" );
  BOOST_TEST( fs::exists( file_ph ) );
  BOOST_TEST( !fs::is_directory( file_ph ) );
  BOOST_TEST( fs::_is_empty( file_ph ) );

  // create a file named "f1"
  file_ph = dir << "f1";
  create_file( file_ph, "foobar1" );
  BOOST_TEST( fs::exists( file_ph ) );
  BOOST_TEST( !fs::is_directory( file_ph ) );
  verify_file( file_ph, "foobar1" );

  // there was an inital bug in directory_iterator that caused premature
  // close of an OS handle. This block will detect regression.
  {
    fs::directory_iterator di;
    { di = fs::directory_iterator( dir ); }
    BOOST_TEST( ++di != fs::directory_iterator() );
  }

  // copy_file() tests
  fs::copy_file( file_ph, d1 << "f2" );
  BOOST_TEST( fs::exists( file_ph ) );
  BOOST_TEST( fs::exists( d1 << "f2" ) );
  BOOST_TEST( !fs::is_directory( d1 << "f2" ) );
  verify_file( d1 << "f2", "foobar1" );

  // rename() on file d1/f2 to d2/f3
  fs::rename( d1 << "f2", d2 << "f3" );
  BOOST_TEST( !fs::exists( d1 << "f2" ) );
  BOOST_TEST( !fs::exists( d2 << "f2" ) );
  BOOST_TEST( fs::exists( d2 << "f3" ) );
  BOOST_TEST( !fs::is_directory( d2 << "f3" ) );
  verify_file( d2 << "f3", "foobar1" );

  // make sure can't rename() a non-existent file
  BOOST_TEST( throws_fs_error( bind( fs::rename, d1 << "f2", d2 << "f4" ) ) );

  // make sure can't rename() to an existent file
  BOOST_TEST( throws_fs_error( bind( fs::rename, dir << "f1", d2 << "f3" ) ) );

  // make sure can't rename() to a nonexistent parent directory
  BOOST_TEST( throws_fs_error( bind( fs::rename, dir << "f1", dir << "d3/f3" ) ) );

  // rename() on directory
  fs::path d3( dir << "d3" );
  fs::rename( d2, d3 );
  BOOST_TEST( !fs::exists( d2 ) );
  BOOST_TEST( fs::exists( d3 ) );
  BOOST_TEST( fs::is_directory( d3 ) );
  BOOST_TEST( !fs::exists( d2 << "f3" ) );
  BOOST_TEST( fs::exists( d3 << "f3" ) );

  // remove() tests on file
  file_ph = dir << "shortlife";
  BOOST_TEST( !fs::exists( file_ph ) );
  create_file( file_ph, "" );
  BOOST_TEST( fs::exists( file_ph ) );
  BOOST_TEST( !fs::is_directory( file_ph ) );
  fs::remove( file_ph );
  BOOST_TEST( !fs::exists( file_ph ) );
  fs::remove( "no-such-file" ); // should be harmless
  fs::remove( "no-such-directory/no-such-file" ); // ditto

  // remove test on directory
  d1 = dir << "shortlife_dir";
  BOOST_TEST( !fs::exists( d1 ) );
  fs::create_directory( d1 );
  BOOST_TEST( fs::exists( d1 ) );
  BOOST_TEST( fs::is_directory( d1 ) );
  BOOST_TEST( fs::_is_empty( d1 ) );
  BOOST_TEST( throws_fs_error( bind( fs::remove, dir ) ) );
  fs::remove( d1 );
  BOOST_TEST( !fs::exists( d1 ) );

  // post-test cleanup
  BOOST_TEST( fs::remove_all( dir ) != 0 );
  // above was added just to simplify testing, but it ended up detecting
  // a bug (failure to close an internal search handle). 
  BOOST_TEST( !fs::exists( dir ) );
  BOOST_TEST( fs::remove_all( dir ) == 0 );

  return 0;
} // main

