//  locale_info.cpp  ---------------------------------------------------------//

//  Copyright Beman Dawes 2011

//  Distributed under the Boost Software License, Version 1.0.
//  See http://www.boost.org/LICENSE_1_0.txt

#include <locale>
#include <iostream>
#include <stdexcept>
#include <cstdlib>
#include <boost/config.hpp>

#ifndef BOOST_NO_CXX11_HDR_CODECVT
# include <codecvt>
#endif

using namespace std;

#ifdef _MSC_VER
#  pragma warning(push)
#  pragma warning(disable: 4996)  // ... Function call with parameters that may be unsafe
#endif

namespace
{
  void facet_info(const locale& loc, const char* msg)
  {
    cout << "has_facet<std::codecvt<char, char, std::mbstate_t> >("
      << msg << ") is "
      << (has_facet<std::codecvt<char, char, std::mbstate_t> >(loc)
          ? "true\n"
          : "false\n");
    cout << "has_facet<std::codecvt<wchar_t, char, std::mbstate_t> >("
      << msg << ") is "
      << (has_facet<std::codecvt<wchar_t, char, std::mbstate_t> >(loc)
          ? "true\n"
          : "false\n");
#ifndef BOOST_NO_CXX11_HDR_CODECVT
    cout << "has_facet<std::codecvt<char16_t, char, std::mbstate_t> >("
      << msg << ") is "
      << (has_facet<std::codecvt<char16_t, char, std::mbstate_t> >(loc)
          ? "true\n"
          : "false\n");
#endif
#ifndef BOOST_NO_CXX11_HDR_CODECVT
    cout << "has_facet<std::codecvt<char32_t, char, std::mbstate_t> >("
      << msg << ") is "
      << (has_facet<std::codecvt<char32_t, char, std::mbstate_t> >(loc)
          ? "true\n"
          : "false\n");
#endif
  }

  void default_info()
  {
    try
    {
      locale loc;
      cout << "\nlocale default construction OK, name is " << loc.name() << endl;
      facet_info(loc, "locale()");
    }
    catch (const exception& ex)
    {
      cout << "\nlocale default construction threw: " << ex.what() << endl;
    }
  }

  void null_string_info()
  {
    try
    {
      locale loc("");
      cout << "\nlocale(\"\") construction OK, name is " << loc.name() << endl;
      facet_info(loc, "locale(\"\")");
    }
    catch (const exception& ex)
    {
      cout << "\nlocale(\"\") construction threw: " << ex.what() << endl;
    }
  }

  void classic_info()
  {
    try
    {
      locale loc(locale::classic());
      cout << "\nlocale(locale::classic()) copy construction OK, name is " << loc.name() << endl;
      facet_info(loc, "locale::classic()");
    }
    catch (const exception& ex)
    {
      cout << "\nlocale(locale::clasic()) copy construction threw: " << ex.what() << endl;
    }
  }

  void codecvt_info(const locale& loc)
  {
    cout << "\ncodecvt conversion for locale " << loc.name() << endl;

    char s[128];
    wchar_t ws[128];

    for (int i = 0; i < 128; ++i)
    {
      s[i] = char(128 + i);
      ws[i] = L'\0';
    }

    mbstate_t state = mbstate_t(); // VC++ needs initialization
    const char* from_next;
    wchar_t* to_next;
    codecvt<wchar_t, char, mbstate_t>::result result;

    try
    {
      result = use_facet<codecvt<wchar_t, char, mbstate_t> >(loc).in(state, s, s+128, from_next, ws, ws+128, to_next);
    }
    catch (const runtime_error& x)
    {
      cout << "exception: " << x.what() << endl;
      return;
    }

    if (result != codecvt<wchar_t, char, mbstate_t>::ok)
    {
      cout << "Oops! conversion returned " << result << endl;
      return;
    }

    cout << hex;
    for (int i = 0; i < 128; ++i)
    {
      cout << s[i] << ':' << (unsigned short)(ws[i]);
      if (i % 8)
        cout << ',';
      else
        cout << endl;
    }
    cout << endl;
  }
}

int main()
{
  const char* lang = getenv("LANG");
  cout << "\nLANG environmental variable is "
    << (lang ? lang : "not present") << endl;

#ifndef BOOST_NO_CXX11_HDR_CODECVT
  cout << "BOOST_NO_CXX11_HDR_CODECVT is not defined" << endl;
#else
  cout << "BOOST_NO_CXX11_HDR_CODECVT is defined" << endl;
#endif

#ifndef BOOST_NO_CXX11_CHAR16_T
  cout << "BOOST_NO_CXX11_CHAR16_T is not defined" << endl;
#else
  cout << "BOOST_NO_CXX11_CHAR16_T is defined" << endl;
#endif

#ifndef BOOST_NO_CXX11_CHAR32_T
  cout << "BOOST_NO_CXX11_CHAR32_T is not defined" << endl;
#else
  cout << "BOOST_NO_CXX11_CHAR32_T is defined" << endl;
#endif

  default_info();
  null_string_info();
  classic_info();

  codecvt_info(locale());
  codecvt_info(locale(""));

  return 0;
}

#ifdef _MSC_VER
#  pragma warning(pop)
#endif
