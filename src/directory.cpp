//  directory_iterator implementation forwarding file  -----------------------//

//  (C) Copyright Beman Dawes 2002. Permission to copy,
//  use, modify, sell and distribute this software is granted provided this
//  copyright notice appears in all copies. This software is provided "as is"
//  without express or implied warranty, and with no claim as to its
//  suitability for any purpose.

//  See http://www.boost.org for most recent version including documentation.

#include "boost/config.hpp"

#if defined( BOOST_POSIX )
# define BOOST_FILESYSTEM_DIR_IMP "directory_posix.cpp"

#elif defined( BOOST_WINDOWS )
# define BOOST_FILESYSTEM_DIR_IMP "directory_windows.cpp"

#else
# error Sorry, unsupported platform (or BOOST_POSIX, BOOST_WINDOWS, etc. not defined)
#endif

#include BOOST_FILESYSTEM_DIR_IMP

