//  libs/filesystem/test/convenience_test.cpp  -------------------------------//

//  Copyright Beman Dawes, 2002
//  (Copyright Vladimir Prus, 2002
//  See accompanying license for terms and conditions of use.

//  See http://www.boost.org/libs/filesystem for documentation.

#include <boost/filesystem/convenience.hpp>
namespace fs = boost::filesystem;
using fs::path;

#include <boost/test/minimal.hpp>

int test_main( int, char*[] )
{
  path::default_name_check( fs::no_check ); // names below not valid on all O/S's
                                            // but they must be tested anyhow

 //  create_directories() tests  ----------------------------------------------//

  fs::create_directories( "" );  // should be harmless
  fs::create_directories( "/" ); // ditto
  fs::remove_all( "xx/yy/zz" );  // make sure slate is blank

  fs::create_directories( "xx" );
  BOOST_TEST( fs::exists( "xx" ) );
  BOOST_TEST( fs::is_directory( "xx" ) );

  fs::create_directories( "xx/ww/zz" );
  BOOST_TEST( fs::exists( "xx" ) );
  BOOST_TEST( fs::exists( "xx/ww" ) );
  BOOST_TEST( fs::exists( "xx/ww/zz" ) );
  BOOST_TEST( fs::is_directory( "xx" ) );
  BOOST_TEST( fs::is_directory( "xx/ww" ) );
  BOOST_TEST( fs::is_directory( "xx/ww/zz" ) );
  
// extension() tests ----------------------------------------------------------//

  BOOST_TEST( fs::extension("a/b") == "" );
  BOOST_TEST( fs::extension("a/b.txt") == ".txt" );
  BOOST_TEST( fs::extension("a/b.") == "." );
  BOOST_TEST( fs::extension("a.b.c") == ".c" );
  BOOST_TEST( fs::extension("a.b.c.") == "." );
  BOOST_TEST( fs::extension("") == "" );
  BOOST_TEST( fs::extension("a/") == "" );
  
// basename() tests ----------------------------------------------------------//

  BOOST_TEST( fs::basename("b") == "b" );
  BOOST_TEST( fs::basename("a/b.txt") == "b" );
  BOOST_TEST( fs::basename("a/b.") == "b" ); 
  BOOST_TEST( fs::basename("a.b.c") == "a.b" );
  BOOST_TEST( fs::basename("a.b.c.") == "a.b.c" );
  BOOST_TEST( fs::basename("") == "" );
  
// change_extension tests ---------------------------------------------------//

  BOOST_TEST( fs::change_extension("a.txt", ".tex").string() == "a.tex" );
  BOOST_TEST( fs::change_extension("a.", ".tex").string() == "a.tex" );
  BOOST_TEST( fs::change_extension("a", ".txt").string() == "a.txt" );
  BOOST_TEST( fs::change_extension("a.b.txt", ".tex").string() == "a.b.tex" );  
  // see the rationale in html docs for explanation why this works
  BOOST_TEST( fs::change_extension("", ".png").string() == ".png" );

  return 0;
}
