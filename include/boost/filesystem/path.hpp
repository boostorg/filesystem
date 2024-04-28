//  filesystem path.hpp  ---------------------------------------------------------------//

//  Copyright Vladimir Prus 2002
//  Copyright Beman Dawes 2002-2005, 2009
//  Copyright Andrey Semashev 2021-2024

//  Distributed under the Boost Software License, Version 1.0.
//  See http://www.boost.org/LICENSE_1_0.txt

//  Library home page: http://www.boost.org/libs/filesystem

//  path::stem(), extension(), and replace_extension() are based on
//  basename(), extension(), and change_extension() from the original
//  filesystem/convenience.hpp header by Vladimir Prus.

#ifndef BOOST_FILESYSTEM_PATH_HPP
#define BOOST_FILESYSTEM_PATH_HPP

#include <boost/filesystem/config.hpp>
#include <cstddef>
#include <iosfwd>
#include <locale>
#include <string>
#include <iterator>
#include <type_traits>
#if !defined(BOOST_NO_CXX17_HDR_STRING_VIEW)
#include <string_view>
#endif
#include <boost/assert.hpp>
#include <boost/iterator/iterator_facade.hpp>
#include <boost/iterator/iterator_categories.hpp>
#include <boost/io/quoted.hpp>
#include <boost/functional/hash_fwd.hpp>
#include <boost/filesystem/detail/path_traits.hpp>
#include <boost/filesystem/detail/type_traits/negation.hpp>
#include <boost/filesystem/detail/type_traits/conjunction.hpp>
#include <boost/filesystem/detail/type_traits/disjunction.hpp>

#include <boost/filesystem/detail/header.hpp> // must be the last #include

namespace boost {
namespace filesystem {

class path;

namespace detail {
namespace path_impl { // intentionally don't use filesystem::detail to not bring internal Boost.Filesystem functions into ADL via path_constants

template< typename Char, Char Separator, Char PreferredSeparator, Char Dot >
struct path_constants
{
    using path_constants_base = path_constants< Char, Separator, PreferredSeparator, Dot >;
    using value_type = Char;
    static BOOST_CONSTEXPR_OR_CONST value_type separator = Separator;
    static BOOST_CONSTEXPR_OR_CONST value_type preferred_separator = PreferredSeparator;
    static BOOST_CONSTEXPR_OR_CONST value_type dot = Dot;
};

#if defined(BOOST_NO_CXX17_INLINE_VARIABLES)
template< typename Char, Char Separator, Char PreferredSeparator, Char Dot >
BOOST_CONSTEXPR_OR_CONST typename path_constants< Char, Separator, PreferredSeparator, Dot >::value_type
path_constants< Char, Separator, PreferredSeparator, Dot >::separator;
template< typename Char, Char Separator, Char PreferredSeparator, Char Dot >
BOOST_CONSTEXPR_OR_CONST typename path_constants< Char, Separator, PreferredSeparator, Dot >::value_type
path_constants< Char, Separator, PreferredSeparator, Dot >::preferred_separator;
template< typename Char, Char Separator, Char PreferredSeparator, Char Dot >
BOOST_CONSTEXPR_OR_CONST typename path_constants< Char, Separator, PreferredSeparator, Dot >::value_type
path_constants< Char, Separator, PreferredSeparator, Dot >::dot;
#endif

class path_iterator;
class path_reverse_iterator;

} // namespace path_impl

struct path_algorithms
{
    // A struct that denotes a contiguous range of characters in a string. A lightweight alternative to string_view.
    struct substring
    {
        std::size_t pos;
        std::size_t size;
    };

    using value_type = path_traits::path_native_char_type;
    using string_type = std::basic_string< value_type >;

    static bool has_filename_v3(path const& p);
    static bool has_filename_v4(path const& p);
    BOOST_FILESYSTEM_DECL static path filename_v3(path const& p);
    static path filename_v4(path const& p);

    BOOST_FILESYSTEM_DECL static path stem_v3(path const& p);
    BOOST_FILESYSTEM_DECL static path stem_v4(path const& p);
    BOOST_FILESYSTEM_DECL static path extension_v3(path const& p);
    static path extension_v4(path const& p);

    BOOST_FILESYSTEM_DECL static void remove_filename_v3(path& p);
    BOOST_FILESYSTEM_DECL static void remove_filename_v4(path& p);

    BOOST_FILESYSTEM_DECL static void replace_extension_v3(path& p, path const& new_extension);
    BOOST_FILESYSTEM_DECL static void replace_extension_v4(path& p, path const& new_extension);

    BOOST_FILESYSTEM_DECL static path lexically_normal_v3(path const& p);
    BOOST_FILESYSTEM_DECL static path lexically_normal_v4(path const& p);

    BOOST_FILESYSTEM_DECL static path generic_path_v3(path const& p);
    BOOST_FILESYSTEM_DECL static path generic_path_v4(path const& p);

#if defined(BOOST_FILESYSTEM_WINDOWS_API)
    BOOST_FILESYSTEM_DECL static void make_preferred_v3(path& p);
    BOOST_FILESYSTEM_DECL static void make_preferred_v4(path& p);
#endif

    BOOST_FILESYSTEM_DECL static int compare_v3(path const& left, path const& right);
    BOOST_FILESYSTEM_DECL static int compare_v4(path const& left, path const& right);

    BOOST_FILESYSTEM_DECL static void append_v3(path& p, const value_type* b, const value_type* e);
    BOOST_FILESYSTEM_DECL static void append_v4(path& p, const value_type* b, const value_type* e);
    static void append_v4(path& left, path const& right);

    //  Returns: If separator is to be appended, m_pathname.size() before append. Otherwise 0.
    //  Note: An append is never performed if size()==0, so a returned 0 is unambiguous.
    BOOST_FILESYSTEM_DECL static string_type::size_type append_separator_if_needed(path& p);
    BOOST_FILESYSTEM_DECL static void erase_redundant_separator(path& p, string_type::size_type sep_pos);

    BOOST_FILESYSTEM_DECL static string_type::size_type find_root_name_size(path const& p);
    BOOST_FILESYSTEM_DECL static string_type::size_type find_root_path_size(path const& p);
    BOOST_FILESYSTEM_DECL static substring find_root_directory(path const& p);
    BOOST_FILESYSTEM_DECL static substring find_relative_path(path const& p);
    BOOST_FILESYSTEM_DECL static string_type::size_type find_parent_path_size(path const& p);
    BOOST_FILESYSTEM_DECL static string_type::size_type find_filename_v4_size(path const& p);
    BOOST_FILESYSTEM_DECL static string_type::size_type find_extension_v4_size(path const& p);

    BOOST_FILESYSTEM_DECL static int lex_compare_v3
    (
        path_impl::path_iterator first1, path_impl::path_iterator const& last1,
        path_impl::path_iterator first2, path_impl::path_iterator const& last2
    );
    BOOST_FILESYSTEM_DECL static int lex_compare_v4
    (
        path_impl::path_iterator first1, path_impl::path_iterator const& last1,
        path_impl::path_iterator first2, path_impl::path_iterator const& last2
    );

    BOOST_FILESYSTEM_DECL static void increment_v3(path_impl::path_iterator& it);
    BOOST_FILESYSTEM_DECL static void increment_v4(path_impl::path_iterator& it);
    BOOST_FILESYSTEM_DECL static void decrement_v3(path_impl::path_iterator& it);
    BOOST_FILESYSTEM_DECL static void decrement_v4(path_impl::path_iterator& it);
};

} // namespace detail

/*!
 * \brief Filesystem path class
 *
 * An object of class `path` represents a path, and contains a pathname. Such an object is concerned
 * only with the lexical and syntactic aspects of a path. The path does not necessarily exist in external
 * storage, and the pathname is not necessarily valid for the current operating system or for a particular
 * file system.
 */
class path :
    public detail::path_impl::path_constants<
#ifdef BOOST_FILESYSTEM_WINDOWS_API
        detail::path_traits::path_native_char_type, L'/', L'\\', L'.'
#else
        detail::path_traits::path_native_char_type, '/', '/', '.'
#endif
    >
{
    friend class detail::path_impl::path_iterator;
    friend class detail::path_impl::path_reverse_iterator;
    friend struct detail::path_algorithms;

public:
    //! Character type used by the operating system API to represent paths
    using value_type = detail::path_algorithms::value_type;
#if !defined(BOOST_FILESYSTEM_DOXYGEN)
    using string_type = detail::path_algorithms::string_type;
    using codecvt_type = detail::path_traits::codecvt_type;
#else
    //! String type used by the operating system API to represent paths
    using string_type = std::basic_string< value_type >;
    //! Character code conversion facet type used to convert path strings between character types
    using codecvt_type = std::codecvt< wchar_t, char, std::mbstate_t >;

    /*!
     * Character used to separate path elements in generic paths.
     */
    static constexpr value_type separator = detail::separator;
    /*!
     * Character used to separate path elements in native paths.
     *
     * This character may be the same as `separator` or different and depends on the target platform
     * conventions. It is "/" on POSIX systems and "\" on Windows.
     */
    static constexpr value_type preferred_separator = detail::preferred_separator;
    /*!
     * Character used to separate file name and extension.
     *
     * It is "." on POSIX systems and Windows.
     */
    static constexpr value_type dot = detail::dot;
#endif

    //  ----- character encoding conversions -----

    //  Following the principle of least astonishment, path input arguments
    //  passed to or obtained from the operating system via objects of
    //  class path behave as if they were directly passed to or
    //  obtained from the O/S API, unless conversion is explicitly requested.
    //
    //  POSIX specfies that path strings are passed unchanged to and from the
    //  API. Note that this is different from the POSIX command line utilities,
    //  which convert according to a locale.
    //
    //  Thus for POSIX, char strings do not undergo conversion.  wchar_t strings
    //  are converted to/from char using the path locale or, if a conversion
    //  argument is given, using a conversion object modeled on
    //  std::wstring_convert.
    //
    //  The path locale, which is global to the thread, can be changed by the
    //  imbue() function. It is initialized to an implementation defined locale.
    //
    //  For Windows, wchar_t strings do not undergo conversion. char strings
    //  are converted using the "ANSI" or "OEM" code pages, as determined by
    //  the AreFileApisANSI() function, or, if a conversion argument is given,
    //  using a conversion object modeled on std::wstring_convert.
    //
    //  See m_pathname comments for further important rationale.

    //  TODO: rules needed for operating systems that use / or .
    //  differently, or format directory paths differently from file paths.
    //
    //  **********************************************************************************
    //
    //  More work needed: How to handle an operating system that may have
    //  slash characters or dot characters in valid filenames, either because
    //  it doesn't follow the POSIX standard, or because it allows MBCS
    //  filename encodings that may contain slash or dot characters. For
    //  example, ISO/IEC 2022 (JIS) encoding which allows switching to
    //  JIS x0208-1983 encoding. A valid filename in this set of encodings is
    //  0x1B 0x24 0x42 [switch to X0208-1983] 0x24 0x2F [U+304F Kiragana letter KU]
    //                                             ^^^^
    //  Note that 0x2F is the ASCII slash character
    //
    //  **********************************************************************************

    //  Supported source arguments: half-open iterator range, container, c-array,
    //  and single pointer to null terminated string.

    //  All source arguments except pointers to null terminated byte strings support
    //  multi-byte character strings which may have embedded nulls. Embedded null
    //  support is required for some Asian languages on Windows.

    //  "const codecvt_type& cvt=codecvt()" default arguments are not used because this
    //  limits the impact of locale("") initialization failures on POSIX systems to programs
    //  that actually depend on locale(""). It further ensures that exceptions thrown
    //  as a result of such failues occur after main() has started, so can be caught.

private:
    //! \cond
    //! Assignment operation
    class assign_op
    {
    private:
        path& m_self;

    public:
        using result_type = void;

        explicit assign_op(path& self) noexcept : m_self(self) {}

        result_type operator() (const value_type* source, const value_type* source_end, const codecvt_type* = nullptr) const
        {
            m_self.m_pathname.assign(source, source_end);
        }

        template< typename OtherChar >
        result_type operator() (const OtherChar* source, const OtherChar* source_end, const codecvt_type* cvt = nullptr) const
        {
            m_self.m_pathname.clear();
            detail::path_traits::convert(source, source_end, m_self.m_pathname, cvt);
        }
    };

    //! Concatenation operation
    class concat_op
    {
    private:
        path& m_self;

    public:
        using result_type = void;

        explicit concat_op(path& self) noexcept : m_self(self) {}

        result_type operator() (const value_type* source, const value_type* source_end, const codecvt_type* = nullptr) const
        {
            m_self.m_pathname.append(source, source_end);
        }

        template< typename OtherChar >
        result_type operator() (const OtherChar* source, const OtherChar* source_end, const codecvt_type* cvt = nullptr) const
        {
            detail::path_traits::convert(source, source_end, m_self.m_pathname, cvt);
        }
    };

    //! Path appending operation
    class append_op
    {
    private:
        path& m_self;

    public:
        using result_type = void;

        explicit append_op(path& self) noexcept : m_self(self) {}

        BOOST_FORCEINLINE result_type operator() (const value_type* source, const value_type* source_end, const codecvt_type* = nullptr) const
        {
            m_self.append(source, source_end);
        }

        template< typename OtherChar >
        BOOST_FORCEINLINE result_type operator() (const OtherChar* source, const OtherChar* source_end, const codecvt_type* cvt = nullptr) const
        {
            string_type src;
            detail::path_traits::convert(source, source_end, src, cvt);
            m_self.append(src.data(), src.data() + src.size());
        }
    };

    //! Path comparison operation
    class compare_op
    {
    private:
        path const& m_self;

    public:
        using result_type = int;

        explicit compare_op(path const& self) noexcept : m_self(self) {}

        result_type operator() (const value_type* source, const value_type* source_end, const codecvt_type* = nullptr) const;

        template< typename OtherChar >
        result_type operator() (const OtherChar* source, const OtherChar* source_end, const codecvt_type* cvt = nullptr) const;
    };

    //! \endcond

public:
    //! Iterator over path elements
    using iterator = detail::path_impl::path_iterator;
    //! Iterator over path elements
    using const_iterator = detail::path_impl::path_iterator;
    //! Reverse iterator over path elements
    using reverse_iterator = detail::path_impl::path_reverse_iterator;
    //! Reverse iterator over path elements
    using const_reverse_iterator = detail::path_impl::path_reverse_iterator;

public:
    //! \name constructors
    //! @{

    /*!
     * \brief Default constructor.
     *
     * \post `this->empty() == true`.
     *
     * \par Effects:
     * Constructs an empty path.
     *
     * \throws nothrow
     */
    path() noexcept {}
    /*!
     * \brief Copy constructor.
     *
     * \par Effects:
     * Constructs a copy of `p`.
     *
     * \post `*this == p`.
     *
     * \param p Path object to copy.
     *
     * \exception std::bad_alloc on memory allocation failure.
     */
    path(path const& p) : m_pathname(p.m_pathname) {}
    /*!
     * \brief Copy constructor.
     *
     * \par Effects:
     * As if `path(p)`.
     *
     * \note The `cvt` character code conversion facet is unused and provided for historical reasons,
     *       for signature compatibility with constructors performing character code conversion.
     */
    path(path const& p, codecvt_type const& cvt) : m_pathname(p.m_pathname) {}

    /*!
     * \brief Move constructor.
     *
     * \par Effects:
     * Move-constructs from `p`.
     *
     * \post `*this` is equal to `p` before the operation.
     *
     * \param p Path object to move from.
     *
     * \throws nothrow
     */
    path(path&& p) noexcept : m_pathname(static_cast< string_type&& >(p.m_pathname))
    {
    }
    /*!
     * \brief Move constructor.
     *
     * \par Effects:
     * As if `path(std::move(p))`.
     *
     * \note The `cvt` character code conversion facet is unused and provided for historical reasons,
     *       for signature compatibility with constructors performing character code conversion.
     */
    path(path&& p, codecvt_type const& cvt) noexcept : m_pathname(static_cast< string_type&& >(p.m_pathname))
    {
    }

    //! \cond
    path(const value_type* s) : m_pathname(s) {}
    path(const value_type* s, codecvt_type const&) : m_pathname(s) {}
    path(string_type const& s) : m_pathname(s) {}
    path(string_type const& s, codecvt_type const&) : m_pathname(s) {}
#if !defined(BOOST_NO_CXX17_HDR_STRING_VIEW)
    path(std::basic_string_view< value_type > const& s) : m_pathname(s) {}
    path(std::basic_string_view< value_type > const& s, codecvt_type const&) : m_pathname(s) {}
#endif
    //! \endcond

    /*!
     * \brief Initializing constructor.
     *
     * \par Effects:
     * As if `path(source, path::codecvt())`.
     */
    template<
        typename Source
        //! \cond
        , typename = typename std::enable_if<
            detail::conjunction<
                detail::path_traits::is_path_source< typename std::remove_cv< Source >::type >,
                detail::negation< detail::path_traits::is_native_path_source< typename std::remove_cv< Source >::type > >
            >::value
        >::type
        //! \endcond
    >
    path(Source const& source)
    {
        assign(source);
    }

    /*!
     * \brief Initializing constructor.
     *
     * \par Effects:
     * Constructs a path from `source`. Uses `cvt` facet to perform character code conversion, if needed.
     *
     * \pre `source` is a valid path source.
     *
     * \param source Path source to construct the path from.
     * \param cvt Character code conversion facet.
     *
     * \throws std::bad_alloc on memory allocation failure.
     * \throws boost::system::system_error in case of character code conversion errors.
     */
    template<
        typename Source
        //! \cond
        , typename = typename std::enable_if<
            detail::conjunction<
                detail::path_traits::is_path_source< typename std::remove_cv< Source >::type >,
                detail::negation< detail::path_traits::is_native_path_source< typename std::remove_cv< Source >::type > >
            >::value
        >::type
        //! \endcond
    >
    explicit path(Source const& source, codecvt_type const& cvt)
    {
        assign(source, cvt);
    }

    /*!
     * \brief Initializing constructor.
     *
     * \par Effects:
     * Constructs a path by moving from `s`.
     *
     * \post `*this` is equal to `s` before the operation.
     *
     * \param s Path source to construct the path from.
     *
     * \throws nothrow
     */
    path(string_type&& s) noexcept : m_pathname(static_cast< string_type&& >(s))
    {
    }
    /*!
     * \brief Initializing constructor.
     *
     * \par Effects:
     * As if `path(std::move(s))`.
     *
     * \note The `cvt` character code conversion facet is unused and provided for historical reasons,
     *       for signature compatibility with constructors performing character code conversion.
     */
    path(string_type&& s, codecvt_type const& cvt) noexcept : m_pathname(static_cast< string_type&& >(s))
    {
    }

    //! \cond
    path(const value_type* begin, const value_type* end) : m_pathname(begin, end) {}
    path(const value_type* begin, const value_type* end, codecvt_type const&) : m_pathname(begin, end) {}
    //! \endcond

    /*!
     * \brief Initializing constructor.
     *
     * \par Effects:
     * As if `path(begin, end, path::codecvt())`.
     */
    template<
        typename InputIterator
        //! \cond
        , typename = typename std::enable_if<
            detail::conjunction<
                detail::path_traits::is_path_source_iterator< InputIterator >,
                detail::negation< detail::path_traits::is_native_char_ptr< InputIterator > >
            >::value
        >::type
        //! \endcond
    >
    path(InputIterator begin, InputIterator end)
    {
        if (begin != end)
        {
            using source_t = std::basic_string< typename std::iterator_traits< InputIterator >::value_type >;
            source_t source(begin, end);
            assign(static_cast< source_t&& >(source));
        }
    }

    /*!
     * \brief Initializing constructor.
     *
     * \par Effects:
     * Constructs a path from a range of characters denoted by `begin` and `end`. Uses `cvt`
     * facet to perform character code conversion, if needed.
     *
     * \pre `[begin, end)` is a valid range of path characters.
     *
     * \param begin Iterator pointing to the beginning of the range of characters to construct the path from.
     * \param end Iterator pointing to the end of the range of characters to construct the path from.
     * \param cvt Character code conversion facet.
     *
     * \throws std::bad_alloc on memory allocation failure.
     * \throws boost::system::system_error in case of character code conversion errors.
     */
    template<
        typename InputIterator
        //! \cond
        , typename = typename std::enable_if<
            detail::conjunction<
                detail::path_traits::is_path_source_iterator< InputIterator >,
                detail::negation< detail::path_traits::is_native_char_ptr< InputIterator > >
            >::value
        >::type
        //! \endcond
    >
    path(InputIterator begin, InputIterator end, codecvt_type const& cvt)
    {
        if (begin != end)
        {
            using source_t = std::basic_string< typename std::iterator_traits< InputIterator >::value_type >;
            source_t source(begin, end);
            assign(static_cast< source_t&& >(source), cvt);
        }
    }

    //! Constructor from null pointers is disabled
    path(std::nullptr_t) = delete;

    //! @}

    //! \name assignment
    //! @{

    /*!
     * \brief Copy assignment.
     *
     * \returns `assign(p)`.
     */
    path& operator=(path const& p);
    /*!
     * \brief Copy assignment.
     *
     * \par Effects:
     * Copies the path `p` into `*this`.
     *
     * \post `*this == p`.
     *
     * \param p Path to assign.
     * \returns `*this`.
     *
     * \throws std::bad_alloc on memory allocation failure.
     */
    path& assign(path const& p)
    {
        m_pathname = p.m_pathname;
        return *this;
    }
    /*!
     * \brief Copy assignment.
     *
     * \returns `assign(p)`.
     *
     * \note The `cvt` character code conversion facet is unused and provided for historical reasons,
     *       for signature compatibility with overloads performing character code conversion.
     */
    path& assign(path const& p, codecvt_type const& cvt)
    {
        m_pathname = p.m_pathname;
        return *this;
    }

    /*!
     * \brief Move assignment.
     *
     * \returns `assign(std::move(p))`.
     */
    path& operator=(path&& p) noexcept
    {
        m_pathname = static_cast< string_type&& >(p.m_pathname);
        return *this;
    }
    /*!
     * \brief Move assignment.
     *
     * \par Effects:
     * Move-assigns the path `p` to `*this`.
     *
     * \post `*this == p`.
     *
     * \param p Path to assign.
     * \returns `*this`.
     *
     * \throws nothrow
     */
    path& assign(path&& p) noexcept
    {
        m_pathname = static_cast< string_type&& >(p.m_pathname);
        return *this;
    }
    /*!
     * \brief Move assignment.
     *
     * \returns `assign(std::move(p))`.
     *
     * \note The `cvt` character code conversion facet is unused and provided for historical reasons,
     *       for signature compatibility with overloads performing character code conversion.
     */
    path& assign(path&& p, codecvt_type const& cvt) noexcept
    {
        m_pathname = static_cast< string_type&& >(p.m_pathname);
        return *this;
    }

    /*!
     * \brief Move-assigns characters in the string to the path.
     *
     * \returns `assign(std::move(s))`.
     */
    path& operator=(string_type&& s) noexcept
    {
        m_pathname = static_cast< string_type&& >(s);
        return *this;
    }
    /*!
     * \brief Move-assigns characters in the string to the path.
     *
     * \par Effects:
     * Move-assigns the string `s` to `*this`.
     *
     * \post `*this == s`.
     *
     * \param s String to assign.
     * \returns `*this`.
     *
     * \throws nothrow
     */
    path& assign(string_type&& s) noexcept
    {
        m_pathname = static_cast< string_type&& >(s);
        return *this;
    }
    /*!
     * \brief Move-assigns characters in the string to the path.
     *
     * \returns `assign(std::move(s))`.
     *
     * \note The `cvt` character code conversion facet is unused and provided for historical reasons,
     *       for signature compatibility with overloads performing character code conversion.
     */
    path& assign(string_type&& s, codecvt_type const& cvt) noexcept
    {
        m_pathname = static_cast< string_type&& >(s);
        return *this;
    }

    /*!
     * \brief Assigns characters in `source` to the path.
     *
     * \returns `assign(source)`.
     */
    template< typename Source >
#if !defined(BOOST_FILESYSTEM_DOXYGEN)
    typename std::enable_if<
        detail::disjunction<
            detail::path_traits::is_path_source< typename std::remove_cv< Source >::type >,
            detail::path_traits::is_convertible_to_path_source< typename std::remove_cv< Source >::type >
        >::value,
        path&
    >::type
#else
    path&
#endif
    operator=(Source const& source)
    {
        return assign(source);
    }
    /*!
     * \brief Assigns characters in `source` to the path.
     *
     * \returns `assign(source, path::codecvt())`.
     */
    template< typename Source >
#if !defined(BOOST_FILESYSTEM_DOXYGEN)
    typename std::enable_if<
        detail::path_traits::is_path_source< typename std::remove_cv< Source >::type >::value,
        path&
    >::type
#else
    path&
#endif
    assign(Source const& source)
    {
        detail::path_traits::dispatch(source, assign_op(*this));
        return *this;
    }

    //! \cond
    template< typename Source >
    typename std::enable_if<
        detail::conjunction<
            detail::path_traits::is_convertible_to_path_source< typename std::remove_cv< Source >::type >,
            detail::negation< detail::path_traits::is_path_source< typename std::remove_cv< Source >::type > >
        >::value,
        path&
    >::type assign(Source const& source)
    {
        detail::path_traits::dispatch_convertible(source, assign_op(*this));
        return *this;
    }
    //! \endcond

    /*!
     * \brief Assigns characters in `source` to the path.
     *
     * \par Effects:
     * Assigns `source` to `*this`. Uses `cvt` facet to perform character code conversion,
     * if needed.
     *
     * \pre `source` is a valid path source.
     *
     * \param source Path source to assign to the path object.
     * \param cvt Character code conversion facet.
     * \returns `*this`.
     *
     * \throws std::bad_alloc on memory allocation failure.
     * \throws boost::system::system_error in case of character code conversion errors.
     */
    template< typename Source >
#if !defined(BOOST_FILESYSTEM_DOXYGEN)
    typename std::enable_if<
        detail::path_traits::is_path_source< typename std::remove_cv< Source >::type >::value,
        path&
    >::type
#else
    path&
#endif
    assign(Source const& source, codecvt_type const& cvt)
    {
        detail::path_traits::dispatch(source, assign_op(*this), &cvt);
        return *this;
    }

    //! \cond
    template< typename Source >
    typename std::enable_if<
        detail::conjunction<
            detail::path_traits::is_convertible_to_path_source< typename std::remove_cv< Source >::type >,
            detail::negation< detail::path_traits::is_path_source< typename std::remove_cv< Source >::type > >
        >::value,
        path&
    >::type assign(Source const& source, codecvt_type const& cvt)
    {
        detail::path_traits::dispatch_convertible(source, assign_op(*this), &cvt);
        return *this;
    }

    path& assign(const value_type* begin, const value_type* end)
    {
        m_pathname.assign(begin, end);
        return *this;
    }

    path& assign(const value_type* begin, const value_type* end, codecvt_type const&)
    {
        m_pathname.assign(begin, end);
        return *this;
    }
    //! \endcond

    /*!
     * \brief Assigns characters in the iterator range to the path.
     *
     * \returns `assign(begin, end, path::codecvt())`.
     */
    template< typename InputIterator >
#if !defined(BOOST_FILESYSTEM_DOXYGEN)
    typename std::enable_if<
        detail::conjunction<
            detail::path_traits::is_path_source_iterator< InputIterator >,
            detail::negation< detail::path_traits::is_native_char_ptr< InputIterator > >
        >::value,
        path&
    >::type
#else
    path&
#endif
    assign(InputIterator begin, InputIterator end)
    {
        m_pathname.clear();
        if (begin != end)
        {
            using source_t = std::basic_string< typename std::iterator_traits< InputIterator >::value_type >;
            source_t source(begin, end);
            assign(static_cast< source_t&& >(source));
        }
        return *this;
    }

    /*!
     * \brief Assigns characters in the iterator range to the path.
     *
     * \par Effects:
     * Assigns a range of characters denoted by `begin` and `end` to `*this`. Uses `cvt`
     * facet to perform character code conversion, if needed.
     *
     * \pre `[begin, end)` is a valid range of path characters.
     *
     * \param begin Iterator pointing to the beginning of the range of characters to assign to the path.
     * \param end Iterator pointing to the end of the range of characters to assign to the path.
     * \param cvt Character code conversion facet.
     * \returns `*this`.
     *
     * \throws std::bad_alloc on memory allocation failure.
     * \throws boost::system::system_error in case of character code conversion errors.
     */
    template< typename InputIterator >
#if !defined(BOOST_FILESYSTEM_DOXYGEN)
    typename std::enable_if<
        detail::conjunction<
            detail::path_traits::is_path_source_iterator< InputIterator >,
            detail::negation< detail::path_traits::is_native_char_ptr< InputIterator > >
        >::value,
        path&
    >::type
#else
    path&
#endif
    assign(InputIterator begin, InputIterator end, codecvt_type const& cvt)
    {
        m_pathname.clear();
        if (begin != end)
        {
            using source_t = std::basic_string< typename std::iterator_traits< InputIterator >::value_type >;
            source_t source(begin, end);
            assign(static_cast< source_t&& >(source), cvt);
        }
        return *this;
    }

    //! Assignment from null pointers is disabled
    path& operator= (std::nullptr_t) = delete;

    //! @}

    //! \name concatenation
    //! @{

    //! \cond
    path& operator+=(value_type c)
    {
        m_pathname.push_back(c);
        return *this;
    }
    //! \endcond

    /*!
     * \brief Concatenates character `c` to the end of the path.
     *
     * \pre `Char` is a path character type.
     *
     * \returns `concat(&c, &c + 1)`.
     */
    template< typename Char >
#if !defined(BOOST_FILESYSTEM_DOXYGEN)
    typename std::enable_if<
        detail::path_traits::is_path_char_type< Char >::value,
        path&
    >::type
#else
    path&
#endif
    operator+=(Char c)
    {
        Char tmp[2];
        tmp[0] = c;
        tmp[1] = static_cast< Char >(0);
        concat_op(*this)(tmp, tmp + 1);
        return *this;
    }

    /*!
     * \brief Concatenates characters from `p` to the end of the path.
     *
     * \returns `concat(p)`.
     */
    path& operator+=(path const& p);
    /*!
     * \brief Concatenates characters from `p` to the end of the path.
     *
     * \par Effects:
     * Copies the path `p` to the end of `*this`.
     *
     * \param p Path to concatenate.
     * \returns `*this`.
     *
     * \throws std::bad_alloc on memory allocation failure.
     */
    path& concat(path const& p)
    {
        m_pathname.append(p.m_pathname);
        return *this;
    }
    /*!
     * \brief Concatenates characters from `p` to the end of the path.
     *
     * \returns `concat(p)`.
     *
     * \note The `cvt` character code conversion facet is unused and provided for historical reasons,
     *       for signature compatibility with overloads performing character code conversion.
     */
    path& concat(path const& p, codecvt_type const& cvt)
    {
        m_pathname.append(p.m_pathname);
        return *this;
    }

    /*!
     * \brief Concatenates characters from `source` to the end of the path.
     *
     * \returns `concat(source)`.
     */
    template< typename Source >
#if !defined(BOOST_FILESYSTEM_DOXYGEN)
    typename std::enable_if<
        detail::path_traits::is_convertible_to_path_source< typename std::remove_cv< Source >::type >::value,
        path&
    >::type
#else
    path&
#endif
    operator+=(Source const& source)
    {
        return concat(source);
    }
    /*!
     * \brief Concatinates characters from `source` to the end of the path.
     *
     * \returns `concat(source, path::codecvt())`.
     */
    template< typename Source >
#if !defined(BOOST_FILESYSTEM_DOXYGEN)
    typename std::enable_if<
        detail::path_traits::is_path_source< typename std::remove_cv< Source >::type >::value,
        path&
    >::type
#else
    path&
#endif
    concat(Source const& source)
    {
        detail::path_traits::dispatch(source, concat_op(*this));
        return *this;
    }

    //! \cond
    template< typename Source >
    typename std::enable_if<
        detail::conjunction<
            detail::path_traits::is_convertible_to_path_source< typename std::remove_cv< Source >::type >,
            detail::negation< detail::path_traits::is_path_source< typename std::remove_cv< Source >::type > >
        >::value,
        path&
    >::type concat(Source const& source)
    {
        detail::path_traits::dispatch_convertible(source, concat_op(*this));
        return *this;
    }
    //! \endcond

    /*!
     * \brief Concatinates characters from `source` to the end of the path.
     *
     * \par Effects:
     * Concatenates the characters from `source` to the end of `*this`. Uses `cvt` facet
     * to perform character code conversion, if needed.
     *
     * \pre `source` is a valid path source.
     *
     * \param source Path source to concatenate to the path object.
     * \param cvt Character code conversion facet.
     * \returns `*this`.
     *
     * \throws std::bad_alloc on memory allocation failure.
     * \throws boost::system::system_error in case of character code conversion errors.
     */
    template< typename Source >
#if !defined(BOOST_FILESYSTEM_DOXYGEN)
    typename std::enable_if<
        detail::path_traits::is_path_source< typename std::remove_cv< Source >::type >::value,
        path&
    >::type
#else
    path&
#endif
    concat(Source const& source, codecvt_type const& cvt)
    {
        detail::path_traits::dispatch(source, concat_op(*this), &cvt);
        return *this;
    }

    //! \cond
    template< typename Source >
    typename std::enable_if<
        detail::conjunction<
            detail::path_traits::is_convertible_to_path_source< typename std::remove_cv< Source >::type >,
            detail::negation< detail::path_traits::is_path_source< typename std::remove_cv< Source >::type > >
        >::value,
        path&
    >::type concat(Source const& source, codecvt_type const& cvt)
    {
        detail::path_traits::dispatch_convertible(source, concat_op(*this), &cvt);
        return *this;
    }

    path& concat(const value_type* begin, const value_type* end)
    {
        m_pathname.append(begin, end);
        return *this;
    }

    path& concat(const value_type* begin, const value_type* end, codecvt_type const&)
    {
        m_pathname.append(begin, end);
        return *this;
    }
    //! \endcond

    /*!
     * \brief Concatinates characters from the iterator range to the end of the path.
     *
     * \returns `concat(begin, end, path::codecvt())`.
     */
    template< typename InputIterator >
#if !defined(BOOST_FILESYSTEM_DOXYGEN)
    typename std::enable_if<
        detail::conjunction<
            detail::path_traits::is_path_source_iterator< InputIterator >,
            detail::negation< detail::path_traits::is_native_char_ptr< InputIterator > >
        >::value,
        path&
    >::type
#else
    path&
#endif
    concat(InputIterator begin, InputIterator end)
    {
        if (begin != end)
        {
            std::basic_string< typename std::iterator_traits< InputIterator >::value_type > source(begin, end);
            detail::path_traits::dispatch(source, concat_op(*this));
        }
        return *this;
    }
    /*!
     * \brief Concatinates characters from the iterator range to the end of the path.
     *
     * \par Effects:
     * Concatenates a range of characters denoted by `begin` and `end` to the end of `*this`.
     * Uses `cvt` facet to perform character code conversion, if needed.
     *
     * \pre `[begin, end)` is a valid range of path characters.
     *
     * \param begin Iterator pointing to the beginning of the range of characters to concatenate to the path.
     * \param end Iterator pointing to the end of the range of characters to concatenate to the path.
     * \param cvt Character code conversion facet.
     * \returns `*this`.
     *
     * \throws std::bad_alloc on memory allocation failure.
     * \throws boost::system::system_error in case of character code conversion errors.
     */
    template< typename InputIterator >
#if !defined(BOOST_FILESYSTEM_DOXYGEN)
    typename std::enable_if<
        detail::conjunction<
            detail::path_traits::is_path_source_iterator< InputIterator >,
            detail::negation< detail::path_traits::is_native_char_ptr< InputIterator > >
        >::value,
        path&
    >::type
#else
    path&
#endif
    concat(InputIterator begin, InputIterator end, codecvt_type const& cvt)
    {
        if (begin != end)
        {
            std::basic_string< typename std::iterator_traits< InputIterator >::value_type > source(begin, end);
            detail::path_traits::dispatch(source, concat_op(*this), &cvt);
        }
        return *this;
    }

    //! @}

    //! \name appending
    //! @{

    //  if a separator is added, it is the preferred separator for the platform;
    //  slash for POSIX, backslash for Windows

    /*!
     * \brief Appends `p` to the end of the path.
     *
     * \returns `append(p)`.
     */
    path& operator/=(path const& p);
    /*!
     * \brief Appends `p` to the end of the path.
     *
     * \par Effects:
     * \parblock
     *
     * \par v3:
     * \parblock
     * Concatenates `path::preferred_separator` to `*this`, converting format and encoding if required, unless:
     *
     *   \li an added separator would be redundant, or
     *   \li would change a relative path to an absolute path, or
     *   \li `p.empty()`, or
     *   \li `*p.native().cbegin()` is a directory separator.
     *
     * Then concatenates `p.native()` to `*this`.
     * \endparblock
     *
     * \par v4:
     * \parblock
     * If `p.is_absolute() || (p.has_root_name() && p.root_name() != this->root_name())`, assigns `p` to `*this`.
     * Otherwise, modifies `*this` as if by these steps:
     *
     *   \li If `p.has_root_directory()`, removes root directory and relative path, if any.
     *   \li Let `x` be a `path` with contents of `p` without a root name. If `this->has_filename()` is `true` and `x`
     *       does not start with a directory separator, concatenates `path::preferred_separator`.
     *   \li Concatenates `x.native()`.
     *
     * \note Whether the path is absolute or not depends on the target OS conventions. Because of this, the result of
     *       append operation may be different for different operating systems for some paths. For example,
     *       `path("//net/foo") / "/bar"` will result in `"/bar"` on POSIX systems and `"//net/foo/bar"` on Windows
     *       because `"/bar"` is an absolute path on POSIX systems but not on Windows. For portable behavior avoid
     *       appending paths with non-empty root path.
     *
     * \endparblock
     * \endparblock
     *
     * \param p Path to append.
     * \returns `*this`.
     *
     * \throws std::bad_alloc on memory allocation failure.
     */
    path& append(path const& p);
    /*!
     * \brief Appends `p` to the end of the path.
     *
     * \returns `append(p)`.
     *
     * \note The `cvt` character code conversion facet is unused and provided for historical reasons,
     *       for signature compatibility with overloads performing character code conversion.
     */
    path& append(path const& p, codecvt_type const&);

    /*!
     * \brief Appends `source` to the end of the path.
     *
     * \returns `append(path(source))`.
     */
    template< typename Source >
#if !defined(BOOST_FILESYSTEM_DOXYGEN)
    BOOST_FORCEINLINE typename std::enable_if<
        detail::path_traits::is_convertible_to_path_source< typename std::remove_cv< Source >::type >::value,
        path&
    >::type
#else
    path&
#endif
    operator/=(Source const& source)
    {
        return append(source);
    }

    /*!
     * \brief Appends `source` to the end of the path.
     *
     * \returns `append(path(source))`.
     */
    template< typename Source >
#if !defined(BOOST_FILESYSTEM_DOXYGEN)
    BOOST_FORCEINLINE typename std::enable_if<
        detail::path_traits::is_path_source< typename std::remove_cv< Source >::type >::value,
        path&
    >::type
#else
    path&
#endif
    append(Source const& source)
    {
        detail::path_traits::dispatch(source, append_op(*this));
        return *this;
    }

    //! \cond
    template< typename Source >
    BOOST_FORCEINLINE typename std::enable_if<
        detail::conjunction<
            detail::path_traits::is_convertible_to_path_source< typename std::remove_cv< Source >::type >,
            detail::negation< detail::path_traits::is_path_source< typename std::remove_cv< Source >::type > >
        >::value,
        path&
    >::type append(Source const& source)
    {
        detail::path_traits::dispatch_convertible(source, append_op(*this));
        return *this;
    }
    //! \endcond

    /*!
     * \brief Appends `source` to the end of the path.
     *
     * \returns `append(path(source, cvt))`.
     */
    template< typename Source >
#if !defined(BOOST_FILESYSTEM_DOXYGEN)
    BOOST_FORCEINLINE typename std::enable_if<
        detail::path_traits::is_path_source< typename std::remove_cv< Source >::type >::value,
        path&
    >::type
#else
    path&
#endif
    append(Source const& source, codecvt_type const& cvt)
    {
        detail::path_traits::dispatch(source, append_op(*this), &cvt);
        return *this;
    }

    //! \cond
    template< typename Source >
    BOOST_FORCEINLINE typename std::enable_if<
        detail::conjunction<
            detail::path_traits::is_convertible_to_path_source< typename std::remove_cv< Source >::type >,
            detail::negation< detail::path_traits::is_path_source< typename std::remove_cv< Source >::type > >
        >::value,
        path&
    >::type append(Source const& source, codecvt_type const& cvt)
    {
        detail::path_traits::dispatch_convertible(source, append_op(*this), &cvt);
        return *this;
    }

    path& append(const value_type* begin, const value_type* end);
    path& append(const value_type* begin, const value_type* end, codecvt_type const&);
    //! \endcond

    /*!
     * \brief Appends characters from the iterator range to the end of the path.
     *
     * \returns `append(path(begin, end))`.
     */
    template< typename InputIterator >
#if !defined(BOOST_FILESYSTEM_DOXYGEN)
    BOOST_FORCEINLINE typename std::enable_if<
        detail::conjunction<
            detail::path_traits::is_path_source_iterator< InputIterator >,
            detail::negation< detail::path_traits::is_native_char_ptr< InputIterator > >
        >::value,
        path&
    >::type
#else
    path&
#endif
    append(InputIterator begin, InputIterator end)
    {
        std::basic_string< typename std::iterator_traits< InputIterator >::value_type > source(begin, end);
        detail::path_traits::dispatch(source, append_op(*this));
        return *this;
    }

    /*!
     * \brief Appends characters from the iterator range to the end of the path.
     *
     * \returns `append(path(begin, end, cvt))`.
     */
    template< typename InputIterator >
#if !defined(BOOST_FILESYSTEM_DOXYGEN)
    BOOST_FORCEINLINE typename std::enable_if<
        detail::conjunction<
            detail::path_traits::is_path_source_iterator< InputIterator >,
            detail::negation< detail::path_traits::is_native_char_ptr< InputIterator > >
        >::value,
        path&
    >::type
#else
    path&
#endif
    append(InputIterator begin, InputIterator end, const codecvt_type& cvt)
    {
        std::basic_string< typename std::iterator_traits< InputIterator >::value_type > source(begin, end);
        detail::path_traits::dispatch(source, append_op(*this), &cvt);
        return *this;
    }

    //! @}

    //! \name modifiers
    //! @{

    /*!
     * \brief Clears the path.
     *
     * \post `this->empty() == true`.
     *
     * \throws nothrow
     */
    void clear() noexcept { m_pathname.clear(); }

    /*!
     * \brief Converts directory separators to `path::preferred_separator`.
     *
     * \returns `*this`.
     */
    path& make_preferred();

    /*!
     * \brief Removes the trailing filename.
     *
     * \par Effects:
     * \parblock
     *
     * \par v3:
     * \parblock
     * As if `*this = parent_path()`.
     *
     * \note This function is needed to efficiently implement `directory_iterator`. It is exposed
     *       to allow additional uses. The actual implementation may be much more efficient than
     *       `*this = parent_path()`.
     * \endparblock
     *
     * \par v4:
     * \parblock
     * Removes the `filename()` path element.
     *
     * \note Unlike **v3**, the trailing directory separator(s) are not removed.
     * \endparblock
     * \endparblock
     *
     * \returns `*this`.
     */
    path& remove_filename();

    /*!
     * \brief Removes the trailing filename and directory separator.
     *
     * \par Effects:
     * As if `*this = parent_path()`.
     *
     * \note This function is similar to `path::remove_filename` from **v3**, but is also usable in **v4**.
     *
     * \returns `*this`.
     */
    BOOST_FILESYSTEM_DECL path& remove_filename_and_trailing_separators();

    /*!
     * \brief Removes the trailing directory separator.
     *
     * \par Effects:
     * If `*this` ends with a directory separator, removes that separator. Otherwise, keeps
     * `*this` unmodified.
     *
     * \returns `*this`.
     */
    BOOST_FILESYSTEM_DECL path& remove_trailing_separator();

    /*!
     * \brief Replaces the trailing filename.
     *
     * \par Effects:
     * As if `remove_filename().append(replacement)`.
     *
     * \param replacement The replacement filename.
     * \returns `*this`.
     *
     * \sa \ref path::remove_filename, \ref path::append.
     */
    BOOST_FILESYSTEM_DECL path& replace_filename(path const& replacement);

    /*!
     * \brief Replaces extension in the trailing filename.
     *
     * \par Effects:
     * \parblock
     *   \li Any existing `extension()` is removed from the stored path, then
     *   \li iff `new_extension` is not empty and does not begin with a dot character, a dot character
     *       is concatenated to the stored path, then
     *   \li `new_extension` is concatenated to the stored path.
     * \endparblock
     *
     * \param new_extension The replacement extension.
     * \returns `*this`.
     */
    path& replace_extension(path const& new_extension = path());

    /*!
     * \brief Swaps `*this` and `rhs` path objects.
     *
     * \param rhs Path object to swap with.
     *
     * \throws nothrow
     */
    void swap(path& rhs) noexcept { m_pathname.swap(rhs.m_pathname); }

    //! @}

    //! \name observers
    //! @{

    /*!
     * \returns `true` if the path is empty and `false` otherwise.
     *
     * \throws nothrow
     */
    bool empty() const noexcept { return m_pathname.empty(); }

    /*!
     * \returns `filename() == path(".")`.
     *
     * \par Example:
     * \parblock
     *
     * \code
     * std::cout << path(".").filename_is_dot();     // outputs 1
     * std::cout << path("/.").filename_is_dot();    // outputs 1
     * std::cout << path("foo/.").filename_is_dot(); // outputs 1
     * std::cout << path("foo/").filename_is_dot();  // v3 outputs 1, v4 outputs 0
     * std::cout << path("/").filename_is_dot();     // outputs 0
     * std::cout << path("/foo").filename_is_dot();  // outputs 0
     * std::cout << path("/foo.").filename_is_dot(); // outputs 0
     * std::cout << path("..").filename_is_dot();    // outputs 0
     * \endcode
     *
     * See the last bullet item in the path iterators forward traversal order
     * list for why `path("foo/").filename()` is a dot filename in **v3**.
     * \endparblock
     */
    bool filename_is_dot() const;

    /*!
     * \returns `filename() == path("..")`.
     */
    bool filename_is_dot_dot() const;

    /*!
     * \returns `! root_path().empty()`.
     */
    bool has_root_path() const { return detail::path_algorithms::find_root_path_size(*this) > 0; }

    /*!
     * \returns `! root_name().empty()`.
     */
    bool has_root_name() const { return detail::path_algorithms::find_root_name_size(*this) > 0; }

    /*!
     * \returns `! root_directory().empty()`.
     */
    bool has_root_directory() const { return detail::path_algorithms::find_root_directory(*this).size > 0; }

    /*!
     * \returns `! relative_path().empty()`.
     */
    bool has_relative_path() const { return detail::path_algorithms::find_relative_path(*this).size > 0; }

    /*!
     * \returns `! parent_path().empty()`.
     */
    bool has_parent_path() const { return detail::path_algorithms::find_parent_path_size(*this) > 0; }

    /*!
     * \returns `! filename().empty()`.
     */
    bool has_filename() const;

    /*!
     * \returns `! stem().empty()`.
     */
    bool has_stem() const { return !stem().empty(); }

    /*!
     * \returns `! extension().empty()`.
     */
    bool has_extension() const { return !extension().empty(); }

    /*!
     * \returns `! is_absolute()`.
     */
    bool is_relative() const { return !is_absolute(); }

    /*!
     * \returns `true` if the elements of `root_path()` uniquely identify a directory, else `false`.
     *
     * \note On POSIX systems, a path is considered absolute if it has a `[root-directory]`,
     *       and on Windows - if it has both `[root-name]` and `[root-directory]`.
     *
     * \sa \ref path::root_path, \ref path::root_name, \ref path::root_directory.
     */
    bool is_absolute() const
    {
#if defined(BOOST_FILESYSTEM_WINDOWS_API)
        return has_root_name() && has_root_directory();
#else
        return has_root_directory();
#endif
    }

    //  For operating systems that format file paths differently than directory
    //  paths, return values from observers are formatted as file names unless there
    //  is a trailing separator, in which case returns are formatted as directory
    //  paths. POSIX and Windows make no such distinction.

    //  Implementations are permitted to return const values or const references.

    //  The string or path returned by an observer are specified as being formatted
    //  as "native" or "generic".
    //
    //  For POSIX, these are all the same format; slashes and backslashes are as input and
    //  are not modified.
    //
    //  For Windows,   native:    as input; slashes and backslashes are not modified;
    //                            this is the format of the internally stored string.
    //                 generic:   backslashes are converted to slashes

    //  -----  native format observers  -----

    /*!
     * \returns The pathname in the native format.
     */
    string_type const& native() const noexcept { return m_pathname; }
    /*!
     * \returns `native().c_str()`.
     */
    const value_type* c_str() const noexcept { return m_pathname.c_str(); }
    /*!
     * \returns `native().size()`.
     */
    string_type::size_type size() const noexcept { return m_pathname.size(); }

    /*!
     * \returns `string< String >(codecvt())`.
     */
    template< typename String >
    String string() const;

    /*!
     * \pre `String` is a specialization of `std::basic_string` for one of the path character types.
     *
     * \param cvt Character code conversion facet.
     * \returns Pathname returned by `native()`. If `string_type` is different from `String`, performs character
     *          encoding conversion using `cvt`.
     *
     * \throws std::bad_alloc on memory allocation failure.
     * \throws boost::system::system_error in case of character code conversion errors.
     */
    template< typename String >
    String string(codecvt_type const& cvt) const;

#if defined(BOOST_FILESYSTEM_DOXYGEN)

    /*!
     * \returns `string< std::string >()`.
     */
    std::string string() const;
    /*!
     * \returns `string< std::string >(cvt)`
     */
    std::string string(codecvt_type const& cvt) const;

    /*!
     * \returns `string< std::wstring >()`.
     */
    std::wstring wstring() const;
    /*!
     * \returns `string< std::wstring >(cvt)`.
     */
    std::wstring wstring(codecvt_type const& cvt) const;

#elif defined(BOOST_FILESYSTEM_WINDOWS_API)

    std::string string() const
    {
        std::string tmp;
        if (!m_pathname.empty())
            detail::path_traits::convert(m_pathname.data(), m_pathname.data() + m_pathname.size(), tmp);
        return tmp;
    }

    std::string string(codecvt_type const& cvt) const
    {
        std::string tmp;
        if (!m_pathname.empty())
            detail::path_traits::convert(m_pathname.data(), m_pathname.data() + m_pathname.size(), tmp, &cvt);
        return tmp;
    }

    //  string_type is std::wstring, so there is no conversion
    std::wstring const& wstring() const { return m_pathname; }
    std::wstring const& wstring(codecvt_type const&) const { return m_pathname; }

#else

    //  string_type is std::string, so there is no conversion
    std::string const& string() const { return m_pathname; }
    std::string const& string(codecvt_type const&) const { return m_pathname; }

    std::wstring wstring() const
    {
        std::wstring tmp;
        if (!m_pathname.empty())
            detail::path_traits::convert(m_pathname.data(), m_pathname.data() + m_pathname.size(), tmp);
        return tmp;
    }

    std::wstring wstring(codecvt_type const& cvt) const
    {
        std::wstring tmp;
        if (!m_pathname.empty())
            detail::path_traits::convert(m_pathname.data(), m_pathname.data() + m_pathname.size(), tmp, &cvt);
        return tmp;
    }

#endif

    //  -----  generic format observers  -----

    //  Experimental generic function returning generic formatted path (i.e. separators
    //  are forward slashes). Motivation: simpler than a family of generic_*string
    //  functions.
    /*!
     * \returns The pathname in the generic format.
     */
    path generic_path() const;

    /*!
     * \returns `generic_string< String >(codecvt())`.
     */
    template< typename String >
    String generic_string() const;

    /*!
     * \pre `String` is a specialization of `std::basic_string` for one of the path character types.
     *
     * \param cvt Character code conversion facet.
     * \returns Pathname returned by `generic_path()`. If `string_type` is different from `String`, performs
     *          character encoding conversion using `cvt`.
     *
     * \throws std::bad_alloc on memory allocation failure.
     * \throws boost::system::system_error in case of character code conversion errors.
     */
    template< typename String >
    String generic_string(codecvt_type const& cvt) const;

    /*!
     * \returns `generic_string< std::string >()`.
     */
    std::string generic_string() const { return generic_path().string(); }
    /*!
     * \returns `generic_string< std::string >(cvt)`.
     */
    std::string generic_string(codecvt_type const& cvt) const { return generic_path().string(cvt); }
    /*!
     * \returns `generic_string< std::wstring >()`.
     */
    std::wstring generic_wstring() const { return generic_path().wstring(); }
    /*!
     * \returns `generic_string< std::wstring >(cvt)`.
     */
    std::wstring generic_wstring(codecvt_type const& cvt) const { return generic_path().wstring(cvt); }

    //! @}

    //! \name comparison
    //! @{

    /*!
     * \returns A value less than 0 if the elements of `*this` are lexicographically less than the elements of `p`,
     *          otherwise a value greater than 0 if the elements of `*this` are lexicographically greater than
     *          the elements of `p`, otherwise 0.
     *
     * \remark The elements are determined as if by iteration over the half-open range [`begin()`, `end()`) for
     *         `*this` and `p`.
     */
    int compare(path const& p) const;

    /*!
     * \returns `compare(path(source))`.
     */
    template< typename Source >
#if !defined(BOOST_FILESYSTEM_DOXYGEN)
    BOOST_FORCEINLINE typename std::enable_if<
        detail::path_traits::is_path_source< typename std::remove_cv< Source >::type >::value,
        int
    >::type
#else
    int
#endif
    compare(Source const& source) const
    {
        return detail::path_traits::dispatch(source, compare_op(*this));
    }

    //! \cond
    template< typename Source >
    BOOST_FORCEINLINE typename std::enable_if<
        detail::conjunction<
            detail::path_traits::is_convertible_to_path_source< typename std::remove_cv< Source >::type >,
            detail::negation< detail::path_traits::is_path_source< typename std::remove_cv< Source >::type > >
        >::value,
        int
    >::type compare(Source const& source) const
    {
        return detail::path_traits::dispatch_convertible(source, compare_op(*this));
    }
    //! \endcond

    /*!
     * \returns `compare(path(source, cvt))`.
     */
    template< typename Source >
#if !defined(BOOST_FILESYSTEM_DOXYGEN)
    BOOST_FORCEINLINE typename std::enable_if<
        detail::path_traits::is_path_source< typename std::remove_cv< Source >::type >::value,
        int
    >::type
#else
    int
#endif
    compare(Source const& source, codecvt_type const& cvt) const
    {
        return detail::path_traits::dispatch(source, compare_op(*this), &cvt);
    }

    //! \cond
    template< typename Source >
    BOOST_FORCEINLINE typename std::enable_if<
        detail::conjunction<
            detail::path_traits::is_convertible_to_path_source< typename std::remove_cv< Source >::type >,
            detail::negation< detail::path_traits::is_path_source< typename std::remove_cv< Source >::type > >
        >::value,
        int
    >::type compare(Source const& source, codecvt_type const& cvt) const
    {
        return detail::path_traits::dispatch_convertible(source, compare_op(*this), &cvt);
    }
    //! \endcond

    //! @}

    //! \name decomposition
    //! @{

    /*!
     * \returns `root_name() / root_path()`.
     */
    path root_path() const { return path(m_pathname.c_str(), m_pathname.c_str() + detail::path_algorithms::find_root_path_size(*this)); }

    /*!
     * \returns `[root-name]`, if the path in the generic format includes a `[root-name]`, otherwise `path()`.
     */
    path root_name() const { return path(m_pathname.c_str(), m_pathname.c_str() + detail::path_algorithms::find_root_name_size(*this)); }

    /*!
     * \returns `[root-directory]`, if the path in the generic format includes a `[root-directory]`, otherwise `path()`.
     */
    path root_directory() const
    {
        detail::path_algorithms::substring root_dir = detail::path_algorithms::find_root_directory(*this);
        const value_type* p = m_pathname.c_str() + root_dir.pos;
        return path(p, p + root_dir.size);
    }

    /*!
     * \returns A `path` composed of the path elements starting with the first `[filename]` after `root_path()`.
     *          Returns `path()` if there are no such path elements.
     */
    path relative_path() const
    {
        detail::path_algorithms::substring rel_path = detail::path_algorithms::find_relative_path(*this);
        const value_type* p = m_pathname.c_str() + rel_path.pos;
        return path(p, p + rel_path.size);
    }

    /*!
     * \brief Returns the path without the last component.
     *
     * \returns `(empty() || begin() == --end()) ? path() : pp`, where `pp` is constructed as if by
     *          starting with an empty `path` and successively applying `operator/=` for each element in
     *          the range [`begin()`, `--end()`).
     *
     * \par Example:
     * \parblock
     * \code
     * std::cout << path("/foo/bar.txt").parent_path(); // outputs "/foo"
     * std::cout << path("/foo/bar").parent_path();     // outputs "/foo"
     * std::cout << path("/foo/bar/").parent_path();    // outputs "/foo/bar"
     * std::cout << path("/").parent_path();            // outputs ""
     * std::cout << path(".").parent_path();            // outputs ""
     * std::cout << path("..").parent_path();           // outputs ""
     * \endcode
     *
     * See the last bullet item in the path iterators forward traversal order list
     * for why the `"/foo/bar/"` example doesn't output `"/foo"`.
     * \endparblock
     */
    path parent_path() const { return path(m_pathname.c_str(), m_pathname.c_str() + detail::path_algorithms::find_parent_path_size(*this)); }

    /*!
     * \brief Returns the last filename component of the path.
     *
     * \returns **v3:** `empty() ? path() : *--end()`.<br/>
     *          **v4:** `*this == root_path() ? path() : *--end()`.
     *
     * \par Example:
     * \parblock
     * \code
     * std::cout << path("/foo/bar.txt").filename(); // outputs "bar.txt"
     * std::cout << path("/foo/bar").filename();     // outputs "bar"
     * std::cout << path("/foo/bar/").filename();    // v3 outputs "."
     *                                               // v4 outputs ""
     * std::cout << path("/").filename();            // v3 outputs "/"
     *                                               // v4 outputs ""
     * std::cout << path(".").filename();            // outputs "."
     * std::cout << path("..").filename();           // outputs ".."
     * \endcode
     *
     * See the last bullet item in the path iterators forward traversal order list
     * for why the `"/foo/bar/"` example doesn't output `"bar"`.
     * \endparblock
     */
    path filename() const;  // returns 0 or 1 element path

    /*!
     * \brief Returns the last filename component of the path without extension.
     *
     * \returns If `p.filename()` does not contain dots, consist solely of one or to two dots,
     *          [<i>Since <b>v4</b>:</i> or contains exactly one dot as the initial character,]
     *          returns `p.filename()`. Otherwise returns the substring of `p.filename()` starting at
     *          its beginning and ending at the last dot (the dot is not included).
     *
     * \par Example:
     * \parblock
     * \code
     * std::cout << path("/foo/bar.txt").stem() << '\\n'; // outputs "bar"
     * std::cout << path(".hidden").stem() << '\\n';      // v3 outputs ""
     *                                                    // v4 outputs ".hidden"
     * path p = "foo.bar.baz.tar";
     * for (; !p.extension().empty(); p = p.stem())       // outputs: .tar
     *   std::cout << p.extension() << '\\n';             //          .baz
     *                                                    //          .bar
     * \endcode
     * \endparblock
     */
    path stem() const;      // returns 0 or 1 element path

    /*!
     * \brief Returns extension of the last filename component of the path.
     *
     * \returns The substring of `p.filename()` that is not included in `p.stem()`.
     *
     * \remark Implementations are permitted but not required to define additional behavior
     *         for file systems which append additional elements to extensions, such as
     *         alternate data streams or partitioned dataset names.
     *
     * \par Example:
     * \parblock
     * \code
     * std::cout << path("/foo/bar.txt").extension(); // outputs ".txt"
     * \endcode
     *
     * \note The dot is included in the return value so that it is possible to distinguish
     *       between no extension and an empty extension. See <a href="https://lists.boost.org/Archives/boost/2010/02/162028.php">
     *       https://lists.boost.org/Archives/boost/2010/02/162028.php</a> for more extensive
     *       rationale.
     * \endparblock
     */
    path extension() const; // returns 0 or 1 element path

    //! @}

    //! \name lexical operations
    //! @{

    /*!
     * \par Overview:
     * Returns `*this` with redundant current directory ("."), parent directory (".."), and
     * directory separator elements removed.
     *
     * \remark Uses `path::operator/=` to compose the returned path.
     *
     * \returns `*this` in normal form.
     *
     * \par Example:
     * \parblock
     * \code
     * std::cout << path("foo/./bar/..").lexically_normal() << std::endl;    // outputs "foo"
     * std::cout << path("foo/.///bar/../").lexically_normal() << std::endl; // v3: outputs "foo/."
     *                                                                       // v4: outputs "foo/"
     * \endcode
     *
     * On Windows, the returned path's directory-separator characters will be backslashes rather than slashes,
     * but that does not affect `path` equality.
     * \endparblock
     */
    path lexically_normal() const;

    /*!
     * \par Overview:
     * Returns `*this` made relative to `base`. Treats empty or identical paths as corner cases,
     * not errors. Does not resolve symlinks. Does not first normalize `*this` or `base`.
     *
     * \remark Uses `std::mismatch(begin(), end(), base.begin(), base.end())` to determine the first mismatched
     *         element of `*this` and `base`. Uses `operator==()` to determine if elements match.
     *
     * \param base Base path, relative to which to produce the path.
     * \returns
     * \parblock
     *   \li `path()` if the first mismatched element of `*this` is equal to `begin()` or the first mismatched
     *       element of `base` is equal to `base.begin()`, or
     *   \li `path(".")` if the first mismatched element of `*this` is equal to `end()` and the first mismatched
     *       element of `base` is equal to `base.end()`, or
     *   \li An object of class `path` composed via application of `operator/=(path(".."))` for each element in
     *       the half-open range [first mismatched element of `base`, `base.end()`), and then application of
     *       `operator/=` for each element in the half-open range [first mismatched element of `*this`, `end()`).
     * \endparblock
     *
     * \par Example:
     * \parblock
     * \code
     * assert(path("/a/d").lexically_relative("/a/b/c") == "../../d");
     * assert(path("/a/b/c").lexically_relative("/a/d") ==  "../b/c");
     * assert(path("a/b/c").lexically_relative("a") == "b/c");
     * assert(path("a/b/c").lexically_relative("a/b/c/x/y") ==  "../..");
     * assert(path("a/b/c").lexically_relative("a/b/c") ==  ".");
     * assert(path("a/b").lexically_relative("c/d") ==  "");
     * \endcode
     *
     * The above assertions will succeed. On Windows, the returned path's directory-separator characters will be
     * backslashes rather than forward slashes, but that does not affect `path` equality.
     * \endparblock
     *
     * \note If symlink following semantics are desired, use the operational function `relative()`.
     *
     * \note If normalization is needed to ensure consistent matching of elements, apply `lexically_normal()`
     *       to `*this`, `base`, or both.
     */
    BOOST_FILESYSTEM_DECL path lexically_relative(path const& base) const;

    /*!
     * \returns If `lexically_relative(base)` returns a non-empty path, returns that path. Otherwise returns `*this`.
     *
     * \note If symlink following semantics are desired, use the operational function `relative()`.
     *
     * \note If normalization is needed to ensure consistent matching of elements, apply `lexically_normal()`
     *       to `*this`, `base`, or both.
     *
     * \sa \ref path::lexically_relative.
     */
    path lexically_proximate(path const& base) const;

    //! @}

    //! \name iterators
    //! @{

    /*!
     * \returns An iterator for the first element in forward traversal order. If no elements are present, the end iterator.
     */
    BOOST_FILESYSTEM_DECL iterator begin() const;
    /*!
     * \returns The end iterator in the forward traversal order.
     */
    BOOST_FILESYSTEM_DECL iterator end() const;
    /*!
     * \returns An iterator for the first element in backward traversal order. If no elements are present, the end iterator.
     */
    reverse_iterator rbegin() const;
    /*!
     * \returns The end iterator in the backward traversal order.
     */
    reverse_iterator rend() const;

    //! @}

    //! \name locale operations
    //! @{

    /*!
     * \par Effects:
     * Stores a copy of `loc` as the imbued `path` locale.
     *
     * \returns The previous imbued `path` locale.
     *
     * \remark The initial value of the imbued `path` locale is operating system dependent. It shall be
     *         a locale with a `codecvt` facet for a `char` string encoding appropriate for the operating
     *         system.
     */
    static BOOST_FILESYSTEM_DECL std::locale imbue(std::locale const& loc);
    /*!
     * \returns The `codecvt` facet for the imbued `path` locale.
     */
    static BOOST_FILESYSTEM_DECL codecvt_type const& codecvt();

    //! @}

private:
    /*
     * m_pathname has the type, encoding, and format required by the native
     * operating system. Thus for POSIX and Windows there is no conversion for
     * passing m_pathname.c_str() to the O/S API or when obtaining a path from the
     * O/S API. POSIX encoding is unspecified other than for dot and slash
     * characters; POSIX just treats paths as a sequence of bytes. Windows
     * encoding is UCS-2 or UTF-16 depending on the version.
     */
    string_type m_pathname;     // Windows: as input; backslashes NOT converted to slashes,
                                // slashes NOT converted to backslashes
};

//! \name path appending
//! @{

/*!
 * \return `lhs.append(rhs)`.
 *
 * \sa \ref path::append.
 */
BOOST_FORCEINLINE path operator/(path lhs, path const& rhs)
{
    lhs.append(rhs);
    return lhs;
}

/*!
 * \return `lhs.append(rhs)`.
 *
 * \sa \ref path::append.
 */
template< typename Source >
#if !defined(BOOST_FILESYSTEM_DOXYGEN)
BOOST_FORCEINLINE typename std::enable_if<
    detail::path_traits::is_convertible_to_path_source< typename std::remove_cv< Source >::type >::value,
    path
>::type
#else
path
#endif
operator/(path lhs, Source const& rhs)
{
    lhs.append(rhs);
    return lhs;
}
//! @}

namespace detail {

BOOST_FILESYSTEM_DECL path const& dot_path();
BOOST_FILESYSTEM_DECL path const& dot_dot_path();

namespace path_impl {

//------------------------------------------------------------------------------------//
//                             class path::iterator                                   //
//------------------------------------------------------------------------------------//

class path_iterator :
    public boost::iterator_facade<
        path_iterator,
        const path,
        boost::bidirectional_traversal_tag
    >
{
private:
    friend class boost::iterator_core_access;
    friend class boost::filesystem::path;
    friend class path_reverse_iterator;
    friend struct boost::filesystem::detail::path_algorithms;

    path const& dereference() const { return m_element; }

    bool equal(path_iterator const& rhs) const noexcept
    {
        return m_path_ptr == rhs.m_path_ptr && m_pos == rhs.m_pos;
    }

    void increment();
    void decrement();

private:
    // current element
    path m_element;
    // path being iterated over
    const path* m_path_ptr;
    // position of m_element in m_path_ptr->m_pathname.
    // if m_element is implicit dot, m_pos is the
    // position of the last separator in the path.
    // end() iterator is indicated by
    // m_pos == m_path_ptr->m_pathname.size()
    path::string_type::size_type m_pos;
};

//------------------------------------------------------------------------------------//
//                         class path::reverse_iterator                               //
//------------------------------------------------------------------------------------//

class path_reverse_iterator :
    public boost::iterator_facade<
        path_reverse_iterator,
        const path,
        boost::bidirectional_traversal_tag
    >
{
public:
    explicit path_reverse_iterator(path_iterator itr) :
        m_itr(itr)
    {
        if (itr != itr.m_path_ptr->begin())
            m_element = *--itr;
    }

private:
    friend class boost::iterator_core_access;
    friend class boost::filesystem::path;

    path const& dereference() const { return m_element; }
    bool equal(path_reverse_iterator const& rhs) const noexcept { return m_itr == rhs.m_itr; }

    void increment()
    {
        --m_itr;
        if (m_itr != m_itr.m_path_ptr->begin())
        {
            path_iterator tmp = m_itr;
            m_element = *--tmp;
        }
    }

    void decrement()
    {
        m_element = *m_itr;
        ++m_itr;
    }

private:
    path_iterator m_itr;
    path m_element;
};

//  std::lexicographical_compare would infinitely recurse because path iterators
//  yield paths, so provide a path aware version
bool lexicographical_compare(path_iterator first1, path_iterator const& last1, path_iterator first2, path_iterator const& last2);

} // namespace path_impl
} // namespace detail

//------------------------------------------------------------------------------------//
//                                                                                    //
//                              non-member functions                                  //
//                                                                                    //
//------------------------------------------------------------------------------------//

#if !defined(BOOST_FILESYSTEM_DOXYGEN)

using detail::path_impl::lexicographical_compare;

BOOST_FORCEINLINE bool operator==(path const& lhs, path const& rhs)
{
    return lhs.compare(rhs) == 0;
}

template< typename Path, typename Source >
BOOST_FORCEINLINE typename std::enable_if<
    detail::conjunction<
        std::is_same< Path, path >,
        detail::path_traits::is_convertible_to_path_source< typename std::remove_cv< Source >::type >
    >::value,
    bool
>::type operator==(Path const& lhs, Source const& rhs)
{
    return lhs.compare(rhs) == 0;
}

template< typename Source, typename Path >
BOOST_FORCEINLINE typename std::enable_if<
    detail::conjunction<
        std::is_same< Path, path >,
        detail::path_traits::is_convertible_to_path_source< typename std::remove_cv< Source >::type >
    >::value,
    bool
>::type operator==(Source const& lhs, Path const& rhs)
{
    return rhs.compare(lhs) == 0;
}

BOOST_FORCEINLINE bool operator!=(path const& lhs, path const& rhs)
{
    return lhs.compare(rhs) != 0;
}

template< typename Path, typename Source >
BOOST_FORCEINLINE typename std::enable_if<
    detail::conjunction<
        std::is_same< Path, path >,
        detail::path_traits::is_convertible_to_path_source< typename std::remove_cv< Source >::type >
    >::value,
    bool
>::type operator!=(Path const& lhs, Source const& rhs)
{
    return lhs.compare(rhs) != 0;
}

template< typename Source, typename Path >
BOOST_FORCEINLINE typename std::enable_if<
    detail::conjunction<
        std::is_same< Path, path >,
        detail::path_traits::is_convertible_to_path_source< typename std::remove_cv< Source >::type >
    >::value,
    bool
>::type operator!=(Source const& lhs, Path const& rhs)
{
    return rhs.compare(lhs) != 0;
}

BOOST_FORCEINLINE bool operator<(path const& lhs, path const& rhs)
{
    return lhs.compare(rhs) < 0;
}

template< typename Path, typename Source >
BOOST_FORCEINLINE typename std::enable_if<
    detail::conjunction<
        std::is_same< Path, path >,
        detail::path_traits::is_convertible_to_path_source< typename std::remove_cv< Source >::type >
    >::value,
    bool
>::type operator<(Path const& lhs, Source const& rhs)
{
    return lhs.compare(rhs) < 0;
}

template< typename Source, typename Path >
BOOST_FORCEINLINE typename std::enable_if<
    detail::conjunction<
        std::is_same< Path, path >,
        detail::path_traits::is_convertible_to_path_source< typename std::remove_cv< Source >::type >
    >::value,
    bool
>::type operator<(Source const& lhs, Path const& rhs)
{
    return rhs.compare(lhs) > 0;
}

BOOST_FORCEINLINE bool operator<=(path const& lhs, path const& rhs)
{
    return lhs.compare(rhs) <= 0;
}

template< typename Path, typename Source >
BOOST_FORCEINLINE typename std::enable_if<
    detail::conjunction<
        std::is_same< Path, path >,
        detail::path_traits::is_convertible_to_path_source< typename std::remove_cv< Source >::type >
    >::value,
    bool
>::type operator<=(Path const& lhs, Source const& rhs)
{
    return lhs.compare(rhs) <= 0;
}

template< typename Source, typename Path >
BOOST_FORCEINLINE typename std::enable_if<
    detail::conjunction<
        std::is_same< Path, path >,
        detail::path_traits::is_convertible_to_path_source< typename std::remove_cv< Source >::type >
    >::value,
    bool
>::type operator<=(Source const& lhs, Path const& rhs)
{
    return rhs.compare(lhs) >= 0;
}

BOOST_FORCEINLINE bool operator>(path const& lhs, path const& rhs)
{
    return lhs.compare(rhs) > 0;
}

template< typename Path, typename Source >
BOOST_FORCEINLINE typename std::enable_if<
    detail::conjunction<
        std::is_same< Path, path >,
        detail::path_traits::is_convertible_to_path_source< typename std::remove_cv< Source >::type >
    >::value,
    bool
>::type operator>(Path const& lhs, Source const& rhs)
{
    return lhs.compare(rhs) > 0;
}

template< typename Source, typename Path >
BOOST_FORCEINLINE typename std::enable_if<
    detail::conjunction<
        std::is_same< Path, path >,
        detail::path_traits::is_convertible_to_path_source< typename std::remove_cv< Source >::type >
    >::value,
    bool
>::type operator>(Source const& lhs, Path const& rhs)
{
    return rhs.compare(lhs) < 0;
}

BOOST_FORCEINLINE bool operator>=(path const& lhs, path const& rhs)
{
    return lhs.compare(rhs) >= 0;
}

template< typename Path, typename Source >
BOOST_FORCEINLINE typename std::enable_if<
    detail::conjunction<
        std::is_same< Path, path >,
        detail::path_traits::is_convertible_to_path_source< typename std::remove_cv< Source >::type >
    >::value,
    bool
>::type operator>=(Path const& lhs, Source const& rhs)
{
    return lhs.compare(rhs) >= 0;
}

template< typename Source, typename Path >
BOOST_FORCEINLINE typename std::enable_if<
    detail::conjunction<
        std::is_same< Path, path >,
        detail::path_traits::is_convertible_to_path_source< typename std::remove_cv< Source >::type >
    >::value,
    bool
>::type operator>=(Source const& lhs, Path const& rhs)
{
    return rhs.compare(lhs) <= 0;
}

// Note: Declared as a template to delay binding to Boost.ContainerHash functions and make the dependency optional
template< typename Path >
inline typename std::enable_if<
    std::is_same< Path, path >::value,
    std::size_t
>::type hash_value(Path const& p) noexcept
{
#ifdef BOOST_FILESYSTEM_WINDOWS_API
    std::size_t seed = 0u;
    for (typename Path::value_type const* it = p.c_str(); *it; ++it)
        hash_combine(seed, *it == L'/' ? L'\\' : *it);
    return seed;
#else // BOOST_FILESYSTEM_POSIX_API
    return hash_range(p.native().begin(), p.native().end());
#endif
}

#else // !defined(BOOST_FILESYSTEM_DOXYGEN)

//! \name path comparison
//! @{

/*!
 * \returns `lhs.compare(rhs) == 0`.
 *
 * \sa \ref path::compare.
 */
bool operator==(path const& lhs, path const& rhs);
/*!
 * \returns `lhs.compare(rhs) == 0`.
 *
 * \sa \ref path::compare.
 */
template< typename Source >
bool operator==(path const& lhs, Source const& rhs);
/*!
 * \returns `rhs.compare(lhs) == 0`.
 *
 * \sa \ref path::compare.
 */
template< typename Source >
bool operator==(Source const& lhs, path const& rhs);

/*!
 * \returns `lhs.compare(rhs) != 0`.
 *
 * \sa \ref path::compare.
 */
bool operator!=(path const& lhs, path const& rhs);
/*!
 * \returns `lhs.compare(rhs) != 0`.
 *
 * \sa \ref path::compare.
 */
template< typename Source >
bool operator!=(path const& lhs, Source const& rhs);
/*!
 * \returns `rhs.compare(lhs) != 0`.
 *
 * \sa \ref path::compare.
 */
template< typename Source >
bool operator!=(Source const& lhs, path const& rhs);

/*!
 * \returns `lhs.compare(rhs) < 0`.
 *
 * \sa \ref path::compare.
 */
bool operator<(path const& lhs, path const& rhs);
/*!
 * \returns `lhs.compare(rhs) < 0`.
 *
 * \sa \ref path::compare.
 */
template< typename Source >
bool operator<(path const& lhs, Source const& rhs);
/*!
 * \returns `rhs.compare(lhs) > 0`.
 *
 * \sa \ref path::compare.
 */
template< typename Source >
bool operator<(Source const& lhs, path const& rhs);

/*!
 * \returns `lhs.compare(rhs) > 0`.
 *
 * \sa \ref path::compare.
 */
bool operator>(path const& lhs, path const& rhs);
/*!
 * \returns `lhs.compare(rhs) > 0`.
 *
 * \sa \ref path::compare.
 */
template< typename Source >
bool operator>(path const& lhs, Source const& rhs);
/*!
 * \returns `rhs.compare(lhs) < 0`.
 *
 * \sa \ref path::compare.
 */
template< typename Source >
bool operator>(Source const& lhs, path const& rhs);

/*!
 * \returns `lhs.compare(rhs) <= 0`.
 *
 * \sa \ref path::compare.
 */
bool operator<=(path const& lhs, path const& rhs);
/*!
 * \returns `lhs.compare(rhs) <= 0`.
 *
 * \sa \ref path::compare.
 */
template< typename Source >
bool operator<=(path const& lhs, Source const& rhs);
/*!
 * \returns `rhs.compare(lhs) >= 0`.
 *
 * \sa \ref path::compare.
 */
template< typename Source >
bool operator<=(Source const& lhs, path const& rhs);

/*!
 * \returns `lhs.compare(rhs) >= 0`.
 *
 * \sa \ref path::compare.
 */
bool operator>=(path const& lhs, path const& rhs);
/*!
 * \returns `lhs.compare(rhs) >= 0`.
 *
 * \sa \ref path::compare.
 */
template< typename Source >
bool operator>=(path const& lhs, Source const& rhs);
/*!
 * \returns `lhs.compare(rhs) <= 0`.
 *
 * \sa \ref path::compare.
 */
template< typename Source >
bool operator>=(Source const& lhs, path const& rhs);

/*!
 * \brief Tests whether the two sequences of path elements are lexicographically equal.
 *
 * \param first1 Beginning of the first sequence.
 * \param last1 End of the first sequence.
 * \param first2 Beginning of the second sequence.
 * \param last2 End of the second sequence.
 * \returns `true` if the sequence of `native()` strings for the elements defined by the half-open range [`first1`, `last1`) is
 *          lexicographically less than the sequence of `native()` strings for the elements defined by the half-open range
 *          [`first2`, `last2`). Returns `false` otherwise.
 *
 * \remark If two sequences have the same number of elements and their corresponding elements are equivalent, then neither sequence
 *         is lexicographically less than the other. If one sequence is a prefix of the other, then the shorter sequence is
 *         lexicographically less than the longer sequence. Otherwise, the lexicographical comparison of the sequences yields
 *         the same result as the comparison of the first corresponding pair of elements that are not equivalent.
 *
 * \note A `path`-aware `lexicographical_compare` algorithm is provided for historical reasons.
 */
bool lexicographical_compare(path::const_iterator first1, path::const_iterator last1, path::const_iterator first2, path::const_iterator last2);
//! @}

//! \name path hashing
//! @{

/*!
 * \brief Computes a hash value for the path.
 *
 * If for two paths, `p1 == p2` then `hash_value(p1) == hash_value(p2)`. The opposite is not necessarily true. This allows paths
 * to be used with Boost.Hash.
 *
 * \param p The path to compute hash for.
 * \returns A hash value for the path `p`.
 */
std::size_t hash_value(path const& p) noexcept;
//! @}

#endif // !defined(BOOST_FILESYSTEM_DOXYGEN)

//! \name path swapping
//! @{

/*!
 * \par Effects:
 * As if `lhs.swap(rhs)`.
 *
 * \sa \ref path::swap.
 */
inline void swap(path& lhs, path& rhs) noexcept
{
    lhs.swap(rhs);
}
//! @}

//! \name path I/O
//! @{

//  inserters and extractors
//    use boost::io::quoted() to handle spaces in paths
//    use '&' as escape character to ease use for Windows paths

/*!
 * \brief Outputs path representation into an output stream.
 *
 * \par Effects:
 * \parblock
 * Insert characters into `os` as follows:
 *
 * \li A double-quote.
 * \li Each character in `p.string< std::basic_string< Char > >()`. If the character to be inserted
 *     is equal to the escape character '&' or a double-quote, as determined by `operator==`, first
 *     insert the escape character.
 * \li A double-quote.
 *
 * \note The effects ensure that the path can safely round-trip with `operator>>`, even if the path
 *       contains spaces, double-quotes and '&' characters.
 * \endparblock
 *
 * \param os Output stream.
 * \param p Path to output.
 * \returns `os`.
 */
template< typename Char, typename Traits >
inline std::basic_ostream< Char, Traits >& operator<<(std::basic_ostream< Char, Traits >& os, path const& p)
{
    return os << boost::io::quoted(p.template string< std::basic_string< Char > >(), static_cast< Char >('&'));
}

/*!
 * \brief Reads path representation from an input stream.
 *
 * \par Effects:
 * \parblock
 * Extract characters from `is` as follows:
 *
 * <ul>
 * <li>If the first character that would be extracted is equal to double-quote, as determined by `operator==`, then:</li>
 *     <ul>
 *     <li>Discard the initial double-quote.</li>
 *     <li>Save the value and then turn off the `skipws` flag in `is`.</li>
 *     <li>`p.clear()`.</li>
 *     <li>Until an unescaped double-quote character is reached or `is.not_good()`, extract characters from `is` and
 *         concatenate them to `p`, except that if an escape character '&' is reached, ignore it and concatenate
 *         the next character to `p`.</li>
 *     <li>Discard the final double-quote character.</li>
 *     <li>Restore the `skipws` flag to its original value in `is`.</li>
 *     </ul>
 * <li>Otherwise, for `str` being a string of type `std::basic_string< Char >`, `is >> str; p = str;`.</li>
 * </ul>
 *
 * \note The effects ensure that the path can safely round-trip with `operator<<`, even if the original path
 *       contained spaces, double-quotes and '&' characters.
 * \endparblock
 *
 * \param is Input stream.
 * \param p Path to read into.
 * \returns `is`.
 */
template< typename Char, typename Traits >
inline std::basic_istream< Char, Traits >& operator>>(std::basic_istream< Char, Traits >& is, path& p)
{
    std::basic_string< Char > str;
    is >> boost::io::quoted(str, static_cast< Char >('&'));
    p = str;
    return is;
}

//! @}

//! \name path name checks
//! @{

//  These functions are holdovers from version 1. It isn't clear they have much
//  usefulness, or how to generalize them for later versions.

/*!
 * \brief Tests if the string can be portably used as a name on POSIX systems.
 *
 * \param name String to test.
 * \returns
 * \parblock
 * `true` if `!name.empty()` and `name` contains only the characters specified in Portable Filename Character Set
 * rules as defined in by POSIX (https://pubs.opengroup.org/onlinepubs/007904975/basedefs/xbd_chap03.html).
 *
 * The allowed characters are "0-9", "a-z", "A-Z", ".", "_", and "-".
 * \endparblock
 */
BOOST_FILESYSTEM_DECL bool portable_posix_name(std::string const& name);
/*!
 * \brief Tests if the string can be used as a name on Windows.
 *
 * \param name String to test.
 * \returns
 * \parblock
 * `true` if
 *
 * \li `!name.empty()`, and
 * \li `name` contains only the characters specified by the Windows platform SDK as valid regardless of the file system, and
 * \li `name` is "." or ".." or does not end with a trailing space or period.
 *
 * The allowed characters are anything except `0x0-0x1F`, "<", ">", ":", """, "/", "\\", and "|".
 * \endparblock
 */
BOOST_FILESYSTEM_DECL bool windows_name(std::string const& name);
/*!
 * \brief Tests if the string can be used as a name on most systems.
 *
 * \param name String to test.
 * \returns
 * \parblock
 * `true` if
 *
 * \li `windows_name(name)`, and
 * \li `portable_posix_name(name)`, and
 * \li `name` is "." or "..", or the first character of `name` is not a period or hyphen.
 * \endparblock
 */
BOOST_FILESYSTEM_DECL bool portable_name(std::string const& name);
/*!
 * \brief Tests if the string can be used as a directory name on most systems.
 *
 * \param name String to test.
 * \returns `true` if `portable_name(name)` and `name` is "." or ".." or contains no periods.
 */
BOOST_FILESYSTEM_DECL bool portable_directory_name(std::string const& name);
/*!
 * \brief Tests if the string can be used as a file name on most systems.
 *
 * \param name String to test.
 * \returns `portable_name(name)`, and any period is followed by one to three additional non-period characters.
 */
BOOST_FILESYSTEM_DECL bool portable_file_name(std::string const& name);
/*!
 * \brief Tests if the string can be considered as a valid name for the native file system.
 *
 * \returns Returns `true` for names considered valid by the operating system's native file systems. The actual
 *          criteria for validity are implementation-defined.
 *
 * \note May return `true` for some names not considered valid by the operating system under all conditions
 *       (particularly on operating systems which support multiple file systems.)
 */
BOOST_FILESYSTEM_DECL bool native(std::string const& name);

//! @}

namespace detail {

//  For POSIX, is_directory_separator() and is_element_separator() are identical since
//  a forward slash is the only valid directory separator and also the only valid
//  element separator. For Windows, forward slash and back slash are the possible
//  directory separators, but colon (example: "c:foo") is also an element separator.
inline bool is_directory_separator(path::value_type c) noexcept
{
    return c == path::separator
#ifdef BOOST_FILESYSTEM_WINDOWS_API
        || c == path::preferred_separator
#endif
        ;
}

inline bool is_element_separator(path::value_type c) noexcept
{
    return c == path::separator
#ifdef BOOST_FILESYSTEM_WINDOWS_API
        || c == path::preferred_separator || c == L':'
#endif
        ;
}

} // namespace detail

//------------------------------------------------------------------------------------//
//                  class path miscellaneous function implementations                 //
//------------------------------------------------------------------------------------//

namespace detail {

inline bool path_algorithms::has_filename_v3(path const& p)
{
    return !p.m_pathname.empty();
}

inline bool path_algorithms::has_filename_v4(path const& p)
{
    return path_algorithms::find_filename_v4_size(p) > 0;
}

inline path path_algorithms::filename_v4(path const& p)
{
    string_type::size_type filename_size = path_algorithms::find_filename_v4_size(p);
    string_type::size_type pos = p.m_pathname.size() - filename_size;
    const value_type* ptr = p.m_pathname.c_str() + pos;
    return path(ptr, ptr + filename_size);
}

inline path path_algorithms::extension_v4(path const& p)
{
    string_type::size_type extension_size = path_algorithms::find_extension_v4_size(p);
    string_type::size_type pos = p.m_pathname.size() - extension_size;
    const value_type* ptr = p.m_pathname.c_str() + pos;
    return path(ptr, ptr + extension_size);
}

inline void path_algorithms::append_v4(path& left, path const& right)
{
    path_algorithms::append_v4(left, right.m_pathname.c_str(), right.m_pathname.c_str() + right.m_pathname.size());
}

} // namespace detail

#if !defined(BOOST_FILESYSTEM_DOXYGEN)

// Note: Because of the range constructor in C++23 std::string_view that involves a check for contiguous_range concept,
//       any non-template function call that requires a check whether the source argument (which may be fs::path)
//       is convertible to std::string_view must be made after fs::path::iterator is defined. This includes overload
//       resolution and SFINAE checks. Otherwise, the concept check result formally changes between fs::path::iterator
//       is not defined and defined, which causes compilation errors with gcc 11 and later.
//       https://gcc.gnu.org/bugzilla/show_bug.cgi?id=106808

BOOST_FORCEINLINE path::compare_op::result_type path::compare_op::operator() (const value_type* source, const value_type* source_end, const codecvt_type*) const
{
    path src;
    src.m_pathname.assign(source, source_end);
    return m_self.compare(src);
}

template< typename OtherChar >
BOOST_FORCEINLINE path::compare_op::result_type path::compare_op::operator() (const OtherChar* source, const OtherChar* source_end, const codecvt_type* cvt) const
{
    path src;
    detail::path_traits::convert(source, source_end, src.m_pathname, cvt);
    return m_self.compare(src);
}

inline path& path::operator=(path const& p)
{
    return assign(p);
}

inline path& path::operator+=(path const& p)
{
    return concat(p);
}

BOOST_FORCEINLINE path& path::operator/=(path const& p)
{
    return append(p);
}

inline path path::lexically_proximate(path const& base) const
{
    path tmp(lexically_relative(base));
    return tmp.empty() ? *this : tmp;
}

inline path::reverse_iterator path::rbegin() const
{
    return reverse_iterator(end());
}

inline path::reverse_iterator path::rend() const
{
    return reverse_iterator(begin());
}

inline bool path::filename_is_dot() const
{
    // implicit dot is tricky, so actually call filename(); see path::filename() example
    // in reference.html
    path p(filename());
    return p.size() == 1 && *p.c_str() == dot;
}

inline bool path::filename_is_dot_dot() const
{
    return size() >= 2 && m_pathname[size() - 1] == dot && m_pathname[size() - 2] == dot &&
        (m_pathname.size() == 2 || detail::is_element_separator(m_pathname[size() - 3]));
    // use detail::is_element_separator() rather than detail::is_directory_separator
    // to deal with "c:.." edge case on Windows when ':' acts as a separator
}

// The following functions are defined differently, depending on Boost.Filesystem version in use.
// To avoid ODR violation, these functions are not defined when the library itself is built.
// This makes sure they are not compiled when the library is built, and the only version there is
// is the one in user's code. Users are supposed to consistently use the same Boost.Filesystem version
// in all their translation units.
#if !defined(BOOST_FILESYSTEM_SOURCE)

BOOST_FORCEINLINE path& path::append(path const& p)
{
    BOOST_FILESYSTEM_VERSIONED_SYM(detail::path_algorithms::append)(*this, p.m_pathname.data(), p.m_pathname.data() + p.m_pathname.size());
    return *this;
}

BOOST_FORCEINLINE path& path::append(path const& p, codecvt_type const&)
{
    BOOST_FILESYSTEM_VERSIONED_SYM(detail::path_algorithms::append)(*this, p.m_pathname.data(), p.m_pathname.data() + p.m_pathname.size());
    return *this;
}

BOOST_FORCEINLINE path& path::append(const value_type* begin, const value_type* end)
{
    BOOST_FILESYSTEM_VERSIONED_SYM(detail::path_algorithms::append)(*this, begin, end);
    return *this;
}

BOOST_FORCEINLINE path& path::append(const value_type* begin, const value_type* end, codecvt_type const&)
{
    BOOST_FILESYSTEM_VERSIONED_SYM(detail::path_algorithms::append)(*this, begin, end);
    return *this;
}

BOOST_FORCEINLINE path& path::remove_filename()
{
    BOOST_FILESYSTEM_VERSIONED_SYM(detail::path_algorithms::remove_filename)(*this);
    return *this;
}

BOOST_FORCEINLINE path& path::replace_extension(path const& new_extension)
{
    BOOST_FILESYSTEM_VERSIONED_SYM(detail::path_algorithms::replace_extension)(*this, new_extension);
    return *this;
}

BOOST_FORCEINLINE int path::compare(path const& p) const
{
    return BOOST_FILESYSTEM_VERSIONED_SYM(detail::path_algorithms::compare)(*this, p);
}

BOOST_FORCEINLINE path path::filename() const
{
    return BOOST_FILESYSTEM_VERSIONED_SYM(detail::path_algorithms::filename)(*this);
}

BOOST_FORCEINLINE path path::stem() const
{
    return BOOST_FILESYSTEM_VERSIONED_SYM(detail::path_algorithms::stem)(*this);
}

BOOST_FORCEINLINE path path::extension() const
{
    return BOOST_FILESYSTEM_VERSIONED_SYM(detail::path_algorithms::extension)(*this);
}

BOOST_FORCEINLINE bool path::has_filename() const
{
    return BOOST_FILESYSTEM_VERSIONED_SYM(detail::path_algorithms::has_filename)(*this);
}

BOOST_FORCEINLINE path path::lexically_normal() const
{
    return BOOST_FILESYSTEM_VERSIONED_SYM(detail::path_algorithms::lexically_normal)(*this);
}

BOOST_FORCEINLINE path path::generic_path() const
{
    return BOOST_FILESYSTEM_VERSIONED_SYM(detail::path_algorithms::generic_path)(*this);
}

BOOST_FORCEINLINE path& path::make_preferred()
{
    // No effect on POSIX
#if defined(BOOST_FILESYSTEM_WINDOWS_API)
    BOOST_FILESYSTEM_VERSIONED_SYM(detail::path_algorithms::make_preferred)(*this);
#endif
    return *this;
}

namespace detail {
namespace path_impl {

BOOST_FORCEINLINE void path_iterator::increment()
{
    BOOST_FILESYSTEM_VERSIONED_SYM(detail::path_algorithms::increment)(*this);
}

BOOST_FORCEINLINE void path_iterator::decrement()
{
    BOOST_FILESYSTEM_VERSIONED_SYM(detail::path_algorithms::decrement)(*this);
}

BOOST_FORCEINLINE bool lexicographical_compare(path_iterator first1, path_iterator const& last1, path_iterator first2, path_iterator const& last2)
{
    return BOOST_FILESYSTEM_VERSIONED_SYM(detail::path_algorithms::lex_compare)(first1, last1, first2, last2) < 0;
}

} // namespace path_impl
} // namespace detail

#endif // !defined(BOOST_FILESYSTEM_SOURCE)

//--------------------------------------------------------------------------------------//
//                     class path member template specializations                       //
//--------------------------------------------------------------------------------------//

template< >
inline std::string path::string< std::string >() const
{
    return string();
}

template< >
inline std::wstring path::string< std::wstring >() const
{
    return wstring();
}

template< >
inline std::string path::string< std::string >(codecvt_type const& cvt) const
{
    return string(cvt);
}

template< >
inline std::wstring path::string< std::wstring >(codecvt_type const& cvt) const
{
    return wstring(cvt);
}

template< >
inline std::string path::generic_string< std::string >() const
{
    return generic_string();
}

template< >
inline std::wstring path::generic_string< std::wstring >() const
{
    return generic_wstring();
}

template< >
inline std::string path::generic_string< std::string >(codecvt_type const& cvt) const
{
    return generic_string(cvt);
}

template< >
inline std::wstring path::generic_string< std::wstring >(codecvt_type const& cvt) const
{
    return generic_wstring(cvt);
}

#endif // !defined(BOOST_FILESYSTEM_DOXYGEN)

} // namespace filesystem
} // namespace boost

//----------------------------------------------------------------------------//

#include <boost/filesystem/detail/footer.hpp>

#endif // BOOST_FILESYSTEM_PATH_HPP
