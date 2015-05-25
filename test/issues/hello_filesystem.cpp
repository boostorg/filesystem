//  Boost hello_filesystem.cpp  --------------------------------------------------------//

//  Copyright Beman Dawes 2014

//  Distributed under the Boost Software License, Version 1.0.
//  See http://www.boost.org/LICENSE_1_0.txt

//  Library home page: http://www.boost.org/libs/filesystem

//--------------------------------------------------------------------------------------//
//                                                                                      //
//  In researching filesystem issues it is convenient to have a program that can be     //
//  quickly modified to test reported problems. That's the purpose of this file and     //
//  its associated Visual Studio and Boost.Build infrastructure.                        //
//                                                                                      //
//--------------------------------------------------------------------------------------//

#include <boost/config/warning_disable.hpp>
#include <iostream>
#include <boost/filesystem.hpp>
#include <boost/filesystem/config_info.hpp>
#include <boost/detail/lightweight_test.hpp>
#include <boost/detail/lightweight_main.hpp>

#include <boost/utility/string_ref.hpp>

using std::cout;
using std::endl;
namespace fs = boost::filesystem;
using boost::basic_string_ref;
using boost::string_ref;
using boost::wstring_ref;

void f1(const string_ref&)
{
  cout << "narrow" << endl;
}
void f1(const wstring_ref&)
{
  cout << "wide" << endl;
}

template <class T>
void foo(const T& from)
{
  f1(basic_string_ref<typename T::value_type>(from));
}

//------------------------------------  cpp_main  --------------------------------------//

int cpp_main(int argc, char* argv[])
{
  foo(std::string("string"));
  //foo<char>(std::string("string"));
  //foo<char, std::char_traits<char>>("string");


  cout << "Hello, filesystem world" << endl;
  cout << fs::config() << endl;

  //const wchar_t* wchar_t_p = L".";
  //const char* char_p = ".";

  //BOOST_TEST(fs::exists(wchar_t_p));
  //cout << "test 1 complete" << endl;
  //BOOST_TEST(fs::exists(L"."));
  //cout << "test 2 complete" << endl;
  //BOOST_TEST(fs::exists(char_p));
  //cout << "test 3 complete" << endl;
  //BOOST_TEST(fs::exists("."));
  //cout << "test 4 complete" << endl;

  return ::boost::report_errors();
} // cpp_main
