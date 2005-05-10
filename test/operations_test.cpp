//  Boost operations_test.cpp  -----------------------------------------------//

//  Copyright Beman Dawes 2002.
//  Use, modification, and distribution is subject to the Boost Software
//  License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

//  See library home page at http://www.boost.org/libs/filesystem

#include <boost/filesystem/operations.hpp>
namespace fs = boost::filesystem;

#include <boost/config.hpp>
#include <boost/test/minimal.hpp>
#include <boost/concept_check.hpp>
#include <boost/bind.hpp>
using boost::bind;

#include <fstream>
#include <iostream>
#include <string>
#include <cerrno>
#include <ctime>

# ifdef BOOST_NO_STDC_NAMESPACE
    namespace std { using ::asctime; using ::gmtime; using ::localtime;
    using ::difftime; using ::time; using ::tm; using ::mktime; }
# endif

namespace
{
  bool report_throws;
  fs::directory_iterator end_itr;

  void create_file( const fs::path & ph, const std::string & contents )
  {
    std::ofstream f( ph.file_string().c_str() );
    if ( !f )
      throw fs::filesystem_error( "operations_test create_file",
        ph, errno );
    if ( !contents.empty() ) f << contents;
  }

  void verify_file( const fs::path & ph, const std::string & expected )
  {
    std::ifstream f( ph.file_string().c_str() );
    if ( !f )
      throw fs::filesystem_error( "operations_test verify_file",
        ph, errno );
    std::string contents;
    f >> contents;
    if ( contents != expected )
      throw fs::filesystem_error( "operations_test verify_file contents \""
        + contents  + "\" != \"" + expected + "\"", ph, 0 );
  }

  template< typename F >
    bool throws_fs_error( F func, fs::error_code ec =
      ::boost::filesystem::no_error ) // VC++ 7.1 build 2292 won't accept fs::
  {
    try { func(); }

    catch ( const fs::filesystem_error & ex )
    {
      if ( report_throws ) std::cout << ex.what() << "\n";
      if ( ec == fs::no_error || ec == fs::lookup_error_code(ex.system_error()) ) return true;
      std::cout << "exception reports " << fs::lookup_error_code(ex.system_error())
        << ", should be " << ec
        << "\n system_error() is " << ex.system_error()
        << std::endl;
      return true;
    }
    return false;
  }

} // unnamed namespace

//  test_main  ---------------------------------------------------------------//

int test_main( int argc, char * argv[] )
{
  if ( argc > 1 && *argv[1]=='-' && *(argv[1]+1)=='t' ) report_throws = true;

  std::string platform( BOOST_PLATFORM );

  // The choice of platform is make at runtime rather than compile-time
  // so that compile errors for all platforms will be detected even though
  // only the current platform is runtime tested.
# if defined( BOOST_POSIX_API )
    platform = "POSIX";
# elif defined( BOOST_WINDOWS_API )
    platform = "Windows";
# else
    platform = ( platform == "Win32" || platform == "Win64" || platform == "Cygwin" )
               ? "Windows"
               : "POSIX";
# endif
  std::cout << "API is " << platform << '\n';

  std::cout << "initial_path<path>().string() is\n  \""
    << fs::initial_path<fs::path>().string()
            << "\"\n";
  std::cout << "initial_path<fs::path>().file_string() is\n  \""
            << fs::initial_path<fs::path>().file_string()
            << "\"\n";
  BOOST_CHECK( fs::initial_path<fs::path>().is_complete() );
  BOOST_CHECK( fs::current_path<fs::path>().is_complete() );
  BOOST_CHECK( fs::initial_path<fs::path>().string()
    == fs::current_path<fs::path>().string() );

  BOOST_CHECK( fs::complete( "" ).empty() );
  BOOST_CHECK( fs::complete( "/" ).string() == fs::initial_path<fs::path>().root_path().string() );
  BOOST_CHECK( fs::complete( "foo" ).string() == fs::initial_path<fs::path>().string()+"/foo" );
  BOOST_CHECK( fs::complete( "/foo" ).string() == fs::initial_path<fs::path>().root_path().string()+"foo" );

  fs::path dir(  fs::initial_path<fs::path>() / "temp_fs_test_directory" );
  
  // Windows only tests
  if ( platform == "Windows" )
  {
    BOOST_CHECK( !fs::exists( fs::path( "//share-not/foo" ) ) );
    BOOST_CHECK( dir.string().size() > 1
      && dir.string()[1] == ':' ); // verify path includes drive

    BOOST_CHECK( fs::system_complete( "" ).empty() );
    BOOST_CHECK( fs::system_complete( "/" ).string()
      == fs::initial_path<fs::path>().root_path().string() );
    BOOST_CHECK( fs::system_complete( "foo" ).string()
      == fs::initial_path<fs::path>().string()+"/foo" );
    BOOST_CHECK( fs::system_complete( "/foo" ).string()
      == fs::initial_path<fs::path>().root_path().string()+"foo" );
    BOOST_CHECK( fs::complete( fs::path( "c:/" ) ).string()
      == "c:/" );
    BOOST_CHECK( fs::complete( fs::path( "c:/foo" ) ).string()
      ==  "c:/foo" );
    BOOST_CHECK( fs::complete( fs::path( "//share" ) ).string()
      ==  "//share" );

    BOOST_CHECK( fs::system_complete( fs::path( fs::initial_path<fs::path>().root_name() ) ).string() == fs::initial_path<fs::path>().string() );
    BOOST_CHECK( fs::system_complete( fs::path( fs::initial_path<fs::path>().root_name()
      + "foo" ) ).string() == fs::initial_path<fs::path>().string()+"/foo" );
    BOOST_CHECK( fs::system_complete( fs::path( "c:/" ) ).string()
      == "c:/" );
    BOOST_CHECK( fs::system_complete( fs::path( "c:/foo" ) ).string()
      ==  "c:/foo" );
    BOOST_CHECK( fs::system_complete( fs::path( "//share" ) ).string()
      ==  "//share" );
  } // Windows

  else if ( platform == "POSIX" )
  {
    BOOST_CHECK( fs::system_complete( "" ).empty() );
    BOOST_CHECK( fs::initial_path<fs::path>().root_path().string() == "/" );
    BOOST_CHECK( fs::system_complete( "/" ).string() == "/" );
    BOOST_CHECK( fs::system_complete( "foo" ).string()
      == fs::initial_path<fs::path>().string()+"/foo" );
    BOOST_CHECK( fs::system_complete( "/foo" ).string()
      == fs::initial_path<fs::path>().root_path().string()+"foo" );
  } // POSIX

  fs::path ng( " no-way, Jose" );
  BOOST_CHECK( !fs::is_directory( ng ) );

  fs::remove_all( dir );  // in case residue from prior failed tests
  BOOST_CHECK( !fs::exists( dir ) );

  // the bound functions should throw, so throws_fs_error() should return true
  BOOST_CHECK( throws_fs_error( bind( fs::file_size<fs::path>, ng ), fs::not_found_error ) );

  // test path::exception members
  try { fs::file_size( ng ); } // will throw

  catch ( const fs::filesystem_error & ex )
  {
    BOOST_CHECK( ex.path1().string() == " no-way, Jose" );
  }

  BOOST_CHECK( fs::create_directory( dir ) );

  BOOST_CHECK( fs::exists( dir ) );
  BOOST_CHECK( fs::is_empty( dir ) );
  BOOST_CHECK( fs::is_directory( dir ) );
  BOOST_CHECK( throws_fs_error( bind( fs::file_size<fs::path>, dir ),
    fs::is_directory_error ) );
  BOOST_CHECK( !fs::create_directory( dir ) );

  BOOST_CHECK( !fs::is_symlink( dir ) );
  BOOST_CHECK( !fs::is_symlink( "nosuchfileordirectory" ) );

  BOOST_CHECK( !fs::symbolic_link_exists( dir ) );
  BOOST_CHECK( !fs::symbolic_link_exists( "nosuchfileordirectory" ) );

  fs::path d1( dir / "d1" );
  BOOST_CHECK( fs::create_directory( d1 ) );
  BOOST_CHECK( fs::exists( d1 ) );
  BOOST_CHECK( fs::is_directory( d1 ) );
  BOOST_CHECK( fs::is_empty( d1 ) );

  boost::function_requires< boost::InputIteratorConcept< fs::directory_iterator > >();

  {
    fs::directory_iterator dir_itr( dir );
    BOOST_CHECK( dir_itr->leaf() == "d1" );
  }

  // create a second directory named d2
  fs::path d2( dir / "d2" );
  fs::create_directory(d2 );
  BOOST_CHECK( fs::exists( d2 ) );
  BOOST_CHECK( fs::is_directory( d2 ) );

  // test the basic operation of directory_iterators, and test that
  // stepping one iterator doesn't affect a different iterator.
  {
    fs::directory_iterator dir_itr( dir );
    fs::directory_iterator dir_itr2( dir );
    BOOST_CHECK( dir_itr->leaf() == "d1" || dir_itr->leaf() == "d2" );
    BOOST_CHECK( dir_itr2->leaf() == "d1" || dir_itr2->leaf() == "d2" );
    if ( dir_itr->leaf() == "d1" )
    {
      BOOST_CHECK( (++dir_itr)->leaf() == "d2" );
      BOOST_CHECK( dir_itr2->leaf() == "d1" );
      BOOST_CHECK( (++dir_itr2)->leaf() == "d2" );
    }
    else
    {
      BOOST_CHECK( (dir_itr)->leaf() == "d2" );
      BOOST_CHECK( (++dir_itr)->leaf() == "d1" );
      BOOST_CHECK( dir_itr2->leaf() == "d2" );
      BOOST_CHECK( (++dir_itr2)->leaf() == "d1" );
    }
    BOOST_CHECK( ++dir_itr == fs::directory_iterator() );
    BOOST_CHECK( dir_itr2 != fs::directory_iterator() );
    BOOST_CHECK( ++dir_itr2 == fs::directory_iterator() );
  }

  { // *i++ must work to meet the standard's InputIterator requirements
    fs::directory_iterator dir_itr( dir );
    BOOST_CHECK( dir_itr->leaf() == "d1" || dir_itr->leaf() == "d2" );
    if ( dir_itr->leaf() == "d1" )
    {
      BOOST_CHECK( (*dir_itr++).leaf() == "d1" );
      BOOST_CHECK( dir_itr->leaf() == "d2" );
    }
    else
    {
      // Check C++98 input iterator requirements
      BOOST_CHECK( (*dir_itr++).leaf() == "d1" );
      // input iterator requirements in the current WP would require this check:
      // BOOST_CHECK( implicit_cast<std::string const&>(*dir_itr++).leaf() == "d1" );

      BOOST_CHECK( dir_itr->leaf() == "d1" );
    }
  }

  // create an empty file named "f0"
  fs::path file_ph( dir / "f0");
  create_file( file_ph, "" );
  BOOST_CHECK( fs::exists( file_ph ) );
  BOOST_CHECK( !fs::is_directory( file_ph ) );
  BOOST_CHECK( fs::is_empty( file_ph ) );
  BOOST_CHECK( fs::file_size( file_ph ) == 0 );
  BOOST_CHECK( throws_fs_error( bind( fs::create_directory<fs::path>, file_ph ),
    fs::not_directory_error ) );
  // create a file named "f1"
  file_ph = dir / "f1";
  create_file( file_ph, "foobar1" );

  // equivalence tests
  fs::path ng2("does_not_exist2");
  BOOST_CHECK( throws_fs_error( bind( fs::equivalent<fs::path>, ng, ng2 ) ) );
  BOOST_CHECK( fs::equivalent( file_ph, dir / "f1" ) );
  BOOST_CHECK( fs::equivalent( dir, d1 / ".." ) );
  BOOST_CHECK( !fs::equivalent( file_ph, dir ) );
  BOOST_CHECK( !fs::equivalent( dir, file_ph ) );
  BOOST_CHECK( !fs::equivalent( d1, d2 ) );
  BOOST_CHECK( !fs::equivalent( dir, ng ) );
  BOOST_CHECK( !fs::equivalent( ng, dir ) );
  BOOST_CHECK( !fs::equivalent( file_ph, ng ) );
  BOOST_CHECK( !fs::equivalent( ng, file_ph ) );
  
  // hard link tests
  fs::path link_ph( dir / "f3" );
  BOOST_CHECK( !exists( link_ph ) );
  BOOST_CHECK( exists( file_ph ) );
  fs::create_hard_link( file_ph, link_ph );
  BOOST_CHECK( exists( link_ph ) );
  BOOST_CHECK( exists( file_ph ) );
  BOOST_CHECK( fs::equivalent( link_ph, file_ph ) );
  BOOST_CHECK( throws_fs_error(
    bind( fs::create_hard_link<fs::path>, dir, dir/"shouldnotwork" ), fs::not_found_error ) );

  // write time tests
  std::time_t ft = fs::last_write_time( file_ph );
  std::cout << "UTC should currently be about " << std::asctime(std::gmtime(&ft)) << "\n";
  std::cout << "Local time should currently be about " << std::asctime(std::localtime(&ft)) << std::endl;

  // hard to test time exactly, but except under the most unusual circumstances,
  // time since file creation should be no more than one minute, I'm hoping.
  double time_diff = std::difftime( std::time(0), fs::last_write_time( file_ph ) );
  BOOST_CHECK( time_diff > -60.0 && time_diff < 60.0 );

  BOOST_CHECK( fs::exists( file_ph ) );
  BOOST_CHECK( !fs::is_directory( file_ph ) );
  BOOST_CHECK( fs::file_size( file_ph ) == 7 );
  verify_file( file_ph, "foobar1" );

  std::tm * tmp = std::localtime( &ft );
  std::cout << "Year is " << tmp->tm_year << std::endl;
  --tmp->tm_year;
  std::cout << "Change year to " << tmp->tm_year << std::endl;
  fs::last_write_time( file_ph, std::mktime( tmp ) );
  std::cout << "Get new value" << std::endl;
  ft = fs::last_write_time( file_ph );
  std::cout << "Local time one year ago should currently be about " << std::asctime(std::localtime(&ft)) << "\n";
  std::cout << "Now get time difference" << std::endl;
  time_diff = std::difftime( std::time(0), fs::last_write_time( file_ph ) );
  std::cout << "Time difference is : " << time_diff << std::endl;
  BOOST_CHECK( time_diff >= 365*24*3600.0 && time_diff < (366*24*3600.0 + 60.0) );
  std::cout << "Reset to current time" << std::endl;
  fs::last_write_time( file_ph, std::time(0) );
  std::cout << "And check that" << std::endl;
  time_diff = std::difftime( std::time(0), fs::last_write_time( file_ph ) );
  BOOST_CHECK( time_diff >= -60.0 && time_diff < 60.0 );
  ft = fs::last_write_time( file_ph );
  std::cout << "Local time should currently be about " << std::asctime(std::localtime(&ft)) << "\n";

  // there was an inital bug in directory_iterator that caused premature
  // close of an OS handle. This block will detect regression.
  {
    fs::directory_iterator di;
    { di = fs::directory_iterator( dir ); }
    BOOST_CHECK( ++di != fs::directory_iterator() );
  }

  // copy_file() tests
/*  fs::copy_file( file_ph, d1 / "f2" );
  BOOST_CHECK( fs::exists( file_ph ) );
  BOOST_CHECK( fs::exists( d1 / "f2" ) );
  BOOST_CHECK( !fs::is_directory( d1 / "f2" ) );
  verify_file( d1 / "f2", "foobar1" );
*/
  // rename() test case numbers refer to operations.htm#rename table

  // [case 1] make sure can't rename() a non-existent file
  BOOST_CHECK( !fs::exists( d1 / "f99" ) );
  BOOST_CHECK( !fs::exists( d1 / "f98" ) );
///  BOOST_CHECK( throws_fs_error( bind( fs::rename, d1 / "f99", d1 / "f98" ),
///    fs::not_found_error ) );
///  BOOST_CHECK( throws_fs_error( bind( fs::rename, fs::path(""), d1 / "f98" ),
///    fs::not_found_error ) );

  // [case 2] rename() target.empty()
///  BOOST_CHECK( throws_fs_error( bind( fs::rename, file_ph, "" ),
///    fs::not_found_error ) );

#if 0
  // [case 3] make sure can't rename() to an existent file or directory
///  BOOST_CHECK( fs::exists( dir / "f1" ) );
  BOOST_CHECK( fs::exists( d1 / "f2" ) );
///  BOOST_CHECK( throws_fs_error( bind( fs::rename, dir / "f1", d1 / "f2" ) ) );
  // several POSIX implementations (cygwin, openBSD) report ENOENT instead of EEXIST,
  // so we don't verify error type on the above test.
///  BOOST_CHECK( throws_fs_error( bind( fs::rename, dir, d1 ) ) );

  // [case 4A] can't rename() file to a nonexistent parent directory
  BOOST_CHECK( !fs::is_directory( dir / "f1" ) );
  BOOST_CHECK( !fs::exists( dir / "d3/f3" ) );
///  BOOST_CHECK( throws_fs_error( bind( fs::rename, dir / "f1", dir / "d3/f3" ),
///    fs::not_found_error ) );

  // [case 4B] rename() file in same directory
  BOOST_CHECK( fs::exists( d1 / "f2" ) );
  BOOST_CHECK( !fs::exists( d1 / "f50" ) );
  fs::rename( d1 / "f2", d1 / "f50" );
  BOOST_CHECK( !fs::exists( d1 / "f2" ) );
  BOOST_CHECK( fs::exists( d1 / "f50" ) );
  fs::rename( d1 / "f50", d1 / "f2" );
  BOOST_CHECK( fs::exists( d1 / "f2" ) );
  BOOST_CHECK( !fs::exists( d1 / "f50" ) );

  // [case 4C] rename() file d1/f2 to d2/f3
  fs::rename( d1 / "f2", d2 / "f3" );
  BOOST_CHECK( !fs::exists( d1 / "f2" ) );
  BOOST_CHECK( !fs::exists( d2 / "f2" ) );
  BOOST_CHECK( fs::exists( d2 / "f3" ) );
  BOOST_CHECK( !fs::is_directory( d2 / "f3" ) );
  verify_file( d2 / "f3", "foobar1" );
  fs::rename( d2 / "f3", d1 / "f2" );
  BOOST_CHECK( fs::exists( d1 / "f2" ) );

  // [case 5A] rename() directory to nonexistent parent directory
  BOOST_CHECK( fs::exists( d1 ) );
  BOOST_CHECK( !fs::exists( dir / "d3/d5" ) );
  BOOST_CHECK( !fs::exists( dir / "d3" ) );
///  BOOST_CHECK( throws_fs_error( bind( fs::rename, d1, dir / "d3/d5" ),
///    fs::not_found_error ) );

  // [case 5B] rename() on directory
  fs::path d3( dir / "d3" );
  BOOST_CHECK( fs::exists( d1 ) );
  BOOST_CHECK( fs::exists( d1 / "f2" ) );
  BOOST_CHECK( !fs::exists( d3 ) );
  fs::rename( d1, d3 );
  BOOST_CHECK( !fs::exists( d1 ) );
  BOOST_CHECK( fs::exists( d3 ) );
  BOOST_CHECK( fs::is_directory( d3 ) );
  BOOST_CHECK( !fs::exists( d1 / "f2" ) );
  BOOST_CHECK( fs::exists( d3 / "f2" ) );
  fs::rename( d3, d1 );
  BOOST_CHECK( fs::exists( d1 ) );
  BOOST_CHECK( fs::exists( d1 / "f2" ) );
  BOOST_CHECK( !fs::exists( d3 ) );

  // [case 5C] rename() rename and move d1 to d2 / "d20"
  BOOST_CHECK( fs::exists( d1 ) );
  BOOST_CHECK( !fs::exists( d2 / "d20" ) );
  BOOST_CHECK( fs::exists( d1 / "f2" ) );
  fs::rename( d1, d2 / "d20" );
  BOOST_CHECK( !fs::exists( d1 ) );
  BOOST_CHECK( fs::exists( d2 / "d20" ) );
  BOOST_CHECK( fs::exists( d2 / "d20" / "f2" ) );
  fs::rename( d2 / "d20", d1 );
  BOOST_CHECK( fs::exists( d1 ) );
  BOOST_CHECK( !fs::exists( d2 / "d20" ) );
  BOOST_CHECK( fs::exists( d1 / "f2" ) );
#endif
  // remove() tests on file
  file_ph = dir / "shortlife";
  BOOST_CHECK( !fs::exists( file_ph ) );
  create_file( file_ph, "" );
  BOOST_CHECK( fs::exists( file_ph ) );
  BOOST_CHECK( !fs::is_directory( file_ph ) );
  BOOST_CHECK( fs::remove( file_ph ) );
  BOOST_CHECK( !fs::exists( file_ph ) );
  BOOST_CHECK( !fs::remove( "no-such-file" ) );
  BOOST_CHECK( !fs::remove( "no-such-directory/no-such-file" ) );

  // remove() test on directory
  d1 = dir / "shortlife_dir";
  BOOST_CHECK( !fs::exists( d1 ) );
  fs::create_directory( d1 );
  BOOST_CHECK( fs::exists( d1 ) );
  BOOST_CHECK( fs::is_directory( d1 ) );
  BOOST_CHECK( fs::is_empty( d1 ) );
///  BOOST_CHECK( throws_fs_error( bind( fs::remove, dir ), fs::not_empty_error ) );
  BOOST_CHECK( fs::remove( d1 ) );
  BOOST_CHECK( !fs::exists( d1 ) );

  // post-test cleanup
  BOOST_CHECK( fs::remove_all( dir ) != 0 );
  // above was added just to simplify testing, but it ended up detecting
  // a bug (failure to close an internal search handle). 
  BOOST_CHECK( !fs::exists( dir ) );
  BOOST_CHECK( fs::remove_all( dir ) == 0 );
  return 0;
} // main

