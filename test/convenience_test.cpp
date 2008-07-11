//  libs/filesystem/test/convenience_test.cpp  -------------------------------//

//  Copyright Beman Dawes, 2002
//  Copyright Vladimir Prus, 2002
//  Use, modification, and distribution is subject to the Boost Software
//  License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

//  See library home page at http://www.boost.org/libs/filesystem

#include <boost/config/warning_disable.hpp>

#include <boost/filesystem/convenience.hpp>
namespace fs = boost::filesystem;
using fs::path;
namespace sys = boost::system;

#include <boost/test/minimal.hpp>
#include <boost/bind.hpp>
#include <fstream>
#include <iostream>

#ifndef BOOST_FILESYSTEM_NARROW_ONLY
# define BOOST_FS_IS_EMPTY fs::is_empty
# define BOOST_BND(BOOST_FUNC_TO_DO) BOOST_FUNC_TO_DO<fs::path>
#else
# define BOOST_FS_IS_EMPTY fs::_is_empty
# define BOOST_BND(BOOST_FUNC_TO_DO) BOOST_FUNC_TO_DO
#endif

namespace
{
  template< typename F >
    bool throws_fs_error( F func )
  {
    try { func(); }

    catch ( const fs::filesystem_error & )
    {
      return true;
    }
    return false;
  }

    void create_recursive_iterator( const fs::path & ph )
    {
      fs::recursive_directory_iterator it( ph );
    }
}

//  --------------------------------------------------------------------------//

int test_main( int, char*[] )
{
  path::default_name_check( fs::no_check ); // names below not valid on all O/S's
                                            // but they must be tested anyhow

//  create_directories() tests  ----------------------------------------------//

  BOOST_CHECK( !fs::create_directories( "" ) );  // should be harmless
  BOOST_CHECK( !fs::create_directories( "/" ) ); // ditto

  fs::remove_all( "xx" );  // make sure slate is blank
  BOOST_CHECK( !fs::exists( "xx" ) ); // reality check

  BOOST_CHECK( fs::create_directories( "xx" ) );
  BOOST_CHECK( fs::exists( "xx" ) );
  BOOST_CHECK( fs::is_directory( "xx" ) );

  BOOST_CHECK( fs::create_directories( "xx/yy/zz" ) );
  BOOST_CHECK( fs::exists( "xx" ) );
  BOOST_CHECK( fs::exists( "xx/yy" ) );
  BOOST_CHECK( fs::exists( "xx/yy/zz" ) );
  BOOST_CHECK( fs::is_directory( "xx" ) );
  BOOST_CHECK( fs::is_directory( "xx/yy" ) );
  BOOST_CHECK( fs::is_directory( "xx/yy/zz" ) );

  path is_a_file( "xx/uu" );
  {
    std::ofstream f( is_a_file.native_file_string().c_str() );
    BOOST_CHECK( !!f );
  }
  BOOST_CHECK( throws_fs_error(
    boost::bind( BOOST_BND(fs::create_directories), is_a_file ) ) );
  BOOST_CHECK( throws_fs_error(
    boost::bind( BOOST_BND(fs::create_directories), is_a_file / "aa" ) ) );
  
// extension() tests ---------------------------------------------------------//

  BOOST_CHECK( fs::extension("a/b") == "" );
  BOOST_CHECK( fs::extension("a/b.txt") == ".txt" );
  BOOST_CHECK( fs::extension("a/b.") == "." );
  BOOST_CHECK( fs::extension("a.b.c") == ".c" );
  BOOST_CHECK( fs::extension("a.b.c.") == "." );
  BOOST_CHECK( fs::extension("") == "" );
  BOOST_CHECK( fs::extension("a/") == "." );
  
// basename() tests ----------------------------------------------------------//

  BOOST_CHECK( fs::basename("b") == "b" );
  BOOST_CHECK( fs::basename("a/b.txt") == "b" );
  BOOST_CHECK( fs::basename("a/b.") == "b" ); 
  BOOST_CHECK( fs::basename("a.b.c") == "a.b" );
  BOOST_CHECK( fs::basename("a.b.c.") == "a.b.c" );
  BOOST_CHECK( fs::basename("") == "" );
  
// change_extension tests ---------------------------------------------------//

  BOOST_CHECK( fs::change_extension("a.txt", ".tex").string() == "a.tex" );
  BOOST_CHECK( fs::change_extension("a.", ".tex").string() == "a.tex" );
  BOOST_CHECK( fs::change_extension("a", ".txt").string() == "a.txt" );
  BOOST_CHECK( fs::change_extension("a.b.txt", ".tex").string() == "a.b.tex" );  
  // see the rationale in html docs for explanation why this works
  BOOST_CHECK( fs::change_extension("", ".png").string() == ".png" );

// recursive_directory_iterator tests ----------------------------------------//

  sys::error_code ec;
  fs::recursive_directory_iterator it( "/no-such-path", ec );
  BOOST_CHECK( ec );
  BOOST_CHECK( throws_fs_error(
    boost::bind( create_recursive_iterator, "/no-such-path" ) ) );

  fs::remove( "xx/uu" );

#ifdef BOOST_WINDOWS_API
  // These tests depends on ordering of directory entries, and that's guaranteed
  // on Windows but not necessarily on other operating systems
  {
    std::ofstream f( "xx/yya" );
    BOOST_CHECK( !!f );
  }

  for ( it = fs::recursive_directory_iterator( "xx" );
        it != fs::recursive_directory_iterator(); ++it )
    { std::cout << *it << '\n'; }

  it = fs::recursive_directory_iterator( "xx" );
  BOOST_CHECK( it->path() == "xx/yy" );
  BOOST_CHECK( it.level() == 0 );
  ++it;
  BOOST_CHECK( it->path() == "xx/yy/zz" );
  BOOST_CHECK( it.level() == 1 );
  it.pop();
  BOOST_CHECK( it->path() == "xx/yya" );
  BOOST_CHECK( it.level() == 0 );
  it++;
  BOOST_CHECK( it == fs::recursive_directory_iterator() );

  it = fs::recursive_directory_iterator( "xx" );
  BOOST_CHECK( it->path() == "xx/yy" );
  it.no_push();
  ++it;
  BOOST_CHECK( it->path() == "xx/yya" );
  ++it;
  BOOST_CHECK( it == fs::recursive_directory_iterator() );

  fs::remove( "xx/yya" );
#endif

  it = fs::recursive_directory_iterator( "xx/yy/zz" );
  BOOST_CHECK( it == fs::recursive_directory_iterator() );
  
  it = fs::recursive_directory_iterator( "xx" );
  BOOST_CHECK( it->path() == "xx/yy" );
  BOOST_CHECK( it.level() == 0 );
  ++it;
  BOOST_CHECK( it->path() == "xx/yy/zz" );
  BOOST_CHECK( it.level() == 1 );
  it++;
  BOOST_CHECK( it == fs::recursive_directory_iterator() );

  it = fs::recursive_directory_iterator( "xx" );
  BOOST_CHECK( it->path() == "xx/yy" );
  it.no_push();
  ++it;
  BOOST_CHECK( it == fs::recursive_directory_iterator() );

  it = fs::recursive_directory_iterator( "xx" );
  BOOST_CHECK( it->path() == "xx/yy" );
  ++it;
  it.pop();
  BOOST_CHECK( it == fs::recursive_directory_iterator() );



  // nothrow wrong. see imp.  Make sure failed basic_directory_iterator
  // ctor creates the end iterator. 




  return 0;
}
