//  filesystem config_info.hpp  --------------------------------------------------------//

//  Copyright Beman Dawes 2014

//  Distributed under the Boost Software License, Version 1.0.
//  See http://www.boost.org/LICENSE_1_0.txt

//  Library home page: http://www.boost.org/libs/filesystem

//  BOOST_FILESYSTEM_SHOW_MACRO and show_macro are based on similar components in
//  Boost.Config's config_info.cpp.

//--------------------------------------------------------------------------------------// 

#ifndef BOOST_FILESYSTEM_CONFIG_INFO_HPP
#define BOOST_FILESYSTEM_CONFIG_INFO_HPP

#include <boost/filesystem/config.hpp>
#include <string>
#include <cstring>
#include <ostream>

#define BOOST_FILESYSTEM_SHOW_MACRO(X)\
 boost::filesystem::detail::show_macro(#X, BOOST_STRINGIZE(=X))

namespace boost
{
  namespace filesystem
  {
    namespace detail
    {
      inline
      std::string show_macro(const char* name, const char* value)
      {
        std::string show;

        show += name;

        // if name == value+1 then then macro is not defined
        if (0 != strcmp(name, value+1))
        {
          if (value[1]) // macro has a value?
          {
            show += " is defined as ";
            show += value;
          }
          else // macro is defined but has no value:
          {
            show += " is defined";
          }
        }
        else
        {
          show += " is not defined";
        }
        return show;
      }

    }  // namespace detail

    struct config {};

    template <class charT, class traits>
    std::basic_ostream<charT, traits>& operator<<(std::basic_ostream<charT, traits>& os, config)
    {
      os
        << BOOST_FILESYSTEM_SHOW_MACRO(BOOST_POSIX_API) << '\n'
        << BOOST_FILESYSTEM_SHOW_MACRO(BOOST_WINDOWS_API) << '\n'
        << BOOST_FILESYSTEM_SHOW_MACRO(BOOST_MSVC) << '\n'
        << BOOST_FILESYSTEM_SHOW_MACRO(BOOST_FILESYSTEM_VERSION_REQUEST) << '\n'
        << BOOST_FILESYSTEM_SHOW_MACRO(BOOST_FILESYSTEM_DETAIL_V3) << '\n'
        << BOOST_FILESYSTEM_SHOW_MACRO(BOOST_FILESYSTEM_VERSION) << '\n'
        << BOOST_FILESYSTEM_SHOW_MACRO(BOOST_FILESYSTEM_STATIC_LINK) << '\n' 
        << BOOST_FILESYSTEM_SHOW_MACRO(BOOST_FILESYSTEM_DYN_LINK) << '\n'
        << BOOST_FILESYSTEM_SHOW_MACRO(BOOST_FILESYSTEM_NO_DEPRECATED) << '\n'
        << BOOST_FILESYSTEM_SHOW_MACRO(BOOST_FILESYSTEM_DECL) << '\n'
        << BOOST_FILESYSTEM_SHOW_MACRO(BOOST_SYMBOL_VISIBLE) << '\n'
        ;
      return os;
    }
  }  // namespace boost
}  // namespace filesystem

#endif  // BOOST_FILESYSTEM_CONFIG_INFO_HPP
