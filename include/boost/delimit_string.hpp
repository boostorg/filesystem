//  Copyright Beman Dawes 2010

//  Distributed under the Boost Software License, Version 1.0.
//  See http://www.boost.org/LICENSE_1_0.txt

//--------------------------------------------------------------------------------------// 

#ifndef BOOST_DELIMIT_STRING
#define BOOST_DELIMIT_STRING

#include <istream>
#include <ostream>
#include <string>
#include <boost/io/ios_state.hpp>

namespace boost
{
  namespace detail
  {
    //  inserter support

    template <class String>
    struct const_proxy
    {
      typedef typename String::value_type value_type;
      const String& s;
      value_type escape;
      value_type delim;
      const_proxy(const String& s_, value_type escape_, value_type delim_)
        : s(s_), escape(escape_), delim(delim_) {}
    };

    template <class String>
    std::basic_ostream<typename String::value_type>&
      operator<<(std::basic_ostream<typename String::value_type>& os,
      const detail::const_proxy<String>& prox)
    {
      os << prox.delim;
      typename String::const_iterator end_it = prox.s.end();
      for (typename String::const_iterator it = prox.s.begin();
        it != end_it;
        ++it )
      {
        if (*it == prox.delim || *it == prox.escape)
          os << prox.escape;
        os << *it;
      }
      os << prox.delim;
      return os;
    }

    template <class Char>
    struct c_str_proxy
    {
      const Char* s;
      Char escape;
      Char delim;
      c_str_proxy(const Char* s_, Char escape_, Char delim_)
        : s(s_), escape(escape_), delim(delim_) {}
    };

    template <class Char>
    std::basic_ostream<Char>&
      operator<<(std::basic_ostream<Char>& os, const detail::c_str_proxy<Char>& prox)
    {
      os << prox.delim;
      for (const Char* it = prox.s;
        *it;
        ++it )
      {
        if (*it == prox.delim || *it == prox.escape)
          os << prox.escape;
        os << *it;
      }
      os << prox.delim;
      return os;
    }

    //  extractor support

    template <class String>
    struct proxy
    {
      typedef typename String::value_type value_type;
      String& s;
      value_type escape;
      value_type delim;
      proxy(String& s_, value_type escape_, value_type delim_)
        : s(s_), escape(escape_), delim(delim_) {}
    };

    template <class String>
    std::basic_istream<typename String::value_type>&
    operator>>(std::basic_istream<typename String::value_type>& is,
      const detail::proxy<String>& prox)
    {
      typename String::value_type c;
      is >> c;
      if (c != prox.delim)
      {
        prox.s = c;
        is >> prox.s;
        return is;
      }
      prox.s.clear();
      {
        boost::io::ios_flags_saver ifs(is);
        is >> std::noskipws;
        for (;;)
        {
          is >> c;
          if (c == prox.escape)
            is >> c;
          else if (c == prox.delim)
            break;
          prox.s += c;
        }
      }
      return is;
    }

  }  // namespace detail

  template <class String>
  inline detail::const_proxy<String> delimit(const String& s,
    typename String::value_type escape='\\',
    typename String::value_type delim='\"')
  {
    return detail::const_proxy<String>(s, escape, delim);
  }

  template <class Char>
  inline detail::c_str_proxy<Char> delimit(const Char* s,
    Char escape='\\',
    Char delim='\"')
  {
    return detail::c_str_proxy<Char>(s, escape, delim);
  }

  template <class String>
  inline detail::proxy<String> undelimit(String& s,
    typename String::value_type escape='\\',
    typename String::value_type delim='\"')
  {
    return detail::proxy<String>(s, escape, delim);
  }
} // namespace boost

#endif // BOOST_DELIMIT_STRING
