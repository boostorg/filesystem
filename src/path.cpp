//  filesystem path.cpp  -------------------------------------------------------------  //

//  Copyright Beman Dawes 2008

//  Distributed under the Boost Software License, Version 1.0.
//  See http://www.boost.org/LICENSE_1_0.txt

//  Library home page: http://www.boost.org/libs/filesystem

//  Old standard library configurations, particularly MingGW, don't support wide strings.
//  Report this with an explicit error message.
#include <boost/config.hpp>
# if defined( BOOST_NO_STD_WSTRING )
#   error Configuration not supported: Boost.Filesystem V3 and later requires std::wstring support
# endif

// define BOOST_FILESYSTEM_SOURCE so that <boost/system/config.hpp> knows
// the library is being built (possibly exporting rather than importing code)
#define BOOST_FILESYSTEM_SOURCE 

#ifndef BOOST_SYSTEM_NO_DEPRECATED 
# define BOOST_SYSTEM_NO_DEPRECATED
#endif

#include <boost/filesystem/config.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/filesystem/operations.hpp>  // for filesystem_error
#include <boost/scoped_array.hpp>
#include <boost/system/error_code.hpp>
#include <boost/assert.hpp>
#include <algorithm>
#include <cstddef>
#include <cstring>
#include <cassert>

#ifdef BOOST_WINDOWS_API
# include "windows_file_codecvt.hpp"
# include <windows.h>
#elif defined(macintosh) || defined(__APPLE__) || defined(__APPLE_CC__) \
 || defined(__FreeBSD__) || defined(__OpenBSD__) || defined(__HAIKU__)
# include <boost/filesystem/detail/utf8_codecvt_facet.hpp>
#endif

#ifdef BOOST_FILESYSTEM_DEBUG
# include <iostream>
# include <iomanip>
#endif

namespace fs = boost::filesystem;

using boost::filesystem::path;

using std::string;
using std::wstring;

using boost::system::error_code;

//--------------------------------------------------------------------------------------//
//                                                                                      //
//                                class path helpers                                    //
//                                                                                      //
//--------------------------------------------------------------------------------------//

namespace
{
  //------------------------------------------------------------------------------------//
  //                        miscellaneous class path helpers                            //
  //------------------------------------------------------------------------------------//

  typedef path::value_type        value_type;
  typedef path::string_type       string_type;
  typedef string_type::size_type  size_type;

# ifdef BOOST_WINDOWS_API

  const wchar_t* const separators = L"/\\";
  const wchar_t* separator_string = L"/";
  const wchar_t* preferred_separator_string = L"\\";
  const wchar_t colon = L':';
  const wchar_t questionmark = L'?';

  inline bool is_letter(wchar_t c)
  {
    return (c >= L'a' && c <=L'z') || (c >= L'A' && c <=L'Z');
  }

# else

  const char* const separators = "/";
  const char* separator_string = "/";
  const char* preferred_separator_string = "/";

# endif

  bool is_root_separator(const string_type& str, size_type pos);
    // pos is position of the separator

  size_type filename_pos(const string_type& str,
                          size_type end_pos); // end_pos is past-the-end position
  //  Returns: 0 if str itself is filename (or empty)

  size_type root_directory_start(const string_type& path, size_type size);
  //  Returns:  npos if no root_directory found

  void first_element(
      const string_type& src,
      size_type& element_pos,
      size_type& element_size,
      size_type size = string_type::npos
    );

}  // unnamed namespace

//--------------------------------------------------------------------------------------//
//                                                                                      //
//                            class path implementation                                 //
//                                                                                      //
//--------------------------------------------------------------------------------------//

namespace boost
{
namespace filesystem
{
  namespace detail
  {
    const path& preferred_separator_path()
    {
#   ifdef BOOST_WINDOWS_API
      static const fs::path preferred_separator(L"\\");
#   else
      static const fs::path preferred_separator("/");
#   endif
      return preferred_separator;
    }
  }

  path& path::operator/=(const path& p)
  {
    if (p.empty())
      return *this;
    if (this == &p)  // self-append
    {
      path rhs(p);
      if (!detail::is_directory_separator(rhs.m_pathname[0]))
        m_append_separator_if_needed();
      m_pathname += rhs.m_pathname;
    }
    else
    {
      if (!detail::is_directory_separator(*p.m_pathname.begin()))
        m_append_separator_if_needed();
      m_pathname += p.m_pathname;
    }
    return *this;
  }

  path& path::operator/=(const value_type* ptr)
  {
    if (!*ptr)
      return *this;
    if (ptr >= m_pathname.data()
      && ptr < m_pathname.data() + m_pathname.size())  // overlapping source
    {
      path rhs(ptr);
      if (!detail::is_directory_separator(rhs.m_pathname[0]))
        m_append_separator_if_needed();
      m_pathname += rhs.m_pathname;
    }
    else
    {
      if (!detail::is_directory_separator(*ptr))
        m_append_separator_if_needed();
      m_pathname += ptr;
    }
    return *this;
  }

  int path::compare(const path& p) const BOOST_NOEXCEPT
  {
    return detail::lex_compare(begin(), end(), p.begin(), p.end());
  }

# ifdef BOOST_WINDOWS_API

  const std::string path::generic_string() const
  {
    path tmp(*this);
    std::replace(tmp.m_pathname.begin(), tmp.m_pathname.end(), L'\\', L'/');
    return tmp.string();
  }

  const std::string path::generic_string(const codecvt_type& cvt) const
  {
    path tmp(*this);
    std::replace(tmp.m_pathname.begin(), tmp.m_pathname.end(), L'\\', L'/');
    return tmp.string(cvt);
  }

  const std::wstring path::generic_wstring() const
  { 
    path tmp(*this);
    std::replace(tmp.m_pathname.begin(), tmp.m_pathname.end(), L'\\', L'/');
    return tmp.wstring();
  }

# endif  // BOOST_WINDOWS_API

  //  m_append_separator_if_needed  ----------------------------------------------------//

  path::string_type::size_type path::m_append_separator_if_needed()
  {
    if (!m_pathname.empty() &&
#     ifdef BOOST_WINDOWS_API
      *(m_pathname.end()-1) != colon && 
#     endif
      !detail::is_directory_separator(*(m_pathname.end()-1)))
    {
      string_type::size_type tmp(m_pathname.size());
      m_pathname += preferred_separator;
      return tmp;
    }
    return 0;
  }

  //  m_erase_redundant_separator  -----------------------------------------------------//

  void path::m_erase_redundant_separator(string_type::size_type sep_pos)
  {
    if (sep_pos                         // a separator was added
      && sep_pos < m_pathname.size()         // and something was appended
      && (m_pathname[sep_pos+1] == separator // and it was also separator
#      ifdef BOOST_WINDOWS_API
       || m_pathname[sep_pos+1] == preferred_separator  // or preferred_separator
#      endif
)) { m_pathname.erase(sep_pos, 1); } // erase the added separator
  }

  //  modifiers  -----------------------------------------------------------------------//

# ifdef BOOST_WINDOWS_API
  path & path::make_preferred()
  {
    std::replace(m_pathname.begin(), m_pathname.end(), L'/', L'\\');
    return *this;
  }
# endif

  path& path::remove_filename()
  {
    if (has_filename())
      m_pathname.erase(m_parent_path_end());
    return *this;
  }

  path&  path::remove_trailing_separator()
  {
    if (!m_pathname.empty()
      && detail::is_directory_separator(m_pathname[m_pathname.size() - 1]))
      m_pathname.erase(m_pathname.size() - 1);
    return *this;
  }

  path& path::replace_extension(const path& new_extension)
  {
    // erase existing extension, including the dot, if any
    m_pathname.erase(m_pathname.size()-extension().m_pathname.size());

    if (!new_extension.empty())
    {
      // append new_extension, adding the dot if necessary
      if (new_extension.m_pathname[0] != dot)
        m_pathname.push_back(dot);
      m_pathname.append(new_extension.m_pathname);
    }

    return *this;
  }

  //  decomposition  -------------------------------------------------------------------//

  size_type path::m_root_name_size() const
  {
    size_type size{ 0 };

# ifdef BOOST_WINDOWS_API
    // Windows drive-letter, e.g. C:
    if (m_pathname.size() > 1 && m_pathname[1] == colon && is_letter(m_pathname[0]))
      size = 2;
    else
# endif
    
    // if too small to be a root-name, or is a filename or is a root-directory
    if (m_pathname.size() < 2                              // is empty
      || !detail::is_directory_separator(m_pathname[0])    // is filename
      || (m_pathname.size() >= 2                           // "/" ...
        && !detail::is_directory_separator(m_pathname[1])) //   is root-directory
      || (m_pathname.size() > 2
        && detail::is_directory_separator(m_pathname[1])   // "///" ...
        && detail::is_directory_separator(m_pathname[2]))  //   is root-directory
      ) {}
    
    // The only remaining possibility is that the pathname begins with exactly two
    // directory-separators (i.e. "//"). The implementation-defined remainder of the
    // root-name may be missing or otherwise invalid, but it isn't path's job to detect
    // or diagnose that.

    else
    {
      size = 2;
      for (; size != m_pathname.size()
        && !detail::is_directory_separator(m_pathname[size]); ++size) {}
    }

    return size;
  }

  size_type path::m_root_directory_size(size_type pos) const
  {
    size_type size{ 0 };
    for (; pos + size != m_pathname.size()
      && detail::is_directory_separator(m_pathname[pos+size]); ++size) {
    }
    return size;
  }

  path path::root_name() const
  {
    size_type sz(m_root_name_size());
    return m_pathname.empty()
      ? path()
      : (sz == m_pathname.size()
        ? *this
        : path(m_pathname.substr(0, sz)));
  }

  path path::root_directory() const
  {
    return has_root_directory() ? path(separator_string) : path();
  }

  path  path::root_path() const
  { 
    path temp(root_name());
    if (!root_directory().empty())
      temp.m_pathname += root_directory().c_str();
    return temp;
  } 

  path path::relative_path() const
  {
    path temp;

    if (has_relative_path())
    {
      auto itr = begin();
      if (has_root_name())        // skip root-name if present
        ++itr;
      if (has_root_directory())   // skip root-directory if present
        ++itr;
      for (; itr != end(); ++itr) // process any filenames & trailing directory-sep if any
      {
        if (!temp.empty())        // have processed at least one prior filename
          temp += separator;
        if (!itr->empty())        // append filename unless trailing directory-separator
          temp += itr->native();
      }
    }

    return temp;
  }

  string_type::size_type path::m_parent_path_end() const
  {
    size_type end_pos(filename_pos(m_pathname, m_pathname.size()));

    bool filename_was_separator(m_pathname.size()
      && detail::is_directory_separator(m_pathname[end_pos]));

    // skip separators unless root directory
    size_type root_dir_pos(root_directory_start(m_pathname, end_pos));
    for (; 
      end_pos > 0
      && (end_pos-1) != root_dir_pos
      && detail::is_directory_separator(m_pathname[end_pos-1])
      ;
      --end_pos) {}

   return (end_pos == 1 && root_dir_pos == 0 && filename_was_separator)
     ? string_type::npos
     : end_pos;
  }

  path path::parent_path() const
  {
    path temp;

    if (!empty())
    {
      auto itr = begin();
      if (itr != end() && has_root_name())
        temp = *itr++;
      if (itr != end() && has_root_directory())
        temp += *itr++;

      if (itr != end())               // has_relative_path() is true
      {
        auto end_itr = --end();       // parent_path does not include last element
        bool prior_filename{ false };
        for (; itr != end_itr; ++itr) // process any filenames
        {
          if (prior_filename)         // have processed at least one prior filename
            temp += separator;
          else
            prior_filename = true;
          temp += itr->native();      // append filename
        }
      }
    }
    return temp;


  }

  path path::stem() const
  {
    size_type pos;
    path name(filename());
    return (name == detail::dot_path()
      || name == detail::dot_dot_path()
      || (pos = name.m_pathname.rfind(dot)) == 0  // == 0 to suppress VC++ C4706
      || pos == string_type::npos)
      ? name
      : path(name.m_pathname.c_str(), name.m_pathname.c_str() + pos);
  }

  path path::extension() const
  {
    size_type pos;
    path name(filename());
    return (name == detail::dot_path()
      || name == detail::dot_dot_path()
      || (pos = name.m_pathname.rfind(dot)) == 0  // == 0 to suppress VC++ C4706
      || pos == string_type::npos)
      ? path()
      : path(name.m_pathname.c_str() + pos);
  }

  //  lexical operations  --------------------------------------------------------------//

  namespace detail
  {
    // C++14 provide a mismatch algorithm with four iterator arguments(), but earlier
    // standard libraries didn't, so provide this needed functionality.
    inline
    std::pair<path::iterator, path::iterator> mismatch(path::iterator it1,
      path::iterator it1end, path::iterator it2, path::iterator it2end)
    {
      for (; it1 != it1end && it2 != it2end && *it1 == *it2;)
      {
        ++it1;
        ++it2;
      }
      return std::make_pair(it1, it2);
    }
  }

  path path::lexically_relative(const path& base) const
  {
    std::pair<path::iterator, path::iterator> mm
      = detail::mismatch(begin(), end(), base.begin(), base.end());
    if (mm.first == begin() && mm.second == base.begin())
      return path();
    if (mm.first == end() && mm.second == base.end())
      return detail::dot_path();
    path tmp;
    for (; mm.second != base.end(); ++mm.second)
      tmp /= detail::dot_dot_path();
    for (; mm.first != end(); ++mm.first)
      tmp /= *mm.first;
    return tmp;
  }

  //  lexically_normal  ----------------------------------------------------------------//

  path path::lexically_normal() const
  {
    // aim for correctness; worry about efficiency later

    // "bullet #" in comments refers to 30.10.4.11 normal form [fs.def.normal.form]
   
    path temp;                  // return value will be built here

    // bullet 1: If the path is empty, stop.
    if (m_pathname.empty())
      return temp;

    //  This implementation assumes iteration over "//foo//bar///baz////"
    //  yields "//foo", "/", "bar", "baz", "". Search for "//foo//bar///baz////" in
    //  test/path_test.cpp to see a test of this.

    temp = root_name();
    iterator itr = begin();
 
    if (!temp.empty())  // i.e. there was a root-name
    {
      ++itr; // bypass the root-name

    // bullet 2: Replace each slash character in the root-name with a preferred-separator.
#ifdef BOOST_WINDOWS_API
      // on POSIX directory-separators are already the preferred_separator
      if (detail::is_directory_separator(temp.m_pathname[0]))
      {
        temp.m_pathname[0] = preferred_separator;
        BOOST_ASSERT_MSG(temp.size() > 2 
          && detail::is_directory_separator(temp.m_pathname[1]),
           "implementation error, please report");   // root_name() must be wacky
        temp.m_pathname[1] = preferred_separator;
      }
#endif
    } // temp was not empty; i.e. there was a root_name

    if (itr != end() && itr->has_root_directory())
    {
      // bullet 3: Replace each directory-separator with a preferred-separator.
      temp += preferred_separator;
      ++itr;

      // bullet 6: If there is a root-directory, remove all dot-dot filenames and any
      // directory-separators immediately following them.
      while (itr != end() && *itr == detail::dot_dot_path())
        ++itr;
    }

    // handle relative-path portion of the pathname if present
    for (; itr != end(); ++itr)
    {
      path prior_filename;   // used by bullet 5 handler below

      // bullet 4: Remove each dot filename and any immediately following
      // directory-separator.
      if (itr->native().size() == 1 && (itr->native())[0] == dot)
        {}  // skip the dot element

      // bullet 5: ... remove a non-dot-dot filename immediately followed by a
      // directory-separator and a dot-dot filename, along with any immediately following
      // directory-separator.
      else if (*itr == detail::dot_dot_path()
        && !(prior_filename = temp.filename()).empty()
        && prior_filename != detail::dot_dot_path())
        temp.remove_filename();

      else if (itr->empty()) // trailing directory-specifying
      {
        // bullet 7: If the last filename is dot-dot, remove any trailing
        // directory-separator.
        if (temp.filename() != detail::dot_dot_path())
          temp /= detail::preferred_separator_path();
      }

      else
        temp /= *itr;  // nothing special about this element, so just add it

    }  // each relative-path element

    // bullet 8: If the path is empty, add a dot.
    if (temp.empty())
      temp = detail::dot_path();

    return temp;
  }

}  // namespace filesystem
}  // namespace boost
  
//--------------------------------------------------------------------------------------//
//                                                                                      //
//                         class path helpers implementation                            //
//                                                                                      //
//--------------------------------------------------------------------------------------//

namespace
{

  //  is_root_separator  ---------------------------------------------------------------//

  bool is_root_separator(const string_type & str, size_type pos)
    // pos is position of the separator
  {
    BOOST_ASSERT_MSG(!str.empty() && fs::detail::is_directory_separator(str[pos]),
      "precondition violation");

    // subsequent logic expects pos to be for leftmost slash of a set
    while (pos > 0 && fs::detail::is_directory_separator(str[pos-1]))
      --pos;

    //  "/" [...]
    if (pos == 0)  
      return true;

# ifdef BOOST_WINDOWS_API
    //  "c:/" [...]
    if (pos == 2 && is_letter(str[0]) && str[1] == colon)  
      return true;
# endif

    //  "//" name "/"
    if (pos < 3 || !fs::detail::is_directory_separator(str[0])
      || !fs::detail::is_directory_separator(str[1]))
      return false;

    return str.find_first_of(separators, 2) == pos;
  }

  //  filename_pos  --------------------------------------------------------------------//

  size_type filename_pos(const string_type & str,
                          size_type end_pos) // end_pos is past-the-end position
    // return 0 if str itself is filename (or empty)
  {
    // case: "//"
    if (end_pos == 2 
      && fs::detail::is_directory_separator(str[0])
      && fs::detail::is_directory_separator(str[1])) return 0;

    // case: ends in "/"
    if (end_pos && fs::detail::is_directory_separator(str[end_pos-1]))
      return end_pos-1;
    
    // set pos to start of last element
    size_type pos(str.find_last_of(separators, end_pos-1));

#   ifdef BOOST_WINDOWS_API
    if (pos == string_type::npos && end_pos > 1)
      pos = str.find_last_of(colon, end_pos-2);
#   endif

    return (pos == string_type::npos // path itself must be a filename (or empty)
      || (pos == 1 && fs::detail::is_directory_separator(str[0]))) // or net
        ? 0 // so filename is entire string
        : pos + 1; // or starts after delimiter
  }

  //  root_directory_start  ------------------------------------------------------------//

  size_type root_directory_start(const string_type& p, size_type size)
  // return npos if no root_directory found
  {
    // case "/"
    if (size == 1
      && fs::detail::is_directory_separator(p[0])) return 0;

    // case "//"
    if (size == 2
      && fs::detail::is_directory_separator(p[0])
      && fs::detail::is_directory_separator(p[1])) return 0;

#   ifdef BOOST_WINDOWS_API
    // case "c:/"
    if (size > 2
      && p[1] == colon
      && fs::detail::is_directory_separator(p[2])) return 2;
#   endif

#   ifdef BOOST_WINDOWS_API
    // case "\\?\"
    if (size > 4
      && fs::detail::is_directory_separator(p[0])
      && fs::detail::is_directory_separator(p[1])
      && p[2] == questionmark
      && fs::detail::is_directory_separator(p[3]))
    {
      string_type::size_type pos(p.find_first_of(separators, 4));
        return pos < size ? pos : string_type::npos;
    }
#   endif

    // case "//name {/}"
    if (size > 3
      && fs::detail::is_directory_separator(p[0])
      && fs::detail::is_directory_separator(p[1])
      && !fs::detail::is_directory_separator(p[2]))
    {
      if (p[2] == path::dot && p[3] == path::dot
        && (size == 4 || fs::detail::is_directory_separator(p[5])))
        return 0;

      string_type::size_type pos(p.find_first_of(separators, 2));
      return pos < size ? pos : string_type::npos;
    }
    
    // case "/" followed by anything else
    if (size > 0 && fs::detail::is_directory_separator(p[0])) return 0;

    return string_type::npos;
  }

}  // unnamed namespace


namespace boost
{
namespace filesystem
{
  namespace detail
  {
    BOOST_FILESYSTEM_DECL
      int lex_compare(path::iterator first1, path::iterator last1,
        path::iterator first2, path::iterator last2)
    {
      for (; first1 != last1 && first2 != last2;)
      {
        if (first1->native() < first2->native()) return -1;
        if (first2->native() < first1->native()) return 1;
        BOOST_ASSERT(first2->native() == first1->native());
        ++first1;
        ++first2;
      }
      if (first1 == last1 && first2 == last2)
        return 0;
      return first1 == last1 ? -1 : 1;
    }

    BOOST_FILESYSTEM_DECL
    const path&  dot_path()
    {
#   ifdef BOOST_WINDOWS_API
      static const fs::path dot_pth(L".");
#   else
      static const fs::path dot_pth(".");
#   endif
      return dot_pth;
    }

    BOOST_FILESYSTEM_DECL
    const path&  dot_dot_path()
    {
#   ifdef BOOST_WINDOWS_API
      static const fs::path dot_dot(L"..");
#   else
      static const fs::path dot_dot("..");
#   endif
      return dot_dot;
    }
  }

//--------------------------------------------------------------------------------------//
//                                                                                      //
//                        class path::iterator implementation                           //
//                                                                                      //
//  note 1:                                                                             //       //
//  [fs.path.itr] ¶ 4 specifies traversal in the generic format.                        //
//  [fs.path.generic.obs] ¶ 1 specifies "A single slash (’/’) character is used as the  //
//  directory-separator."                                                               //
//                                                                                      //
//--------------------------------------------------------------------------------------//

  path::iterator path::begin() const
  {
    iterator itr;
    itr.m_path_ptr = this;
    itr.m_pos = 0;

    size_type element_sz{ 0 };

    // empty or has root-name
    if (m_pathname.empty() || (element_sz = m_root_name_size()) != 0)
    {
      itr.m_element = m_pathname.substr(0, element_sz);
    }

    // has root-directory
    else if ((element_sz = m_root_directory_size(0)) != 0)
    {
       itr.m_element = separator_string;  // see note 1 above
    }

    // only possibility left is a filename
    else
    {
      for (; element_sz != m_pathname.size()
               && !fs::detail::is_directory_separator(m_pathname[element_sz]);
             ++element_sz) {}
      itr.m_element = m_pathname.substr(0, element_sz);
    }
    return itr;
  }

  path::iterator path::end() const
  {
    iterator itr;
    itr.m_path_ptr = this;
    itr.m_pos = m_pathname.size();
    return itr;
  }

  void path::m_path_iterator_increment(path::iterator& it)
  {
    BOOST_ASSERT_MSG(it.m_pos < it.m_path_ptr->m_pathname.size(),
      "path::basic_iterator increment past end()");

    bool was_root_name = (it.m_pos == 0 && it.m_element.m_root_name_size() != 0);

    // increment to position past current element; if current element is "" (ie trailing
    // "/") this will cause it.m_pos to represent the end iterator
    it.m_pos += (it.m_element.m_pathname.size() ? it.m_element.m_pathname.size() : 1);

    // if the end is reached, we are done
    if (it.m_pos == it.m_path_ptr->m_pathname.size())
    {
      it.m_element.clear();  // aids debugging and may release unneeded memory
      return;
    }

    // directory-separator
    if (detail::is_directory_separator(it.m_path_ptr->m_pathname[it.m_pos]))
    {
      // detect root-directory following root-name
      if (was_root_name)
      {
        it.m_element = separator_string;  // see note 1 above
        return;
      }

      // skip directory-separators until it.m_pos points to start of next element
      while (it.m_pos != it.m_path_ptr->m_pathname.size()
        && detail::is_directory_separator(it.m_path_ptr->m_pathname[it.m_pos]))
        { ++it.m_pos; }

      // detect trailing directory-, and treat it as path(), per [fs.path.itr] ¶ 4.4
      if (it.m_pos == it.m_path_ptr->m_pathname.size()
        && !is_root_separator(it.m_path_ptr->m_pathname, it.m_pos-1)) 
      {
        --it.m_pos;
        it.m_element = path();
        return;
      }
    }

    // filename
    size_type end_pos(it.m_path_ptr->m_pathname.find_first_of(separators, it.m_pos));
    if (end_pos == string_type::npos)
      end_pos = it.m_path_ptr->m_pathname.size();
    it.m_element = it.m_path_ptr->m_pathname.substr(it.m_pos, end_pos - it.m_pos);
  }

  void path::m_path_iterator_decrement(path::iterator & it)
  {
    BOOST_ASSERT_MSG(it.m_pos, "path::iterator decrement past begin()");

    size_type end_pos(it.m_pos);

    // if at end and there was a trailing '/', return path()
    if (it.m_pos == it.m_path_ptr->m_pathname.size()
      && it.m_path_ptr->m_pathname.size() > 1
      && detail::is_directory_separator(it.m_path_ptr->m_pathname[it.m_pos-1])
      && !is_root_separator(it.m_path_ptr->m_pathname, it.m_pos-1) 
       )
    {
      --it.m_pos;
      it.m_element = path();
      return;
    }

    size_type root_dir_pos(root_directory_start(it.m_path_ptr->m_pathname, end_pos));

    // skip separators unless root directory
    for (
      ; 
      end_pos > 0
      && (end_pos-1) != root_dir_pos
      && detail::is_directory_separator(it.m_path_ptr->m_pathname[end_pos-1])
      ;
      --end_pos) {}

    it.m_pos = filename_pos(it.m_path_ptr->m_pathname, end_pos);
    it.m_element = it.m_path_ptr->m_pathname.substr(it.m_pos, end_pos - it.m_pos);
    if (it.m_element.m_pathname == preferred_separator_string) // needed for Windows, harmless on POSIX 
      it.m_element.m_pathname = separator_string;    // generic format; see docs 
  }

}  // namespace filesystem
}  // namespace boost

namespace
{

  //------------------------------------------------------------------------------------//
  //                                locale helpers                                      //
  //------------------------------------------------------------------------------------//

  //  Prior versions of these locale and codecvt implementations tried to take advantage
  //  of static initialization where possible, kept a local copy of the current codecvt
  //  facet (to avoid codecvt() having to call use_facet()), and was not multi-threading
  //  safe (again for efficiency).
  //
  //  This was error prone, and required different implementation techniques depending
  //  on the compiler and also whether static or dynamic linking was used. Furthermore,
  //  users could not easily provide their multi-threading safe wrappers because the
  //  path interface requires the implementation itself to call codecvt() to obtain the
  //  default facet, and the initialization of the static within path_locale() could race.
  //
  //  The code below is portable to all platforms, is much simpler, and hopefully will be 
  //  much more robust. Timing tests (on Windows, using a Visual C++ release build)
  //  indicated the current code is roughly 9% slower than the previous code, and that
  //  seems a small price to pay for better code that is easier to use. 

  std::locale default_locale()
  {
# if defined(BOOST_WINDOWS_API)
    std::locale global_loc = std::locale();
    return std::locale(global_loc, new windows_file_codecvt);
# elif defined(macintosh) || defined(__APPLE__) || defined(__APPLE_CC__) \
  || defined(__FreeBSD__) || defined(__OpenBSD__) || defined(__HAIKU__)
    // "All BSD system functions expect their string parameters to be in UTF-8 encoding
    // and nothing else." See
    // http://developer.apple.com/mac/library/documentation/MacOSX/Conceptual/BPInternational/Articles/FileEncodings.html
    //
    // "The kernel will reject any filename that is not a valid UTF-8 string, and it will
    // even be normalized (to Unicode NFD) before stored on disk, at least when using HFS.
    // The right way to deal with it would be to always convert the filename to UTF-8
    // before trying to open/create a file." See
    // http://lists.apple.com/archives/unix-porting/2007/Sep/msg00023.html
    //
    // "How a file name looks at the API level depends on the API. Current Carbon APIs
    // handle file names as an array of UTF-16 characters; POSIX ones handle them as an
    // array of UTF-8, which is why UTF-8 works well in Terminal. How it's stored on disk
    // depends on the disk format; HFS+ uses UTF-16, but that's not important in most
    // cases." See
    // http://lists.apple.com/archives/applescript-users/2002/Sep/msg00319.html
    //
    // Many thanks to Peter Dimov for digging out the above references!

    std::locale global_loc = std::locale();
    return std::locale(global_loc, new boost::filesystem::detail::utf8_codecvt_facet);
# else  // Other POSIX
    // ISO C calls std::locale("") "the locale-specific native environment", and this
    // locale is the default for many POSIX-based operating systems such as Linux.
    return std::locale("");
# endif
  }

  std::locale& path_locale()
  // std::locale("") construction, needed on non-Apple POSIX systems, can throw
  // (if environmental variables LC_MESSAGES or LANG are wrong, for example), so
  // path_locale() provides lazy initialization via a local static to ensure that any 
  // exceptions occur after main() starts and so can be caught. Furthermore,
  // path_locale() is only called if path::codecvt() or path::imbue() are themselves
  // actually called, ensuring that an exception will only be thrown if std::locale("")
  // is really needed.
  {
    // [locale] paragraph 6: Once a facet reference is obtained from a locale object by
    // calling use_facet<>, that reference remains usable, and the results from member
    // functions of it may be cached and re-used, as long as some locale object refers
    // to that facet.
    static std::locale loc(default_locale());
#ifdef BOOST_FILESYSTEM_DEBUG
    std::cout << "***** path_locale() called" << std::endl;
#endif
    return loc;
  }
}  // unnamed namespace

//--------------------------------------------------------------------------------------//
//              path::codecvt() and path::imbue() implementation                        //
//--------------------------------------------------------------------------------------//

namespace boost
{
namespace filesystem
{
  // See comments above

  const path::codecvt_type& path::codecvt()
  {
#ifdef BOOST_FILESYSTEM_DEBUG
    std::cout << "***** path::codecvt() called" << std::endl;
#endif
    BOOST_ASSERT_MSG(&path_locale(), "boost::filesystem::path locale initialization error");

    return std::use_facet<std::codecvt<wchar_t, char, std::mbstate_t> >(path_locale());
  }

  std::locale path::imbue(const std::locale& loc)
  {
#ifdef BOOST_FILESYSTEM_DEBUG
    std::cout << "***** path::imbue() called" << std::endl;
#endif
    std::locale temp(path_locale());
    path_locale() = loc;
    return temp;
  }

}  // namespace filesystem
}  // namespace boost
