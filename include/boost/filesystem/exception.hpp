//  boost/filesystem/exception.hpp  ------------------------------------------//

// < ----------------------------------------------------------------------- > 
// <   Copyright © 2001 Dietmar Kühl, All Rights Reserved                    > 
// <                                                                         > 
// <   Permission to use, copy, modify, distribute and sell this             > 
// <   software for any purpose is hereby granted without fee, provided      > 
// <   that the above copyright notice appears in all copies and that        > 
// <   both that copyright notice and this permission notice appear in       > 
// <   supporting documentation. Dietmar Kühl makes no representations about > 
// <   the suitability of this software for any purpose. It is provided      > 
// <   "as is" without express or implied warranty.                          > 
// < ----------------------------------------------------------------------- > 

// Original author: Dietmar Kühl. Revised by Beman Dawes.

//  See http://www.boost.org for most recent version including documentation.

//----------------------------------------------------------------------------// 

#ifndef BOOST_FILESYSTEM_EXCEPTION_HPP
#define BOOST_FILESYSTEM_EXCEPTION_HPP

#include <string>
#include <stdexcept>

//----------------------------------------------------------------------------// 

namespace boost
{
  namespace filesystem
  {
    class filesystem_error:
      public std::runtime_error
    {
    public:

      explicit filesystem_error(int err, std::string const& msg = "");
      // Effects: : std::runtime_error("..."),
      // m_msg(msg), m_err(err)

      explicit filesystem_error(std::string const& msg= "");
      // Effects: filesystem_error( value, msg ), where value is appropriate
      // for the operating system (for example, GetLastError() on Windows,
      // errno on POSIX)

      ~filesystem_error() throw();
      char const* what() const throw();
      int         get_error() const { return m_err; }

    private:
      mutable std::string m_msg;
      mutable int         m_err;
    };

  } // namespace filesystem
} // namespace boost

#endif // BOOST_FILESYSTEM_EXCEPTION_HPP
