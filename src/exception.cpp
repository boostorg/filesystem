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
# else
#   include <errno.h> // for POSIX error codes
# endif

//----------------------------------------------------------------------------//

namespace boost
{
  namespace filesystem
  {
    namespace detail
    {
      std::string prep_msg( const std::string & msg, error_code ec )
      {
        std::string str( "File system error: " );
        str += msg;
        if ( ec == system_error )
        {
          str += ": ";
#         ifdef BOOST_WINDOWS
            LPVOID lpMsgBuf;
            ::FormatMessageA( 
                FORMAT_MESSAGE_ALLOCATE_BUFFER | 
                FORMAT_MESSAGE_FROM_SYSTEM | 
                FORMAT_MESSAGE_IGNORE_INSERTS,
                NULL,
                ::GetLastError(),
                MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
                (LPSTR) &lpMsgBuf,
                0,
                NULL 
            );
            str += static_cast<LPCSTR>(lpMsgBuf);
            ::LocalFree( lpMsgBuf ); // free the buffer
#         else
            str += std::strerror( errno );
#         endif
        }
        return str;
      }

      struct ec_xlate { int sys_ec; error_code ec; };
      const ec_xlate ec_table[] =
      {
#     ifdef BOOST_WINDOWS
        { ERROR_ACCESS_DENIED, security_error },
        { ERROR_INVALID_ACCESS, security_error },
        { ERROR_SHARING_VIOLATION, security_error },
        { ERROR_LOCK_VIOLATION, security_error },
        { ERROR_LOCKED, security_error },
        { ERROR_NOACCESS, security_error },
        { ERROR_WRITE_PROTECT, read_only_error },
        { ERROR_NOT_READY, io_error },
        { ERROR_SEEK, io_error },
        { ERROR_READ_FAULT, io_error },
        { ERROR_WRITE_FAULT, io_error },
        { ERROR_CANTOPEN, io_error },
        { ERROR_CANTREAD, io_error },
        { ERROR_CANTWRITE, io_error },
        { ERROR_DIRECTORY, path_error },
        { ERROR_INVALID_NAME, path_error },
        { ERROR_FILE_NOT_FOUND, not_found_error },
        { ERROR_PATH_NOT_FOUND, not_found_error },
        { ERROR_DEV_NOT_EXIST, not_found_error },
        { ERROR_DEVICE_IN_USE, busy_error },
        { ERROR_OPEN_FILES, busy_error },
        { ERROR_BUSY_DRIVE, busy_error },
        { ERROR_BUSY, busy_error },
        { ERROR_FILE_EXISTS, already_exists_error },
        { ERROR_ALREADY_EXISTS, already_exists_error },
        { ERROR_DIR_NOT_EMPTY, not_empty_error },
        { ERROR_HANDLE_DISK_FULL, out_of_space_error },
        { ERROR_DISK_FULL, out_of_space_error },
        { ERROR_OUTOFMEMORY, out_of_memory_error },
        { ERROR_NOT_ENOUGH_MEMORY, out_of_memory_error },
        { ERROR_TOO_MANY_OPEN_FILES, out_of_resource_error }
#     else
        { EACCES, security_error },
        { EROFS, read_only_error },
        { EIO, io_error },
        { ENAMETOOLONG, path_error },
        { ENOENT, not_found_error },
        { ENOTDIR, not_directory_error },
        { EAGAIN, busy_error },
        { EBUSY, busy_error },
        { ETXTBSY, busy_error },
        { EEXIST, already_exists_error },
        { ENOTEMPTY, not_empty_error },
        { EISDIR, is_directory_error },
        { ENOSPC, out_of_space_error },
        { ENOMEM, out_of_memory_error },
        { EMFILE, out_of_resource_error }
#     endif
      };
    }

    filesystem_error::filesystem_error( std::string const& msg, error_code ec )
      : std::runtime_error( detail::prep_msg( msg, ec ).c_str() ),
        m_sys_err(0), m_err(ec)
    {
      if ( ec == system_error )
      {
#     ifdef BOOST_WINDOWS
        m_sys_err = ::GetLastError();
#     else
        m_sys_err = errno; // GCC 3.1 won't accept ::errno
#     endif

        const detail::ec_xlate * cur = &detail::ec_table[0];
        for ( ; cur != detail::ec_table
              + sizeof(detail::ec_table)/sizeof(detail::ec_xlate)
            && m_sys_err != cur->sys_ec;
            ++cur ) {}
        if ( cur != detail::ec_table
          + sizeof(detail::ec_table)/sizeof(detail::ec_xlate) )
          { m_err = cur->ec; }
      }
    }

    filesystem_error::~filesystem_error() throw()
    {
    }

  } // namespace filesystem
} // namespace boost
 
