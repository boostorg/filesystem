//  boost/filesystem/config.hpp  ---------------------------------------------//

//  © Copyright Beman Dawes 2003
//  Use, modification, and distribution is subject to the Boost Software
//  License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

//  See library home page at http://www.boost.org/libs/filesystem

//----------------------------------------------------------------------------// 

#ifndef BOOST_FILESYSTEM_CONFIG_HPP
#define BOOST_FILESYSTEM_CONFIG_HPP

#include <boost/config.hpp>

#ifdef BOOST_HAS_DECLSPEC // defined in config system
// we need to import/export our code only if the user has specifically
// asked for it by defining either BOOST_ALL_DYN_LINK if they want all boost
// libraries to be dynamically linked, or BOOST_FILESYSTEM_DYN_LINK
// if they want just this one to be dynamically liked:
#if defined(BOOST_ALL_DYN_LINK) || defined(BOOST_FILESYSTEM_DYN_LINK)
// export if this is our own source, otherwise import:
#ifdef BOOST_FILESYSTEM_SOURCE
# define BOOST_FILESYSTEM_DECL __declspec(dllexport)
#else
# define BOOST_FILESYSTEM_DECL __declspec(dllimport)
#endif  // BOOST_FILESYSTEM_SOURCE
#endif  // DYN_LINK
#endif  // BOOST_HAS_DECLSPEC
//
// if BOOST_FILESYSTEM_DECL isn't defined yet define it now:
#ifndef BOOST_FILESYSTEM_DECL
#define BOOST_FILESYSTEM_DECL
#endif

#endif // BOOST_FILESYSTEM_CONFIG_HPP
