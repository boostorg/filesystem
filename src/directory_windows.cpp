//  Directory implementation file  -------------------------------------------//

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

#include "windows.h"

#include <string>
#include <cstdio>
#include <cerrno>
#include <cassert>

//  helpers  -----------------------------------------------------------------//

namespace
{
  fs::directory_iterator end_itr;

  inline DWORD get_attributes( const std::string & path )
    { return GetFileAttributes( path.c_str() ); }

  inline DWORD get_cur_dir( DWORD sz, char * buf )
    { return GetCurrentDirectory( sz, buf ); }

  bool empty_directory( const std::string & dir_path )
  {
    return fs::directory_iterator(dir_path)
             == fs::directory_iterator();
  }
    
  HANDLE find_first_file( const std::string & arg_dirpath,
                          WIN32_FIND_DATAA & data )
  {
    std::string dirpath( arg_dirpath + "/*" );
    return FindFirstFileA( dirpath.c_str(), &data );
  }  

  inline bool internal_name( const std::string & name )
    { return name == "." || name == ".."; }

  inline bool find_next_file( HANDLE handle, WIN32_FIND_DATAA & data )
    { return FindNextFileA( handle, &data ) != 0; }

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
        bool is_directory;
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
      return m_imp->is_directory;
    }

//  directory_iterator_imp  --------------------------------------------------// 

    namespace detail
    {
      class directory_iterator_imp
      {
      public:
        directory_entry   dir_entry;
        HANDLE            handle;
        WIN32_FIND_DATA   find_data;
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
                    ? find_first_file( dir_path, m_imp->find_data )
                    : INVALID_HANDLE_VALUE;
		  if ( m_imp->handle != INVALID_HANDLE_VALUE )
      {
        m_imp->dir_entry.m_imp->directory_path = dir_path;
        m_imp->dir_entry.m_imp->name = m_imp->find_data.cFileName;
        m_imp->dir_entry.m_imp->is_directory 
          = (m_imp->find_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0;
		    while ( m_imp.get()
             && internal_name( m_imp->dir_entry.m_imp->name ) )
		      { operator++(); }
      }
      else
      {
        throw filesystem_error( "iterator constructor failure: " + dir_path );
      }  
    }

    directory_iterator::~directory_iterator()
    {
      if ( m_imp.get() && m_imp->handle != INVALID_HANDLE_VALUE)
        FindClose( m_imp->handle );
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
      assert( m_imp->handle != INVALID_HANDLE_VALUE ); // imp reality check
      if ( find_next_file( m_imp->handle, m_imp->find_data ) )
      {
        m_imp->dir_entry.m_imp->name = m_imp->find_data.cFileName;
        m_imp->dir_entry.m_imp->full_path.clear(); 
        m_imp->dir_entry.m_imp->is_directory 
          = (m_imp->find_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0;
      }
      else if ( GetLastError() != ERROR_NO_MORE_FILES )
      {
        throw filesystem_error( "++iterator failure" );
      }
      else // eof
      {
        FindClose( m_imp->handle );
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
      return get_attributes( path ) != 0xFFFFFFFF;  
    }

    bool is_directory( const std::string & path )
    {
      DWORD attributes = get_attributes( path );
      if ( attributes == 0xFFFFFFFF )
        throw filesystem_error( "is_directory(): " + path );
      return (attributes & FILE_ATTRIBUTE_DIRECTORY) != 0;
    }

    bool is_file( const std::string & path )
    {
      DWORD attributes = get_attributes( path );
      if ( attributes == 0xFFFFFFFF )
        throw filesystem_error( "is_file(): " + path );
      return (attributes & FILE_ATTRIBUTE_DIRECTORY) == 0;
    }

    bool empty( const std::string & path )
    {
      WIN32_FILE_ATTRIBUTE_DATA fad;
      if ( !GetFileAttributesExA( path.c_str(),
              GetFileExInfoStandard, &fad ) )
              throw filesystem_error( "is_empty(): " + path );
      
      return ( fad.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY )
        ? empty_directory( path )
        :( !fad.nFileSizeHigh && !fad.nFileSizeLow );
    }

    void create_directory( const std::string & dir_path )
    {
      if ( !CreateDirectory( dir_path.c_str(), 0 ) )
        throw filesystem_error( "create_directory(): " + dir_path );
    }

    void remove_file( const std::string & file_path )
    {
      if ( !DeleteFile( file_path.c_str() ) )
        throw fs::filesystem_error( "remove_file() on: " + file_path );
    }

    void remove_directory( const std::string & dir_path )
    {
      if ( !RemoveDirectory( dir_path.c_str() ) )
        throw fs::filesystem_error( "remove_directory() on: " + dir_path );
    }

    unsigned long remove_all( const std::string & path )
    {
      return exists( path ) ? remove_all_aux( path ) : 0;
    }

    void rename( const std::string & old_path,
                 const std::string & new_path )
    {
      if ( !MoveFile( old_path.c_str(), new_path.c_str() ) )
        throw filesystem_error( "move_file(): " + old_path + ", " + new_path );
    }

    void copy_file( const std::string & from_file_path,
                    const std::string & to_file_path )
    {
      if ( !CopyFile( from_file_path.c_str(),
                      to_file_path.c_str(), /*fail_if_exists=*/true ) )
        throw fs::filesystem_error( "copy() files: " + from_file_path
          + ", " + to_file_path );
    }

    const std::string & initial_directory()
    {
      static std::string dir;
      if ( !dir.size() )
      {
        DWORD sz;
        if ( (sz = get_cur_dir( 0, static_cast<char*>(0) )) == 0 )
          throw filesystem_error( "initial_directory()" );
        boost::scoped_array<char> buf( new char[sz] );
        if ( get_cur_dir( sz, buf.get() ) == 0 )
          throw filesystem_error( "initial_directory()" );
        dir = buf.get();
      }
      return dir;
    }

  } // namespace filesystem
} // namespace boost

