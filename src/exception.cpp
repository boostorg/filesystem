//  Exception implementation file  -------------------------------------------//

//  Copyright © 2002 Beman Dawes
//  Copyright © 2001 Dietmar Kühl 
//  Use, modification, and distribution is subject to the Boost Software
//  License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy
//  at http://www.boost.org/LICENSE_1_0.txt)

//  See library home page at http://www.boost.org/libs/filesystem

//----------------------------------------------------------------------------//

// define BOOST_FILESYSTEM_SOURCE so that <boost/filesystem/config.hpp> knows
// the library is being built (possibly exporting rather than importing code)
#define BOOST_FILESYSTEM_SOURCE 

#include <boost/filesystem/config.hpp>
#include <boost/filesystem/path.hpp>

namespace fs = boost::filesystem;

#include <cstring> // SGI MIPSpro compilers need this
#include <string>

# ifdef BOOST_NO_STDC_NAMESPACE
    namespace std { using ::strerror; }
# endif

# if defined( BOOST_WINDOWS_API )
#   include "windows.h"
# else
#   include <errno.h> // for POSIX error codes
# endif

#include <boost/config/abi_prefix.hpp> // must be the last header

//----------------------------------------------------------------------------//

namespace
{
  struct ec_xlate { int sys_ec; fs::error_code ec; };
  const ec_xlate ec_table[] =
  {
# ifdef BOOST_WINDOWS_API
    { 0, fs::other_error },
    { ERROR_ACCESS_DENIED, fs::security_error },
    { ERROR_INVALID_ACCESS, fs::security_error },
    { ERROR_SHARING_VIOLATION, fs::security_error },
    { ERROR_LOCK_VIOLATION, fs::security_error },
    { ERROR_LOCKED, fs::security_error },
    { ERROR_NOACCESS, fs::security_error },
    { ERROR_WRITE_PROTECT, fs::read_only_error },
    { ERROR_NOT_READY, fs::not_ready_error },
    { ERROR_SEEK, fs::io_error },
    { ERROR_READ_FAULT, fs::io_error },
    { ERROR_WRITE_FAULT, fs::io_error },
    { ERROR_CANTOPEN, fs::io_error },
    { ERROR_CANTREAD, fs::io_error },
    { ERROR_CANTWRITE, fs::io_error },
    { ERROR_DIRECTORY, fs::path_error },
    { ERROR_INVALID_NAME, fs::path_error },
    { ERROR_FILE_NOT_FOUND, fs::not_found_error },
    { ERROR_PATH_NOT_FOUND, fs::not_found_error },
    { ERROR_DEV_NOT_EXIST, fs::not_found_error },
    { ERROR_DEVICE_IN_USE, fs::busy_error },
    { ERROR_OPEN_FILES, fs::busy_error },
    { ERROR_BUSY_DRIVE, fs::busy_error },
    { ERROR_BUSY, fs::busy_error },
    { ERROR_FILE_EXISTS, fs::already_exists_error },
    { ERROR_ALREADY_EXISTS, fs::already_exists_error },
    { ERROR_DIR_NOT_EMPTY, fs::not_empty_error },
    { ERROR_HANDLE_DISK_FULL, fs::out_of_space_error },
    { ERROR_DISK_FULL, fs::out_of_space_error },
    { ERROR_OUTOFMEMORY, fs::out_of_memory_error },
    { ERROR_NOT_ENOUGH_MEMORY, fs::out_of_memory_error },
    { ERROR_TOO_MANY_OPEN_FILES, fs::out_of_resource_error }
# else
    { 0, fs::other_error },
    { EACCES, fs::security_error },
    { EROFS, fs::read_only_error },
    { EIO, fs::io_error },
    { EINVAL, fs::path_error },
    { ENAMETOOLONG, fs::path_error },
    { ENOENT, fs::not_found_error },
    { ENOTDIR, fs::not_directory_error },
    { EAGAIN, fs::busy_error },
    { EBUSY, fs::busy_error },
    { ETXTBSY, fs::busy_error },
    { EEXIST, fs::already_exists_error },
    { ENOTEMPTY, fs::not_empty_error },
    { EISDIR, fs::is_directory_error },
    { ENOSPC, fs::out_of_space_error },
    { ENOMEM, fs::out_of_memory_error },
    { EMFILE, fs::out_of_resource_error }
# endif
  };

} // unnamed namespace

namespace boost
{
  namespace filesystem
  {
    BOOST_FILESYSTEM_DECL error_code
      lookup_error_code( system_error_type sys_err_code )
    {
      for ( const ec_xlate * cur = &ec_table[0];
        cur != ec_table
          + sizeof(ec_table)/sizeof(ec_xlate); ++cur )
      {
        if ( sys_err_code == cur->sys_ec ) return cur->ec;
      }
      return system_error; // general system error code
    }

# ifdef BOOST_WINDOWS_API
    BOOST_FILESYSTEM_DECL void
    system_message( system_error_type sys_err_code, std::string & target )
    {
      LPVOID lpMsgBuf;
      ::FormatMessageA( 
          FORMAT_MESSAGE_ALLOCATE_BUFFER | 
          FORMAT_MESSAGE_FROM_SYSTEM | 
          FORMAT_MESSAGE_IGNORE_INSERTS,
          NULL,
          sys_err_code,
          MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
          (LPSTR) &lpMsgBuf,
          0,
          NULL 
      );
      target += static_cast<LPCSTR>(lpMsgBuf);
      ::LocalFree( lpMsgBuf ); // free the buffer
      while ( target.size()
        && (target[target.size()-1] == '\n' || target[target.size()-1] == '\r') )
          target.erase( target.size()-1 );
    }
# else
    void
    system_message( system_error_type sys_err_code, std::string & target )
    {
      target += std::strerror( sys_err_code );
    }
# endif

    namespace detail
    {
      BOOST_FILESYSTEM_DECL void
      what_formatter( system_error_type sys_err_code,
        const std::string & p1, const std::string & p2, std::string & target )
      {
        if ( !p1.empty() )
        {
          target += ": \"";
          target += p1;
          target += "\"";
        }
        if ( !p2.empty() )
        {
          target += ", \"";
          target += p2;
          target += "\"";
        }
        if ( sys_err_code )
        {
          target += " ";
          fs::system_message( sys_err_code, target );
        }
      }

#   ifdef BOOST_WINDOWS_API
      BOOST_FILESYSTEM_DECL void
      what_formatter( system_error_type sys_err_code,
        const std::wstring &, const std::wstring &, std::string & target )
      {
        if ( sys_err_code )
        {
          target += ": ";
          fs::system_message( sys_err_code, target );
        }
      }

#   endif
    } // namespace detail

  } // namespace filesystem
} // namespace boost
 
#include <boost/config/abi_suffix.hpp>
