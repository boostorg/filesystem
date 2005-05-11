//  path.cpp  ----------------------------------------------------------------//

//  Copyright © 2005 Beman Dawes
//  Use, modification, and distribution is subject to the Boost Software
//  License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy
//  at http://www.boost.org/LICENSE_1_0.txt)

//  See library home page at http://www.boost.org/libs/filesystem

//----------------------------------------------------------------------------// 

#include <boost/filesystem/config.hpp>

#ifdef BOOST_POSIX_API

//  Because this is POSIX only code, we don't have to worry about ABI issues
//  described in http://www.boost.org/more/separate_compilation.html

#include <boost/filesystem/path.hpp>
#include "../src//utf8_codecvt_facet.hpp"
#include <boost/scoped_array.hpp>

#include <cerrno>

namespace
{
  // for operating systems where UTF-8 is not the preferred external
  // encoding, insert #ifdef's here to choose the correct codecvt facet.

  boost::detail::utf8_codecvt_facet convertor;
}

namespace boost
{
  namespace filesystem
  {
    namespace detail
    {

      wpath_traits::external_string_type
      wpath_traits::to_external( const wpath & ph, 
        const internal_string_type & src )
      {
        std::size_t work_size( convertor.max_length() * (src.size()+1) );
        boost::scoped_array<char> work( new char[ work_size ] );
        std::mbstate_t state;
        const internal_string_type::value_type * from_next;
        external_string_type::value_type * to_next;
        if ( convertor.out( 
          state, src.c_str(), src.c_str()+src.size(), from_next, work.get(),
          work.get()+work_size, to_next ) != std::codecvt_base::ok )
          boost::throw_exception( boost::filesystem::wfilesystem_error(
            "boost::filesystem::wpath::to_external conversion error",
            ph, EINVAL ) );
        *to_next = '\0';
        return external_string_type( work.get() );
      }

      wpath_traits::internal_string_type
      wpath_traits::to_internal( const external_string_type & src )
      {
        std::size_t work_size( src.size()+1 );
        boost::scoped_array<wchar_t> work( new wchar_t[ work_size ] );
        std::mbstate_t state;
        const external_string_type::value_type * from_next;
        internal_string_type::value_type * to_next;
        if ( convertor.in( 
          state, src.c_str(), src.c_str()+src.size(), from_next, work.get(),
          work.get()+work_size, to_next ) != std::codecvt_base::ok )
          boost::throw_exception( boost::filesystem::wfilesystem_error(
            "boost::filesystem::wpath::to_internal conversion error", EINVAL ) );
        *to_next = L'\0';
        return internal_string_type( work.get() );
      }

    } // namespace detail
  } // namespace filesystem
} // namespace boost

#endif
