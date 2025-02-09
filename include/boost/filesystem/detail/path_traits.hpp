//  filesystem path_traits.hpp  --------------------------------------------------------//

//  Copyright Beman Dawes 2009
//  Copyright Andrey Semashev 2022-2024

//  Distributed under the Boost Software License, Version 1.0.
//  See http://www.boost.org/LICENSE_1_0.txt

//  Library home page: http://www.boost.org/libs/filesystem

#ifndef BOOST_FILESYSTEM_DETAIL_PATH_TRAITS_HPP
#define BOOST_FILESYSTEM_DETAIL_PATH_TRAITS_HPP

#include <boost/filesystem/config.hpp>
#include <cstddef>
#include <cstring> // for strlen
#include <cwchar> // for mbstate_t, wcslen
#include <locale>
#include <string>
#include <iterator>
#include <type_traits>
#if !defined(BOOST_NO_CXX17_HDR_STRING_VIEW)
#include <string_view>
#endif
#include <boost/assert.hpp>
#include <boost/system/error_category.hpp>
#include <boost/iterator/is_iterator.hpp>
#include <boost/filesystem/detail/type_traits/negation.hpp>
#include <boost/filesystem/detail/type_traits/conjunction.hpp>
#if defined(BOOST_FILESYSTEM_DETAIL_CXX23_STRING_VIEW_HAS_IMPLICIT_RANGE_CTOR)
#include <boost/filesystem/detail/type_traits/disjunction.hpp>
#endif
#if defined(BOOST_FILESYSTEM_DEPRECATED) && BOOST_FILESYSTEM_VERSION < 4
#include <vector>
#include <list>
#endif

#include <boost/filesystem/detail/header.hpp> // must be the last #include

namespace boost {

template< typename, typename > class basic_string_view;

namespace container {
template< typename, typename, typename > class basic_string;
} // namespace container

namespace filesystem {

BOOST_FILESYSTEM_DECL system::error_category const& codecvt_error_category() noexcept;

class path;
class directory_entry;

namespace detail {
namespace path_traits {

#if defined(BOOST_WINDOWS_API)
using path_native_char_type = wchar_t;
#define BOOST_FILESYSTEM_DETAIL_IS_CHAR_NATIVE false
#define BOOST_FILESYSTEM_DETAIL_IS_WCHAR_T_NATIVE true
#else
using path_native_char_type = char;
#define BOOST_FILESYSTEM_DETAIL_IS_CHAR_NATIVE true
#define BOOST_FILESYSTEM_DETAIL_IS_WCHAR_T_NATIVE false
#endif

using codecvt_type = std::codecvt< wchar_t, char, std::mbstate_t >;

struct unknown_type_tag {};
struct ntcts_type_tag {};
struct char_ptr_tag : ntcts_type_tag {};
struct char_array_tag : ntcts_type_tag {};
struct string_class_tag {};
struct std_string_tag : string_class_tag {};
struct boost_container_string_tag : string_class_tag {};
struct std_string_view_tag : string_class_tag {};
struct boost_string_view_tag : string_class_tag {};
struct range_type_tag {};
struct directory_entry_tag {};

//! The traits define a number of properties of a path source
template< typename T >
struct path_source_traits
{
    //! The kind of the path source. Useful for dispatching.
    using tag_type = unknown_type_tag;
    //! Character type that the source contains
    using char_type = void;
    //! Indicates whether the source is natively supported by \c path::string_type as arguments for constructors/assignment/appending
    static BOOST_CONSTEXPR_OR_CONST bool is_native = false;
};

template< >
struct path_source_traits< char* >
{
    using tag_type = char_ptr_tag;
    using char_type = char;
    static BOOST_CONSTEXPR_OR_CONST bool is_native = BOOST_FILESYSTEM_DETAIL_IS_CHAR_NATIVE;
};

template< >
struct path_source_traits< const char* >
{
    using tag_type = char_ptr_tag;
    using char_type = char;
    static BOOST_CONSTEXPR_OR_CONST bool is_native = BOOST_FILESYSTEM_DETAIL_IS_CHAR_NATIVE;
};

template< >
struct path_source_traits< wchar_t* >
{
    using tag_type = char_ptr_tag;
    using char_type = wchar_t;
    static BOOST_CONSTEXPR_OR_CONST bool is_native = BOOST_FILESYSTEM_DETAIL_IS_WCHAR_T_NATIVE;
};

template< >
struct path_source_traits< const wchar_t* >
{
    using tag_type = char_ptr_tag;
    using char_type = wchar_t;
    static BOOST_CONSTEXPR_OR_CONST bool is_native = BOOST_FILESYSTEM_DETAIL_IS_WCHAR_T_NATIVE;
};

template< >
struct path_source_traits< char[] >
{
    using tag_type = char_array_tag;
    using char_type = char;
    static BOOST_CONSTEXPR_OR_CONST bool is_native = BOOST_FILESYSTEM_DETAIL_IS_CHAR_NATIVE;
};

template< >
struct path_source_traits< const char[] >
{
    using tag_type = char_array_tag;
    using char_type = char;
    static BOOST_CONSTEXPR_OR_CONST bool is_native = BOOST_FILESYSTEM_DETAIL_IS_CHAR_NATIVE;
};

template< >
struct path_source_traits< wchar_t[] >
{
    using tag_type = char_array_tag;
    using char_type = wchar_t;
    static BOOST_CONSTEXPR_OR_CONST bool is_native = BOOST_FILESYSTEM_DETAIL_IS_WCHAR_T_NATIVE;
};

template< >
struct path_source_traits< const wchar_t[] >
{
    using tag_type = char_array_tag;
    using char_type = wchar_t;
    static BOOST_CONSTEXPR_OR_CONST bool is_native = BOOST_FILESYSTEM_DETAIL_IS_WCHAR_T_NATIVE;
};

template< std::size_t N >
struct path_source_traits< char[N] >
{
    using tag_type = char_array_tag;
    using char_type = char;
    static BOOST_CONSTEXPR_OR_CONST bool is_native = BOOST_FILESYSTEM_DETAIL_IS_CHAR_NATIVE;
};

template< std::size_t N >
struct path_source_traits< const char[N] >
{
    using tag_type = char_array_tag;
    using char_type = char;
    static BOOST_CONSTEXPR_OR_CONST bool is_native = BOOST_FILESYSTEM_DETAIL_IS_CHAR_NATIVE;
};

template< std::size_t N >
struct path_source_traits< wchar_t[N] >
{
    using tag_type = char_array_tag;
    using char_type = wchar_t;
    static BOOST_CONSTEXPR_OR_CONST bool is_native = BOOST_FILESYSTEM_DETAIL_IS_WCHAR_T_NATIVE;
};

template< std::size_t N >
struct path_source_traits< const wchar_t[N] >
{
    using tag_type = char_array_tag;
    using char_type = wchar_t;
    static BOOST_CONSTEXPR_OR_CONST bool is_native = BOOST_FILESYSTEM_DETAIL_IS_WCHAR_T_NATIVE;
};

template< >
struct path_source_traits< std::string >
{
    using tag_type = std_string_tag;
    using char_type = char;
    static BOOST_CONSTEXPR_OR_CONST bool is_native = BOOST_FILESYSTEM_DETAIL_IS_CHAR_NATIVE;
};

template< >
struct path_source_traits< std::wstring >
{
    using tag_type = std_string_tag;
    using char_type = wchar_t;
    static BOOST_CONSTEXPR_OR_CONST bool is_native = BOOST_FILESYSTEM_DETAIL_IS_WCHAR_T_NATIVE;
};

template< >
struct path_source_traits< boost::container::basic_string< char, std::char_traits< char >, void > >
{
    using tag_type = boost_container_string_tag;
    using char_type = char;
    static BOOST_CONSTEXPR_OR_CONST bool is_native = false;
};

template< >
struct path_source_traits< boost::container::basic_string< wchar_t, std::char_traits< wchar_t >, void > >
{
    using tag_type = boost_container_string_tag;
    using char_type = wchar_t;
    static BOOST_CONSTEXPR_OR_CONST bool is_native = false;
};

#if !defined(BOOST_NO_CXX17_HDR_STRING_VIEW)

template< >
struct path_source_traits< std::string_view >
{
    using tag_type = std_string_view_tag;
    using char_type = char;
    static BOOST_CONSTEXPR_OR_CONST bool is_native = BOOST_FILESYSTEM_DETAIL_IS_CHAR_NATIVE;
};

template< >
struct path_source_traits< std::wstring_view >
{
    using tag_type = std_string_view_tag;
    using char_type = wchar_t;
    static BOOST_CONSTEXPR_OR_CONST bool is_native = BOOST_FILESYSTEM_DETAIL_IS_WCHAR_T_NATIVE;
};

#endif // !defined(BOOST_NO_CXX17_HDR_STRING_VIEW)

template< >
struct path_source_traits< boost::basic_string_view< char, std::char_traits< char > > >
{
    using tag_type = boost_string_view_tag;
    using char_type = char;
    static BOOST_CONSTEXPR_OR_CONST bool is_native = false;
};

template< >
struct path_source_traits< boost::basic_string_view< wchar_t, std::char_traits< wchar_t > > >
{
    using tag_type = boost_string_view_tag;
    using char_type = wchar_t;
    static BOOST_CONSTEXPR_OR_CONST bool is_native = false;
};

#if defined(BOOST_FILESYSTEM_DEPRECATED) && BOOST_FILESYSTEM_VERSION < 4
template< >
struct
BOOST_FILESYSTEM_DETAIL_DEPRECATED("Boost.Filesystem path construction/assignment/appending from containers is deprecated, use strings or iterators instead.")
path_source_traits< std::vector< char > >
{
    // Since C++11 this could be string_class_tag as std::vector gained data() member
    using tag_type = range_type_tag;
    using char_type = char;
    static BOOST_CONSTEXPR_OR_CONST bool is_native = false;
};

template< >
struct
BOOST_FILESYSTEM_DETAIL_DEPRECATED("Boost.Filesystem path construction/assignment/appending from containers is deprecated, use strings or iterators instead.")
path_source_traits< std::vector< wchar_t > >
{
    // Since C++11 this could be string_class_tag as std::vector gained data() member
    using tag_type = range_type_tag;
    using char_type = wchar_t;
    static BOOST_CONSTEXPR_OR_CONST bool is_native = false;
};

template< >
struct
BOOST_FILESYSTEM_DETAIL_DEPRECATED("Boost.Filesystem path construction/assignment/appending from containers is deprecated, use strings or iterators instead.")
path_source_traits< std::list< char > >
{
    using tag_type = range_type_tag;
    using char_type = char;
    static BOOST_CONSTEXPR_OR_CONST bool is_native = false;
};

template< >
struct
BOOST_FILESYSTEM_DETAIL_DEPRECATED("Boost.Filesystem path construction/assignment/appending from containers is deprecated, use strings or iterators instead.")
path_source_traits< std::list< wchar_t > >
{
    using tag_type = range_type_tag;
    using char_type = wchar_t;
    static BOOST_CONSTEXPR_OR_CONST bool is_native = false;
};
#endif // defined(BOOST_FILESYSTEM_DEPRECATED) && BOOST_FILESYSTEM_VERSION < 4

template< >
struct path_source_traits< directory_entry >
{
    using tag_type = directory_entry_tag;
    using char_type = path_native_char_type;
    static BOOST_CONSTEXPR_OR_CONST bool is_native = false;
};

#undef BOOST_FILESYSTEM_DETAIL_IS_CHAR_NATIVE
#undef BOOST_FILESYSTEM_DETAIL_IS_WCHAR_T_NATIVE


//! The trait tests if the type is a known path Source tag
template< typename Tag >
struct is_known_path_source_tag :
    public std::true_type
{
};

template< >
struct is_known_path_source_tag< unknown_type_tag > :
    public std::false_type
{
};

//! The trait tests if the type is compatible with path Source requirements
template< typename T >
struct is_path_source :
    public is_known_path_source_tag< typename path_source_traits< T >::tag_type >::type
{
};


//! The trait indicates whether the type is a path Source that is natively supported by path::string_type as the source for construction/assignment/appending
template< typename T >
struct is_native_path_source :
    public std::integral_constant< bool, path_source_traits< T >::is_native >
{
};


//! The trait indicates whether the type is one of the supported path character types
template< typename T >
struct is_path_char_type :
    public std::false_type
{
};

template< >
struct is_path_char_type< char > :
    public std::true_type
{
};

template< >
struct is_path_char_type< wchar_t > :
    public std::true_type
{
};


template< typename Iterator >
struct is_iterator_to_path_chars :
    public is_path_char_type< typename std::iterator_traits< Iterator >::value_type >::type
{
};

//! The trait indicates whether the type is an iterator over a sequence of path characters
template< typename Iterator >
struct is_path_source_iterator :
    public std::integral_constant<
        bool,
        detail::conjunction<
            boost::iterators::is_iterator< Iterator >,
            is_iterator_to_path_chars< Iterator >
        >::value
    >
{
};


//! The trait indicates whether the type is a pointer to a sequence of native path characters
template< typename T >
struct is_native_char_ptr :
    public std::false_type
{
};

template< >
struct is_native_char_ptr< path_native_char_type* > :
    public std::true_type
{
};

template< >
struct is_native_char_ptr< const path_native_char_type* > :
    public std::true_type
{
};


//! Converts character encoding using the supplied codecvt facet. If \a cvt is \c nullptr then \c path::codecvt() will be used.
BOOST_FILESYSTEM_DECL
void convert(const char* from, const char* from_end, std::wstring& to, const codecvt_type* cvt = nullptr);

//! \overload convert
BOOST_FILESYSTEM_DECL
void convert(const wchar_t* from, const wchar_t* from_end, std::string& to, const codecvt_type* cvt = nullptr);


//  Source dispatch  -----------------------------------------------------------------//

template< typename Source, typename Callback >
typename Callback::result_type dispatch(Source const& source, Callback cb, const codecvt_type* cvt = nullptr);

template< typename Callback >
BOOST_FORCEINLINE typename Callback::result_type dispatch(const char* source, Callback cb, const codecvt_type* cvt, ntcts_type_tag)
{
    return cb(source, source + std::strlen(source), cvt);
}

template< typename Callback >
BOOST_FORCEINLINE typename Callback::result_type dispatch(const wchar_t* source, Callback cb, const codecvt_type* cvt, ntcts_type_tag)
{
    return cb(source, source + std::wcslen(source), cvt);
}

template< typename Source, typename Callback >
BOOST_FORCEINLINE typename Callback::result_type dispatch(Source const& source, Callback cb, const codecvt_type* cvt, string_class_tag)
{
    return cb(source.data(), source.data() + source.size(), cvt);
}

template< typename Source, typename Callback >
BOOST_FORCEINLINE typename Callback::result_type dispatch(Source const& source, Callback cb, const codecvt_type* cvt, range_type_tag)
{
    std::basic_string< typename Source::value_type > src(source.begin(), source.end());
    return cb(src.data(), src.data() + src.size(), cvt);
}

#if defined(BOOST_FILESYSTEM_DEPRECATED) && BOOST_FILESYSTEM_VERSION < 4

template< typename Callback >
BOOST_FORCEINLINE typename Callback::result_type dispatch(std::vector< char > const& source, Callback cb, const codecvt_type* cvt, range_type_tag)
{
    const char* data = nullptr, *data_end = nullptr;
    if (!source.empty())
    {
        data = &source[0];
        data_end = data + source.size();
    }
    return cb(data, data_end, cvt);
}

template< typename Callback >
BOOST_FORCEINLINE typename Callback::result_type dispatch(std::vector< wchar_t > const& source, Callback cb, const codecvt_type* cvt, range_type_tag)
{
    const wchar_t* data = nullptr, *data_end = nullptr;
    if (!source.empty())
    {
        data = &source[0];
        data_end = data + source.size();
    }
    return cb(data, data_end, cvt);
}

#endif // defined(BOOST_FILESYSTEM_DEPRECATED) && BOOST_FILESYSTEM_VERSION < 4

// Defined in directory.hpp to avoid circular header dependencies
template< typename Callback >
typename Callback::result_type dispatch(directory_entry const& de, Callback cb, const codecvt_type* cvt, directory_entry_tag);

template< typename Source, typename Callback >
BOOST_FORCEINLINE typename Callback::result_type dispatch(Source const& source, Callback cb, const codecvt_type* cvt)
{
    return path_traits::dispatch(source, cb, cvt,
        typename path_traits::path_source_traits< typename std::remove_cv< Source >::type >::tag_type());
}


using yes_type = char;
struct no_type { char buf[2]; };

#if !defined(BOOST_FILESYSTEM_DETAIL_CXX23_STRING_VIEW_HAS_IMPLICIT_RANGE_CTOR)

namespace is_convertible_to_path_source_impl {

yes_type check_convertible(const char*);
yes_type check_convertible(const wchar_t*);
yes_type check_convertible(std::string const&);
yes_type check_convertible(std::wstring const&);
yes_type check_convertible(boost::container::basic_string< char, std::char_traits< char >, void > const&);
yes_type check_convertible(boost::container::basic_string< wchar_t, std::char_traits< wchar_t >, void > const&);
#if !defined(BOOST_NO_CXX17_HDR_STRING_VIEW)
yes_type check_convertible(std::string_view const&);
yes_type check_convertible(std::wstring_view const&);
#endif
yes_type check_convertible(boost::basic_string_view< char, std::char_traits< char > > const&);
yes_type check_convertible(boost::basic_string_view< wchar_t, std::char_traits< wchar_t > > const&);
no_type check_convertible(std::nullptr_t);
no_type check_convertible(...);

} // namespace is_convertible_to_path_source_impl

template< typename T >
struct check_is_convertible_to_path_source :
    public std::integral_constant<
        bool,
        sizeof(is_convertible_to_path_source_impl::check_convertible(std::declval< T const& >())) == sizeof(yes_type)
    >
{
};

/*!
 * \brief The type trait indicates whether the type has a conversion path to one of the path source types.
 *
 * \note The type trait returns `false` if the type is convertible to `path`. This prevents testing other
 *       conversion paths and forces the conversion to `path` to be chosen instead, to invoke a non-template
 *       member of `path` accepting a `path` argument.
 */
template< typename T >
struct is_convertible_to_path_source :
    public std::integral_constant<
        bool,
        detail::conjunction<
            detail::negation< std::is_convertible< T, path > >,
            check_is_convertible_to_path_source< T >
        >::value
    >
{
};

#else // !defined(BOOST_FILESYSTEM_DETAIL_CXX23_STRING_VIEW_HAS_IMPLICIT_RANGE_CTOR)

// Note: We use separate checks for convertibility to std::string_view and other types to avoid ambiguity with an implicit range constructor
//       of std::string_view in the early C++23 draft (N4892). If a user's type is convertible to e.g. std::string and also satisfies
//       ranges::contiguous_range and ranges::sized_range concepts then the conversion is ambiguous: the type is convertible to std::string
//       through the conversion operator in the user's class and is also convertible to std::string_view through the implicit conversion
//       constructor in std::string_view. The solution is to check convertibility to std::string_view separately first.

namespace is_convertible_to_std_string_view_impl {

yes_type check_convertible(std::string_view const&);
yes_type check_convertible(std::wstring_view const&);
no_type check_convertible(std::nullptr_t);
no_type check_convertible(...);

} // namespace is_convertible_to_std_string_view_impl

template< typename T >
struct check_is_convertible_to_std_string_view :
    public std::integral_constant<
        bool,
        sizeof(is_convertible_to_std_string_view_impl::check_convertible(std::declval< T const& >())) == sizeof(yes_type)
    >
{
};

namespace is_convertible_to_path_source_non_std_string_view_impl {

yes_type check_convertible(const char*);
yes_type check_convertible(const wchar_t*);
yes_type check_convertible(std::string const&);
yes_type check_convertible(std::wstring const&);
yes_type check_convertible(boost::container::basic_string< char, std::char_traits< char >, void > const&);
yes_type check_convertible(boost::container::basic_string< wchar_t, std::char_traits< wchar_t >, void > const&);
yes_type check_convertible(boost::basic_string_view< char, std::char_traits< char > > const&);
yes_type check_convertible(boost::basic_string_view< wchar_t, std::char_traits< wchar_t > > const&);
no_type check_convertible(std::nullptr_t);
no_type check_convertible(...);

} // namespace is_convertible_to_path_source_non_std_string_view_impl

template< typename T >
struct check_is_convertible_to_path_source_non_std_string_view :
    public std::integral_constant<
        bool,
        sizeof(is_convertible_to_path_source_non_std_string_view_impl::check_convertible(std::declval< T const& >())) == sizeof(yes_type)
    >
{
};

/*!
 * \brief The type trait indicates whether the type has a conversion path to one of the path source types.
 *
 * \note The type trait returns `false` if the type is convertible to `path`. This prevents testing other
 *       conversion paths and forces the conversion to `path` to be chosen instead, to invoke a non-template
 *       member of `path` accepting a `path` argument.
 */
template< typename T >
struct is_convertible_to_path_source :
    public std::integral_constant<
        bool,
        detail::conjunction<
            detail::negation< std::is_convertible< T, path > >,
            detail::disjunction<
                check_is_convertible_to_std_string_view< T >,
                check_is_convertible_to_path_source_non_std_string_view< T >
            >
        >::value
    >
{
};

#endif // !defined(BOOST_FILESYSTEM_DETAIL_CXX23_STRING_VIEW_HAS_IMPLICIT_RANGE_CTOR)

//! The type trait makes \a T dependent on the second template argument. Used to delay type resolution and name binding.
template< typename T, typename >
struct make_dependent
{
    using type = T;
};

template< typename Source, typename Callback >
BOOST_FORCEINLINE typename Callback::result_type dispatch_convertible_impl(const char* source, Callback cb, const codecvt_type* cvt)
{
    using source_t = typename path_traits::make_dependent< const char*, Source >::type;
    return path_traits::dispatch(static_cast< source_t >(source), cb, cvt);
}

template< typename Source, typename Callback >
BOOST_FORCEINLINE typename Callback::result_type dispatch_convertible_impl(const wchar_t* source, Callback cb, const codecvt_type* cvt)
{
    using source_t = typename path_traits::make_dependent< const wchar_t*, Source >::type;
    return path_traits::dispatch(static_cast< source_t >(source), cb, cvt);
}

template< typename Source, typename Callback >
BOOST_FORCEINLINE typename Callback::result_type dispatch_convertible_impl(std::string const& source, Callback cb, const codecvt_type* cvt)
{
    using source_t = typename path_traits::make_dependent< std::string, Source >::type;
    return path_traits::dispatch(static_cast< source_t const& >(source), cb, cvt);
}

template< typename Source, typename Callback >
BOOST_FORCEINLINE typename Callback::result_type dispatch_convertible_impl(std::wstring const& source, Callback cb, const codecvt_type* cvt)
{
    using source_t = typename path_traits::make_dependent< std::wstring, Source >::type;
    return path_traits::dispatch(static_cast< source_t const& >(source), cb, cvt);
}

template< typename Source, typename Callback >
BOOST_FORCEINLINE typename Callback::result_type dispatch_convertible_impl
(
    boost::container::basic_string< char, std::char_traits< char >, void > const& source,
    Callback cb,
    const codecvt_type* cvt
)
{
    using source_t = typename path_traits::make_dependent< boost::container::basic_string< char, std::char_traits< char >, void >, Source >::type;
    return path_traits::dispatch(static_cast< source_t const& >(source), cb, cvt);
}

template< typename Source, typename Callback >
BOOST_FORCEINLINE typename Callback::result_type dispatch_convertible_impl
(
    boost::container::basic_string< wchar_t, std::char_traits< wchar_t >, void > const& source,
    Callback cb,
    const codecvt_type* cvt
)
{
    using source_t = typename path_traits::make_dependent< boost::container::basic_string< wchar_t, std::char_traits< wchar_t >, void >, Source >::type;
    return path_traits::dispatch(static_cast< source_t const& >(source), cb, cvt);
}

template< typename Source, typename Callback >
BOOST_FORCEINLINE typename Callback::result_type dispatch_convertible_impl
(
    boost::basic_string_view< char, std::char_traits< char > > const& source,
    Callback cb,
    const codecvt_type* cvt
)
{
    using source_t = typename path_traits::make_dependent< boost::basic_string_view< char, std::char_traits< char > >, Source >::type;
    return path_traits::dispatch(static_cast< source_t const& >(source), cb, cvt);
}

template< typename Source, typename Callback >
BOOST_FORCEINLINE typename Callback::result_type dispatch_convertible_impl
(
    boost::basic_string_view< wchar_t, std::char_traits< wchar_t > > const& source,
    Callback cb,
    const codecvt_type* cvt
)
{
    using source_t = typename path_traits::make_dependent< boost::basic_string_view< wchar_t, std::char_traits< wchar_t > >, Source >::type;
    return path_traits::dispatch(static_cast< source_t const& >(source), cb, cvt);
}

#if !defined(BOOST_FILESYSTEM_DETAIL_CXX23_STRING_VIEW_HAS_IMPLICIT_RANGE_CTOR)

#if !defined(BOOST_NO_CXX17_HDR_STRING_VIEW)

template< typename Source, typename Callback >
BOOST_FORCEINLINE typename Callback::result_type dispatch_convertible_impl(std::string_view const& source, Callback cb, const codecvt_type* cvt)
{
    using source_t = typename path_traits::make_dependent< std::string_view, Source >::type;
    return path_traits::dispatch(static_cast< source_t const& >(source), cb, cvt);
}

template< typename Source, typename Callback >
BOOST_FORCEINLINE typename Callback::result_type dispatch_convertible_impl(std::wstring_view const& source, Callback cb, const codecvt_type* cvt)
{
    using source_t = typename path_traits::make_dependent< std::wstring_view, Source >::type;
    return path_traits::dispatch(static_cast< source_t const& >(source), cb, cvt);
}

#endif // !defined(BOOST_NO_CXX17_HDR_STRING_VIEW)

template< typename Source, typename Callback >
BOOST_FORCEINLINE typename Callback::result_type dispatch_convertible(Source const& source, Callback cb, const codecvt_type* cvt = nullptr)
{
    using source_t = typename std::remove_cv< Source >::type;
    return path_traits::dispatch_convertible_impl< source_t >(source, cb, cvt);
}

#else // !defined(BOOST_FILESYSTEM_DETAIL_CXX23_STRING_VIEW_HAS_IMPLICIT_RANGE_CTOR)

template< typename Source, typename Callback >
BOOST_FORCEINLINE typename Callback::result_type dispatch_convertible_sv_impl(std::string_view const& source, Callback cb, const codecvt_type* cvt)
{
    using source_t = typename path_traits::make_dependent< std::string_view, Source >::type;
    return path_traits::dispatch(static_cast< source_t const& >(source), cb, cvt);
}

template< typename Source, typename Callback >
BOOST_FORCEINLINE typename Callback::result_type dispatch_convertible_sv_impl(std::wstring_view const& source, Callback cb, const codecvt_type* cvt)
{
    using source_t = typename path_traits::make_dependent< std::wstring_view, Source >::type;
    return path_traits::dispatch(static_cast< source_t const& >(source), cb, cvt);
}

template< typename Source, typename Callback >
BOOST_FORCEINLINE typename std::enable_if<
    !check_is_convertible_to_std_string_view< typename std::remove_cv< Source >::type >::value,
    typename Callback::result_type
>::type dispatch_convertible(Source const& source, Callback cb, const codecvt_type* cvt = nullptr)
{
    using source_t = typename std::remove_cv< Source >::type;
    return path_traits::dispatch_convertible_impl< source_t >(source, cb, cvt);
}

template< typename Source, typename Callback >
BOOST_FORCEINLINE typename std::enable_if<
    check_is_convertible_to_std_string_view< typename std::remove_cv< Source >::type >::value,
    typename Callback::result_type
>::type dispatch_convertible(Source const& source, Callback cb, const codecvt_type* cvt = nullptr)
{
    using source_t = typename std::remove_cv< Source >::type;
    return path_traits::dispatch_convertible_sv_impl< source_t >(source, cb, cvt);
}

#endif // !defined(BOOST_FILESYSTEM_DETAIL_CXX23_STRING_VIEW_HAS_IMPLICIT_RANGE_CTOR)

} // namespace path_traits
} // namespace detail
} // namespace filesystem
} // namespace boost

#include <boost/filesystem/detail/footer.hpp>

#endif // BOOST_FILESYSTEM_DETAIL_PATH_TRAITS_HPP
