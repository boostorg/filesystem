//  Exception implementation file  -------------------------------------------//

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

//  See http://www.boost.org/libs/filesystem for documentation.

//----------------------------------------------------------------------------//

#include <boost/config.hpp>
#include <boost/filesystem/exception.hpp>

#include <cstring> // SGI MIPSpro compilers need this
#include <cerrno>
#include <string>

# ifdef BOOST_NO_STDC_NAMESPACE
    namespace std { using ::strerror; }
# endif

// BOOST_POSIX or BOOST_WINDOWS specify which API to use, not the current
// operating system. GCC defaults to BOOST_POSIX; it doesn't predefine _WIN32.

# if !defined( BOOST_WINDOWS ) && !defined( BOOST_POSIX )
#   if defined(_WIN32)
#     define BOOST_WINDOWS
#   else
#     define BOOST_POSIX
#   endif
# endif

# if defined( BOOST_WINDOWS )
#   include "windows.h"
# endif

//----------------------------------------------------------------------------//

namespace boost
{
  namespace filesystem
  {

    filesystem_error::filesystem_error( std::string const& msg ):
      std::runtime_error("filesystem error"),
      m_msg(msg),
      m_err(0)
    {
    }

    filesystem_error::filesystem_error( std::string const& msg, error_type ):
      std::runtime_error("filesystem error"),
      m_msg(msg),
#     ifdef BOOST_WINDOWS
        m_err( ::GetLastError() )
#     else
        m_err(errno) // GCC 3.1 won't accept ::errno
#     endif
    {
    }

    filesystem_error::~filesystem_error() throw()
    {
    }

    char const* filesystem_error::what() const throw()
    {
      if (m_err)
      {
        m_msg += ": ";
#       ifdef BOOST_WINDOWS
          LPVOID lpMsgBuf;
          ::FormatMessageA( 
              FORMAT_MESSAGE_ALLOCATE_BUFFER | 
              FORMAT_MESSAGE_FROM_SYSTEM | 
              FORMAT_MESSAGE_IGNORE_INSERTS,
              NULL,
              m_err,
              MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
              (LPSTR) &lpMsgBuf,
              0,
              NULL 
          );
          m_msg += static_cast<LPCSTR>(lpMsgBuf);
          LocalFree( lpMsgBuf ); // free the buffer
#       else
          m_msg += std::strerror(m_err);
#       endif
        m_err = 0;
      }
      return m_msg.c_str();
    }
  } // namespace filesystem
} // namespace boost
 
