//  boost/filesystem/exception.hpp  ------------------------------------------//

// < ----------------------------------------------------------------------- > 
// <   Copyright © 2002 Beman Dawes                                          > 
// <   Copyright © 2001 Dietmar Kühl, All Rights Reserved                    > 
// <                                                                         > 
// <   Permission to use, copy, modify, distribute and sell this             > 
// <   software for any purpose is hereby granted without fee, provided      > 
// <   that the above copyright notice appears in all copies and that        > 
// <   both that copyright notice and this permission notice appear in       > 
// <   supporting documentation. The authors make no representations about   > 
// <   the suitability of this software for any purpose. It is provided      > 
// <   "as is" without express or implied warranty.                          > 
// < ----------------------------------------------------------------------- > 

//  See http://www.boost.org/libs/filesystem for documentation.

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
    enum error_code
    {
      no_error = 0,
      system_error,     // system generated error; if possible, is translated
                        // to one of the more specific errors below.
      other_error,      // library generated error
      security_error,   // includes access rights, permissions failures
      read_only_error,
      io_error,
      path_error,       // path format or a name in the path is invalid
      not_found_error,
      not_directory_error,
      busy_error,       // implies trying again might succeed
      already_exists_error,
      not_empty_error,
      is_directory_error,
      out_of_space_error,
      out_of_memory_error,
      out_of_resource_error
    };


    class filesystem_error : public std::runtime_error
    {
    public:

      filesystem_error( const std::string & msg, error_code ec );
      // if ec==system_error, unspecified processing of msg and ec will occur

      ~filesystem_error() throw();

      int         native_error() const { return m_sys_err; }
      // Note: a value of 0 implies a library (rather than system) error
      error_code  error() const { return m_err; }
      
     private:
      int                 m_sys_err;
      error_code          m_err;
    };

  } // namespace filesystem
} // namespace boost

#endif // BOOST_FILESYSTEM_EXCEPTION_HPP
