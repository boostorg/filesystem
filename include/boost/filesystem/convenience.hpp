//  boost/filesystem/convenience.hpp  ----------------------------------------//

//  © Copyright Beman Dawes, 2002
//  © Copyright Vladimir Prus, 2002
//  Use, modification, and distribution is subject to the Boost Software
//  License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

//  See library home page at http://www.boost.org/libs/filesystem

//----------------------------------------------------------------------------// 

#ifndef BOOST_FILESYSTEM_CONVENIENCE_HPP
#define BOOST_FILESYSTEM_CONVENIENCE_HPP

#include <boost/filesystem/operations.hpp>

#include <boost/config/abi_prefix.hpp> // must be the last #include

namespace boost
{
  namespace filesystem
  {

    template<class Path>
    typename boost::enable_if<is_basic_path<Path>, bool>::type
    create_directories(const Path& ph)
    {
         if (ph.empty() || exists(ph))
         {
           if ( !ph.empty() && !is_directory(ph) )
               boost::throw_exception( basic_filesystem_error<Path>(
                 "boost::filesystem::create_directories", ph, -1 ) );
           return false;
         }

         // First create branch, by calling ourself recursively
         create_directories(ph.branch_path());
         // Now that parent's path exists, create the directory
         create_directory(ph);
         return true;
     }
    inline bool create_directories(const path& ph)
      { return create_directories<path>(ph); }
    inline bool create_directories(const wpath& ph)
      { return create_directories<wpath>(ph); }

    template<class Path>
    typename boost::enable_if<is_basic_path<Path>,
      typename Path::string_type>::type
    extension(const Path& ph)
    {
      typedef typename Path::string_type string_type;
      string_type leaf = ph.leaf();

      typename string_type::size_type n = leaf.rfind('.');
      if (n != string_type::npos)
        return leaf.substr(n);
      else
        return string_type();
    }
    inline std::string extension(const path& ph)
      { return extension<path>(ph); }
    inline std::wstring extension(const wpath& ph)
      { return extension<wpath>(ph); }

    template<class Path>
    typename boost::enable_if<is_basic_path<Path>,
      typename Path::string_type>::type
    basename(const Path& ph)
    {
      typedef typename Path::string_type string_type;
      string_type leaf = ph.leaf();
      typename string_type::size_type n = leaf.rfind('.');
      return leaf.substr(0, n);
    }
    inline std::string basename(const path& ph)
      { return basename<path>( ph ); }
    inline std::wstring basename(const wpath& ph)
      { return basename<wpath>( ph ); }

    template<class Path>
    typename boost::enable_if<is_basic_path<Path>, Path>::type
    change_extension( const Path & ph,
      const typename Path::string_type & new_extension )
      { return ph.branch_path() / (basename(ph) + new_extension); }
    inline path change_extension( const path & ph, const std::string& new_ex )
      { return change_extension<path>( ph, new_ex ); }
    inline wpath change_extension( const wpath & ph, const std::wstring& new_ex )
      { return change_extension<wpath>( ph, new_ex ); }


  } // namespace filesystem
} // namespace boost

#include <boost/config/abi_suffix.hpp> // pops abi_prefix.hpp pragmas
#endif // BOOST_FILESYSTEM_CONVENIENCE_HPP
