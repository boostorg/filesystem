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

  void lexically_relative_tests()
  {
    cout << "lexically_relative_tests..." << endl;

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

  //  lexically_normal_tests  ----------------------------------------------------------//

  void lexically_normal_tests()
  {
    std::cout << "lexically_normal_tests..." << std::endl;

    // the lexically_normal implementation depends on root_name() so test that first
    BOOST_TEST(path("//").has_root_name());
    BOOST_TEST(path("//foo").has_root_name());
    BOOST_TEST(!path("///foo").has_root_name());

    // the lexically_normal implementation assumes that iteration over 
    // "//foo//bar///baz////" yields "//foo", "/", "bar", "baz", ""
    path foo_bar_baz("//foo//bar///baz////");
    path::iterator it = foo_bar_baz.begin();
    BOOST_TEST(*it == "//foo");
    BOOST_TEST(*++it == "/");
    BOOST_TEST(*++it == "bar");
    BOOST_TEST(*++it == "baz");
    BOOST_TEST(*++it == "");
    BOOST_TEST(++it == foo_bar_baz.end());

    //  Note: lexically_normal() uses /= to build up some results, so these results will
    //  have the platform's preferred separator. Since that is immaterial to most
    //  of these tests, the results are converted to generic form,
    //  and the expected results are also given in generic form. Otherwise many of the
    //  tests would incorrectly be reported as failing on Windows.

    //  Tests from 30.10.8.4.11 path generation [fs.path.gen]
    PATH_TEST_EQ(path("foo/./bar/..").lexically_normal().generic_path(), "foo/");
    PATH_TEST_EQ(path("foo/.///bar/../").lexically_normal().generic_path(), "foo/");

    //  Tests corresponding to the table in US-37 of c++ standards committee document
    //  P0492R1 from the 2017 Kona pre-meeting mailing. 
    PATH_TEST_EQ(path("").lexically_normal().generic_path(), "");            // case 1 
    PATH_TEST_EQ(path(".").lexically_normal().generic_path(), ".");		       // case 2
    PATH_TEST_EQ(path("..").lexically_normal().generic_path(), "..");	       // case 3
    PATH_TEST_EQ(path("/").lexically_normal().generic_path(), "/");		       // case 4
    PATH_TEST_EQ(path("//").lexically_normal().generic_path(), "//");	       // case 5
    PATH_TEST_EQ(path("/foo").lexically_normal().generic_path(), "/foo"); 	 // case 6
    PATH_TEST_EQ(path("/foo/").lexically_normal().generic_path(), "/foo/");	 // case 7
    PATH_TEST_EQ(path("/foo/.").lexically_normal().generic_path(), "/foo/"); // case 8 Nico 
    PATH_TEST_EQ(path("/foo/bar/..").lexically_normal().generic_path(), "/foo/"); // 9 Nico
    PATH_TEST_EQ(path("/foo/..").lexically_normal().generic_path(), "/");	   // case 10
    PATH_TEST_EQ(path("/.").lexically_normal().generic_path(), "/");		     // case 11
    PATH_TEST_EQ(path("/./").lexically_normal().generic_path(), "/");		     // case 12 Nico
    PATH_TEST_EQ(path("/./.").lexically_normal().generic_path(), "/");		   // case 13
    PATH_TEST_EQ(path("/././").lexically_normal().generic_path(), "/");	     // case 14 Nico
    PATH_TEST_EQ(path("/././.").lexically_normal().generic_path(), "/");	   // case 15
    PATH_TEST_EQ(path("./").lexically_normal().generic_path(), ".");		     // case 16 R2
    PATH_TEST_EQ(path("./.").lexically_normal().generic_path(), ".");		     // case 17
    PATH_TEST_EQ(path("././").lexically_normal().generic_path(), ".");		   // case 18 R2
    PATH_TEST_EQ(path("././.").lexically_normal().generic_path(), ".");		   // case 19
    PATH_TEST_EQ(path("./././").lexically_normal().generic_path(), ".");	   // case 20 R2
    PATH_TEST_EQ(path("./././.").lexically_normal().generic_path(), ".");	   // case 21
    PATH_TEST_EQ(path("foo/..").lexically_normal().generic_path(), ".");	   // case 22
    PATH_TEST_EQ(path("foo/../").lexically_normal().generic_path(), ".");	   // case 23 R2
    PATH_TEST_EQ(path("foo/../..").lexically_normal().generic_path(), ".."); // case 24
#ifdef BOOST_WINDOWS_API  
    PATH_TEST_EQ(path("C:bar/..").lexically_normal().generic_path(), "C:");  // case 25
    PATH_TEST_EQ(path("C:").lexically_normal().generic_path(), "C:");        // case 26
#endif
    PATH_TEST_EQ(path("//host/bar/..").lexically_normal().generic_path(), "//host/");//27
    PATH_TEST_EQ(path("//host").lexically_normal().generic_path(), "//host");      // 28
    PATH_TEST_EQ(path("foo/../foo/..").lexically_normal().generic_path(), ".");    // 29
    PATH_TEST_EQ(path("foo/../foo/../..").lexically_normal().generic_path(), "..");// 30
    PATH_TEST_EQ(path("../foo/../foo/..").lexically_normal().generic_path(), "..");// 31 R2 bullet 7
    PATH_TEST_EQ(path("../.f/../f").lexically_normal().generic_path(), "../f");    // 32
    PATH_TEST_EQ(path("../f/../.f").lexically_normal().generic_path(), "../.f");   // 33
    PATH_TEST_EQ(path(".././../.").lexically_normal().generic_path(), "../..");    // 34 R2 bullet 7
    PATH_TEST_EQ(path(".././.././").lexically_normal().generic_path(), "../..");   // 35 R2 bullet 7

                                                                                   // cases Nico suggested we concentrate on to resolve questions about invariants
    PATH_TEST_EQ(path(".").lexically_normal().generic_path(), ".");		       // 1
    PATH_TEST_EQ(path("./").lexically_normal().generic_path(), ".");		     // 2 R2
    PATH_TEST_EQ(path("/./").lexically_normal().generic_path(), "/");		     // 3 R2
    PATH_TEST_EQ(path("foo/").lexically_normal().generic_path(), "foo/");	   // 4
    PATH_TEST_EQ(path("foo/.").lexically_normal().generic_path(), "foo/");	 // 5 R2
    PATH_TEST_EQ(path("foo/..").lexically_normal().generic_path(), ".");	   // 6 
    PATH_TEST_EQ(path("foo/../").lexically_normal().generic_path(), ".");    // 7 R2
    PATH_TEST_EQ(path("foo/../.").lexically_normal().generic_path(), ".");   // 8
    PATH_TEST_EQ(path("/foo/..").lexically_normal().generic_path(), "/");	   // 9 
    PATH_TEST_EQ(path("/foo/../.").lexically_normal().generic_path(), "/");	 // 10
    PATH_TEST_EQ(path("/foo/bar/.").lexically_normal().generic_path(), "/foo/bar/"); // 11 R2
    PATH_TEST_EQ(path("/foo/bar/./").lexically_normal().generic_path(), "/foo/bar/");// 12
    PATH_TEST_EQ(path("/foo/bar/..").lexically_normal().generic_path(), "/foo/");	   // 13 R2
    PATH_TEST_EQ(path("/foo/bar/../").lexically_normal().generic_path(), "/foo/");	 // 14

                                                                                     // other tests
    PATH_TEST_EQ(path("///").lexically_normal().generic_path(), "/");
    PATH_TEST_EQ(path("f").lexically_normal().generic_path(), "f");
    PATH_TEST_EQ(path("foo").lexically_normal().generic_path(), "foo");
    PATH_TEST_EQ(path("foo/").lexically_normal().generic_path(), "foo/");
    PATH_TEST_EQ(path("f/").lexically_normal().generic_path(), "f/");
    PATH_TEST_EQ(path("/foo").lexically_normal().generic_path(), "/foo");
    PATH_TEST_EQ(path("foo/bar").lexically_normal().generic_path(), "foo/bar");
    PATH_TEST_EQ(path("../..").lexically_normal().generic_path(), "../..");
    PATH_TEST_EQ(path("/..").lexically_normal().generic_path(), "/");
    PATH_TEST_EQ(path("/../..").lexically_normal().generic_path(), "/");
    PATH_TEST_EQ(path("../foo").lexically_normal().generic_path(), "../foo");
    PATH_TEST_EQ(path("/foo/../").lexically_normal().generic_path(), "/");
    PATH_TEST_EQ(path("foo/../..").lexically_normal().generic_path(), "..");
    PATH_TEST_EQ((path("foo") / "..").lexically_normal().generic_path(), ".");
    PATH_TEST_EQ(path("foo/...").lexically_normal().generic_path(), "foo/...");
    PATH_TEST_EQ(path("foo/.../").lexically_normal().generic_path(), "foo/.../");
    PATH_TEST_EQ(path("foo/..bar").lexically_normal().generic_path(), "foo/..bar");
    PATH_TEST_EQ(path("../f").lexically_normal().generic_path(), "../f");
    PATH_TEST_EQ(path("/../f").lexically_normal().generic_path(), "/f");
    PATH_TEST_EQ(path("f/..").lexically_normal().generic_path(), ".");
    PATH_TEST_EQ((path("f") / "..").lexically_normal().generic_path(), ".");
    PATH_TEST_EQ(path("foo/../../").lexically_normal().generic_path(), "..");  // bullet 7
    PATH_TEST_EQ(path("foo/../../..").lexically_normal().generic_path(), "../..");
    PATH_TEST_EQ(path("foo/../../../").lexically_normal().generic_path(), "../..");  // bullet 7
    PATH_TEST_EQ(path("foo/../bar").lexically_normal().generic_path(), "bar");
    PATH_TEST_EQ(path("foo/../bar/").lexically_normal().generic_path(), "bar/");
    PATH_TEST_EQ(path("foo/bar/..").lexically_normal().generic_path(), "foo/");
    PATH_TEST_EQ(path("foo/./bar/..").lexically_normal().generic_path(), "foo/");
    std::cout << path("foo/./bar/..").lexically_normal() << std::endl;  // outputs "foo/"
    PATH_TEST_EQ(path("foo/bar/../").lexically_normal().generic_path(), "foo/");
    PATH_TEST_EQ(path("foo/./bar/../").lexically_normal().generic_path(), "foo/");
    std::cout << path("foo/./bar/../").lexically_normal() << std::endl;  // POSIX: "foo/", Windows: "foo\" 
    PATH_TEST_EQ(path("foo/bar/../..").lexically_normal().generic_path(), ".");
    PATH_TEST_EQ(path("foo/bar/../../").lexically_normal().generic_path(), ".");
    PATH_TEST_EQ(path("foo/bar/../blah").lexically_normal().generic_path(), "foo/blah");
    PATH_TEST_EQ(path("f/../b").lexically_normal().generic_path(), "b");
    PATH_TEST_EQ(path("f/b/..").lexically_normal().generic_path(), "f/");
    PATH_TEST_EQ(path("f/b/../").lexically_normal().generic_path(), "f/");
    PATH_TEST_EQ(path("f/b/../a").lexically_normal().generic_path(), "f/a");
    PATH_TEST_EQ(path("foo/bar/blah/../..").lexically_normal().generic_path(), "foo/");
    PATH_TEST_EQ(path("foo/bar/blah/../../bletch").lexically_normal().generic_path(), "foo/bletch");
    PATH_TEST_EQ(path("//net").lexically_normal().generic_path(), "//net");
    PATH_TEST_EQ(path("//net/").lexically_normal().generic_path(), "//net/");
    PATH_TEST_EQ(path("//..net").lexically_normal().generic_path(), "//..net");
    PATH_TEST_EQ(path("//net/..").lexically_normal().generic_path(), "//net/");
    PATH_TEST_EQ(path("//net/foo").lexically_normal().generic_path(), "//net/foo");
    PATH_TEST_EQ(path("//net/foo/").lexically_normal().generic_path(), "//net/foo/");
    PATH_TEST_EQ(path("//net/foo/..").lexically_normal().generic_path(), "//net/");
    PATH_TEST_EQ(path("//net/foo/../").lexically_normal().generic_path(), "//net/");

    PATH_TEST_EQ(path("/net/foo/bar").lexically_normal().generic_path(), "/net/foo/bar");
    PATH_TEST_EQ(path("/net/foo/bar/").lexically_normal().generic_path(), "/net/foo/bar/");
    PATH_TEST_EQ(path("/net/foo/..").lexically_normal().generic_path(), "/net/");
    PATH_TEST_EQ(path("/net/foo/../").lexically_normal().generic_path(), "/net/");

    PATH_TEST_EQ(path("//net//foo//bar").lexically_normal().generic_path(), "//net/foo/bar");
    PATH_TEST_EQ(path("//net//foo//bar//").lexically_normal().generic_path(), "//net/foo/bar/");
    PATH_TEST_EQ(path("//net//foo//..").lexically_normal().generic_path(), "//net/");
    PATH_TEST_EQ(path("//net//foo//..//").lexically_normal().generic_path(), "//net/");

    PATH_TEST_EQ(path("///net///foo///bar").lexically_normal().generic_path(), "/net/foo/bar");
    PATH_TEST_EQ(path("///net///foo///bar///").lexically_normal().generic_path(), "/net/foo/bar/");
    PATH_TEST_EQ(path("///net///foo///..").lexically_normal().generic_path(), "/net/");
    PATH_TEST_EQ(path("///net///foo///..///").lexically_normal().generic_path(), "/net/");

#ifdef BOOST_WINDOWS_API  
    PATH_TEST_EQ(path("c:..").lexically_normal().generic_path(), "c:..");
    PATH_TEST_EQ(path("c:foo/..").lexically_normal().generic_path(), "c:");

    PATH_TEST_EQ(path("c:foo/../").lexically_normal().generic_path(), "c:");

    PATH_TEST_EQ(path("c:/foo/..").lexically_normal().generic_path(), "c:/");
    PATH_TEST_EQ(path("c:/foo/../").lexically_normal().generic_path(), "c:/");
    PATH_TEST_EQ(path("c:/..").lexically_normal().generic_path(), "c:/");
    PATH_TEST_EQ(path("c:/../").lexically_normal().generic_path(), "c:/");
    PATH_TEST_EQ(path("c:/../..").lexically_normal().generic_path(), "c:/");
    PATH_TEST_EQ(path("c:/../../").lexically_normal().generic_path(), "c:/");
    PATH_TEST_EQ(path("c:/../foo").lexically_normal().generic_path(), "c:/foo");
    PATH_TEST_EQ(path("c:/../foo/").lexically_normal().generic_path(), "c:/foo/");
    PATH_TEST_EQ(path("c:/../../foo").lexically_normal().generic_path(), "c:/foo");
    PATH_TEST_EQ(path("c:/../../foo/").lexically_normal().generic_path(), "c:/foo/");
    PATH_TEST_EQ(path("c:/..foo").lexically_normal().generic_path(), "c:/..foo");
#else
    PATH_TEST_EQ(path("c:..").lexically_normal(), "c:..");
    PATH_TEST_EQ(path("c:foo/..").lexically_normal(), ".");
    PATH_TEST_EQ(path("c:foo/../").lexically_normal(), "./");
    PATH_TEST_EQ(path("c:/foo/..").lexically_normal(), "c:");
    PATH_TEST_EQ(path("c:/foo/../").lexically_normal(), "c:/");
    PATH_TEST_EQ(path("c:/..").lexically_normal(), ".");
    PATH_TEST_EQ(path("c:/../").lexically_normal(), "./");
    PATH_TEST_EQ(path("c:/../..").lexically_normal(), "..");
    PATH_TEST_EQ(path("c:/../../").lexically_normal(), "../");
    PATH_TEST_EQ(path("c:/../foo").lexically_normal(), "foo");
    PATH_TEST_EQ(path("c:/../foo/").lexically_normal(), "foo/");
    PATH_TEST_EQ(path("c:/../../foo").lexically_normal(), "../foo");
    PATH_TEST_EQ(path("c:/../../foo/").lexically_normal(), "../foo/");
    PATH_TEST_EQ(path("c:/..foo").lexically_normal(), "c:/..foo");
#endif
  }

  void lexically_proximate_tests()
  {
    cout << "lexically_proximate_test..." << endl;

    // paths unrelated
    PATH_TEST_EQ(path("a/b/c").lexically_proximate("x"), "a/b/c");
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

  lexically_relative_tests();
  lexically_normal_tests();
  lexically_proximate_tests();

  return ::boost::report_errors();
}
