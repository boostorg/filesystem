//  filesystem relative_test.cpp  ----------------------------------------------------  //

//  Copyright Beman Dawes 2015

//  Distributed under the Boost Software License, Version 1.0.
//  See http://www.boost.org/LICENSE_1_0.txt

//  Library home page: http://www.boost.org/libs/filesystem

//  ----------------------------------------------------------------------------------  //
//
//  At least initially, development is easier if these tests are in a separate file.
//
//  ----------------------------------------------------------------------------------  //

#include <boost/config/warning_disable.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/detail/lightweight_test_report.hpp>
#include <iostream>

using boost::filesystem::path;
using boost::filesystem::lexically_relative;
using std::cout;
using std::endl;

//--------------------------------------------------------------------------------------//
//                                                                                      //
//                                     main                                             //
//                                                                                      //
//--------------------------------------------------------------------------------------//

int test_main(int, char*[])
{
// document state of critical macros
#ifdef BOOST_POSIX_API
  cout << "BOOST_POSIX_API" << endl;
#endif
#ifdef BOOST_WINDOWS_API
  cout << "BOOST_WINDOWS_API" << endl;
#endif

  BOOST_TEST(lexically_relative("", "") == "");
  BOOST_TEST(lexically_relative("", "/foo") == "");
  BOOST_TEST(lexically_relative("/foo", "") == "");
  BOOST_TEST(lexically_relative("/foo", "/foo") == ".");
  BOOST_TEST(lexically_relative("", "foo") == "");
  BOOST_TEST(lexically_relative("foo", "") == "");
  BOOST_TEST(lexically_relative("foo", "foo") == ".");

  BOOST_TEST(lexically_relative("a/b/c", "a") == "b/c");
  BOOST_TEST(lexically_relative("a//b//c", "a") == "b/c");
  BOOST_TEST(lexically_relative("a/b/c", "a/b") == "c");
  BOOST_TEST(lexically_relative("a///b//c", "a//b") == "c");
  BOOST_TEST(lexically_relative("a/b/c", "a/b/c") == ".");
  BOOST_TEST(lexically_relative("a/b/c", "a/b/c/x") == "..");
  BOOST_TEST(lexically_relative("a/b/c", "a/b/c/x/y") == "../..");
  BOOST_TEST(lexically_relative("a/b/c", "a/x") == "../b/c");
  BOOST_TEST(lexically_relative("a/b/c", "a/b/x") == "../c");
  BOOST_TEST(lexically_relative("a/b/c", "a/x/y") == "../../b/c");
  BOOST_TEST(lexically_relative("a/b/c", "a/b/x/y") == "../../c");
  BOOST_TEST(lexically_relative("a/b/c", "a/b/c/x/y/z") == "../../..");
 
  // paths unrelated except first element, and first element is root directory
  BOOST_TEST(lexically_relative("/a/b/c", "/x") == "../a/b/c");
  BOOST_TEST(lexically_relative("/a/b/c", "/x/y") == "../../a/b/c");
  BOOST_TEST(lexically_relative("/a/b/c", "/x/y/z") == "../../../a/b/c");
 
  // paths unrelated
  BOOST_TEST(lexically_relative("a/b/c", "x") == "");
  BOOST_TEST(lexically_relative("a/b/c", "x/y") == "");
  BOOST_TEST(lexically_relative("a/b/c", "x/y/z") == "");
  BOOST_TEST(lexically_relative("a/b/c", "/x") == "");
  BOOST_TEST(lexically_relative("a/b/c", "/x/y") == "");
  BOOST_TEST(lexically_relative("a/b/c", "/x/y/z") == "");
  BOOST_TEST(lexically_relative("a/b/c", "/a/b/c") == "");
  
  // TODO: add some Windows-only test cases that probe presence or absence of
  // drive specifier-and root-directory

  //  Some tests from Jamie Allsop's paper
  BOOST_TEST(lexically_relative("/a/d", "/a/b/c") == "../../d");
  BOOST_TEST(lexically_relative("/a/b/c", "/a/d") == "../b/c");
#ifdef BOOST_WINDOWS_API  
  BOOST_TEST(lexically_relative("c:\\y", "c:\\x") == "../y");
#else
  BOOST_TEST(lexically_relative("c:\\y", "c:\\x") == "");
#endif  
  BOOST_TEST(lexically_relative("d:\\y", "c:\\x") == "");

  //  From issue #1976
  BOOST_TEST(lexically_relative("/foo/new", "/foo/bar") == "../new");

  return ::boost::report_errors();
}
