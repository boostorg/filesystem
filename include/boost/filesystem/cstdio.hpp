//  boost/filesystem/cstdio.hpp  ------------------------------------------------------//

//  Copyright Andrey Semashev 2023-2026

//  Distributed under the Boost Software License, Version 1.0.
//  See http://www.boost.org/LICENSE_1_0.txt

//  Library home page: http://www.boost.org/libs/filesystem

//--------------------------------------------------------------------------------------//

#ifndef BOOST_FILESYSTEM_CSTDIO_HPP
#define BOOST_FILESYSTEM_CSTDIO_HPP

#include <boost/filesystem/config.hpp>
#include <boost/filesystem/path.hpp>
#include <cstdio>
#if defined(BOOST_FILESYSTEM_WINDOWS_API)
#include <wchar.h>
#include <cstddef>
#include <cstring>
#include <cstdlib>
#endif

#include <boost/filesystem/detail/header.hpp> // must be the last #include

namespace boost {
namespace filesystem {

/*!
 * \brief Opens a file using C standard library file descriptor.
 *
 * This function is equivalent to `std::fopen` from `<cstdio>` with the only difference being that the path
 * argument is expressed as `const path&`. The function attempts to open the file without path character
 * encoding conversion, permitting non-ASCII characters in filesystem paths. If the function does perform
 * character code conversion, it does so by calling `path` methods, using the locale facet returned by
 * `path::codecvt`.
 *
 * \param p Path to the file.
 * \param mode File opening mode. Refer to `std::fopen` documentation for the supported modes.
 *
 * \returns If the file has been opened successfully, a pointer to the file descriptor, to be closed with
 *          `std::fclose`. Otherwise, a null pointer and `errno` is set to the error code. Refer to `std::fopen`
 *          documentation for the list of error codes.
 *
 * \error_reporting
 * \parblock
 * C standard library and system errors are reported using the return value of a null pointer and `errno` mechanism.
 * An exception may be thrown by one of the `path` methods, if called by the implementation.
 *
 * \remark In particular, `path::string` may be called on a platform that uses `wchar_t` as the native path
 *         character type but does not provide a low level API to open a file using a wide character path. This
 *         is the case on the legacy MinGW32 in strict mode, for instance.
 * \endparblock
 */
inline std::FILE* fopen(filesystem::path const& p, const char* mode)
{
#if defined(BOOST_FILESYSTEM_WINDOWS_API)

#if defined(__MINGW32__) && !defined(__MINGW64_VERSION_MAJOR) && defined(__STRICT_ANSI__)
    // MinGW32 in strict ANSI mode does not declare _wfopen
    return std::fopen(p.string().c_str(), mode);
#else
    // mode should only contain ASCII characters and is likely short
    struct small_array
    {
        wchar_t buf[128u];
        wchar_t* p;

        small_array() noexcept : p(buf) {}
        ~small_array() noexcept
        {
            if (BOOST_UNLIKELY(p != buf))
                std::free(p);
        }
    }
    wmode;
    std::size_t wmode_len = std::mbstowcs(wmode.p, mode, sizeof(wmode.buf) / sizeof(wchar_t));
    if (BOOST_UNLIKELY(wmode_len >= (sizeof(wmode.buf) / sizeof(wchar_t))))
    {
        wmode_len = std::mbstowcs(nullptr, mode, 0u);
        // Check for size overflow, including (size_t)-1, which indicates mbstowcs error
        if (BOOST_UNLIKELY(wmode_len >= (static_cast< std::size_t >(-1) / sizeof(wchar_t))))
            return nullptr;

        wmode.p = static_cast< wchar_t* >(std::malloc((wmode_len + 1u) * sizeof(wchar_t)));
        if (BOOST_UNLIKELY(!wmode.p))
            return nullptr;

        std::size_t wmode_len2 = std::mbstowcs(wmode.p, mode, wmode_len + 1u);
        if (BOOST_UNLIKELY(wmode_len2 > wmode_len))
            return nullptr;
    }

    return ::_wfopen(p.c_str(), wmode.p);
#endif

#else // defined(BOOST_FILESYSTEM_WINDOWS_API)

    return std::fopen(p.c_str(), mode);

#endif // defined(BOOST_FILESYSTEM_WINDOWS_API)
}

} // namespace filesystem
} // namespace boost

#include <boost/filesystem/detail/footer.hpp>

#endif // BOOST_FILESYSTEM_CSTDIO_HPP
