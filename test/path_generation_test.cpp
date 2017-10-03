//  filesystem path_generation_test.cpp  ---------------------------------------------  //

//  Copyright Beman Dawes 2015

//  Distributed under the Boost Software License, Version 1.0.
//  See http://www.boost.org/LICENSE_1_0.txt

//  Library home page: http://www.boost.org/libs/filesystem

#include <boost/config/warning_disable.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/utility/string_view.hpp>
#include <boost/detail/lightweight_test_report.hpp>
#include <iostream>

using boost::filesystem::path;
using boost::string_view;
using boost::wstring_view;
using std::cout;
using std::endl;

#define PATH_TEST_EQ(a, b) check(a, b, __FILE__, __LINE__)

namespace
{

  // character type insensitive equality  ----------------------------------------------//
  //   Used to allow string comparison regardless of path::value_type, and thus allowing
  //   expected test results to be expressed as undecorated string literals.
  //   Requires: both arguments contain only ascii characters
#ifdef BOOST_POSIX_API
  inline bool ascii_eq(wstring_view lhs, string_view rhs)
  {
    if (lhs.size() != rhs.size())
      return false;

    string_view::const_iterator left = lhs.cbegin();
    string_view::const_iterator right = rhs.cbegin();
    for (; left != lhs.end(); ++left, ++right)
    {
      if (*left != *right)
        return false;
    }
    return true;
  }
#else
  inline bool ascii_eq(wstring_view lhs, string_view rhs)
  {
    if (lhs.size() != rhs.size())
      return false;

    wstring_view::const_iterator left = lhs.cbegin();
    string_view::const_iterator right = rhs.cbegin();
    for (; left != lhs.end(); ++left, ++right)
    {
      if (*left != *right)
        return false;
    }
    return true;
  }
#endif

  void check(const path& result,
    const std::string & expected, const char* file, int line)
  {
    if (ascii_eq(result.native(), expected))
      return;

    // The format of this message is choosen to ensure that double-clicking on the
    // message in the Visual Studio IDE output window will take you to the line in the
    // file that caused the error.
    cout << file
      << '(' << line << "): result: \"" << result.string()
      << "\" != expected \"" << expected
      << "\"" << endl;

    ++::boost::detail::test_errors();
  }

  void lexically_relative_test()
  {
    cout << "lexically_relative_test..." << endl;

    // Caution: Results of test cases with a root-directory may differ between POSIX and
    // Windows because is_absolute(), which the spec depends on, may return different
    // results for these platforms.

    // examples from 30.10.8.4.11 path generation [fs.path.gen] ¶ 5
    BOOST_TEST_EQ(path("/a/d").lexically_relative("/a/b/c"), "../../d");
    BOOST_TEST_EQ(path("/a/b/c").lexically_relative("/a/d"), "../b/c");
    BOOST_TEST_EQ(path("a/b/c").lexically_relative("a"), "b/c");
    BOOST_TEST_EQ(path("a/b/c").lexically_relative("a/b/c/x/y"), "../..");
    BOOST_TEST_EQ(path("a/b/c").lexically_relative("a/b/c"), ".");
    BOOST_TEST_EQ(path("a/b").lexically_relative("c/d"), "../../a/b");

    // simple test cases
    BOOST_TEST_EQ(path("").lexically_relative(""), ".");
    BOOST_TEST_EQ(path(" ").lexically_relative(" "), ".");
    BOOST_TEST_EQ(path("").lexically_relative(" "), "..");
    BOOST_TEST_EQ(path(" ").lexically_relative(""), " ");

    BOOST_TEST_EQ(path("/foo").lexically_relative(""), "/foo");
    BOOST_TEST_EQ(path("/foo").lexically_relative("/foo"), ".");
    BOOST_TEST_EQ(path("").lexically_relative("/foo"), "");
    BOOST_TEST_EQ(path("").lexically_relative("foo"), "..");
    BOOST_TEST_EQ(path("foo").lexically_relative(""), "foo");
    BOOST_TEST_EQ(path("foo").lexically_relative("foo"), ".");

    BOOST_TEST_EQ(path("a/b/c").lexically_relative("a"), "b/c");
    BOOST_TEST_EQ(path("a//b//c").lexically_relative("a"), "b/c");
    BOOST_TEST_EQ(path("a/b/c").lexically_relative("a/b"), "c");
    BOOST_TEST_EQ(path("a///b//c").lexically_relative("a//b"), "c");
    BOOST_TEST_EQ(path("a/b/c").lexically_relative("a/b/c"), ".");
    BOOST_TEST_EQ(path("a/b/c").lexically_relative("a/b/c/x"), "..");
    BOOST_TEST_EQ(path("a/b/c").lexically_relative("a/b/c/x/y"), "../..");
    BOOST_TEST_EQ(path("a/b/c").lexically_relative("a/x"), "../b/c");
    BOOST_TEST_EQ(path("a/b/c").lexically_relative("a/b/x"), "../c");
    BOOST_TEST_EQ(path("a/b/c").lexically_relative("a/x/y"), "../../b/c");
    BOOST_TEST_EQ(path("a/b/c").lexically_relative("a/b/x/y"), "../../c");
    BOOST_TEST_EQ(path("a/b/c").lexically_relative("a/b/c/x/y/z"), "../../..");
 
    // paths unrelated except first element, and first element is root directory
    BOOST_TEST_EQ(path("/a/b/c").lexically_relative("/x"), "../a/b/c");
    BOOST_TEST_EQ(path("/a/b/c").lexically_relative("/x/y"), "../../a/b/c");
    BOOST_TEST_EQ(path("/a/b/c").lexically_relative("/x/y/z"), "../../../a/b/c");
 
    // paths unrelated
    BOOST_TEST_EQ(path("a/b/c").lexically_relative("x"), "../a/b/c");
    BOOST_TEST_EQ(path("a/b/c").lexically_relative("x/y"), "../../a/b/c");
    BOOST_TEST_EQ(path("a/b/c").lexically_relative("x/y/z"), "../../../a/b/c");
    BOOST_TEST_EQ(path("a/b/c").lexically_relative("/x"), "");
    BOOST_TEST_EQ(path("a/b/c").lexically_relative("/x/y"), "");
    BOOST_TEST_EQ(path("a/b/c").lexically_relative("/x/y/z"), "");
    BOOST_TEST_EQ(path("a/b/c").lexically_relative("/a/b/c"), "");
  
    // TODO: add some Windows-only test cases that probe presence or absence of
    // drive specifier-and root-directory

    //  Some tests from Jamie Allsop's paper
    BOOST_TEST_EQ(path("/a/d").lexically_relative("/a/b/c"), "../../d");
    BOOST_TEST_EQ(path("/a/b/c").lexically_relative("/a/d"), "../b/c");
  #ifdef BOOST_WINDOWS_API  
    BOOST_TEST_EQ(path("c:\\y").lexically_relative("c:\\x"), "../y");
  #else
    BOOST_TEST_EQ(path("c:\\y").lexically_relative("c:\\x"), "");
  #endif  
    BOOST_TEST_EQ(path("d:\\y").lexically_relative("c:\\x"), "");

    //  From issue #1976
    BOOST_TEST_EQ(path("/foo/new").lexically_relative("/foo/bar"), "../new");
  }

  void lexically_proximate_test()
  {
    cout << "lexically_proximate_test..." << endl;
    // paths unrelated
//    BOOST_TEST(path("a/b/c").lexically_proximate("x"), "a/b/c");
  }
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

  lexically_relative_test();
  lexically_proximate_test();

  return ::boost::report_errors();
}
