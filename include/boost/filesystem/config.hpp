//  boost/filesystem/v3/config.hpp  ----------------------------------------------------//

//  Copyright Beman Dawes 2003

//  Distributed under the Boost Software License, Version 1.0.
//  See http://www.boost.org/LICENSE_1_0.txt

//  Library home page: http://www.boost.org/libs/filesystem

//--------------------------------------------------------------------------------------// 

#ifndef BOOST_FILESYSTEM_CONFIG_HPP
#define BOOST_FILESYSTEM_CONFIG_HPP

#include <boost/filesystem/detail/version.hpp>  // for BOOST_FILESYSTEM_DEFAULT_VERSION

// Reality check BOOST_FILESYSTEM_VERSION
# if BOOST_FILESYSTEM_DEFAULT_VERSION != 3 && BOOST_FILESYSTEM_DEFAULT_VERSION != 4
#   error "BOOST_FILESYSTEM_VERSION_DEFAULT" not defined as 3 or 4.
# endif

// Define BOOST_FILESYSTEM_VERSION if needed
# if !defined(BOOST_FILESYSTEM_VERSION)
#   define BOOST_FILESYSTEM_VERSION BOOST_FILESYSTEM_DEFAULT_VERSION
# endif

// Reality check BOOST_FILESYSTEM_VERSION
# if BOOST_FILESYSTEM_VERSION != 3 && BOOST_FILESYSTEM_VERSION != 4
#   error "BOOST_FILESYSTEM_VERSION" not defined as 3 or 4.
# endif

#define BOOST_FILESYSTEM_I18N  // aid users wishing to compile several versions

// This header implements separate compilation features as described in
// http://www.boost.org/more/separate_compilation.html

#include <boost/config.hpp>
#include <boost/system/api_config.hpp>  // for BOOST_POSIX_API or BOOST_WINDOWS_API
#include <boost/detail/workaround.hpp> 

//  BOOST_FILESYSTEM_DEPRECATED needed for source compiles

# ifdef BOOST_FILESYSTEM_SOURCE
#   define BOOST_FILESYSTEM_DEPRECATED
# endif

//  Support for C++11 char16_t and char32_t paths requires both character type (or
//  equivalent typedef) support and header <codecvt> support. 

# if BOOST_FILESYSTEM_VERSION > 3 \
     && !defined(BOOST_NO_CXX11_HDR_CODECVT) \
     && ((!defined(BOOST_NO_CXX11_CHAR16_T) && (!defined(BOOST_NO_CXX11_CHAR32_T)) \
          || (defined(_MSC_VER) && _MSC_VER >= 1600)))
#   define BOOST_FILESYSTEM_CHAR16_CHAR32
#endif

//  throw an exception  ----------------------------------------------------------------//
//
//  Exceptions were originally thrown via boost::throw_exception().
//  As throw_exception() became more complex, it caused user error reporting
//  to be harder to interpret, since the exception reported became much more complex.
//  The immediate fix was to throw directly, wrapped in a macro to make any later change
//  easier.

#define BOOST_FILESYSTEM_THROW(EX) throw EX

# if defined( BOOST_NO_STD_WSTRING )
#   error Configuration not supported: Boost.Filesystem V3 and later requires std::wstring support
# endif

//  This header implements separate compilation features as described in
//  http://www.boost.org/more/separate_compilation.html

//  normalize macros  ------------------------------------------------------------------//

#if !defined(BOOST_FILESYSTEM_DYN_LINK) && !defined(BOOST_FILESYSTEM_STATIC_LINK) \
  && !defined(BOOST_ALL_DYN_LINK) && !defined(BOOST_ALL_STATIC_LINK)
# define BOOST_FILESYSTEM_STATIC_LINK
#endif

#if defined(BOOST_ALL_DYN_LINK) && !defined(BOOST_FILESYSTEM_DYN_LINK)
# define BOOST_FILESYSTEM_DYN_LINK 
#elif defined(BOOST_ALL_STATIC_LINK) && !defined(BOOST_FILESYSTEM_STATIC_LINK)
# define BOOST_FILESYSTEM_STATIC_LINK 
#endif

#if defined(BOOST_FILESYSTEM_DYN_LINK) && defined(BOOST_FILESYSTEM_STATIC_LINK)
# error Must not define both BOOST_FILESYSTEM_DYN_LINK and BOOST_FILESYSTEM_STATIC_LINK
#endif

#if defined(BOOST_ALL_NO_LIB) && !defined(BOOST_FILESYSTEM_NO_LIB)
# define BOOST_FILESYSTEM_NO_LIB 
#endif

//  enable dynamic linking  ------------------------------------------------------------//

#if defined(BOOST_ALL_DYN_LINK) || defined(BOOST_FILESYSTEM_DYN_LINK)
# if defined(BOOST_FILESYSTEM_SOURCE)
#   define BOOST_FILESYSTEM_DECL BOOST_SYMBOL_EXPORT
# else 
#   define BOOST_FILESYSTEM_DECL BOOST_SYMBOL_IMPORT
# endif
#else
# define BOOST_FILESYSTEM_DECL
#endif

//  enable automatic library variant selection  ----------------------------------------// 

#if !defined(BOOST_FILESYSTEM_SOURCE) && !defined(BOOST_ALL_NO_LIB) \
  && !defined(BOOST_FILESYSTEM_NO_LIB)
//
// Set the name of our library, this will get undef'ed by auto_link.hpp
// once it's done with it:
//
#define BOOST_LIB_NAME boost_filesystem
//
// If we're importing code from a dll, then tell auto_link.hpp about it:
//
#if defined(BOOST_ALL_DYN_LINK) || defined(BOOST_FILESYSTEM_DYN_LINK)
#  define BOOST_DYN_LINK
#endif
//
// And include the header that does the work:
//
#include <boost/config/auto_link.hpp>
#endif  // auto-linking disabled

//  set up the supported namespaces and alias  -----------------------------------------//
//
//  this is the mechanism that allows building multiple versions into a single object
//  library, without name clashes.

#define BOOST_FILESYSTEM_NAMESPACE BOOST_JOIN(filesystem, BOOST_FILESYSTEM_VERSION)

namespace boost
{
  namespace filesystem3 {}
  namespace filesystem4 {}

  namespace filesystem = BOOST_FILESYSTEM_NAMESPACE;
}


#endif // BOOST_FILESYSTEM_CONFIG_HPP
