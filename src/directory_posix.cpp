//  Directory implementation file for POSIX  ---------------------------------//

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

#include "boost/config.hpp"
#include "boost/filesystem/directory.hpp"
#include "boost/filesystem/exception.hpp"
//#include "boost/filesystem/path.hpp"

namespace fs = boost::filesystem;

//  This POSIX version uses the functions opendir(), readdir(), and closdir()
//  to find directory entries. In addition, stat() is used to find out
//  about specific file attributes.

#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h>
#include <fcntl.h>

#include <string>
#include <cstdio>
#include <cerrno>
#include <cassert>

//  helpers  -----------------------------------------------------------------//

namespace
{
  fs::directory_iterator end_itr;

  bool empty_directory( const std::string & dir_path )
  {
    return fs::directory_iterator(dir_path)
             == fs::directory_iterator();
  }
    
  inline bool internal_name( const std::string & name )
    { return name == "." || name == ".."; }

  unsigned long remove_all_aux( const std::string & path )
  {
    unsigned long count = 1;
    if ( boost::filesystem::is_file( path ) )
    { 
      boost::filesystem::remove_file( path );
    }
    else
    {
      for ( boost::filesystem::directory_iterator itr( path );
            itr != end_itr; ++itr )
      {
        count += remove_all_aux( itr->path() );
      }
      boost::filesystem::remove_directory( path );
    }
    return count;
  }

} // unnamed namespace

//  directory_entry_imp  -----------------------------------------------------// 

namespace boost
{
  namespace filesystem
  {
    namespace detail
    {
      class directory_entry_imp
      {
      public:
        std::string name;
        std::string directory_path;
        std::string full_path; // initialized by directory_entry::path()
      };
    } // namespace detail

//  directory_entry implementation  ------------------------------------------//

    directory_entry::directory_entry()
      : m_imp( new boost::filesystem::detail::directory_entry_imp )
    {
    }

    directory_entry::directory_entry(
      const directory_entry & rhs )
      : m_imp( new detail::directory_entry_imp )
    {
      *m_imp = *rhs.m_imp; // deep copy
    }

    directory_entry::~directory_entry()
    {
    }

    directory_entry & directory_entry::operator=(
      const directory_entry & rhs )
    {
      if ( this != &rhs ) *m_imp = *rhs.m_imp; // deep copy
      return *this;
    }

    const std::string & directory_entry::name() const
    {
      return m_imp->name;
    }

    const std::string & directory_entry::directory_path() const
    {
      return m_imp->directory_path;
    }

    const std::string & directory_entry::path() const
    {
      m_imp->full_path = m_imp->directory_path + "/" + m_imp->name;
      return m_imp->full_path;
    }

    bool directory_entry::is_directory() const
    {
      // on Windows there was a bit of efficiency by not rechecking, but that
      // doesn't appear possible with POSIX, so just call the free function.
      return fs::is_directory( path() );
    }

//  directory_iterator_imp  --------------------------------------------------// 

    namespace detail
    {
      class directory_iterator_imp
      {
      public:
        directory_entry   dir_entry;
        DIR *             handle;        // 0 means handle not open
      };

    } // namespace detail

//  directory_iterator implementation  ---------------------------------------//

    directory_iterator::directory_iterator()
    {
      // default ctor creates the "end" iterator (by letting m_imp default to 0)
    }

    directory_iterator::directory_iterator(
      std::string const & dir_path)
      : m_imp( new detail::directory_iterator_imp )
    {
      m_imp->handle = dir_path.size()
                    ? ::opendir( dir_path.c_str() )
                    : 0;
		  if ( m_imp->handle != 0 )
      {
        m_imp->dir_entry.m_imp->directory_path = dir_path;

		    do
        {
          operator++();
        } while ( m_imp.get()
             && internal_name( m_imp->dir_entry.m_imp->name ) );
      }
      else
      {
        throw filesystem_error( "iterator constructor failure: " + dir_path );
      }  
    }

    directory_iterator::~directory_iterator()
    {
      if ( m_imp.get() && m_imp->handle != 0)
        ::closedir( m_imp->handle );
    }

    bool directory_iterator::operator== (
      directory_iterator const& itr ) const
    {
      return m_imp == itr.m_imp;
    }

    directory_entry const & directory_iterator::operator* () const
    {
      assert( m_imp.get() ); // fails if dereference end iterator
      return m_imp->dir_entry;
    }

    directory_iterator & directory_iterator::operator++ ()
    {
      assert( m_imp.get() ); // fails on increment end iterator
      assert( m_imp->handle != 0 ); // imp reality check

/******************************************************************************/
/******************************************************************************/

//  TODO: use readdir_r() if available, so code is multi-thread safe.
//  Fly-in-ointment: not all platforms support readdir_r(). 

/******************************************************************************/
/******************************************************************************/

      struct dirent * dp;

      errno = 0;
      if ( (dp = ::readdir( m_imp->handle )) != 0 )

      {
        m_imp->dir_entry.m_imp->name = dp->d_name;
//        m_imp->dir_entry.m_imp->full_path.clear();
        m_imp->dir_entry.m_imp->full_path = "";  // workaround for gcc 2.95.x   

      }
      else if ( errno != 0 )
      {
        throw filesystem_error( "++iterator failure" );
      }
      else // eof
      {
        ::closedir( m_imp->handle );
        m_imp.reset(); // make m_imp the end iterator
      }
      return *this;
    }

  } // namespace filesystem
} // namespace boost


//  free functions  ----------------------------------------------------------//

namespace boost
{
  namespace filesystem
  {
    bool exists( const std::string & path )
    {
      struct ::stat path_stat;
      return ::stat( path.c_str(), &path_stat ) == 0;  
    }

    bool is_directory( const std::string & path )
    {
      struct ::stat path_stat;
      if ( ::stat( path.c_str(), &path_stat ) != 0 )
        throw filesystem_error( "is_directory(): " + path );
      return S_ISDIR( path_stat.st_mode );
    }

    bool is_file( const std::string & path )
    {
      struct ::stat path_stat;
      if ( ::stat( path.c_str(), &path_stat ) != 0 )
        throw filesystem_error( "is_file(): " + path );
      return !S_ISDIR( path_stat.st_mode );;
    }

    bool empty( const std::string & path )
    {
      struct ::stat path_stat;
      if ( ::stat( path.c_str(), &path_stat ) != 0 )
              throw filesystem_error( "is_empty(): " + path );
      
      return S_ISDIR( path_stat.st_mode )
        ? empty_directory( path )
        : path_stat.st_size == 0;
    }

    void create_directory( const std::string & dir_path )
    {
      if ( ::mkdir( dir_path.c_str(), S_IRWXU|S_IRWXG|S_IRWXO ) !=0 )
        throw filesystem_error( "create_directory(): " + dir_path );
    }

    void remove_file( const std::string & file_path )
    {
      if ( ::unlink( file_path.c_str() ) != 0 )
        throw fs::filesystem_error( "remove_file() on: " + file_path );
    }

    void remove_directory( const std::string & dir_path )
    {
      if ( ::rmdir( dir_path.c_str() ) != 0 )
        throw fs::filesystem_error( "remove_directory() on: " + dir_path );
    }

    unsigned long remove_all( const std::string & path )
    {
      return exists( path ) ? remove_all_aux( path ) : 0;
    }

    void rename( const std::string & old_path,
                 const std::string & new_path )
    {
      if ( exists( new_path ) // POSIX unlink is too permissive so must check
        || ::rename( old_path.c_str(), new_path.c_str() ) != 0 )
        throw filesystem_error( "move_file(): " + old_path + ", " + new_path );
    }

    void copy_file( const std::string & from_file_path,
                    const std::string & to_file_path )
    {
      // TODO: Ask a POSIX expert if this is the best way to copy a file

      const std::size_t buf_sz = 32768;
      boost::scoped_array<char> buf( new char [buf_sz] );
      int infile, outfile;

      if ( (infile = ::open( from_file_path.c_str(),
                             O_RDONLY )) < 0
        || (outfile = ::open( to_file_path.c_str(),
                              O_WRONLY | O_CREAT | O_EXCL,
                              S_IRWXU|S_IRWXG|S_IRWXO )) < 0 )
      {
        if ( infile >= 0 ) ::close( infile );
        throw fs::filesystem_error( "copy() files: " + from_file_path
          + ", " + to_file_path );
      }

      ssize_t sz;
      while ( (sz = ::read( infile, buf.get(), buf_sz )) > 0
        && (sz = ::write( outfile, buf.get(), sz )) > 0 ) {}

      ::close( infile );
      ::close( outfile );

      if ( sz != 0 )
        throw fs::filesystem_error( "copy() files: " + from_file_path
          + ", " + to_file_path );
    }

    const std::string & initial_directory()
    {
      static std::string dir;
      if ( !dir.size() )
      {
        long path_max = ::pathconf( ".", _PC_PATH_MAX );
        if ( path_max == -1 )
          throw filesystem_error( "initial_directory()" );
        boost::scoped_array<char>
          buf( new char[static_cast<std::size_t>(path_max)] );
        if ( ::getcwd( buf.get(), static_cast<std::size_t>(path_max) ) == 0 )
          throw filesystem_error( "initial_directory()" );
        dir = buf.get();
      }
      return dir;
    }

  } // namespace filesystem
} // namespace boost

