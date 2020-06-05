//  Copyright 2020 Andrey Semashev

//  Distributed under the Boost Software License, Version 1.0.
//  See http://www.boost.org/LICENSE_1_0.txt

//  See library home page at http://www.boost.org/libs/filesystem

// Include Boost.Predef first so that windows.h is guaranteed to be not included
#include <boost/predef/os/windows.h>
#if !BOOST_OS_WINDOWS
#error "This config test is for Windows only"
#endif

#include <boost/winapi/config.hpp>
#include <boost/predef/platform.h>
#if !(BOOST_USE_WINAPI_VERSION >= BOOST_WINAPI_VERSION_WIN6 && BOOST_WINAPI_PARTITION_APP_SYSTEM)
#error "No BCrypt API"
#endif

#include <cstddef>
#include <boost/winapi/basic_types.hpp>
#include <boost/winapi/bcrypt.hpp>

int main()
{
  boost::winapi::BCRYPT_ALG_HANDLE_ handle;
  boost::winapi::NTSTATUS_ status = boost::winapi::BCryptOpenAlgorithmProvider(&handle, boost::winapi::BCRYPT_RNG_ALGORITHM_, NULL, 0);
  status = boost::winapi::BCryptGenRandom(handle, static_cast<boost::winapi::PUCHAR_>(buf), static_cast<boost::winapi::ULONG_>(len), 0);
  boost::winapi::BCryptCloseAlgorithmProvider(handle, 0);
}
