//  path implementation  -----------------------------------------------------//

//  (C) Copyright Beman Dawes 2002. Permission to copy,
//  use, modify, sell and distribute this software is granted provided this
//  copyright notice appears in all copies. This software is provided "as is"
//  without express or implied warranty, and with no claim as to its
//  suitability for any purpose.

//  See http://www.boost.org for most recent version including documentation.


//****************************************************************************//

//  WARNING: This code was hacked time and time again as different library 
//  designs were tried.  Thus portions may be residue from the earlier
//  experiments, and be totally stupid or wrong in light of the final library
//  specifications.  The code needs to be reviewed line-by-line to elmininate
//  such problems.

//****************************************************************************//

# if !defined( BOOST_WINDOWS ) && !defined( BOOST_POSIX )
#   error macro BOOST_POSIX or BOOST_WINDOWS must be defined
# endif

#include <boost/filesystem/path.hpp>
#include <boost/filesystem/exception.hpp>

namespace fs = boost::filesystem;

#include <vector>
#include <cassert>

//  helpers  -----------------------------------------------------------------// 

namespace
{

  std::string::size_type leaf_pos( const std::string & path_str )
  // return 0 if path_str itself is leaf (or empty) 
  {
    if ( path_str == "/" // root
#     ifdef BOOST_WINDOWS
      || (path_str.size() > 1 // drive or device
      && (*(path_str.end()-1) == ':' || *(path_str.end()-2) == ':'))
#     endif
      ) return 0;

    std::string::size_type pos( path_str.find_last_of( '/' ) );
#   ifdef BOOST_WINDOWS
    if ( pos == std::string::npos ) pos = path_str.find_last_of( ':' );
#   endif

    return ( pos == std::string::npos // path itself must be a leaf (or empty)
#     ifdef BOOST_WINDOWS
      || (pos == 1 && path_str[0] == '/') // or share
#     endif
      ) ? 0 // so leaf is entire string
        : pos + 1; // or starts after delimiter
  }

  const char invalid_chars[] =
    "\x01\x02\x03\x04\x05\x06\x07\x08\x09\x0A\x0B\x0C\x0D\x0E\x0F"
    "\x10\x11\x12\x13\x14\x15\x16\x17\x18\x19\x1A\x1B\x1C\x1D\x1E\x1F"
    "<>:\"/\\|*?";
  // note that the terminating '\0' is part of the string - thus the size below
  // is sizeof(invalid_chars) rather than sizeof(invalid_chars)-1.  I 
  const std::string invalid_generic( invalid_chars, sizeof(invalid_chars) );

  const std::string valid_posix(
    "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789._-" );

  const std::string valid_boost_file(
    "abcdefghijklmnopqrstuvwxyz0123456789._-" );
  const std::string valid_boost_directory(
    "abcdefghijklmnopqrstuvwxyz0123456789_-" );

} // unnamed namespace

//----------------------------------------------------------------------------// 

namespace boost
{
  namespace filesystem
  {

    //  error checking functions  --------------------------------------------//


    bool generic_name( const std::string & name )
    {
      return name.size() != 0
        && name.find_first_of( invalid_generic ) == std::string::npos
        && name != "."
        && name != ".."
        && *name.begin() != ' '
        && *(name.end()-1) != ' ';     
    }

    bool posix_name( const std::string & name )
    {
      return name.find_first_not_of( valid_posix ) == std::string::npos
        && name != ".";     
    }

    const path & check_posix_leaf( const path & ph )
    {
      if ( !posix_name( ph.leaf() ) )
        throw filesystem_error( "invalid posix name: \""
          + ph.leaf() + "\"" );
      return ph;
    }

    bool boost_file_name( const std::string & name )
    {
      return name.size() <= 31
        && name.find_first_not_of( valid_boost_file ) == std::string::npos
        && name != ".";     
    }

    bool boost_directory_name( const std::string & name )
    {
      return name.size() <= 31
        && name.find_first_not_of( valid_boost_directory ) == std::string::npos;     
    }

    //  path implementation  -------------------------------------------------//

    path::path( const std::string & src )
    {
      m_path_append( src );
    }

    path::path( const char * src )
    {
      m_path_append( src );
    }

    path::path( const std::string & src, format )
    {
      m_path_append( src, platform );
    }

    path::path( const char * src, format )
    {
      m_path_append( src, platform );
    }

    path & path::operator <<=( const path & rhs )
    {
      m_path_append( rhs.m_path, nocheck );
      return *this;
    }

    void path::m_path_append( const std::string & src, source_context context )
    {
      // convert backslash to forward slash if context is "platform" 
      // check names if context is "generic"
      // allow system-specific-root if context is not "generic"

      assert( src.size() == strlen( src.c_str() ) ); // no embedded 0

      if ( src.size() == 0 ) return;

      std::string::const_iterator itr( src.begin() );

      // system-specific-root like drive: or first slash of //share
      if ( context != generic )
      {
        if ( *itr == '/'
#         ifdef BOOST_WINDOWS
          || (*itr == '\\' && context == platform)
#         endif
          ) { ++itr; m_path += '/'; }
        else if ( itr+1 != src.end() && *(itr+1) == ':' )
          { m_path += *itr++; m_path += *itr++; }

#       ifdef BOOST_WINDOWS
        // "/" or second slash of //share
        if ( *itr == '/' || (*itr == '\\' && context == platform) )
          { ++itr; m_path += '/';}
#       endif

        if ( itr == src.end() ) return;
      }

      // relative-path ::= element { "/" element }
      while ( itr != src.end() )
      {
        // append '/' if needed
        if ( !empty() // append '/'
            && *(m_path.end()-1) != ':' && *(m_path.end()-1) != '/' )
            m_path += '/'; 

        if ( *itr == '.'&& (itr+1) != src.end() && *(itr+1) == '.' ) // ".."
        {
          if ( m_path.size() >= 2 // there is a named parent directory present
            && *(m_path.end()-1) == '/'
#           ifdef BOOST_WINDOWS
            && *(m_path.end()-2) != ':'
#           endif
            && *(m_path.end()-2) != '.' )    
          {
            // reference to parent so erase child
            std::string::iterator child( m_path.end()-2 );

            while ( child != m_path.begin() && *child != '/'
#             ifdef BOOST_WINDOWS
              && *child != ':'
#             endif
              ) --child;

            // only erase '/' if it is a separator rather than part of the root
            if ( (*child == '/'
              && (child == m_path.begin()
#             ifdef BOOST_WINDOWS
                || *(child-1) == ':'))
              || *child == ':'
#             else                           
              ))         
#             endif              
              ) ++child;

            m_path.erase( child, m_path.end() );
          }
          else { m_path += ".."; }
          ++itr;
          ++itr;
        } // ".."
        else // name
        {
          std::string name;
          do
            { name += *itr; }
          while ( ++itr != src.end() && *itr != '/'
#           ifdef BOOST_WINDOWS
            && (*itr != '\\' || context != platform)
#           endif
            );

          if ( context == generic && !generic_name( name ) )
          {
            throw filesystem_error( "invalid path name: \""
              + src + "\"" );
          }

          m_path += name;
        }

        if ( itr != src.end() )
        {  
          if ( !( (*itr == '/'
#           ifdef BOOST_WINDOWS
            || (context == platform && *itr == '\\')
#           endif
            ) && (itr+1) != src.end() ) )
          {
            throw filesystem_error( "invalid path syntax: \""
              + src + "\"" );
          }
          ++itr;
        }

      } // while more elements
    }

    void path::m_replace_leaf( const char * new_leaf )
    {
      m_path.erase( leaf_pos( m_path ) );
      m_path += new_leaf;
    }

    const std::string path::leaf() const
    {
      return m_path.substr( leaf_pos( m_path ) );
    }

    const path path::branch() const
    {
      std::string::size_type len( leaf_pos( m_path ) );
       
      if ( len >= 2  // unless delimiter is part of root, don't include it
#     ifdef BOOST_WINDOWS
        && m_path[len-1] != ':'
        && m_path[len-2] != ':'
#     endif
        ) --len;

      return path( m_path.substr( 0, len ), system_specific );
    }

    //bool path::is_absolute() const
    //{
    //  return ( m_path.size() 
    //           && m_path[0] == '/' )  // covers both "/" and "//share"
    //      || ( m_path.size() > 2
    //           && m_path[1] == ':'
    //           && m_path[2] == '/' )  // "c:/"
    //      || ( m_path.size() > 3
    //           && m_path[m_path.size()-1] == ':' ); // "device:"
    //}


  } // namespace filesystem
} // namespace boost
