//  filesystem version.hpp  ------------------------------------------------------------//

//  Copyright Beman Dawes 2014

//  Distributed under the Boost Software License, Version 1.0.
//  See http://www.boost.org/LICENSE_1_0.txt

//  Library home page: http://www.boost.org/libs/filesystem

//  The sole purpose of this header is to specify the default version of the library.

# if !defined(BOOST_FILESYSTEM_USE_TS) && !defined(BOOST_FILESYSTEM_USE_V3)
#   define BOOST_FILESYSTEM_USE_TS
# endif

#include "version_imp.hpp"
