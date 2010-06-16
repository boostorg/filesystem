//  Copyright Beman Dawes 2010

//  Distributed under the Boost Software License, Version 1.0.
//  See http://www.boost.org/LICENSE_1_0.txt

//--------------------------------------------------------------------------------------// 

#ifndef BOOST_DELIMIT_STRING
#define BOOST_DELIMIT_STRING

#include <istream>
#include <ostream>
#include <string>
#include <sstream>
#include <iomanip>

namespace boost
{
  namespace string
  {
    namespace detail
    {
      //  ostream operator<< takes a const_proxy& argument, and thus works for arguments
      //  of both type const_proxy and type proxy. istream operator>> takes a proxy&
      //  argument, and thus works for arguments of type proxy but not type const_proxy.
      //  That's what ensures const strings can't be inadvertently written into, not
      //  whether or not const_proxy::s is const.
      template <class String>
      struct const_proxy
      {
        typedef typename String::value_type value_type;
        String& s;      // must be non-const
        value_type escape;
        value_type delim;
        const_proxy(std::string& s_, value_type escape_, value_type delim_)
          : s(s_), escape(escape_), delim(delim_) {}
      };

      template <class String>
      struct proxy : public const_proxy<String>
      {
        proxy(String& s_, value_type escape_, value_type delim_)
          : const_proxy(s_, escape_, delim_ ) {}
      };
    }

    template <class String>
    inline detail::const_proxy<String> delimit_string(const String& s,
      typename String::value_type escape='\\',
      typename String::value_type delim='\"')
    {
      // const safety is provided by the detail::proxy - detail::const_proxy relationship,
      // so we are not giving up type safety by casting away const.
      return detail::const_proxy<String>(const_cast<std::string&>(s), escape, delim);
    }

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

    template <class String>
    inline detail::proxy<String> delimit_string(String& s,
      typename String::value_type escape='\\',
      typename String::value_type delim='\"')
    {
      return detail::proxy<String>(s, escape, delim);
    }

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
      for (;;)
      {
      is >> std::noskipws >> c;  //************ save and restore?
        if (c == prox.escape)
          is >> c;
        else if (c == prox.delim)
          break;
        prox.s += c;
      }
      return is;
    }
  } // namespace string
} // namespace boost

#endif // BOOST_DELIMIT_STRING
