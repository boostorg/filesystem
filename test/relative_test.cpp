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

namespace fs = boost::filesystem;
using boost::filesystem::path;
using std::cout;
using std::endl;


namespace
{
  boost::system::error_code ec;
  const boost::system::error_code ok;
  const boost::system::error_code ng(-1, boost::system::system_category());

  std::string platform(BOOST_PLATFORM);

}  // unnamed namespace

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

  //  Given p and base are objects of class path; if !p.empty() && !base.empty() &&
  //  *p.begin() == *base.begin() then return a path such that
  //  p == base / p.relative(base) otherwise return path(). However, if all elements of
  //  p are equal to the corresponding element of base, return path(".").

  BOOST_TEST(path("").relative("") == path());
  BOOST_TEST(path("").relative("/foo") == path());
  BOOST_TEST(path("/foo").relative("") == path());
  BOOST_TEST(path("/foo").relative("/foo") == path("."));
  BOOST_TEST(path("").relative("foo") == path());
  BOOST_TEST(path("foo").relative("") == path());
  BOOST_TEST(path("foo").relative("foo") == path("."));

  BOOST_TEST(path("a/b/c").relative("a") == path("b/c"));
  BOOST_TEST(path("a/b/c").relative("a/b") == path("c"));
  BOOST_TEST(path("a/b/c").relative("a/b/c") == path("."));
  BOOST_TEST(path("a/b/c").relative("a/b/c/x") == path(".."));
  BOOST_TEST(path("a/b/c").relative("a/b/c/x/y") == path("../.."));
  BOOST_TEST(path("a/b/c").relative("a/x") == path("../b/c"));
  BOOST_TEST(path("a/b/c").relative("a/b/x") == path("../c"));
  BOOST_TEST(path("a/b/c").relative("a/x/y") == path("../../b/c"));
  BOOST_TEST(path("a/b/c").relative("a/b/x/y") == path("../../c"));
  BOOST_TEST(path("a/b/c").relative("a/b/c/x/y/z") == path("../../.."));
 
  BOOST_TEST(path("a/b/c").relative("x") == path());
  BOOST_TEST(path("a/b/c").relative("x/y") == path());
  BOOST_TEST(path("a/b/c").relative("x/y/z") == path());
  BOOST_TEST(path("a/b/c").relative("x") == path());
  BOOST_TEST(path("a/b/c").relative("x/y") == path());
  BOOST_TEST(path("a/b/c").relative("x/y/z") == path());
  
  // TODO: add some Windows-only test cases that probe presence or absence of
  // drive specifier-and root-directory

  //  Some tests from Jamie Allsop's paper
  BOOST_TEST(path("/a/d").relative("/a/b/c") == path("../../d"));
  BOOST_TEST(path("/a/b/c").relative("/a/d") == path("../b/c"));
#ifdef BOOST_WINDOWS_API  
  BOOST_TEST(path("c:\\y").relative("c:\\x") == path("../y"));
#else
  BOOST_TEST(path("c:\\y").relative("c:\\x") == path(""));
#endif  
  BOOST_TEST(path("d:\\y").relative("c:\\x") == path(""));

  //  From issue #1976
  BOOST_TEST(path("/foo/new").relative("/foo/bar") == path("../new"));

  return ::boost::report_errors();
}
