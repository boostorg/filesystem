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
#include <boost/filesystem/mutable_constant.hpp>
#include <boost/iterator/iterator_facade.hpp>
#include <boost/throw_exception.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/type_traits/is_same.hpp>
#include <boost/static_assert.hpp>
#include <string>
#include <algorithm> // for lexicographical_compare
#include <stdexcept>
#include <cassert>

# ifndef BOOST_FILESYSTEM_NARROW_ONLY
#   include <locale>
# endif

#include <boost/config/abi_prefix.hpp> // must be the last #include

//----------------------------------------------------------------------------//

namespace boost
{
  namespace filesystem
  {
    struct path_traits;

    template<class String, class Traits> class basic_path;
    typedef basic_path< std::string, path_traits >    path;

    struct path_traits
    {
      typedef std::string internal_string_type;
      typedef std::string external_string_type;
      static external_string_type to_external( const path &,
        const internal_string_type & src ) { return src; }
      static internal_string_type to_internal(
        const external_string_type & src ) { return src; }
    };


# ifndef BOOST_FILESYSTEM_NARROW_ONLY

    struct wpath_traits;
    typedef basic_path< std::wstring, wpath_traits >  wpath;

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
      BOOST_FILESYSTEM_DECL static void imbue( const std::locale & loc );
      BOOST_FILESYSTEM_DECL static bool imbue( const std::locale & loc,
        const std::nothrow_t & );
    };
# endif // ifndef BOOST_FILESYSTEM_NARROW_ONLY

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
      not_ready_error,  // ditto
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

# if defined(BOOST_WINDOWS_API) && !defined(BOOST_FILESYSTEM_NARROW_ONLY)
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

      explicit basic_filesystem_error(
        const std::string & what, system_error_type sys_ec = 0 );

      basic_filesystem_error( const std::string & what,
        const path_type & path1, system_error_type sys_err_code );

      basic_filesystem_error( const std::string & what, const path_type & path1,
        const path_type & path2, system_error_type sys_err_code );

      ~basic_filesystem_error() throw() {}

      virtual const char * what() const throw();

      system_error_type  system_error() const { return m_sys_err; }
      // Note: system_error() == 0 implies a library (rather than system) error

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
    };

    typedef basic_filesystem_error<path> filesystem_error;

# ifndef BOOST_FILESYSTEM_NARROW_ONLY
    typedef basic_filesystem_error<wpath> wfilesystem_error;
# endif

    //  path traits  ---------------------------------------------------------//

    template<class Path> struct is_basic_path
      { BOOST_STATIC_CONSTANT( bool, value = false ); };
    template<> struct is_basic_path<path>
      { BOOST_STATIC_CONSTANT( bool, value = true ); };
# ifndef BOOST_FILESYSTEM_NARROW_ONLY
    template<> struct is_basic_path<wpath>
      { BOOST_STATIC_CONSTANT( bool, value = true ); };
# endif

    // these only have to be specialized if Path::string_type::value_type
    // is not convertible from char
    template<class Path> struct path_separator
      { BOOST_STATIC_CONSTANT( char, value = '/' ); };

    template<class Path> struct path_relative
      { BOOST_STATIC_CONSTANT( char, value = '.' ); };

# ifdef BOOST_WINDOWS_PATH
    template<class Path> struct path_alt_separator
      { BOOST_STATIC_CONSTANT( char, value = '\\' ); };

    template<class Path> struct path_device_delim
      { BOOST_STATIC_CONSTANT( char, value = ':' ); };
# endif

    //  workaround for VC++ 7.0 and earlier issues with nested classes
    namespace detail
    {
      template<class Path>
      class iterator_helper
      {
      public:
        static void do_increment( typename Path::iterator  & ph );
        static void do_decrement( typename Path::iterator  & ph );
      };
    }

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

      basic_path( const string_type & str )
        { operator/=( str ); }
      basic_path( const typename string_type::value_type * s )
        { operator/=( s ); }

      ~basic_path() {}

      // append operations:
      basic_path & operator /=( const typename string_type::value_type * rhs );
      basic_path & operator /=( const basic_path & rhs )
        { return operator /=( rhs.string().c_str() ); }
      basic_path & operator /=( const string_type & rhs )
       { return operator /=( rhs.c_str() ); }

      basic_path operator /( const basic_path & rhs ) const
        { return basic_path<String, Traits>( *this ) /= rhs; }
      basic_path operator /( const string_type & rhs ) const
        { return basic_path<String, Traits>( *this ) /= rhs; }
      basic_path operator /( const typename string_type::value_type * rhs ) const
        { return basic_path<String, Traits>( *this ) /= rhs; }

      // modification functions:
      basic_path & canonize();
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

        const string_type & dereference() const
          { return m_name; }
        bool equal( const iterator & rhs ) const
          { return m_path_ptr == rhs.m_path_ptr && m_pos == rhs.m_pos; }

        friend class boost::filesystem::detail::iterator_helper<path_type>;

        void increment()
        { 
          boost::filesystem::detail::iterator_helper<path_type>::do_increment(
            *this );
        }
        void decrement()
        { 
          boost::filesystem::detail::iterator_helper<path_type>::do_decrement(
            *this );
        }

        string_type             m_name;     // current element
        const basic_path *      m_path_ptr; // path being iterated over
        typename string_type::size_type  m_pos;  // position of name in
                                            // path_ptr->string(). The
                                            // end() iterator is indicated by 
                                            // pos == path_ptr->m_path.size()
      }; // iterator

      typedef iterator const_iterator;

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

      string_type  m_path; // invariant: portable generic path grammar

      // Was qualified; como433beta8 reports:
      //    warning #427-D: qualified name is not allowed in member declaration 
      friend class iterator;
      friend class boost::filesystem::detail::iterator_helper<path_type>;

      // Deprecated features ease transition for existing code. Don't use these
      // in new code.
# ifndef BOOST_FILESYSTEM_NO_DEPRECATED
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
# endif
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

    BOOST_FILESYSTEM_DECL bool portable_posix_name( const std::string & name );
    BOOST_FILESYSTEM_DECL bool windows_name( const std::string & name );
    BOOST_FILESYSTEM_DECL bool portable_name( const std::string & name );
    BOOST_FILESYSTEM_DECL bool portable_directory_name( const std::string & name );
    BOOST_FILESYSTEM_DECL bool portable_file_name( const std::string & name );
    BOOST_FILESYSTEM_DECL bool native( const std::string & name );
      // native(name) must return true for any name which MIGHT be valid
      // on the native platform.
    inline bool no_check( const std::string & )
      { return true; }

// implementation  -----------------------------------------------------------//

    namespace detail
    {

      //  is_separator helper ------------------------------------------------//

      template<class Path>
      inline  bool is_separator( typename Path::string_type::value_type c )
      {
        return c == path_separator<Path>::value
#     ifdef BOOST_WINDOWS_PATH
          || c == path_alt_separator<Path>::value
#     endif
          ;
      }

      // leaf_pos helper  ----------------------------------------------------//

      template<class String, class Traits>
      typename String::size_type leaf_pos(
        const String & str, // precondition: portable generic path grammar
        typename String::size_type end_pos ) // end_pos is past-the-end position
      // return 0 if str itself is leaf (or empty)
      {
        typedef typename boost::filesystem::basic_path<String, Traits> path_type;

        // case: "//"
        if ( end_pos == 2 
          && str[0] == path_separator<path_type>::value
          && str[1] == path_separator<path_type>::value ) return 0;

        // case: ends in "/"
        if ( end_pos && str[end_pos-1] == path_separator<path_type>::value )
          return end_pos-1;
        
        // set pos to start of last element
        typename String::size_type pos(
          str.find_last_of( path_separator<path_type>::value, end_pos-1 ) );
#       ifdef BOOST_WINDOWS_PATH
        if ( pos == String::npos )
          pos = str.find_last_of( path_alt_separator<path_type>::value, end_pos-1 );
        if ( pos == String::npos )
          pos = str.find_last_of( path_device_delim<path_type>::value, end_pos-2 );
#       endif

        return ( pos == String::npos // path itself must be a leaf (or empty)
          || (pos == 1 && str[0] == path_separator<path_type>::value) ) // or net
            ? 0 // so leaf is entire string
            : pos + 1; // or starts after delimiter
      }

      // first_element helper  -----------------------------------------------//
      //   sets pos and len of first element, excluding extra separators
      //   if src.empty(), sets pos,len, to 0,0.

      template<class String, class Traits>
        void first_element(
          const String & src, // precondition: portable generic path grammar
          typename String::size_type & element_pos,
          typename String::size_type & element_size,
          typename String::size_type size = -1
          )
      {
        if ( size == -1 ) size = src.size();
        element_pos = 0;
        element_size = 0;
        if ( src.empty() ) return;

        typedef typename boost::filesystem::basic_path<String, Traits> path_type;

        typename String::size_type cur(0);
        
        // deal with // [network]
        if ( size >= 2 && src[0] == path_separator<path_type>::value
          && src[1] == path_separator<path_type>::value
          && (size == 2
            || src[2] != path_separator<path_type>::value) )
        { 
          cur += 2;
          element_size += 2;
        }

        // leading (not non-network) separator
        else if ( src[0] == path_separator<path_type>::value )
        {
          ++element_size;
          // bypass extra leading separators
          while ( cur+1 < size
            && src[cur+1] == path_separator<path_type>::value )
          {
            ++cur;
            ++element_pos;
          }
          return;
        }

        // at this point, we have either a plain name, a network name,
        // or (on Windows only) a device name

        // find the end
        while ( cur < size
#         ifdef BOOST_WINDOWS_PATH
          && src[cur] != path_device_delim<path_type>::value
#         endif
          && src[cur] != path_separator<path_type>::value )
        {
          ++cur;
          ++element_size;
        }

#       ifdef BOOST_WINDOWS_PATH
        if ( cur == size ) return;
        // include device delimiter
        if ( src[cur] == path_device_delim<path_type>::value )
          { ++element_size; }
#       endif

        return;
      }

      // root_directory_start helper  ----------------------------------------//

      template<class String, class Traits>
      typename String::size_type root_directory_start(
        const String & s, // precondition: portable generic path grammar
        typename String::size_type size )
      // return npos if no root_directory found
      {
        typedef typename boost::filesystem::basic_path<String, Traits> path_type;

#     ifdef BOOST_WINDOWS_PATH
        // case "c:/"
        if ( size > 2
          && s[1] == path_device_delim<path_type>::value
          && s[2] == path_separator<path_type>::value ) return 2;
#     endif

        // case "//"
        if ( size == 2
          && s[0] == path_separator<path_type>::value
          && s[1] == path_separator<path_type>::value ) return String::npos;

        // case "//net {/}"
        if ( size > 3
          && s[0] == path_separator<path_type>::value
          && s[1] == path_separator<path_type>::value
          && s[2] != path_separator<path_type>::value )
        {
          typename String::size_type pos(
            s.find( path_separator<path_type>::value, 2 ) );
          return pos < size ? pos : String::npos;
        }
        
        // case "/"
        if ( size > 0 && s[0] == path_separator<path_type>::value ) return 0;

        return String::npos;

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

      bool leaf_was_separator( m_path.size()
        && m_path[end_pos] == path_separator<path_type>::value );

      // skip separators unless root directory
      string_type::size_type root_dir_pos( detail::root_directory_start
        <string_type, traits_type>( m_path, end_pos ) );
      for ( ; 
        end_pos > 0
        && (end_pos-1) != root_dir_pos
        && m_path[end_pos-1] == path_separator<path_type>::value
        ;
        --end_pos ) {}

     return (end_pos == 1 && root_dir_pos == 0 && leaf_was_separator)
       ? path_type()
       : path_type( m_path.substr( 0, end_pos ) );
    }

    template<class String, class Traits>
    basic_path<String, Traits> basic_path<String, Traits>::relative_path() const
    {
      iterator itr( begin() );
      for ( ; itr.m_pos != m_path.size()
          && itr.m_name[0] == path_separator<path_type>::value
#     ifdef BOOST_WINDOWS_PATH
          || itr.m_name[itr.m_name.size()-1]
            == path_device_delim<path_type>::value
#     endif
        ; ++itr ) {}

      return basic_path<String, Traits>( m_path.substr( itr.m_pos ) );
    }

    template<class String, class Traits>
    String basic_path<String, Traits>::root_name() const
    {
      iterator itr( begin() );

      return ( itr.m_pos != m_path.size()
        && (
            ( itr.m_name.size() > 1
              && itr.m_name[0] == path_separator<path_type>::value
              && itr.m_name[1] == path_separator<path_type>::value
            )
#     ifdef BOOST_WINDOWS_PATH
          || itr.m_name[itr.m_name.size()-1]
            == path_device_delim<path_type>::value
#     endif
           ) )
        ? *itr
        : String();
    }

    template<class String, class Traits>
    String basic_path<String, Traits>::root_directory() const
    {
      string_type::size_type start(
        detail::root_directory_start<String, Traits>( m_path, m_path.size() ) );

      return start == string_type::npos
        ? string_type()
        : m_path.substr( start, 1 );
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
    inline bool basic_path<String, Traits>::is_complete() const
    {
#   ifdef BOOST_WINDOWS_PATH
      return has_root_name() && has_root_directory();
#   else
      return has_root_directory();
#   endif
    }

    template<class String, class Traits>
    inline bool basic_path<String, Traits>::has_root_path() const
    {
      return !root_path().empty();
    }

    template<class String, class Traits>
    inline bool basic_path<String, Traits>::has_root_name() const
    {
      return !root_name().empty();
    }

    template<class String, class Traits>
    inline bool basic_path<String, Traits>::has_root_directory() const
    {
      return !root_directory().empty();
    }

    // append  ---------------------------------------------------------------//
      
    template<class String, class Traits>
    basic_path<String, Traits> & basic_path<String, Traits>::operator /=
      ( const typename string_type::value_type * next_p )
    {
      // append path_separator<path_type>::value if needed
      if ( !empty() && *next_p != 0
        && !detail::is_separator<path_type>( *next_p )
#     ifdef BOOST_WINDOWS_PATH
        && *(m_path.end()-1) != path_device_delim<path_type>::value
#     endif
        && *(m_path.end()-1) != path_separator<path_type>::value )
          { m_path += path_separator<path_type>::value; }

      for ( ; *next_p != 0; ++next_p )
      {
#     ifdef BOOST_WINDOWS_PATH
        // for BOOST_WINDOWS_PATH, convert alt_separator ('\') to separator ('/')
        if ( *next_p == path_alt_separator<path_type>::value )
        {
          m_path += path_separator<path_type>::value;
        }
        else m_path += *next_p;
#     else
        m_path += *next_p;
#     endif
      }

    return *this;
    }

    // canonize  ------------------------------------------------------------//

    template<class String, class Traits>
    basic_path<String, Traits> & basic_path<String, Traits>::canonize()
    {
      static const string_type::value_type dot[]
        = { path_relative<path_type>::value, 0 };

      if ( m_path.empty() ) return *this;
        
      path_type temp;

      for ( iterator itr( begin() ); itr != end(); ++itr )
      {
        temp /= *itr;
      };

      if ( temp.empty() ) temp /= dot;
      m_path = temp.m_path;
      return *this;
    }

    // normalize  ------------------------------------------------------------//

    template<class String, class Traits>
    basic_path<String, Traits> & basic_path<String, Traits>::normalize()
    {
      static const string_type::value_type dot[]
        = { path_relative<path_type>::value, 0 };

      if ( m_path.empty() ) return *this;
        
      path_type temp;
      iterator start( begin() );
      iterator last( end() );
      iterator stop( last-- );
      for ( iterator itr( start ); itr != stop; ++itr )
      {
        // ignore "." except at start and last
        if ( itr->size() == 1
          && (*itr)[0] == path_relative<path_type>::value
          && itr != start
          && itr != last ) continue;

        // ignore a name and following ".."
        if ( !temp.empty()
          && itr->size() == 2
          && (*itr)[0] == path_relative<path_type>::value
          && (*itr)[1] == path_relative<path_type>::value ) // dot dot
        {
          string_type lf( temp.leaf() );  
          if ( lf.size() > 0  
            && (lf.size() != 1
              || (lf[0] != path_relative<path_type>::value
                && lf[0] != path_separator<path_type>::value))
            && (lf.size() != 2 
              || (lf[0] != path_relative<path_type>::value
                && lf[1] != path_relative<path_type>::value
#             ifdef BOOST_WINDOWS_PATH
                && lf[1] != path_device_delim<path_type>::value
#             endif
                 )
               )
            )
          {
            temp.remove_leaf();
            // if not root directory, must also remove "/" if any
            if ( temp.m_path.size() > 0
              && temp.m_path[temp.m_path.size()-1]
                == path_separator<path_type>::value )
            {
              string_type::size_type rds(
                detail::root_directory_start<String,Traits>( temp.m_path,
                  temp.m_path.size() ) );
              if ( rds == string_type::npos
                || rds != temp.m_path.size()-1 ) 
                { temp.m_path.erase( temp.m_path.size()-1 ); }
            }

            iterator next( itr );
            if ( temp.empty() && ++next != stop
              && next == last && *last == dot ) temp /= dot;
            continue;
          }
        }

        temp /= *itr;
      };

      if ( temp.empty() ) temp /= dot;
      m_path = temp.m_path;
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
      // for Windows, use the alternate separator, and bypass extra 
      // root separators

      string_type::size_type root_dir_start(
        detail::root_directory_start<String, Traits>( m_path, m_path.size() ) );
      bool in_root( root_dir_start != string_type::npos );
      String s;
      for ( string_type::size_type pos( 0 );
        pos != m_path.size(); ++pos )
      {
        // special case // [net]
        if ( pos == 0 && m_path.size() > 1
          && m_path[0] == path_separator<path_type>::value
          && m_path[1] == path_separator<path_type>::value
          && ( m_path.size() == 2 
            || !detail::is_separator<path_type>( m_path[2] )
             ) )
        {
          ++pos;
          s += path_alt_separator<path_type>::value;
          s += path_alt_separator<path_type>::value;
          continue;
        }   

        // bypass extra root separators
        if ( in_root )
        { 
          if ( s.size() > 0
            && s[s.size()-1] == path_alt_separator<path_type>::value
            && m_path[pos] == path_separator<path_type>::value
            ) continue;
        }

        if ( m_path[pos] == path_separator<path_type>::value )
          s += path_alt_separator<path_type>::value;
        else
          s += m_path[pos];

        if ( pos > root_dir_start
          && m_path[pos] == path_separator<path_type>::value )
          { in_root = false; }
      }
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
      string_type::size_type element_size;
      detail::first_element<String, Traits>( m_path, itr.m_pos, element_size );
      itr.m_name = m_path.substr( itr.m_pos, element_size );
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

    namespace detail
    {
      //  do_increment  ------------------------------------------------------//

      template<class Path>
      void iterator_helper<Path>::do_increment( typename Path::iterator & ph )
      {
        assert( ph.m_pos < ph.m_path_ptr->m_path.size() && "basic_path::iterator increment past end()" );

        bool was_net( ph.m_name.size() > 2
          && ph.m_name[0] == path_separator<Path>::value
          && ph.m_name[1] == path_separator<Path>::value
          && ph.m_name[2] != path_separator<Path>::value );

        ph.m_pos += ph.m_name.size();
        if ( ph.m_pos == ph.m_path_ptr->m_path.size() ) // end
        {
          ph.m_name.clear();  // not strictly required, but might aid debugging
          return;
        }

        if ( ph.m_path_ptr->m_path[ph.m_pos] == path_separator<Path>::value )
        {
          if ( was_net
  #       ifdef BOOST_WINDOWS_PATH
            // case "c:/"
            || ph.m_name[ph.m_name.size()-1] == path_device_delim<Path>::value
  #       endif
             )
          {
            ph.m_name = path_separator<Path>::value;
            return;
          }

          while ( ph.m_pos != ph.m_path_ptr->m_path.size()
            && ph.m_path_ptr->m_path[ph.m_pos] == path_separator<Path>::value )
            { ++ph.m_pos; }

          // treat trailing "/" as if "/.", per POSIX spec
          if ( ph.m_pos == ph.m_path_ptr->m_path.size() ) 
          {
            --ph.m_pos;
            ph.m_name = path_relative<Path>::value;
            return;
          }
        }

        typedef typename Path::string_type string_type;
        string_type::size_type end_pos(
          ph.m_path_ptr->m_path.find( path_separator<Path>::value, ph.m_pos ) );
#     ifdef BOOST_WINDOWS_PATH
        if ( end_pos == string_type::npos )
          end_pos =
            ph.m_path_ptr->m_path.find( path_alt_separator<Path>::value, ph.m_pos );
#     endif
        ph.m_name = ph.m_path_ptr->m_path.substr( ph.m_pos, end_pos - ph.m_pos );
      } 

      //  do_decrement  ------------------------------------------------------//

      template<class Path>
      void iterator_helper<Path>::do_decrement( typename Path::iterator & ph )
      {                                                                                
        assert( ph.m_pos && "basic_path::iterator decrement past begin()"  );

        typedef typename Path::string_type string_type;

        static const string_type::value_type separators[] = {
          path_separator<Path>::value,
#       ifdef BOOST_WINDOWS_PATH
          path_alt_separator<Path>::value,
#       endif
          0 };

        string_type::size_type end_pos( ph.m_pos );

        string_type::size_type root_dir_pos( detail::root_directory_start
          <string_type, typename Path::traits_type>(
            ph.m_path_ptr->m_path, end_pos ) );

        // if at end and there was a trailing non-root '/', return "."
        if ( ph.m_pos == ph.m_path_ptr->m_path.size()
          && ph.m_path_ptr->m_path.size() > 1
          && ph.m_path_ptr->m_path[ph.m_pos-1] == path_separator<Path>::value
          && (root_dir_pos == string_type::npos // there is no root
          || ph.m_path_ptr->m_path.find_first_not_of( separators,
            root_dir_pos ) != string_type::npos) // this is not the root
           )
        {
          --ph.m_pos;
            ph.m_name = path_relative<Path>::value;
            return;
        }

        // skip separators unless root directory
        for ( 
          ; 
          end_pos > 0
          && (end_pos-1) != root_dir_pos
          && ph.m_path_ptr->m_path[end_pos-1] == path_separator<Path>::value
          ;
          --end_pos ) {}

        ph.m_pos = detail::leaf_pos
          <typename Path::string_type, typename Path::traits_type>
            ( ph.m_path_ptr->m_path, end_pos );
        ph.m_name = ph.m_path_ptr->m_path.substr( ph.m_pos, end_pos - ph.m_pos );
      }
    } // namespace detail

    //  basic_filesystem_error implementation --------------------------------//

    template<class Path>
    basic_filesystem_error<Path>::basic_filesystem_error(
      const std::string & what, system_error_type sys_ec )
      : m_sys_err(sys_ec)
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
      const std::string & what, const path_type & path1,
      system_error_type sys_err_code )
      : m_sys_err(sys_err_code)
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
      : m_sys_err(sys_err_code)
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

#   if defined(BOOST_WINDOWS_API) && !defined(BOOST_FILESYSTEM_NARROW_ONLY)
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
