//  directory_posix_windows.cpp  ---------------------------------------------//

// < ----------------------------------------------------------------------- > 
// <   Copyright © 2002 Beman Dawes.                                         >
// <   Copyright © 2001 Dietmar Kühl, All Rights Reserved                    >
// <                                                                         > 
// <   Permission to use, copy, modify, distribute and sell this             > 
// <   software for any purpose is hereby granted without fee, provided      > 
// <   that the above copyright notice appears in all copies and that        > 
// <   both that copyright notice and this permission notice appear in       > 
// <   supporting documentation. The authors make no representations about   > 
// <   the suitability of this software for any purpose. It is provided      > 
// <   "as is" without expressed or implied warranty.                        > 
// < ----------------------------------------------------------------------- > 

//  See http://www.boost.org for most recent version including documentation.

//----------------------------------------------------------------------------//


//  The point of this implementation is to prove the interface.  There is no
//  claim the implementation is efficient, follows good coding practice, etc.


//----------------------------------------------------------------------------// 

#include <boost/config.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/exception.hpp>
//#include <iostream>

namespace fs = boost::filesystem;

// BOOST_POSIX or BOOST_WINDOWS specify which API to use, not the current
// operating system. GCC defaults to BOOST_POSIX; it doesn't predefine _WIN32.

# if defined(BOOST_WINDOWS) || (!defined(BOOST_POSIX) && defined(_WIN32))
#   include "windows.h"
# else
#   define BOOST_POSIX
#   include "sys/stat.h"
#   include "dirent.h"
#   include "unistd.h"
#   include "fcntl.h"
# endif

#include <string>
#include <cstdio>
#include <cerrno>
#include <cassert>

//  helpers  -----------------------------------------------------------------//

namespace
{
#ifdef BOOST_POSIX

# define BOOST_HANDLE DIR *
# define BOOST_INVALID_HANDLE_VALUE 0
# define BOOST_SYSTEM_DIRECTORY_TYPE struct dirent *

  inline const char *  find_first_file( const char * dir,
    BOOST_HANDLE & handle, BOOST_SYSTEM_DIRECTORY_TYPE & )
  // Returns: 0 if error, otherwise name
  {
    const char * dummy_first_name = ".";
    return ( (handle = ::opendir( dir ))
      == BOOST_INVALID_HANDLE_VALUE ) ? 0 : dummy_first_name;
  }  

  inline void find_close( BOOST_HANDLE handle )
  {
    assert( handle != BOOST_INVALID_HANDLE_VALUE );
    ::closedir( handle );
  }

  inline const char * find_next_file(
    BOOST_HANDLE handle, BOOST_SYSTEM_DIRECTORY_TYPE & )
  // Returns: if EOF 0, otherwise name
  // Throws: if system reports error
  {

//  TODO: use readdir_r() if available, so code is multi-thread safe.
//  Fly-in-ointment: not all platforms support readdir_r().

    struct dirent * dp;
    errno = 0;
    if ( (dp = ::readdir( handle )) == 0 )
    {
      if ( errno != 0 )
      {
        throw fs::filesystem_error( "directory_iterator ++() failure" );
      }
      else { return 0; } // end reached
    }
    return dp->d_name;
  }
#else // BOOST_WINDOWS

# define BOOST_HANDLE HANDLE
# define BOOST_INVALID_HANDLE_VALUE INVALID_HANDLE_VALUE
# define BOOST_SYSTEM_DIRECTORY_TYPE WIN32_FIND_DATA

  inline const char *  find_first_file( const char * dir,
    BOOST_HANDLE & handle, BOOST_SYSTEM_DIRECTORY_TYPE & data )
  // Returns: 0 if error, otherwise name
  {
    std::string dirpath( std::string(dir) + "/*" );
    return ( (handle = ::FindFirstFile( dirpath.c_str(), &data ))
      == BOOST_INVALID_HANDLE_VALUE ) ? 0 : data.cFileName;
  }  

  inline void find_close( BOOST_HANDLE handle )
  {
    assert( handle != BOOST_INVALID_HANDLE_VALUE );
    ::FindClose( handle );
  }

  inline const char * find_next_file(
    BOOST_HANDLE handle, BOOST_SYSTEM_DIRECTORY_TYPE & data )
  // Returns: 0 if EOF, otherwise name
  // Throws: if system reports error
  {
    if ( ::FindNextFile( handle, &data ) == 0 )
    {
      if ( ::GetLastError() != ERROR_NO_MORE_FILES )
      {
        throw fs::filesystem_error(
          "directory_iterator ++() failure", fs::system_error );
      }
      else { return 0; } // end reached
     }
    return data.cFileName;
  }

#endif

  fs::directory_iterator end_itr;

  bool is_empty_directory( const fs::path & dir_path )
  {
    return fs::directory_iterator(dir_path) == end_itr;
  }

  unsigned long remove_all_aux( const fs::path & ph )
  {
    unsigned long count = 1;
    if ( fs::is_directory( ph ) )
    {
      for ( boost::filesystem::directory_iterator itr( ph );
            itr != end_itr; ++itr )
      {
        count += remove_all_aux( *itr );
      }
    }
    fs::remove( ph );
    return count;
  }

} // unnamed namespace

namespace boost
{
  namespace filesystem
  {
    namespace detail
    {
#ifdef BOOST_POSIX
      const char * implementation_name() { return "POSIX"; }
#else
      const char * implementation_name() { return "Windows"; }
#endif

//  directory_iterator_imp  --------------------------------------------------// 

      class directory_iterator_imp
      {
      public:
        path              entry_path;
        BOOST_HANDLE      handle;

        ~directory_iterator_imp()
        {
          if ( handle != BOOST_INVALID_HANDLE_VALUE ) find_close( handle );
        }
      };

    } // namespace detail

//  directory_iterator implementation  ---------------------------------------//

    // default ctor creates the "end" iterator (by letting base default to 0)
    directory_iterator::directory_iterator() {}

    directory_iterator::directory_iterator( const path & dir_path )
    {
      base().imp.reset( new detail::directory_iterator_imp );
      BOOST_SYSTEM_DIRECTORY_TYPE scratch;
      const char * name;
      if ( dir_path.is_null() )
       base().imp->handle = BOOST_INVALID_HANDLE_VALUE;
      else
        name = find_first_file( dir_path.directory_path().c_str(),
          base().imp->handle, scratch );  // sets handle

 		  if ( base().imp->handle != BOOST_INVALID_HANDLE_VALUE )
      {
        base().imp->entry_path = dir_path;
        base().imp->entry_path.m_path_append( name, path::nocheck );
		    while ( base().imp.get()
             && ( base().imp->entry_path.leaf() == "."
              || base().imp->entry_path.leaf() == ".." ) )
		      { operator++(); }
      }
      else
      {
        throw filesystem_error( std::string( 
          "directory_iterator constructor failure: " )
          + dir_path.directory_path().c_str(), system_error );
      }  
    }

    namespace detail
    {
      path const & directory_iterator_internals::deref() const
      {
        assert( imp.get() ); // fails if dereference end iterator
        return imp->entry_path;
      }

      void directory_iterator_internals::inc()
      {
        assert( imp.get() ); // fails on increment end iterator
        assert( imp->handle != BOOST_INVALID_HANDLE_VALUE ); // imp reality check

        BOOST_SYSTEM_DIRECTORY_TYPE scratch;
        const char * name;
        if ( (name = find_next_file( imp->handle, scratch )) != 0 )
        {
          imp->entry_path.m_replace_leaf( name );
        }
        else {
          imp.reset(); // make base() the end iterator
        }
      }
    }

//  free functions  ----------------------------------------------------------//

    bool exists( const path & ph )
    {
#   ifdef BOOST_POSIX
      struct stat path_stat;
      return ::stat( ph.file_path().c_str(), &path_stat ) == 0;  
#   else
      return ::GetFileAttributes( ph.file_path().c_str() ) != 0xFFFFFFFF;
#   endif
    }

    bool is_directory( const path & ph )
    {
#   ifdef BOOST_POSIX
      struct stat path_stat;
      if ( ::stat( ph.directory_path().c_str(), &path_stat ) != 0 )
        throw filesystem_error( std::string("is_directory(): ")
          + ph.directory_path().c_str(), system_error );
      return S_ISDIR( path_stat.st_mode );
#   else
      DWORD attributes = ::GetFileAttributes( ph.directory_path().c_str() );
      if ( attributes == 0xFFFFFFFF )
        throw filesystem_error( std::string("is_directory(): ")
          + ph.directory_path().c_str(), system_error );
      return (attributes & FILE_ATTRIBUTE_DIRECTORY) != 0;
#   endif
    }

    bool _is_empty( const path & ph )
    {
#   ifdef BOOST_POSIX
      struct stat path_stat;
      if ( ::stat( ph.file_path().c_str(), &path_stat ) != 0 )
              throw filesystem_error( std::string("is_empty(): ")
                + ph.file_path().c_str(), system_error );
      
      return S_ISDIR( path_stat.st_mode )
        ? is_empty_directory( ph )
        : path_stat.st_size == 0;
#   else
      WIN32_FILE_ATTRIBUTE_DATA fad;
      if ( !::GetFileAttributesEx( ph.file_path().c_str(),
        ::GetFileExInfoStandard, &fad ) )
              throw filesystem_error( std::string("is_empty(): ")
                + ph.file_path().c_str(), system_error );
      
      return ( fad.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY )
        ? is_empty_directory( ph )
        :( !fad.nFileSizeHigh && !fad.nFileSizeLow );
#   endif
    }

    void create_directory( const path & dir_path )
    {
#   ifdef BOOST_POSIX
      if ( ::mkdir( dir_path.directory_path().c_str(),
        S_IRWXU|S_IRWXG|S_IRWXO ) != 0 )
#   else
      if ( !::CreateDirectory( dir_path.directory_path().c_str(), 0 ) )
#   endif
        throw filesystem_error( std::string("create_directory(): ")
          + dir_path.directory_path().c_str(), system_error );
    }

    void remove( const path & ph )
    {
      if ( exists( ph ) )
      {
        if ( is_directory( ph ) )
        {
#   ifdef BOOST_POSIX
          if ( ::rmdir( ph.file_path().c_str() ) != 0 )
#   else
          if ( !::RemoveDirectory( ph.file_path().c_str() ) )
#   endif
            throw fs::filesystem_error( std::string("remove() on: ")
              + ph.file_path().c_str(), system_error );
        }
        else
        {
#   ifdef BOOST_POSIX
          if ( ::remove( ph.file_path().c_str() ) != 0 )
#   else
          if ( !::DeleteFile( ph.file_path().c_str() ) )
#   endif
            throw fs::filesystem_error( std::string("remove() on: ")
              + ph.file_path().c_str(), system_error );
        }
      }
    }

    unsigned long remove_all( const path & ph )
    {
      return exists( ph ) ? remove_all_aux( ph ) : 0;
    }

    void rename( const path & old_path,
                 const path & new_path )
    {
#   ifdef BOOST_POSIX
      if ( exists( new_path ) // POSIX is too permissive so must check
        || ::rename( old_path.file_path().c_str(), new_path.file_path().c_str() ) != 0 )
#   else
      if ( !::MoveFile( old_path.file_path().c_str(), new_path.file_path().c_str() ) )
#   endif
        throw filesystem_error( std::string("move_file(): ")
          + old_path.file_path().c_str() + ", " + new_path.file_path().c_str(), system_error );
    }

    void copy_file( const path & from_file_ph,
                    const path & to_file_ph )
    {
#   ifdef BOOST_POSIX
      // TODO: Ask POSIX experts if this is the best way to copy a file

      const std::size_t buf_sz = 32768;
      boost::scoped_array<char> buf( new char [buf_sz] );
      int infile, outfile;

      if ( (infile = ::open( from_file_ph.file_path().c_str(),
                             O_RDONLY )) < 0
        || (outfile = ::open( to_file_ph.file_path().c_str(),
                              O_WRONLY | O_CREAT | O_EXCL,
                              S_IRWXU|S_IRWXG|S_IRWXO )) < 0 )
      {
        if ( infile != 0 ) ::close( infile );
        throw fs::filesystem_error( std::string("copy() files: ")
          + from_file_ph.file_path().c_str()
          + ", " + to_file_ph.file_path().c_str(), system_error );
      }

      ssize_t sz;
      while ( (sz = ::read( infile, buf.get(), buf_sz )) > 0
        && (sz = ::write( outfile, buf.get(), sz )) > 0 ) {}

      ::close( infile );
      ::close( outfile );

      if ( sz != 0 )
#   else
      if ( !::CopyFile( from_file_ph.file_path().c_str(),
                      to_file_ph.file_path().c_str(), /*fail_if_exists=*/true ) )
#   endif
        throw fs::filesystem_error( std::string("copy() files: ")
          + from_file_ph.file_path().c_str()
          + ", " + to_file_ph.file_path().c_str(), system_error );
    }

    const path & initial_directory()
    {
      static path init_dir;
      if ( init_dir.is_null() )
      {
#     ifdef BOOST_POSIX
        long path_max = ::pathconf( ".", _PC_PATH_MAX );
        if ( path_max == -1 )
          throw filesystem_error( "initial_directory()" );
        boost::scoped_array<char>
          buf( new char[static_cast<std::size_t>(path_max)] );
        if ( ::getcwd( buf.get(), static_cast<std::size_t>(path_max) ) == 0 )
#     else
        DWORD sz;
        if ( (sz = ::GetCurrentDirectory( 0, static_cast<char*>(0) )) == 0 )
          throw filesystem_error( "initial_directory()" );
        boost::scoped_array<char> buf( new char[sz] );
        if ( ::GetCurrentDirectory( sz, buf.get() ) == 0 )
#     endif
          { throw filesystem_error( "initial_directory()", system_error ); }
        init_dir = path( buf.get(), system_specific );
      }
      return init_dir;
    }

  } // namespace filesystem
} // namespace boost

