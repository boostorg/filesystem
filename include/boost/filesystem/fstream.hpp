//  boost/filesystem/fstream.hpp  ------------------------------------------------------//

//  Copyright Beman Dawes 2002
//  Copyright Andrey Semashev 2021-2026

//  Distributed under the Boost Software License, Version 1.0.
//  See http://www.boost.org/LICENSE_1_0.txt

//  Library home page: http://www.boost.org/libs/filesystem

//--------------------------------------------------------------------------------------//

#ifndef BOOST_FILESYSTEM_FSTREAM_HPP
#define BOOST_FILESYSTEM_FSTREAM_HPP

#include <boost/filesystem/config.hpp>
#include <boost/filesystem/path.hpp>
#include <iosfwd>
#include <fstream>

#include <boost/filesystem/detail/header.hpp> // must be the last #include

#if !defined(BOOST_FILESYSTEM_DOXYGEN)

#if defined(BOOST_FILESYSTEM_WINDOWS_API)
// On Windows, except for standard libaries known to have wchar_t overloads for
// file stream I/O, use path::string() to get a narrow character c_str()
#if (defined(_CPPLIB_VER) && _CPPLIB_VER >= 405 && !defined(_STLPORT_VERSION)) || \
    (defined(_LIBCPP_VERSION) && _LIBCPP_VERSION >= 7000 && defined(_LIBCPP_HAS_OPEN_WITH_WCHAR))
// Use wide characters directly
// Note: We don't use C++17 std::filesystem::path as a means to pass wide paths
// to file streams because of various problems:
// - std::filesystem is available in gcc 8 but it is broken there (fails to compile path definition
//   on Windows). Compilation errors seem to be fixed since gcc 9.
// - In gcc 10.2 and clang 8.0.1 on Cygwin64, the path attempts to convert the wide string to narrow
//   and fails in runtime. This may be system locale dependent, and performing character code conversion
//   is against the purpose of using std::filesystem::path anyway.
// - Other std::filesystem implementations were not tested, so it is not known if they actually work
//   with wide paths.
#define BOOST_FILESYSTEM_C_STR(p) p.c_str()
#else
// Use narrow characters, since wide not available
#define BOOST_FILESYSTEM_C_STR(p) p.string().c_str()
#endif
#endif // defined(BOOST_FILESYSTEM_WINDOWS_API)

#if !defined(BOOST_FILESYSTEM_C_STR)
#define BOOST_FILESYSTEM_C_STR(p) p.c_str()
#endif

#endif // !defined(BOOST_FILESYSTEM_DOXYGEN)

#if defined(BOOST_MSVC)
#pragma warning(push)
// 'boost::filesystem::basic_fstream<Char>' : inherits 'std::basic_istream<_Elem,_Traits>::std::basic_istream<_Elem,_Traits>::_Add_vtordisp1' via dominance
#pragma warning(disable : 4250)
#endif

namespace boost {
namespace filesystem {

//--------------------------------------------------------------------------------------//
//                                  basic_filebuf                                       //
//--------------------------------------------------------------------------------------//
/*!
 * \brief File buffer class template.
 *
 * The class template is equivalent to `std::basic_filebuf` from `<fstream>` with the only difference being that
 * the `open` method accepts `path` as the first argument.
 */
template< class Char, class Traits = std::char_traits< Char > >
class basic_filebuf :
    public std::basic_filebuf< Char, Traits >
{
private:
    using base_type = std::basic_filebuf< Char, Traits >;

public:
    basic_filebuf() = default;

#if !defined(BOOST_FILESYSTEM_DETAIL_NO_CXX11_MOVABLE_FSTREAMS)
    basic_filebuf(basic_filebuf&&) = default;
    basic_filebuf& operator= (basic_filebuf&&) = default;
#endif // !defined(BOOST_FILESYSTEM_DETAIL_NO_CXX11_MOVABLE_FSTREAMS)

    basic_filebuf(basic_filebuf const&) = delete;
    basic_filebuf const& operator= (basic_filebuf const&) = delete;

public:
    /*!
     * \brief Opens a file identified by `p`.
     *
     * This method is equivalent to `std::basic_filebuf::open`, except that it accepts `path` as the first argument.
     * The implementation will attempt to open the file using the native path character encoding, if possible. If
     * `std::basic_filebuf` implementation does not support opening files using the native path character encoding,
     * the implementation will perform path character code conversion by calling `path` methods, using the locale
     * facet returned by `path::codecvt`.
     *
     * \param p Path to the file.
     * \param mode File opening mode. Refer to `std::basic_filebuf::open` documentation for the supported modes.
     *
     * \returns `this` if the file was successfully opened, otherwise a null pointer.
     *
     * \error_reporting
     * \parblock
     * C++ standard library and system errors are reported as documented by `std::basic_filebuf::open`.
     * Additionally, an exception may be thrown by one of the `path` methods as part of character code conversion,
     * if called by the implementation.
     * \endparblock
     */
    basic_filebuf* open(path const& p, std::ios_base::openmode mode)
    {
        return base_type::open(BOOST_FILESYSTEM_C_STR(p), mode) ? this : nullptr;
    }
};

//--------------------------------------------------------------------------------------//
//                                 basic_ifstream                                       //
//--------------------------------------------------------------------------------------//
/*!
 * \brief Input file stream class template.
 *
 * The class template is equivalent to `std::basic_ifstream` from `<fstream>` with the only difference being that
 * the constructors and `open` methods accept `path` as the first argument.
 */
template< class Char, class Traits = std::char_traits< Char > >
class basic_ifstream :
    public std::basic_ifstream< Char, Traits >
{
private:
    using base_type = std::basic_ifstream< Char, Traits >;

public:
    basic_ifstream() = default;

    /*!
     * \brief Constructs a stream and opens a file identified by `p`.
     *
     * This method is equivalent to the `std::basic_ifstream` constructor taking a path string and `mode` as arguments,
     * except that it accepts `path` as the first argument. The implementation will attempt to open the file using
     * the native path character encoding, if possible. If `std::basic_ifstream` implementation does not support
     * opening files using the native path character encoding, the implementation will perform path character code
     * conversion by calling `path` methods, using the locale facet returned by `path::codecvt`.
     *
     * \param p Path to the file.
     * \param mode File opening mode. Refer to the `std::basic_ifstream` constructor documentation for the supported
     *             modes.
     *
     * \error_reporting
     * \parblock
     * C++ standard library and system errors are reported as documented by the `std::basic_ifstream` constructor.
     * Additionally, an exception may be thrown by one of the `path` methods as part of character code conversion,
     * if called by the implementation.
     * \endparblock
     */
    explicit basic_ifstream(path const& p, std::ios_base::openmode mode = std::ios_base::in) :
        base_type(BOOST_FILESYSTEM_C_STR(p), mode) {}

#if !defined(BOOST_FILESYSTEM_DETAIL_NO_CXX11_MOVABLE_FSTREAMS)
    basic_ifstream(basic_ifstream&& that) :
        base_type(static_cast< base_type&& >(that)) {}

    basic_ifstream& operator= (basic_ifstream&& that)
    {
        *static_cast< base_type* >(this) = static_cast< base_type&& >(that);
        return *this;
    }
#endif

    basic_ifstream(basic_ifstream const&) = delete;
    basic_ifstream const& operator= (basic_ifstream const&) = delete;

public:
    /*!
     * \brief Opens a file identified by `p`.
     *
     * This method is equivalent to `std::basic_ifstream::open`, except that it accepts `path` as the first
     * argument. The implementation will attempt to open the file using the native path character encoding, if
     * possible. If `std::basic_ifstream` implementation does not support opening files using the native path
     * character encoding, the implementation will perform path character code conversion by calling `path` methods,
     * using the locale facet returned by `path::codecvt`.
     *
     * \param p Path to the file.
     * \param mode File opening mode. Refer to `std::basic_ifstream::open` documentation for the supported modes.
     *
     * \error_reporting
     * \parblock
     * C++ standard library and system errors are reported as documented by `std::basic_ifstream::open`.
     * Additionally, an exception may be thrown by one of the `path` methods as part of character code conversion,
     * if called by the implementation.
     * \endparblock
     */
    void open(path const& p, std::ios_base::openmode mode = std::ios_base::in)
    {
        base_type::open(BOOST_FILESYSTEM_C_STR(p), mode);
    }
};

//--------------------------------------------------------------------------------------//
//                                 basic_ofstream                                       //
//--------------------------------------------------------------------------------------//
/*!
 * \brief Output file stream class template.
 *
 * The class template is equivalent to `std::basic_ofstream` from `<fstream>` with the only difference being that
 * the constructors and `open` methods accept `path` as the first argument.
 */
template< class Char, class Traits = std::char_traits< Char > >
class basic_ofstream :
    public std::basic_ofstream< Char, Traits >
{
private:
    using base_type = std::basic_ofstream< Char, Traits >;

public:
    basic_ofstream() = default;

    /*!
     * \brief Constructs a stream and opens a file identified by `p`.
     *
     * This method is equivalent to the `std::basic_ofstream` constructor taking a path string and `mode` as arguments,
     * except that it accepts `path` as the first argument. The implementation will attempt to open the file using
     * the native path character encoding, if possible. If `std::basic_ofstream` implementation does not support
     * opening files using the native path character encoding, the implementation will perform path character code
     * conversion by calling `path` methods, using the locale facet returned by `path::codecvt`.
     *
     * \param p Path to the file.
     * \param mode File opening mode. Refer to the `std::basic_ofstream` constructor documentation for the supported
     *             modes.
     *
     * \error_reporting
     * \parblock
     * C++ standard library and system errors are reported as documented by the `std::basic_ofstream` constructor.
     * Additionally, an exception may be thrown by one of the `path` methods as part of character code conversion,
     * if called by the implementation.
     * \endparblock
     */
    explicit basic_ofstream(path const& p, std::ios_base::openmode mode = std::ios_base::out) :
        base_type(BOOST_FILESYSTEM_C_STR(p), mode) {}

#if !defined(BOOST_FILESYSTEM_DETAIL_NO_CXX11_MOVABLE_FSTREAMS)
    basic_ofstream(basic_ofstream&& that) :
        base_type(static_cast< base_type&& >(that)) {}

    basic_ofstream& operator= (basic_ofstream&& that)
    {
        *static_cast< base_type* >(this) = static_cast< base_type&& >(that);
        return *this;
    }
#endif

    basic_ofstream(basic_ofstream const&) = delete;
    basic_ofstream const& operator= (basic_ofstream const&) = delete;

public:
    /*!
     * \brief Opens a file identified by `p`.
     *
     * This method is equivalent to `std::basic_ofstream::open`, except that it accepts `path` as the first
     * argument. The implementation will attempt to open the file using the native path character encoding, if
     * possible. If `std::basic_ofstream` implementation does not support opening files using the native path
     * character encoding, the implementation will perform path character code conversion by calling `path` methods,
     * using the locale facet returned by `path::codecvt`.
     *
     * \param p Path to the file.
     * \param mode File opening mode. Refer to `std::basic_ofstream::open` documentation for the supported modes.
     *
     * \error_reporting
     * \parblock
     * C++ standard library and system errors are reported as documented by `std::basic_ofstream::open`.
     * Additionally, an exception may be thrown by one of the `path` methods as part of character code conversion,
     * if called by the implementation.
     * \endparblock
     */
    void open(path const& p, std::ios_base::openmode mode = std::ios_base::out)
    {
        base_type::open(BOOST_FILESYSTEM_C_STR(p), mode);
    }
};

//--------------------------------------------------------------------------------------//
//                                  basic_fstream                                       //
//--------------------------------------------------------------------------------------//
/*!
 * \brief Input/output file stream class template.
 *
 * The class template is equivalent to `std::basic_fstream` from `<fstream>` with the only difference being that
 * the constructors and `open` methods accept `path` as the first argument.
 */
template< class Char, class Traits = std::char_traits< Char > >
class basic_fstream :
    public std::basic_fstream< Char, Traits >
{
private:
    using base_type = std::basic_fstream< Char, Traits >;

public:
    basic_fstream() = default;

    /*!
     * \brief Constructs a stream and opens a file identified by `p`.
     *
     * This method is equivalent to the `std::basic_fstream` constructor taking a path string and `mode` as arguments,
     * except that it accepts `path` as the first argument. The implementation will attempt to open the file using
     * the native path character encoding, if possible. If `std::basic_fstream` implementation does not support
     * opening files using the native path character encoding, the implementation will perform path character code
     * conversion by calling `path` methods, using the locale facet returned by `path::codecvt`.
     *
     * \param p Path to the file.
     * \param mode File opening mode. Refer to the `std::basic_fstream` constructor documentation for the supported
     *             modes.
     *
     * \error_reporting
     * \parblock
     * C++ standard library and system errors are reported as documented by the `std::basic_fstream` constructor.
     * Additionally, an exception may be thrown by one of the `path` methods as part of character code conversion,
     * if called by the implementation.
     * \endparblock
     */
    explicit basic_fstream(path const& p, std::ios_base::openmode mode = std::ios_base::in | std::ios_base::out) :
        base_type(BOOST_FILESYSTEM_C_STR(p), mode) {}

#if !defined(BOOST_FILESYSTEM_DETAIL_NO_CXX11_MOVABLE_FSTREAMS)
    basic_fstream(basic_fstream&& that) :
        base_type(static_cast< base_type&& >(that)) {}

    basic_fstream& operator= (basic_fstream&& that)
    {
        *static_cast< base_type* >(this) = static_cast< base_type&& >(that);
        return *this;
    }
#endif

    basic_fstream(basic_fstream const&) = delete;
    basic_fstream const& operator= (basic_fstream const&) = delete;

public:
    /*!
     * \brief Opens a file identified by `p`.
     *
     * This method is equivalent to `std::basic_fstream::open`, except that it accepts `path` as the first
     * argument. The implementation will attempt to open the file using the native path character encoding, if
     * possible. If `std::basic_fstream` implementation does not support opening files using the native path
     * character encoding, the implementation will perform path character code conversion by calling `path` methods,
     * using the locale facet returned by `path::codecvt`.
     *
     * \param p Path to the file.
     * \param mode File opening mode. Refer to `std::basic_fstream::open` documentation for the supported modes.
     *
     * \error_reporting
     * \parblock
     * C++ standard library and system errors are reported as documented by the `std::basic_fstream::open`.
     * Additionally, an exception may be thrown by one of the `path` methods as part of character code conversion,
     * if called by the implementation.
     * \endparblock
     */
    void open(path const& p, std::ios_base::openmode mode = std::ios_base::in | std::ios_base::out)
    {
        base_type::open(BOOST_FILESYSTEM_C_STR(p), mode);
    }
};

//--------------------------------------------------------------------------------------//
//                                    typedefs                                          //
//--------------------------------------------------------------------------------------//

using filebuf = basic_filebuf< char >;
using ifstream = basic_ifstream< char >;
using ofstream = basic_ofstream< char >;
using fstream = basic_fstream< char >;

using wfilebuf = basic_filebuf< wchar_t >;
using wifstream = basic_ifstream< wchar_t >;
using wofstream = basic_ofstream< wchar_t >;
using wfstream = basic_fstream< wchar_t >;

} // namespace filesystem
} // namespace boost

#if defined(BOOST_MSVC)
#pragma warning(pop)
#endif

#include <boost/filesystem/detail/footer.hpp>

#endif // BOOST_FILESYSTEM_FSTREAM_HPP
