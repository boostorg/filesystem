//  boost/filesystem/fstream.hpp  --------------------------------------------//

//  Copyright Beman Dawes 2002.
//  Use, modification, and distribution is subject to the Boost Software
//  License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

//  See library home page at http://www.boost.org/libs/filesystem

//----------------------------------------------------------------------------// 

#ifndef BOOST_FILESYSTEM_FSTREAM_HPP
#define BOOST_FILESYSTEM_FSTREAM_HPP

#include <boost/filesystem/path.hpp>
#include <boost/utility/enable_if.hpp>
#include <boost/detail/workaround.hpp>

#include <iosfwd>
#include <fstream>

#include <boost/config/abi_prefix.hpp> // must be the last #include

// NOTE: fstream.hpp for Boost 1.32.0 and earlier supplied workarounds for
// various compiler problems. They have been removed to ease development of the
// basic i18n functionality. Once the new interface is stable, the workarounds
// will be reinstated for any compilers that otherwise can support the rest of
// the library after internationalization.

namespace boost
{
  namespace filesystem
  {
    namespace detail
    {
#   ifdef BOOST_WINDOWS_API
      // The 8.3 hack:
      // C++98 does not supply a wchar_t open, so try to get an equivalent
      // narrow char name based on the short, so-called 8.3, name.
      BOOST_FILESYSTEM_DECL  boost::filesystem::status_flag
        status_api( const std::wstring & ph,
                    boost::filesystem::system_error_type * ec );
      BOOST_FILESYSTEM_DECL boost::filesystem::system_error_type
        remove_api( const std::wstring & ph );
      BOOST_FILESYSTEM_DECL bool create_file_api( const std::wstring & ph,
        std::ios_base::openmode mode ); // true if succeeds
      BOOST_FILESYSTEM_DECL std::string narrow_path_api(
        const std::wstring & ph ); // return is empty if fails

      std::string narrow_path( const std::wstring & file_ph,
        std::ios_base::openmode mode )
      //   return will be empty() if cannot supply narrow short path
      {
        std::string narrow_ph;
        bool created_file( false );
        if ( status_api( file_ph, 0 ) == boost::filesystem::not_found_flag )
        {
          if ( (mode & std::ios_base::out) == 0 
            || !create_file_api( file_ph, mode ) ) return narrow_ph;
          created_file = true;
        }
        narrow_ph = narrow_path_api( file_ph );
        if ( narrow_ph.empty()&& created_file ) remove_api( file_ph );
        return narrow_ph;
      }
#   endif 

      inline const std::string & narrow_path( const std::string & file_ph,
        std::ios_base::openmode )
        { return file_ph; }

    } // namespace detail

    template < class charT, class traits = std::char_traits<charT> >
    class basic_filebuf : public std::basic_filebuf<charT,traits>
    {
    private: // disallow copying
      basic_filebuf( const basic_filebuf & );
      const basic_filebuf & operator=( const basic_filebuf & ); 
    public:
      basic_filebuf() {}
      virtual ~basic_filebuf() {}

      template<class Path>
      typename boost::enable_if<is_basic_path<Path>,
        basic_filebuf<charT,traits> *>::type
      open( const Path & file_ph, std::ios_base::openmode mode );

      basic_filebuf<charT,traits> *
      open( const path & file_ph, std::ios_base::openmode mode );

      basic_filebuf<charT,traits> *
      open( const wpath & file_ph, std::ios_base::openmode mode );
    };

    typedef basic_filebuf<char> filebuf;
    typedef basic_filebuf<wchar_t> wfilebuf;

    template < class charT, class traits = std::char_traits<charT> >
    class basic_ifstream : public std::basic_ifstream<charT,traits>
    {
    private: // disallow copying
      basic_ifstream( const basic_ifstream & );
      const basic_ifstream & operator=( const basic_ifstream & ); 
    public:
      basic_ifstream() {}

      // use two signatures, rather than one signature with default second
      // argument, to workaround VC++ 7.1 bug (ID VSWhidbey 38416)

      template<class Path>
      explicit basic_ifstream( const Path & file_ph,
        typename boost::enable_if<is_basic_path<Path> >::type* dummy = 0 );
      explicit basic_ifstream( const path & file_ph );
      explicit basic_ifstream( const wpath & file_ph );

      template<class Path>
      basic_ifstream( const Path & file_ph, std::ios_base::openmode mode,
        typename boost::enable_if<is_basic_path<Path> >::type* dummy = 0 );
      basic_ifstream( const path & file_ph, std::ios_base::openmode mode );
      basic_ifstream( const wpath & file_ph, std::ios_base::openmode mode );

      template<class Path>
      typename boost::enable_if<is_basic_path<Path>, void>::type
      open( const Path & file_ph );
      void open( const path & file_ph );
      void open( const wpath & file_ph );

      template<class Path>
      typename boost::enable_if<is_basic_path<Path>, void>::type
      open( const Path & file_ph, std::ios_base::openmode mode );
      void open( const path & file_ph, std::ios_base::openmode mode );
      void open( const wpath & file_ph, std::ios_base::openmode mode );

      virtual ~basic_ifstream() {}
    };

    typedef basic_ifstream<char> ifstream;
    typedef basic_ifstream<wchar_t> wifstream;

    template < class charT, class traits = std::char_traits<charT> >
    class basic_ofstream : public std::basic_ofstream<charT,traits>
    {
    private: // disallow copying
      basic_ofstream( const basic_ofstream & );
      const basic_ofstream & operator=( const basic_ofstream & ); 
    public:
      basic_ofstream() {}

      // use two signatures, rather than one signature with default second
      // argument, to workaround VC++ 7.1 bug (ID VSWhidbey 38416)

      template<class Path>
      explicit basic_ofstream( const Path & file_ph,
        typename boost::enable_if<is_basic_path<Path> >::type* dummy = 0 );
      explicit basic_ofstream( const path & file_ph );
      explicit basic_ofstream( const wpath & file_ph );

      template<class Path>
      basic_ofstream( const Path & file_ph, std::ios_base::openmode mode,
        typename boost::enable_if<is_basic_path<Path> >::type* dummy = 0 );
      basic_ofstream( const path & file_ph, std::ios_base::openmode mode );
      basic_ofstream( const wpath & file_ph, std::ios_base::openmode mode );

      template<class Path>
      typename boost::enable_if<is_basic_path<Path>, void>::type
      open( const Path & file_ph );
      void open( const path & file_ph );
      void open( const wpath & file_ph );

      template<class Path>
      typename boost::enable_if<is_basic_path<Path>, void>::type
      open( const Path & file_ph, std::ios_base::openmode mode );
      void open( const path & file_ph, std::ios_base::openmode mode );
      void open( const wpath & file_ph, std::ios_base::openmode mode );

      virtual ~basic_ofstream() {}
    };

    typedef basic_ofstream<char> ofstream;
    typedef basic_ofstream<wchar_t> wofstream;

    template < class charT, class traits = std::char_traits<charT> >
    class basic_fstream : public std::basic_fstream<charT,traits>
    {
    private: // disallow copying
      basic_fstream( const basic_fstream & );
      const basic_fstream & operator=( const basic_fstream & ); 
    public:
      basic_fstream() {}

      // use two signatures, rather than one signature with default second
      // argument, to workaround VC++ 7.1 bug (ID VSWhidbey 38416)

      template<class Path>
      explicit basic_fstream( const Path & file_ph,
        typename boost::enable_if<is_basic_path<Path> >::type* dummy = 0 );
      explicit basic_fstream( const path & file_ph );
      explicit basic_fstream( const wpath & file_ph );

      template<class Path>
      basic_fstream( const Path & file_ph, std::ios_base::openmode mode,
        typename boost::enable_if<is_basic_path<Path> >::type* dummy = 0 );
      basic_fstream( const path & file_ph, std::ios_base::openmode mode );
      basic_fstream( const wpath & file_ph, std::ios_base::openmode mode );

      template<class Path>
      typename boost::enable_if<is_basic_path<Path>, void>::type
      open( const Path & file_ph );
      void open( const path & file_ph );
      void open( const wpath & file_ph );

      template<class Path>
      typename boost::enable_if<is_basic_path<Path>, void>::type
      open( const Path & file_ph, std::ios_base::openmode mode );
      void open( const path & file_ph, std::ios_base::openmode mode );
      void open( const wpath & file_ph, std::ios_base::openmode mode );

      virtual ~basic_fstream() {}
    };
 
    typedef basic_fstream<char> fstream;
    typedef basic_fstream<wchar_t> wfstream;

//  basic_filebuf definitions  -----------------------------------------------//

    template <class charT, class traits>
    template<class Path>
    typename boost::enable_if<is_basic_path<Path>,
      basic_filebuf<charT,traits> *>::type
    basic_filebuf<charT,traits>::open( const Path & file_ph,
      std::ios_base::openmode mode )
    {
      
      std::string narrow_ph( detail::narrow_path(
        file_ph.external_file_string(), mode ) );
      return narrow_ph.empty() 
        || (std::basic_filebuf<charT,traits>::open( narrow_ph.c_str(), mode )
          == 0) ? 0 : this;
    }

    template <class charT, class traits>
    basic_filebuf<charT,traits> *
    basic_filebuf<charT, traits>::open( const path & file_ph,
      std::ios_base::openmode mode )
    {
      return std::basic_filebuf<charT,traits>::open(
        file_ph.file_string().c_str(), mode ) == 0 ? 0 : this;
    }

    template <class charT, class traits>
    basic_filebuf<charT,traits> *
    basic_filebuf<charT, traits>::open( const wpath & file_ph,
      std::ios_base::openmode mode )
    {
      return open<wpath>( file_ph, mode );
    }

//  basic_ifstream definitions  ----------------------------------------------//

    template <class charT, class traits> template<class Path>
    basic_ifstream<charT,traits>::basic_ifstream(const Path & file_ph,
      typename boost::enable_if<is_basic_path<Path> >::type* )
      : std::basic_ifstream<charT,traits>(
        detail::narrow_path( file_ph.external_file_string(),
          std::ios_base::in ).c_str(), std::ios_base::in ) {}

    template <class charT, class traits>
    basic_ifstream<charT,traits>::basic_ifstream( const path & file_ph )
      : std::basic_ifstream<charT,traits>(
          file_ph.file_string().c_str(), std::ios_base::in ) {}

    template <class charT, class traits>
    basic_ifstream<charT,traits>::basic_ifstream( const wpath & file_ph )
      : std::basic_ifstream<charT,traits>(
        detail::narrow_path( file_ph.external_file_string(),
          std::ios_base::in ).c_str(), std::ios_base::in ) {}

    template <class charT, class traits> template<class Path>
    basic_ifstream<charT,traits>::basic_ifstream( const Path & file_ph,
      std::ios_base::openmode mode,
      typename boost::enable_if<is_basic_path<Path> >::type* )
      : std::basic_ifstream<charT,traits>(
        detail::narrow_path( file_ph.external_file_string(),
          mode ).c_str(), mode ) {}

    template <class charT, class traits>
    basic_ifstream<charT,traits>::basic_ifstream( const path & file_ph,
      std::ios_base::openmode mode )
      : std::basic_ifstream<charT,traits>(
          file_ph.file_string().c_str(), mode ) {}
    
    template <class charT, class traits>
    basic_ifstream<charT,traits>::basic_ifstream( const wpath & file_ph,
      std::ios_base::openmode mode )
      : std::basic_ifstream<charT,traits>(
        detail::narrow_path( file_ph.external_file_string(),
          mode ).c_str(), mode ) {}

    template <class charT, class traits> template<class Path>
    typename boost::enable_if<is_basic_path<Path>, void>::type
    basic_ifstream<charT,traits>::open( const Path & file_ph )
    {
      std::basic_ifstream<charT,traits>::open(
        detail::narrow_path( file_ph.external_file_string(),
          std::ios_base::in ).c_str(), std::ios_base::in );
    }
    
    template <class charT, class traits>
    void basic_ifstream<charT,traits>::open( const path & file_ph )
    {
      std::basic_ifstream<charT,traits>::open(
        file_ph.file_string().c_str(), std::ios_base::in );
    }
    
    template <class charT, class traits>
    void basic_ifstream<charT,traits>::open( const wpath & file_ph )
    {
      std::basic_ifstream<charT,traits>::open(
        detail::narrow_path( file_ph.external_file_string(),
          std::ios_base::in ).c_str(), std::ios_base::in );
    }

    template <class charT, class traits> template<class Path>
    typename boost::enable_if<is_basic_path<Path>, void>::type
    basic_ifstream<charT,traits>::open( const Path & file_ph,
      std::ios_base::openmode mode )
    {
      std::basic_ifstream<charT,traits>::open(
        detail::narrow_path( file_ph.external_file_string(),
          mode ).c_str(), mode );
    }
    
    template <class charT, class traits>
    void basic_ifstream<charT,traits>::open( const path & file_ph,
      std::ios_base::openmode mode )
    {
      std::basic_ifstream<charT,traits>::open(
        file_ph.file_string().c_str(), mode );
    }
    
    template <class charT, class traits>
    void basic_ifstream<charT,traits>::open( const wpath & file_ph,
      std::ios_base::openmode mode )
    {
      std::basic_ifstream<charT,traits>::open(
        detail::narrow_path( file_ph.external_file_string(),
          mode ).c_str(), mode );
    }

//  basic_ofstream definitions  ----------------------------------------------//

    template <class charT, class traits> template<class Path>
    basic_ofstream<charT,traits>::basic_ofstream(const Path & file_ph,
      typename boost::enable_if<is_basic_path<Path> >::type* )
      : std::basic_ofstream<charT,traits>(
        detail::narrow_path( file_ph.external_file_string(),
          std::ios_base::out ).c_str(), std::ios_base::out ) {}

    template <class charT, class traits>
    basic_ofstream<charT,traits>::basic_ofstream( const path & file_ph )
      : std::basic_ofstream<charT,traits>(
          file_ph.file_string().c_str(), std::ios_base::out ) {}

    template <class charT, class traits>
    basic_ofstream<charT,traits>::basic_ofstream( const wpath & file_ph )
      : std::basic_ofstream<charT,traits>(
        detail::narrow_path( file_ph.external_file_string(),
          std::ios_base::out ).c_str(), std::ios_base::out ) {}

    template <class charT, class traits> template<class Path>
    basic_ofstream<charT,traits>::basic_ofstream( const Path & file_ph,
      std::ios_base::openmode mode,
      typename boost::enable_if<is_basic_path<Path> >::type* )
      : std::basic_ofstream<charT,traits>(
        detail::narrow_path( file_ph.external_file_string(),
          mode ).c_str(), mode ) {}

    template <class charT, class traits>
    basic_ofstream<charT,traits>::basic_ofstream( const path & file_ph,
      std::ios_base::openmode mode )
      : std::basic_ofstream<charT,traits>(
          file_ph.file_string().c_str(), mode ) {}
    
    template <class charT, class traits>
    basic_ofstream<charT,traits>::basic_ofstream( const wpath & file_ph,
      std::ios_base::openmode mode )
      : std::basic_ofstream<charT,traits>(
        detail::narrow_path( file_ph.external_file_string(),
          mode ).c_str(), mode ) {}

    template <class charT, class traits> template<class Path>
    typename boost::enable_if<is_basic_path<Path>, void>::type
    basic_ofstream<charT,traits>::open( const Path & file_ph )
    {
      std::basic_ofstream<charT,traits>::open(
        detail::narrow_path( file_ph.external_file_string(),
          std::ios_base::out ).c_str(), std::ios_base::out );
    }
    
    template <class charT, class traits>
    void basic_ofstream<charT,traits>::open( const path & file_ph )
    {
      std::basic_ofstream<charT,traits>::open(
        file_ph.file_string().c_str(), std::ios_base::out );
    }
    
    template <class charT, class traits>
    void basic_ofstream<charT,traits>::open( const wpath & file_ph )
    {
      std::basic_ofstream<charT,traits>::open(
        detail::narrow_path( file_ph.external_file_string(),
          std::ios_base::out ).c_str(), std::ios_base::out );
    }

    template <class charT, class traits> template<class Path>
    typename boost::enable_if<is_basic_path<Path>, void>::type
    basic_ofstream<charT,traits>::open( const Path & file_ph,
      std::ios_base::openmode mode )
    {
      std::basic_ofstream<charT,traits>::open(
        detail::narrow_path( file_ph.external_file_string(),
          mode ).c_str(), mode );
    }
    
    template <class charT, class traits>
    void basic_ofstream<charT,traits>::open( const path & file_ph,
      std::ios_base::openmode mode )
    {
      std::basic_ofstream<charT,traits>::open(
        file_ph.file_string().c_str(), mode );
    }
    
    template <class charT, class traits>
    void basic_ofstream<charT,traits>::open( const wpath & file_ph,
      std::ios_base::openmode mode )
    {
      std::basic_ofstream<charT,traits>::open(
        detail::narrow_path( file_ph.external_file_string(),
          mode ).c_str(), mode );
    }

//  basic_fstream definitions  -----------------------------------------------//

    template <class charT, class traits> template<class Path>
    basic_fstream<charT,traits>::basic_fstream(const Path & file_ph,
      typename boost::enable_if<is_basic_path<Path> >::type* )
      : std::basic_fstream<charT,traits>(
        detail::narrow_path( file_ph.external_file_string(),
          std::ios_base::in|std::ios_base::out ).c_str(),
          std::ios_base::in|std::ios_base::out ) {}

    template <class charT, class traits>
    basic_fstream<charT,traits>::basic_fstream( const path & file_ph )
      : std::basic_fstream<charT,traits>(
          file_ph.file_string().c_str(),
          std::ios_base::in|std::ios_base::out ) {}

    template <class charT, class traits>
    basic_fstream<charT,traits>::basic_fstream( const wpath & file_ph )
      : std::basic_fstream<charT,traits>(
        detail::narrow_path( file_ph.external_file_string(),
          std::ios_base::in|std::ios_base::out ).c_str(),
          std::ios_base::in|std::ios_base::out ) {}

    template <class charT, class traits> template<class Path>
    basic_fstream<charT,traits>::basic_fstream( const Path & file_ph,
      std::ios_base::openmode mode,
      typename boost::enable_if<is_basic_path<Path> >::type* )
      : std::basic_fstream<charT,traits>(
        detail::narrow_path( file_ph.external_file_string(),
          mode ).c_str(), mode ) {}

    template <class charT, class traits>
    basic_fstream<charT,traits>::basic_fstream( const path & file_ph,
      std::ios_base::openmode mode )
      : std::basic_fstream<charT,traits>(
          file_ph.file_string().c_str(), mode ) {}
    
    template <class charT, class traits>
    basic_fstream<charT,traits>::basic_fstream( const wpath & file_ph,
      std::ios_base::openmode mode )
      : std::basic_fstream<charT,traits>(
        detail::narrow_path( file_ph.external_file_string(),
          mode ).c_str(), mode ) {}

    template <class charT, class traits> template<class Path>
    typename boost::enable_if<is_basic_path<Path>, void>::type
    basic_fstream<charT,traits>::open( const Path & file_ph )
    {
      std::basic_fstream<charT,traits>::open(
        detail::narrow_path( file_ph.external_file_string(),
          std::ios_base::in|std::ios_base::out ).c_str(),
          std::ios_base::in|std::ios_base::out );
    }
    
    template <class charT, class traits>
    void basic_fstream<charT,traits>::open( const path & file_ph )
    {
      std::basic_fstream<charT,traits>::open(
        file_ph.file_string().c_str(), std::ios_base::in|std::ios_base::out );
    }
    
    template <class charT, class traits>
    void basic_fstream<charT,traits>::open( const wpath & file_ph )
    {
      std::basic_fstream<charT,traits>::open(
        detail::narrow_path( file_ph.external_file_string(),
          std::ios_base::in|std::ios_base::out ).c_str(),
          std::ios_base::in|std::ios_base::out );
    }

    template <class charT, class traits> template<class Path>
    typename boost::enable_if<is_basic_path<Path>, void>::type
    basic_fstream<charT,traits>::open( const Path & file_ph,
      std::ios_base::openmode mode )
    {
      std::basic_fstream<charT,traits>::open(
        detail::narrow_path( file_ph.external_file_string(),
          mode ).c_str(), mode );
    }
    
    template <class charT, class traits>
    void basic_fstream<charT,traits>::open( const path & file_ph,
      std::ios_base::openmode mode )
    {
      std::basic_fstream<charT,traits>::open(
        file_ph.file_string().c_str(), mode );
    }
    
    template <class charT, class traits>
    void basic_fstream<charT,traits>::open( const wpath & file_ph,
      std::ios_base::openmode mode )
    {
      std::basic_fstream<charT,traits>::open(
        detail::narrow_path( file_ph.external_file_string(),
          mode ).c_str(), mode );
    }

  } // namespace filesystem
} // namespace boost

#include <boost/config/abi_suffix.hpp> // pops abi_prefix.hpp pragmas
#endif  // BOOST_FILESYSTEM_FSTREAM_HPP
