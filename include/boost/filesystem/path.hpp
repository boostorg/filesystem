//  boost/filesystem/path.hpp  -----------------------------------------------//

//  © Copyright Beman Dawes 2002-2005
//  Use, modification, and distribution is subject to the Boost Software
//  License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

//  See library home page at http://www.boost.org/libs/filesystem

//----------------------------------------------------------------------------// 

#ifndef BOOST_FILESYSTEM_PATH_HPP
#define BOOST_FILESYSTEM_PATH_HPP

#include <boost/filesystem/config.hpp>
#include <boost/iterator/iterator_facade.hpp>
#include <boost/throw_exception.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/type_traits/is_same.hpp>
#include <boost/static_assert.hpp>
#include <string>
#include <algorithm> // for lexicographical_compare
#include <stdexcept>
#include <cassert>

#include <boost/config/abi_prefix.hpp> // must be the last #include

//----------------------------------------------------------------------------//

namespace boost
{
  namespace filesystem
  {
    namespace detail
    {
      struct path_traits;
      struct wpath_traits;
    }

    template<class String, class Traits> class basic_path;
    typedef basic_path< std::string, detail::path_traits >    path;
    typedef basic_path< std::wstring, detail::wpath_traits >  wpath;

    namespace detail
    {
      struct path_traits
      {
        typedef std::string internal_string_type;
        typedef std::string external_string_type;
        static external_string_type to_external( const path &,
          const internal_string_type & src ) { return src; }
        static internal_string_type to_internal(
          const external_string_type & src ) { return src; }
      };
      struct wpath_traits
      {
        typedef std::wstring internal_string_type;
# ifdef BOOST_WINDOWS_API
        typedef std::wstring external_string_type;
        static external_string_type to_external( const wpath &,
          const internal_string_type & src ) { return src; }
        static internal_string_type to_internal(
          const external_string_type & src ) { return src; }
# else
        typedef std::string external_string_type;
        static external_string_type to_external( const wpath & ph,
          const internal_string_type & src );
        static internal_string_type to_internal(
          const external_string_type & src );
# endif
      };
    } // namespace detail

    enum error_code
    {
      no_error = 0,
      system_error,     // system generated error; if possible, is translated
                        // to one of the more specific errors below.
      other_error,      // library generated error
      security_error,   // includes access rights, permissions failures
      read_only_error,
      io_error,
      path_error,       // such as bad syntax
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

    typedef int system_error_type;

    BOOST_FILESYSTEM_DECL error_code
    lookup_error_code( system_error_type sys_err_code );
 
    BOOST_FILESYSTEM_DECL void
    system_message( system_error_type sys_err_code, std::string & target );
    // Effects: appends error message to target

# ifdef BOOST_WINDOWS_API
    BOOST_FILESYSTEM_DECL void
    system_message( system_error_type sys_err_code, std::wstring & target );
# endif

    //  basic_filesystem_error  ----------------------------------------------//

    template<class Path>
    class basic_filesystem_error : public std::exception
    //                                         ^^^^^^^^^
    // should probably derive from runtime_error, but that causes a default
    // constructor problem. changed back to std::exception as a workaround.
    {
    // see www.boost.org/more/error_handling.html for design rationale
    public:
      // compiler generates copy constructor and copy assignment

      typedef Path path_type;

      explicit basic_filesystem_error( const std::string & what );

      explicit basic_filesystem_error(
        const std::string & what, system_error_type sys_ec );

      basic_filesystem_error( const std::string & what,
        const path_type & path1, error_code ec = other_error );

      basic_filesystem_error( const std::string & what,
        const path_type & path1, system_error_type sys_err_code );

      basic_filesystem_error( const std::string & what, const path_type & path1,
        const path_type & path2, system_error_type sys_err_code );

      ~basic_filesystem_error() throw() {}

      virtual const char * what() const throw();

      system_error_type  system_error() const { return m_sys_err; }
      // Note: system_error() == 0 implies a library (rather than system) error
      error_code      error() const { return m_err; }
      const path_type & path1() const
      {
        static const path_type empty_path;
        return m_imp_ptr.get() ? m_imp_ptr->m_path1 : empty_path ;
      }
      const path_type & path2() const
      {
        static const path_type empty_path;
        return m_imp_ptr.get() ? m_imp_ptr->m_path2 : empty_path ;
      }

    private:
      struct m_imp
      {
        std::string     m_what;
        path_type       m_path1; // may be empty()
        path_type       m_path2; // may be empty()
      };
      boost::shared_ptr<m_imp> m_imp_ptr;
      system_error_type m_sys_err;
      error_code        m_err;
    };

    typedef basic_filesystem_error<path> filesystem_error;
    typedef basic_filesystem_error<wpath> wfilesystem_error;

    //  path traits  ---------------------------------------------------------//

    template<class Path> struct is_basic_path
     { static const bool value = false; };
    template<> struct is_basic_path<path> { static const bool value = true; };
    template<> struct is_basic_path<wpath> { static const bool value = true; };

    // these only have to be specialized if Path::string_type::value_type
    // is not convertible from char
    template<class Path> struct path_separator
      { static const char value = '/'; };

    template<class Path> struct path_relative
      { static const char value = '.'; };

# ifdef BOOST_WINDOWS_PATH
    template<class Path> struct path_alt_separator
      { static const char value = '\\'; };

    template<class Path> struct path_device_delim
      { static const char value = ':'; };
# endif

    //  basic_path  ----------------------------------------------------------//
  
    template<class String, class Traits>
    class basic_path
    {
    // invariant: m_path valid according to the portable generic path grammar

      // validate template arguments
// TODO: get these working
//      BOOST_STATIC_ASSERT( ::boost::is_same<String,typename Traits::internal_string_type>::value );
//      BOOST_STATIC_ASSERT( ::boost::is_same<typename Traits::external_string_type,std::string>::value || ::boost::is_same<typename Traits::external_string_type,std::wstring>::value );

    public:
      // compiler generates copy constructor and copy assignment

      typedef basic_path<String, Traits> path_type;
      typedef String string_type;
      typedef Traits traits_type;
      typedef typename Traits::external_string_type external_string_type; 

      basic_path(){}

      basic_path( const string_type & str ) { operator/=( str ); }
      basic_path( const typename string_type::value_type * s )
        { operator/=( s );}

      ~basic_path() {}

      // append operations:
      basic_path & operator /=( const typename string_type::value_type * rhs );
      basic_path & operator /=( const basic_path & rhs )
        { return operator /=( rhs.string().c_str() ); }
      basic_path & operator /=( const string_type & rhs )
       { return operator /=( rhs.c_str() ); }

      basic_path operator /( const basic_path & rhs ) const
        { return basic_path( *this ) /= rhs; }
      basic_path operator /( const string_type & rhs ) const
        { return basic_path( *this ) /= rhs; }
      basic_path operator /( const typename string_type::value_type * rhs ) const
        { return basic_path( *this ) /= rhs; }

      // modification functions:
      basic_path & normalize();
      basic_path & remove_leaf();   // no effect if !branch_path()

      // conversion functions:
      // const returns (see Effective C++, item 21) allow implementations to
      // return const & if they so desire, since const returns prevent user code
      // from chaining to non-const functions.
      const string_type & string() const { return m_path; }

      const string_type file_string() const;
      const string_type directory_string() const { return file_string(); }

      const external_string_type external_file_string() const
        { return Traits::to_external( *this, file_string() ); }
      const external_string_type external_directory_string() const
        { return Traits::to_external( *this, directory_string() ); }

      // decomposition functions:
      basic_path   root_path() const;
      string_type  root_name() const;
      string_type  root_directory() const;
      basic_path   relative_path() const;
      string_type  leaf() const;
      basic_path   branch_path() const;

      // query functions:
      bool empty() const { return m_path.empty(); } // name consistent with std containers

      bool is_complete() const;

      bool has_root_path() const;
      bool has_root_name() const;
      bool has_root_directory() const;
      bool has_relative_path() const { return !relative_path().empty(); }
      bool has_leaf() const { return !m_path.empty(); }
      bool has_branch_path() const { return !branch_path().empty(); }

      // iteration over the names in the path:
      class iterator : public boost::iterator_facade<
        iterator,
        string_type const,
        boost::bidirectional_traversal_tag >
      {
      private:
        friend class boost::iterator_core_access;
        friend class boost::filesystem::basic_path<String, Traits>;

        typename basic_path<String, Traits>::iterator::reference dereference() const
          { return m_name; }
        bool equal( const iterator & rhs ) const
          { return m_path_ptr == rhs.m_path_ptr && m_pos == rhs.m_pos; }
        void increment();
        void decrement();

        string_type             m_name;     // cache current element.
        const basic_path *      m_path_ptr; // path being iterated over.
        typename string_type::size_type  m_pos;  // position of name in
                                            // path_ptr->string(). The
                                            // end() iterator is indicated by 
                                            // pos == path_ptr->string().size()
      };

      iterator begin() const;
      iterator end() const;

// TODO: should these definitions follow the std::basic_string practice?
      // relational operators
      bool operator<( const basic_path & that ) const
      {
        return std::lexicographical_compare(
          begin(), end(), that.begin(), that.end() );
      }

      bool operator==( const basic_path & that ) const
        { return !(*this < that) && !(that < *this); }
      bool operator!=( const basic_path & that ) const
        { return !(*this == that); }
      bool operator>( const basic_path & that ) const 
        { return that < *this; }
      bool operator<=( const basic_path & that ) const
        { return !(that < *this); }
      bool operator>=( const basic_path & that ) const
        { return !(*this < that); }

    private:
      // Note: This is an implementation for POSIX and Windows, where there
      // are only minor differences between generic and native path grammars.
      // Private members might be quite different in other implementations,
      // particularly where there were wide differences between generic and
      // native path formats, or between file_string() and
      // directory_string() formats, or simply that the implementer
      // was willing expend additional memory to achieve greater speed.

      string_type  m_path;

      // Was qualified; como433beta8 reports:
      //    warning #427-D: qualified name is not allowed in member declaration 
      friend class iterator;

      // Deprecated features ease transition for existing code. Don't use these
      // in new code.
    public:
      typedef bool (*name_check)( const std::string & name );
      basic_path( const string_type & str, name_check ) { operator/=( str ); }
      basic_path( const typename string_type::value_type * s, name_check )
        { operator/=( s );}
      string_type native_file_string() const { return file_string(); }
      string_type native_directory_string() const { return directory_string(); }
      static bool default_name_check_writable() { return false; } 
      static void default_name_check( name_check ) {}
      static name_check default_name_check() { return 0; }
    };

  //  basic_path non-member functions  ---------------------------------------//

    template<class String, class Traits>
    inline basic_path<String, Traits> operator / ( 
    const typename String::value_type * lhs, const basic_path<String, Traits> & rhs )
      { return basic_path<String, Traits>( lhs ) /= rhs; }

    template<class String, class Traits>
    inline basic_path<String, Traits> operator / (
      const String & lhs, const basic_path<String, Traits> & rhs )
      { return basic_path<String, Traits>( lhs ) /= rhs; }
   
  //  path::name_checks  -----------------------------------------------------//

/*      
    BOOST_FILESYSTEM_DECL bool portable_posix_name( const std::string & name );
    BOOST_FILESYSTEM_DECL bool windows_name( const std::string & name );
    BOOST_FILESYSTEM_DECL bool portable_name( const std::string & name );
    BOOST_FILESYSTEM_DECL bool portable_directory_name( const std::string & name );
    BOOST_FILESYSTEM_DECL bool portable_file_name( const std::string & name );
    BOOST_FILESYSTEM_DECL bool no_check( const std::string & name );   // always returns true
    BOOST_FILESYSTEM_DECL bool native( const std::string & name );
      // native(name) must return true for any name which MIGHT be valid
      // on the native platform.
*/
// implementation  -----------------------------------------------------------//

    namespace detail
    {
      // leaf_pos helper  ----------------------------------------------------//

      template<class String, class Traits>
      typename String::size_type leaf_pos( const String & str,
        typename String::size_type end_pos ) // end_pos is past-the-end position
      // return 0 if str itself is leaf (or empty) 
      {
        typedef typename boost::filesystem::basic_path<String, Traits> path_type;
        if ( end_pos && str[end_pos-1] == path_separator<path_type>::value )
          return end_pos-1;
        
        typename String::size_type pos(
          str.find_last_of( path_separator<path_type>::value, end_pos-1 ) );
#       ifdef BOOST_WINDOWS_PATH
        if ( pos == String::npos )
          pos = str.find_last_of( path_device_delim<path_type>::value, end_pos-2 );
#       endif

        return ( pos == String::npos // path itself must be a leaf (or empty)
#         ifdef BOOST_WINDOWS_PATH
          || (pos == 1 && str[0] == path_separator<path_type>::value) // or share
#         endif
          ) ? 0 // so leaf is entire string
            : pos + 1; // or starts after delimiter
      }

      // first_name helper  --------------------------------------------------//

      template<class String, class Traits>
      void first_name( const String & src, String & target )
      {
        typedef typename boost::filesystem::basic_path<String, Traits> path_type;
        target.erase( target.begin(), target.end() ); // VC++ 6.0 doesn't have string::clear()
        typename String::const_iterator itr( src.begin() );

#       ifdef BOOST_WINDOWS_PATH
        // deal with //network
        if ( src.size() >= 2 && src[0] == path_separator<path_type>::value
          && src[1] == path_separator<path_type>::value )
        { 
          target = path_separator<path_type>::value;
          target += path_separator<path_type>::value;
          itr += 2;
        }
#       endif

        while ( itr != src.end()
#         ifdef BOOST_WINDOWS_PATH
          && *itr != path_device_delim<path_type>::value
#         endif
          && *itr != path_separator<path_type>::value ) { target += *itr++; }

        if ( itr == src.end() ) return;

#       ifdef BOOST_WINDOWS_PATH
        if ( *itr == path_device_delim<path_type>::value )
        {
          target += *itr++;
          return;
        }
#       endif

        // *itr is '/'
        if ( itr == src.begin() ) { target += path_separator<path_type>::value; }
        return;
      }

      // is_absolute_root helper  --------------------------------------------//

      template<class String, class Traits>
      bool is_absolute_root( const String & s,
        typename String::size_type len )
      {
        typedef typename boost::filesystem::basic_path<String, Traits> path_type;
        return
          len && s[len-1] == path_separator<path_type>::value
          &&
          (
            len == 1 // "/"
#       ifdef BOOST_WINDOWS_PATH
            || ( len > 1
                 && ( s[len-2] == path_device_delim<path_type>::value // drive or device
                   || ( s[0] == path_separator<path_type>::value   // network
                     && s[1] == path_separator<path_type>::value
                     && s.find( path_separator<path_type>::value, 2 ) == len-1
                      )
                    )
                )
#       endif
          );
      }
    } // namespace detail

    // decomposition functions  ----------------------------------------------//

    template<class String, class Traits>
    String basic_path<String, Traits>::leaf() const
    {
      return m_path.substr(
        detail::leaf_pos<String, Traits>( m_path, m_path.size() ) );
    }

    template<class String, class Traits>
    basic_path<String, Traits> basic_path<String, Traits>::branch_path() const
    {
      typename String::size_type end_pos(
        detail::leaf_pos<String, Traits>( m_path, m_path.size() ) );

      // skip a path_separator<path_type>::value unless it is a root directory
      if ( end_pos && m_path[end_pos-1] == path_separator<path_type>::value
        && !detail::is_absolute_root<String, Traits>( m_path, end_pos ) )
        --end_pos;
      return basic_path<String, Traits>( m_path.substr( 0, end_pos ) );
    }

    template<class String, class Traits>
    basic_path<String, Traits> basic_path<String, Traits>::relative_path() const
    {
      typename String::size_type pos( 0 );
      if ( m_path.size() && m_path[0] == path_separator<path_type>::value )
      { pos = 1;
#     ifdef BOOST_WINDOWS_PATH
        if ( m_path.size()>1 && m_path[1] == path_separator<path_type>::value ) // share
        {
          if ( (pos = m_path.find( path_separator<path_type>::value, 2 ))
            != String::npos ) ++pos;
          else return path();
        }
      }
      else if ( (pos = m_path.find( path_device_delim<path_type>::value ))
        == String::npos ) pos = 0;
      else // has path_device_delim<path_type>::value
      {
        if ( ++pos < m_path.size() && m_path[pos]
          == path_separator<path_type>::value ) ++pos;
#     endif
      }
      return basic_path<String, Traits>( m_path.substr( pos ) );
    }

    template<class String, class Traits>
    String basic_path<String, Traits>::root_name() const
    {
#   ifdef BOOST_WINDOWS_PATH
      typename String::size_type pos(
        m_path.find( path_device_delim<path_type>::value ) );
      if ( pos != std::string::npos ) return m_path.substr( 0, pos+1 );
      if ( m_path.size() > 2
        && m_path[0] == path_separator<path_type>::value
        && m_path[1] == path_separator<path_type>::value )
      {
        pos = m_path.find( path_separator<path_type>::value, 2 );
        return m_path.substr( 0, pos );
      }
#   endif
      return String();
    }

    template<class String, class Traits>
    String basic_path<String, Traits>::root_directory() const
    {
      String s;
      // "/" or "//network"
      if ( m_path.size() && m_path[0] == path_separator<path_type>::value
#       ifdef BOOST_WINDOWS_PATH
        || ( m_path.size() > 2
             && m_path[1] == path_device_delim<path_type>::value
             && m_path[2] == path_separator<path_type>::value )  // "c:/"
#       endif
         ) s += path_separator<path_type>::value;
      return s;
    }

    template<class String, class Traits>
    basic_path<String, Traits> basic_path<String, Traits>::root_path() const
    {
      return basic_path<String, Traits>(
#   ifdef BOOST_WINDOWS_PATH
        root_name() ) /= root_directory();
#   else
        root_directory() );
#   endif
    }

    // path query functions  -------------------------------------------------//

    template<class String, class Traits>
    bool basic_path<String, Traits>::is_complete() const
    {
#   ifdef BOOST_WINDOWS_PATH
      return m_path.size() > 2
        && ( (m_path[1] == path_device_delim<path_type>::value
          && m_path[2] == path_separator<path_type>::value) // "c:/"
          || (m_path[0] == path_separator<path_type>::value
            && m_path[1] == path_separator<path_type>::value) // "//share"
          || m_path[m_path.size()-1] == path_device_delim<path_type>::value );
#   else
      return m_path.size() && m_path[0] == path_separator<path_type>::value;
#   endif
    }

    template<class String, class Traits>
    bool basic_path<String, Traits>::has_root_path() const
    {
      return ( m_path.size() 
               && m_path[0] == path_separator<path_type>::value )  // covers both "/" and "//share"
#            ifdef BOOST_WINDOWS_PATH
               || ( m_path.size() > 1
                    && m_path[1] == path_device_delim<path_type>::value ) // "c:" and "c:/"
               || ( m_path.size() > 3
                    && m_path[m_path.size()-1] == path_device_delim<path_type>::value ) // "device:"
#            endif
               ;
    }

    template<class String, class Traits>
    bool basic_path<String, Traits>::has_root_name() const
    {
#   ifdef BOOST_WINDOWS_PATH
      return m_path.size() > 1
        && ( m_path[1] == ':' // "c:"
          || m_path[m_path.size()-1] == path_device_delim<path_type>::value // "prn:"
          || (m_path[0] == path_separator<path_type>::value
            && m_path[1] == path_separator<path_type>::value) // "//share"
           );
#   else
      return false;
#   endif
    }

    template<class String, class Traits>
    bool basic_path<String, Traits>::has_root_directory() const
    {
      return ( m_path.size() 
               && m_path[0] == path_separator<path_type>::value )  // covers both "/" and "//share"
#            ifdef BOOST_WINDOWS_PATH
               || ( m_path.size() > 2
                    && m_path[1] == path_device_delim<path_type>::value
                    && m_path[2] == path_separator<path_type>::value ) // "c:/"
#            endif
               ;
    }


    // append  ---------------------------------------------------------------//
      
    template<class String, class Traits>
    basic_path<String, Traits> & basic_path<String, Traits>::operator /=
      ( const typename string_type::value_type * src_p )
    {
      typename string_type::value_type cur;
      const typename string_type::value_type * next_p = src_p;

      // append path_separator<path_type>::value if needed
      if ( !empty() && *next_p != 0
#     ifdef BOOST_WINDOWS_PATH
        && *(m_path.end()-1) != path_device_delim<path_type>::value
#     endif
        && *(m_path.end()-1) != path_separator<path_type>::value )
          { m_path += path_separator<path_type>::value; }

      // append with leading separator is only legal if m_path is empty
      // or it's Windows and m_path is a drive specifier.
      if ( *next_p == path_separator<path_type>::value
        && !(empty()
#     ifdef BOOST_WINDOWS_PATH
        || (m_path.size() == 2
          && m_path[1] == path_device_delim<path_type>::value)
#     endif
        ) )
        boost::throw_exception( basic_filesystem_error<path_type>(
         "boost::filesystem::basic_path operator/= appending / to",
         *this, path_error ) );

      while ( *next_p != 0 )
      {
        cur = *next_p++;

        // for BOOST_WINDOWS_PATH, convert alt_separator ('\') to separator ('/')
#       ifdef BOOST_WINDOWS_PATH
        if ( cur == path_alt_separator<path_type>::value )
          { cur = path_separator<path_type>::value; }
#       endif

        // appending '/' is an error except as a name separator
        if ( cur == path_separator<path_type>::value
          && m_path.size() > 1
          && m_path[m_path.size()-1] == path_separator<path_type>::value )
          boost::throw_exception( basic_filesystem_error<path_type>(
            "boost::filesystem::basic_path operator/= appending / to",
            *this, path_error ) );
        m_path += cur;
      }

    return *this;
    }

    // normalize  ------------------------------------------------------------//

    template<class String, class Traits>
    basic_path<String, Traits> & basic_path<String, Traits>::normalize()
    {
      static const typename string_type::value_type parent_dir[]
        = { path_separator<path_type>::value,
            path_relative<path_type>::value,
            path_relative<path_type>::value,
            0 };
      if ( m_path.empty() ) return *this;
      bool trailing_separator( 
        m_path[m_path.size()-1] == path_separator<path_type>::value );
      std::string::size_type end, beg(0), start(0);

#   ifdef BOOST_WINDOWS_PATH
      // ignore any drive specifier
      if ( m_path.size() > 2
        && m_path[0] != path_separator<path_type>::value
        && m_path[1] == path_device_delim<path_type>::value )
        { start = 2; }
#     endif

      // ignore '/' or "//name"
      if ( start < m_path.size()
        && m_path[start] == path_separator<path_type>::value )
      {
        ++start;
        if ( start < m_path.size()
          && m_path[start] == path_separator<path_type>::value )
        {
          ++start;
          while ( start < m_path.size()
            && m_path[start] != path_separator<path_type>::value ) ++start;
        }
      }

      // for each "/.."
      beg = start;
      while ( (beg=m_path.find( parent_dir, beg )) != std::string::npos )
      {
        end = beg + 3;
        // cases ".", "..", "/.." where we don't want to erase prior element
        if ( (beg == 1 && m_path[0] == path_relative<path_type>::value)
          || (beg == 2 && m_path[0] == path_relative<path_type>::value
            && m_path[1] == path_relative<path_type>::value)
          || (beg > 2 && m_path[beg-3] == path_separator<path_type>::value
                      && m_path[beg-2] == path_relative<path_type>::value
                      && m_path[beg-1] == path_relative<path_type>::value) )
        {
          beg = end;
          continue;
        }

        // detect name starting with ..
        if ( end < m_path.size()
          && m_path[end] != path_separator<path_type>::value )
          { beg = end; continue; }

        // end is one past end of substr to be erased; now set beg
        while ( beg > start
          && m_path[--beg] != path_separator<path_type>::value ) {}

        // the end should be removed if it is '/'
        bool need_to_backup( false );
        if ( end < m_path.size()
          && m_path[end] == path_separator<path_type>::value )
          { 
            ++end;
            if ( m_path[beg] == path_separator<path_type>::value )
             { ++beg; need_to_backup = true; }
          }

        m_path.erase( beg, end - beg );
        if ( need_to_backup ) --beg;
      }

      // if the path is now empty, it is a reference to the current directory
      if ( m_path.empty() )
      { 
        m_path = path_relative<path_type>::value;
        if ( trailing_separator ) m_path += path_separator<path_type>::value;
      }
      return *this;
    }

    // remove_leaf  ----------------------------------------------------------//

    template<class String, class Traits>
    basic_path<String, Traits> & basic_path<String, Traits>::remove_leaf()
    {
      m_path.erase(
        detail::leaf_pos<String, Traits>( m_path, m_path.size() ) );
      return *this;
    }

    // path conversion functions  --------------------------------------------//

    template<class String, class Traits>
    const String
    basic_path<String, Traits>::file_string() const
    {
#   ifdef BOOST_WINDOWS_PATH
      String s( m_path );
      for ( typename String::iterator itr( s.begin() );
        itr != s.end(); ++itr )
        if ( *itr == path_separator<path_type>::value )
          *itr = path_alt_separator<path_type>::value;
      return s;
#   else
      return m_path;
#   endif
    }

    // iterator functions  ---------------------------------------------------//

    template<class String, class Traits>
    typename basic_path<String, Traits>::iterator basic_path<String, Traits>::begin() const
    {
      iterator itr;
      itr.m_path_ptr = this;
      detail::first_name<String, Traits>( m_path, itr.m_name );
      itr.m_pos = 0;
      return itr;
    }

    template<class String, class Traits>
    typename basic_path<String, Traits>::iterator basic_path<String, Traits>::end() const
      {
        iterator itr;
        itr.m_path_ptr = this;
        itr.m_pos = m_path.size();
        return itr;
      }

    template<class String, class Traits>
    void basic_path<String, Traits>::iterator::increment()
    {
      assert( m_pos < m_path_ptr->m_path.size() ); // detect increment past end
      m_pos += m_name.size();
      if ( m_pos == m_path_ptr->m_path.size() )
      {
        m_name.clear();  // not strictly required, but might aid debugging
        return;
      }
      if ( m_path_ptr->m_path[m_pos] == path_separator<path_type>::value )
      {
#       ifdef BOOST_WINDOWS_PATH
        if ( m_name[m_name.size()-1] == path_device_delim<path_type>::value // drive or device
          || (m_name[0] == '/' && m_name[1]
            == path_separator<path_type>::value) ) // share
        {
          m_name = path_separator<path_type>::value;
          return;
        }
#       endif
        ++m_pos;
      }
      typename String::size_type end_pos(
        m_path_ptr->m_path.find( path_separator<path_type>::value, m_pos ) );
      if ( end_pos == std::string::npos )
        end_pos = m_path_ptr->m_path.size();
      m_name = m_path_ptr->m_path.substr( m_pos, end_pos - m_pos );
    }

    template<class String, class Traits>
    void basic_path<String, Traits>::iterator::decrement()
    {                                                                                
      assert( m_pos ); // detect decrement of begin
      typename String::size_type end_pos( m_pos );

      // skip a '/' unless it is a root directory
      if ( m_path_ptr->m_path[end_pos-1] == path_separator<path_type>::value
        && !detail::is_absolute_root<String, Traits>(
          m_path_ptr->m_path, end_pos ) ) --end_pos;
      m_pos = detail::leaf_pos<String, Traits>( m_path_ptr->m_path, end_pos );
      m_name = m_path_ptr->m_path.substr( m_pos, end_pos - m_pos );
    }

    //  basic_filesystem_error implementation --------------------------------//

    template<class Path>
    basic_filesystem_error<Path>::basic_filesystem_error(
      const std::string & what )
      : m_sys_err(0), m_err(other_error)
    {
      try
      {
        m_imp_ptr.reset( new m_imp );
        m_imp_ptr->m_what = what;
      }
      catch (...) { m_imp_ptr.reset(); }
    }

    template<class Path>
    basic_filesystem_error<Path>::basic_filesystem_error(
      const std::string & what, system_error_type sys_ec )
      : m_sys_err(sys_ec), m_err(lookup_error_code(sys_ec))
    {
      try
      {
        m_imp_ptr.reset( new m_imp );
        m_imp_ptr->m_what = what;
      }
      catch (...) { m_imp_ptr.reset(); }
    }

    template<class Path>
    basic_filesystem_error<Path>::basic_filesystem_error(
      const std::string & what, const path_type & path1, error_code ec )
      : m_sys_err(0), m_err(ec)
    {
      try
      {
        m_imp_ptr.reset( new m_imp );
        m_imp_ptr->m_what = what;
        m_imp_ptr->m_path1 = path1;
      }
      catch (...) { m_imp_ptr.reset(); }
    }

    template<class Path>
    basic_filesystem_error<Path>::basic_filesystem_error(
      const std::string & what, const path_type & path1,
      system_error_type sys_err_code )
      : m_sys_err(sys_err_code), m_err(lookup_error_code(sys_err_code))
    {
      try
      {
        m_imp_ptr.reset( new m_imp );
        m_imp_ptr->m_what = what;
        m_imp_ptr->m_path1 = path1;
      }
      catch (...) { m_imp_ptr.reset(); }
    }

    template<class Path>
    basic_filesystem_error<Path>::basic_filesystem_error(
      const std::string & what, const path_type & path1,
      const path_type & path2, system_error_type sys_err_code )
      : m_sys_err(sys_err_code), m_err(lookup_error_code(sys_err_code))
    {
      try
      {
        m_imp_ptr.reset( new m_imp );
        m_imp_ptr->m_what = what;
        m_imp_ptr->m_path1 = path1;
        m_imp_ptr->m_path2 = path2;
      }
      catch (...) { m_imp_ptr.reset(); }
    }

    namespace detail
    {
      BOOST_FILESYSTEM_DECL void
      what_formatter( system_error_type sys_err_code,
        const std::string & p1, const std::string & p2, std::string & target );

#   ifdef BOOST_WINDOWS_API
      BOOST_FILESYSTEM_DECL void
      what_formatter( system_error_type sys_err_code,
        const std::wstring &, const std::wstring &, std::string & target );
#   endif
    } // namespace detail

    template<class Path>
    const char * basic_filesystem_error<Path>::what() const throw()
    {
      if ( !m_imp_ptr.get() ) return "filesystem error";
      try 
      {
        detail::what_formatter( m_sys_err,
          m_imp_ptr->m_path1.external_file_string(),
          m_imp_ptr->m_path2.external_file_string(),
          m_imp_ptr->m_what );
      }
      catch (...) {}
      return m_imp_ptr->m_what.c_str();
    }

  } // namespace filesystem
} // namespace boost

#include <boost/config/abi_suffix.hpp> // pops abi_prefix.hpp pragmas

#endif // BOOST_FILESYSTEM_PATH_HPP
