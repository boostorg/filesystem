//  filesystem version_imp.hpp  --------------------------------------------------------//

//  Copyright Beman Dawes 2014

//  Distributed under the Boost Software License, Version 1.0.
//  See http://www.boost.org/LICENSE_1_0.txt

//  Library home page: http://www.boost.org/libs/filesystem

#ifdef BOOST_FILESYSTEM_VERSION_IMP_HPP
#define BOOST_FILESYSTEM_VERSION_IMP_HPP
# error Multiple inclusion of version headers is not allowed
#else

# if defined(BOOST_FILESYSTEM_TS)
#  error BOOST_FILESYSTEM_TS is an interal implementation macro. It must not be user defined.
# endif

# if defined(BOOST_FILESYSTEM_USE_TS) && defined(BOOST_FILESYSTEM_USE_V3)
#   error BOOST_FILESYSTEM_USE_TS and BOOST_FILESYSTEM_USE_V3 are both defined; define one at most.
# endif

# if defined(BOOST_FILESYSTEM_USE_V3)
#   define BOOST_FILESYSTEM_DETAIL_V3
# endif

#endif  // BOOST_FILESYSTEM_VERSION_IMP_HPP
