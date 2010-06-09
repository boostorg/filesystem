//  filesystem path.hpp  ---------------------------------------------------------------//

//  Copyright Beman Dawes 2002-2005, 2009
//  Copyright Vladimir Prus 2002

//  Distributed under the Boost Software License, Version 1.0.
//  See http://www.boost.org/LICENSE_1_0.txt

//  Library home page: http://www.boost.org/libs/filesystem

//  path::stem(), extension(), and replace_extension() are based on
//  basename(), extension(), and change_extension() from the original
//  filesystem/convenience.hpp header by Vladimir Prus.

#ifndef BOOST_FILESYSTEM_PATH_HPP
#define BOOST_FILESYSTEM_PATH_HPP

#include <boost/filesystem/v3/config.hpp>
#include <boost/filesystem/v3/path_traits.hpp>
#include <boost/system/error_code.hpp>
#include <boost/system/system_error.hpp>
#include <boost/iterator/iterator_facade.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/static_assert.hpp>
#include <string>
#include <iterator>
#include <cstring>
#include <iosfwd>    // needed by basic_path inserter and extractor
#include <stdexcept>
#include <cassert>
#include <locale>
#include <algorithm>

#include <boost/config/abi_prefix.hpp> // must be the last #include

namespace boost
{
namespace filesystem3
{
  //------------------------------------------------------------------------------------//
  //                                                                                    //
  //                                    class path                                      //
  //                                                                                    //
  //------------------------------------------------------------------------------------//

/*
   Why are there no const codecvt_type& arguments?
   ------------------------------------------------

   To hold down the size of the class path interface. Per function codecvt facets
   just aren't needed very often in practice.

   An RAII idiom can be used to ensure push/pop behavior as an alternative.

   Note that codecvt() is passed to the path_traits::convert functions, since that
   decouples the convert functions from class path.

   const codecvt_type & can be added later, but once added, they can never be removed
   since that would break user code.
*/

  class BOOST_FILESYSTEM_DECL path
  {
  public:

    //  value_type is the character type used by the operating system API to
    //  represent paths.

#   ifdef BOOST_WINDOWS_API
    typedef wchar_t                        value_type;
#   else 
    typedef char                           value_type;
#   endif
    typedef std::basic_string<value_type>  string_type;  
    typedef path_traits::codecvt_type      codecvt_type;

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

    //  Design alternative; each function can have an additional overload that
    //  supplies a conversion locale. For example:
    //
    //      template< class ForwardIterator, class WStringConvert >
    //      path(ForwardIterator begin, ForwardIterator end,
    //            const std::locale & loc,
    //            system::error_code & ec = boost::throws());
    // 
    //  This alternative was rejected as too complex for the limited benefits;
    //  it nearly doubles the size of the interface, and adds a lot of
    //  implementation and test code, yet would likely be rarely used. The same
    //  effect can be achieved via the much simpler imbue() mechanism.


    //  TODO: rules needed for operating systems that use / or .
    //  differently, or format directory paths differently from file paths. 
    //
    //  ************************************************************************
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
    //  ************************************************************************

    //  Supported source arguments: half-open iterator range, container, c-array,
    //  and single pointer to null terminated string.

    //  All source arguments except pointers to null terminated byte strings support
    //  multi-byte character strings which may have embedded nulls. Embedded null
    //  support is required for some Asian languages on Windows.

    //  -----  constructors  -----

    path(){}                                          

    path(const path& p) : m_pathname(p.m_pathname) {}

    template <class InputIterator>
    path(InputIterator begin, InputIterator end)
    { 
      if (begin != end)
      {
        std::basic_string<typename std::iterator_traits<InputIterator>::value_type>
          s(begin, end);
        path_traits::convert(s.c_str(), s.c_str()+s.size(), m_pathname, codecvt());
      }
    }

    template <class Source>
    path(Source const& source)
    {
      path_traits::dispatch(source, m_pathname, codecvt());
    }

    //  -----  assignments  -----

    path& operator=(const path& p)
    {
      m_pathname = p.m_pathname;
      return *this;
    }

    template <class InputIterator>
    path& assign(InputIterator begin, InputIterator end)
    { 
      m_pathname.clear();
      if (begin != end)
      {
        std::basic_string<typename std::iterator_traits<InputIterator>::value_type>
          s(begin, end);
        path_traits::convert(s.c_str(), s.c_str()+s.size(), m_pathname, codecvt());
      }
      return *this;
    }

    template <class Source>
    path& operator=(Source const& source)
    {
      m_pathname.clear();
      path_traits::dispatch(source, m_pathname, codecvt());
      return *this;
    }

    //  -----  appends  -----

    //  if a separator is added, it is the preferred separator for the platform;
    //  slash for POSIX, backslash for Windows

    path& operator/=(const path& p);

    template <class InputIterator>
    path& append(InputIterator begin, InputIterator end);

    template <class Source>
    path& operator/=(Source const& source);

    //  -----  modifiers  -----

    void   clear()             { m_pathname.clear(); }
    path&  make_absolute(const path& base); 
    path&  make_preferred()
#                              ifdef BOOST_POSIX_API
                               { return *this; }  // POSIX no effect
#                              else // BOOST_WINDOWS_API
                               ;  // change slashes to backslashes
#                              endif
    path&  remove_filename();
    path&  replace_extension(const path& new_extension = path());
    void   swap(path& rhs)     { m_pathname.swap(rhs.m_pathname); }

    //  -----  observers  -----
  
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

    const string_type&  native() const { return m_pathname; }          // Throws: nothing
    const value_type*   c_str() const  { return m_pathname.c_str(); }  // Throws: nothing

    template <class String>
    String string() const;

#   ifdef BOOST_WINDOWS_API
    const std::string    string() const;
    const std::wstring&  wstring() const { return m_pathname; }

#   else   // BOOST_POSIX_API
    const std::string&  string() const   { return m_pathname; }
    const std::wstring  wstring() const
    { 
      std::wstring tmp;
      if (!m_pathname.empty())
        path_traits::convert(&*m_pathname.begin(), &*m_pathname.begin()+m_pathname.size(),
          tmp, codecvt());
      return tmp;
    }

#   endif

    //  -----  generic format observers  -----

    template <class String>
    String generic_string() const;

#   ifdef BOOST_WINDOWS_API
    const std::string   generic_string() const; 
    const std::wstring  generic_wstring() const;

#   else // BOOST_POSIX_API
    const std::string&  generic_string() const  { return m_pathname; }
    const std::wstring  generic_wstring() const { return wstring(); }

#   endif

    //  -----  decomposition  -----

    path  root_path() const; 
    path  root_name() const;         // returns 0 or 1 element path
                                     // even on POSIX, root_name() is non-empty() for network paths
    path  root_directory() const;    // returns 0 or 1 element path
    path  relative_path() const;
    path  parent_path() const;
    path  filename() const;          // returns 0 or 1 element path
    path  stem() const;              // returns 0 or 1 element path
    path  extension() const;         // returns 0 or 1 element path

    //  -----  query  -----

    bool empty() const               { return m_pathname.empty(); } // name consistent with std containers
    bool has_root_path() const       { return has_root_directory() || has_root_name(); }
    bool has_root_name() const       { return !root_name().empty(); }
    bool has_root_directory() const  { return !root_directory().empty(); }
    bool has_relative_path() const   { return !relative_path().empty(); }
    bool has_parent_path() const     { return !parent_path().empty(); }
    bool has_filename() const        { return !m_pathname.empty(); }
    bool has_stem() const            { return !stem().empty(); }
    bool has_extension() const       { return !extension().empty(); }
    bool is_absolute() const
    {
#     ifdef BOOST_WINDOWS_API
      return has_root_name() && has_root_directory();
#     else
      return has_root_directory();
#     endif
    }
    bool is_relative() const         { return !is_absolute(); } 

    //  -----  imbue  -----

    static std::locale imbue(const std::locale & loc);

    //  -----  codecvt  -----

    static const codecvt_type & codecvt()
    {
      return *wchar_t_codecvt_facet();
    }

    //  -----  iterators  -----

    class iterator;
    typedef iterator const_iterator;

    iterator begin() const;
    iterator end() const;

    //  -----  deprecated functions  -----

# if defined(BOOST_FILESYSTEM_DEPRECATED) && defined(BOOST_FILESYSTEM_NO_DEPRECATED)
#   error both BOOST_FILESYSTEM_DEPRECATED and BOOST_FILESYSTEM_NO_DEPRECATED are defined
# endif

# if !defined(BOOST_FILESYSTEM_NO_DEPRECATED)
    //  recently deprecated functions supplied by default
    path&  normalize()              { return m_normalize(); }
    path&  remove_leaf()            { return remove_filename(); }
    path   leaf() const             { return filename(); }
    path   branch_path() const      { return parent_path(); }
    bool   has_leaf() const         { return !m_pathname.empty(); }
    bool   has_branch_path() const  { return !parent_path().empty(); }
    bool   is_complete() const      { return is_absolute(); }
# endif

# if defined(BOOST_FILESYSTEM_DEPRECATED)
    //  deprecated functions with enough signature or semantic changes that they are
    //  not supplied by default 
    const std::string file_string() const               { return string(); }
    const std::string directory_string() const          { return string(); }
    const std::string native_file_string() const        { return string(); }
    const std::string native_directory_string() const   { return string(); }
    const string_type external_file_string() const      { return native(); }
    const string_type external_directory_string() const { return native(); }

    //  older functions no longer supported
    //typedef bool (*name_check)(const std::string & name);
    //basic_path(const string_type& str, name_check) { operator/=(str); }
    //basic_path(const typename string_type::value_type* s, name_check)
    //  { operator/=(s);}
    //static bool default_name_check_writable() { return false; } 
    //static void default_name_check(name_check) {}
    //static name_check default_name_check() { return 0; }
    //basic_path& canonize();
# endif

//--------------------------------------------------------------------------------------//
//                            class path private members                                //
//--------------------------------------------------------------------------------------//

  private:
#   if defined(_MSC_VER)
#     pragma warning(push) // Save warning settings
#     pragma warning(disable : 4251) // disable warning: class 'std::basic_string<_Elem,_Traits,_Ax>'
#   endif                            // needs to have dll-interface...
/*
      m_pathname has the type, encoding, and format required by the native
      operating system. Thus for POSIX and Windows there is no conversion for
      passing m_pathname.c_str() to the O/S API or when obtaining a path from the
      O/S API. POSIX encoding is unspecified other than for dot and slash
      characters; POSIX just treats paths as a sequence of bytes. Windows
      encoding is UCS-2 or UTF-16 depending on the version.
*/
    string_type  m_pathname;  // Windows: as input; backslashes NOT converted to slashes,
                              // slashes NOT converted to backslashes
#   if defined(_MSC_VER)
#     pragma warning(pop) // restore warning settings.
#   endif 

    string_type::size_type m_append_separator_if_needed();
    //  Returns: If separator is to be appended, m_pathname.size() before append. Otherwise 0.
    //  Note: An append is never performed if size()==0, so a returned 0 is unambiguous.

    void m_erase_redundant_separator(string_type::size_type sep_pos);
    string_type::size_type m_parent_path_end() const;
    void m_portable();

    path& m_normalize();

    // Was qualified; como433beta8 reports:
    //    warning #427-D: qualified name is not allowed in member declaration 
    friend class iterator;
    friend bool operator<(const path& lhs, const path& rhs);

    // see path::iterator::increment/decrement comment below
    static void m_path_iterator_increment(path::iterator & it);
    static void m_path_iterator_decrement(path::iterator & it);

    static const codecvt_type *&  wchar_t_codecvt_facet();

  };  // class path

# ifndef BOOST_FILESYSTEM_NO_DEPRECATED
  typedef path wpath;
# endif

  //------------------------------------------------------------------------------------//
  //                             class path::iterator                                   //
  //------------------------------------------------------------------------------------//
 
  class path::iterator
    : public boost::iterator_facade<
      iterator,
      path const,
      boost::bidirectional_traversal_tag >
  {
  private:
    friend class boost::iterator_core_access;
    friend class boost::filesystem3::path;
    friend void m_path_iterator_increment(path::iterator & it);
    friend void m_path_iterator_decrement(path::iterator & it);

    const path& dereference() const { return m_element; }

    bool equal(const iterator & rhs) const
    {
      return m_path_ptr == rhs.m_path_ptr && m_pos == rhs.m_pos;
    }

    // iterator_facade derived classes don't seem to like implementations in
    // separate translation unit dll's, so forward to class path static members
    void increment() { m_path_iterator_increment(*this); }
    void decrement() { m_path_iterator_decrement(*this); }

    path                    m_element;   // current element
    const path *            m_path_ptr;  // path being iterated over
    string_type::size_type  m_pos;       // position of name in
                                         // m_path_ptr->m_pathname. The
                                         // end() iterator is indicated by 
                                         // m_pos == m_path_ptr->m_pathname.size()
  }; // path::iterator

  //------------------------------------------------------------------------------------//
  //                                                                                    //
  //                            class scoped_path_locale                                //
  //                                                                                    //
  //------------------------------------------------------------------------------------//
 
  class scoped_path_locale
  {
  public:
    scoped_path_locale(const std::locale & loc)
                      : m_saved_locale(loc)
    {
      path::imbue(loc);
    }

    ~scoped_path_locale()   // never throws()
    {
      try { path::imbue(m_saved_locale); }
      catch (...) {}
    };

  private:
    std::locale m_saved_locale;
  };
   
  //------------------------------------------------------------------------------------//
  //                                                                                    //
  //                              non-member functions                                  //
  //                                                                                    //
  //------------------------------------------------------------------------------------//

  //  std::lexicographical_compare would infinately recurse because path iterators
  //  yield paths, so provide a path aware version
  inline bool lexicographical_compare(path::iterator first1, path::iterator last1,
    path::iterator first2, path::iterator last2)
  {
    for (; first1 != last1 && first2 != last2 ; ++first1, ++first2)
    {
      if (first1->native() < first2->native()) return true;
      if (first2->native() < first1->native()) return false;
    }
    return first1 == last1 && first2 != last2;
  }
  
  inline bool operator<(const path& lhs, const path& rhs)
  {
    return lexicographical_compare(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
  }

  inline bool operator<=(const path& lhs, const path& rhs) { return !(rhs < lhs); }
  inline bool operator> (const path& lhs, const path& rhs) { return rhs < lhs; }
  inline bool operator>=(const path& lhs, const path& rhs) { return !(lhs < rhs);  }

  // equality operators act as if comparing generic format strings, to achieve the
  // effect of lexicographical_compare element by element compare.
  // operator==() efficiency is a concern; a user reported the original version 2
  // !(lhs < rhs) && !(rhs < lhs) implementation caused a serious performance problem
  // for a map of 10,000 paths.

# ifdef BOOST_WINDOWS_API
  inline bool operator==(const path& lhs, const path::value_type* rhs)
  {
    const path::value_type* l(lhs.c_str());
    while ((*l == *rhs || (*l == L'\\' && *rhs == L'/') || (*l == L'/' && *rhs == L'\\'))
      && *l) { ++l; ++rhs; }
    return *l == *rhs || (*l == L'\\' && *rhs == L'/') || (*l == L'/' && *rhs == L'\\');  
  }
  inline bool operator==(const path& lhs, const path& rhs)              { return lhs == rhs.c_str(); }
  inline bool operator==(const path& lhs, const path::string_type& rhs) { return lhs == rhs.c_str(); }
  inline bool operator==(const path::string_type& lhs, const path& rhs) { return rhs == lhs.c_str(); }
  inline bool operator==(const path::value_type* lhs, const path& rhs)  { return rhs == lhs; }
# else   // BOOST_POSIX_API
  inline bool operator==(const path& lhs, const path& rhs)              { return lhs.native() == rhs.native(); }
  inline bool operator==(const path& lhs, const path::string_type& rhs) { return lhs.native() == rhs; }
  inline bool operator==(const path& lhs, const path::value_type* rhs)  { return lhs.native() == rhs; }
  inline bool operator==(const path::string_type& lhs, const path& rhs) { return lhs == rhs.native(); }
  inline bool operator==(const path::value_type* lhs, const path& rhs)  { return lhs == rhs.native(); }
# endif

  inline bool operator!=(const path& lhs, const path& rhs)              { return !(lhs == rhs); }
  inline bool operator!=(const path& lhs, const path::string_type& rhs) { return !(lhs == rhs); }
  inline bool operator!=(const path& lhs, const path::value_type* rhs)  { return !(lhs == rhs); }
  inline bool operator!=(const path::string_type& lhs, const path& rhs) { return !(lhs == rhs); }
  inline bool operator!=(const path::value_type* lhs, const path& rhs)  { return !(lhs == rhs); }

  inline void swap(path& lhs, path& rhs)                   { lhs.swap(rhs); }

  inline path operator/(const path& lhs, const path& rhs)  { return path(lhs) /= rhs; }

  //  inserters and extractors

  inline std::ostream& operator<<(std::ostream & os, const path& p)
  {
    os << p.string();
    return os;
  }
  
  inline std::wostream& operator<<(std::wostream & os, const path& p)
  {
    os << p.wstring();
    return os;
  }
  
  inline std::istream& operator>>(std::istream & is, path& p)
  {
    std::string str;
    is >> str;
    p = str;
    return is;
  }
  
  inline std::wistream& operator>>(std::wistream & is, path& p)
  {
    std::wstring str;
    is >> str;
    p = str;
    return is;
  }

  //  name_checks

  BOOST_FILESYSTEM_DECL bool portable_posix_name(const std::string & name);
  BOOST_FILESYSTEM_DECL bool windows_name(const std::string & name);
  BOOST_FILESYSTEM_DECL bool portable_name(const std::string & name);
  BOOST_FILESYSTEM_DECL bool portable_directory_name(const std::string & name);
  BOOST_FILESYSTEM_DECL bool portable_file_name(const std::string & name);
  BOOST_FILESYSTEM_DECL bool native(const std::string & name);
 
//--------------------------------------------------------------------------------------//
//                     class path member template implementation                        //
//--------------------------------------------------------------------------------------//

  template <class InputIterator>
  path& path::append(InputIterator begin, InputIterator end)
  { 
    if (begin == end)
      return *this;
    string_type::size_type sep_pos(m_append_separator_if_needed());
    std::basic_string<typename std::iterator_traits<InputIterator>::value_type>
      s(begin, end);
    path_traits::convert(s.c_str(), s.c_str()+s.size(), m_pathname, codecvt());
    if (sep_pos)
      m_erase_redundant_separator(sep_pos);
    return *this;
  }

  template <class Source>
  path& path::operator/=(Source const & source)
  {
    if (path_traits::empty(source))
      return *this;
    string_type::size_type sep_pos(m_append_separator_if_needed());
    path_traits::dispatch(source, m_pathname, codecvt());
    if (sep_pos)
      m_erase_redundant_separator(sep_pos);
    return *this;
  }

//--------------------------------------------------------------------------------------//
//                     class path member template specializations                       //
//--------------------------------------------------------------------------------------//

  template <> inline
  std::string path::string<std::string>() const    { return string(); }

  template <> inline
  std::wstring path::string<std::wstring>() const  { return wstring(); }

  template <> inline
  std::string path::generic_string<std::string>() const   { return generic_string(); }

  template <> inline
  std::wstring path::generic_string<std::wstring>() const { return generic_wstring(); }


}  // namespace filesystem3
}  // namespace boost

//----------------------------------------------------------------------------//

namespace boost
{
  namespace filesystem
  {
    using filesystem3::path;
# ifndef BOOST_FILESYSTEM_NO_DEPRECATED
    using filesystem3::wpath;
# endif
    using filesystem3::lexicographical_compare;
    using filesystem3::portable_posix_name;
    using filesystem3::windows_name;
    using filesystem3::portable_name;
    using filesystem3::portable_directory_name;
    using filesystem3::portable_file_name;
    using filesystem3::native;
    using filesystem3::swap;
    using filesystem3::operator<;
    using filesystem3::operator==;
    using filesystem3::operator!=;
    using filesystem3::operator>;
    using filesystem3::operator<=;
    using filesystem3::operator>=;
    using filesystem3::operator/;
    using filesystem3::operator<<;
    using filesystem3::operator>>;
  }
}

//----------------------------------------------------------------------------//

#include <boost/config/abi_suffix.hpp> // pops abi_prefix.hpp pragmas

#endif  // BOOST_FILESYSTEM_PATH_HPP
