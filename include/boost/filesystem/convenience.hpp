//  boost/filesystem/convenience.hpp  ----------------------------------------//

//  Copyright Beman Dawes, 2002
//  Copyright Vladimir Prus, 2002
//  Use, modification, and distribution is subject to the Boost Software
//  License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

//  See library home page at http://www.boost.org/libs/filesystem

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


