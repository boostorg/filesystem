//  boost/filesystem/convenience.hpp  ----------------------------------------//

//  Copyright Beman Dawes, 2002
//  Copyright Vladimir Prus, 2002

//  See accompanying license for terms and conditions of use.

//  See http://www.boost.org/libs/filesystem for documentation.

//----------------------------------------------------------------------------// 

#ifndef BOOST_FILESYSTEM_CONVENIENCE_HPP
#define BOOST_FILESYSTEM_CONVENIENCE_HPP

#include <boost/filesystem/path.hpp>
#include <boost/filesystem/operations.hpp>

namespace boost
{
  namespace filesystem
  {

//  create_directories (contributed by Vladimir Prus)  -----------------------//


    /** Creates directory 'ph' and all necessary parent directories.
        @post exists(directory_ph) && is_directory(directory_ph) && is_empty(directory_ph)
     */
    void create_directories(const path& ph);

    std::string extension(const path& ph);

    std::string basename(const path& ph);

    path change_extension(const path& ph, const std::string& new_extension);

  } // namespace filesystem
} // namespace boost
#endif // BOOST_FILESYSTEM_CONVENIENCE_HPP


