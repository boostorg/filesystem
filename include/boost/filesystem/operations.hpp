//  boost/filesystem/operations.hpp  ---------------------------------------------------//

//  Copyright Beman Dawes 2002-2009
//  Copyright Jan Langer 2002
//  Copyright Dietmar Kuehl 2001
//  Copyright Vladimir Prus 2002

//  Distributed under the Boost Software License, Version 1.0.
//  See http://www.boost.org/LICENSE_1_0.txt

//  Library home page: http://www.boost.org/libs/filesystem

//--------------------------------------------------------------------------------------//

#ifndef BOOST_FILESYSTEM3_OPERATIONS_HPP
#define BOOST_FILESYSTEM3_OPERATIONS_HPP

#include <boost/config.hpp>

# if defined( BOOST_NO_STD_WSTRING )
#   error Configuration not supported: Boost.Filesystem V3 and later requires std::wstring support
# endif

#include <boost/filesystem/config.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/filesystem/exception.hpp>
#include <boost/filesystem/file_status.hpp>

#include <boost/core/scoped_enum.hpp>
#include <boost/detail/bitmask.hpp>
#include <boost/system/error_code.hpp>
#include <boost/smart_ptr/intrusive_ptr.hpp>
#include <boost/smart_ptr/intrusive_ref_counter.hpp>
#include <boost/type_traits/is_same.hpp>
#include <boost/cstdint.hpp>
#include <boost/assert.hpp>
#include <string>
#include <utility> // std::move
#include <ctime>
#include <vector>
#include <stack>
#include <new> // std::nothrow, std::bad_alloc

#ifdef BOOST_WINDOWS_API
#  include <fstream>
#endif

#include <boost/config/abi_prefix.hpp> // must be the last #include

//--------------------------------------------------------------------------------------//

namespace boost {
namespace filesystem {

  struct space_info
  {
    // all values are byte counts
    boost::uintmax_t capacity;
    boost::uintmax_t free;      // <= capacity
    boost::uintmax_t available; // <= free
  };

  BOOST_SCOPED_ENUM_DECLARE_BEGIN(copy_option)
    {none=0, fail_if_exists = none, overwrite_if_exists}
  BOOST_SCOPED_ENUM_DECLARE_END(copy_option)

//--------------------------------------------------------------------------------------//
//                             implementation details                                   //
//--------------------------------------------------------------------------------------//

  namespace detail
  {
    //  We cannot pass a BOOST_SCOPED_ENUM to a compled function because it will result
    //  in an undefined reference if the library is compled with -std=c++0x but the use
    //  is compiled in C++03 mode, or vice versa. See tickets 6124, 6779, 10038.
    enum copy_option {none=0, fail_if_exists = none, overwrite_if_exists};

    BOOST_FILESYSTEM_DECL
    file_status status(const path&p, system::error_code* ec=0);
    BOOST_FILESYSTEM_DECL
    file_status symlink_status(const path& p, system::error_code* ec=0);
    BOOST_FILESYSTEM_DECL
    bool is_empty(const path& p, system::error_code* ec=0);
    BOOST_FILESYSTEM_DECL
    path initial_path(system::error_code* ec=0);
    BOOST_FILESYSTEM_DECL
    path canonical(const path& p, const path& base, system::error_code* ec=0);
    BOOST_FILESYSTEM_DECL
    void copy(const path& from, const path& to, system::error_code* ec=0);
    BOOST_FILESYSTEM_DECL
    void copy_directory(const path& from, const path& to, system::error_code* ec=0);
    BOOST_FILESYSTEM_DECL
    void copy_file(const path& from, const path& to,  // See ticket #2925
                    detail::copy_option option, system::error_code* ec=0);
    BOOST_FILESYSTEM_DECL
    void copy_symlink(const path& existing_symlink, const path& new_symlink, system::error_code* ec=0);
    BOOST_FILESYSTEM_DECL
    bool create_directories(const path& p, system::error_code* ec=0);
    BOOST_FILESYSTEM_DECL
    bool create_directory(const path& p, system::error_code* ec=0);
    BOOST_FILESYSTEM_DECL
    void create_directory_symlink(const path& to, const path& from,
                                  system::error_code* ec=0);
    BOOST_FILESYSTEM_DECL
    void create_hard_link(const path& to, const path& from, system::error_code* ec=0);
    BOOST_FILESYSTEM_DECL
    void create_symlink(const path& to, const path& from, system::error_code* ec=0);
    BOOST_FILESYSTEM_DECL
    path current_path(system::error_code* ec=0);
    BOOST_FILESYSTEM_DECL
    void current_path(const path& p, system::error_code* ec=0);
    BOOST_FILESYSTEM_DECL
    bool equivalent(const path& p1, const path& p2, system::error_code* ec=0);
    BOOST_FILESYSTEM_DECL
    boost::uintmax_t file_size(const path& p, system::error_code* ec=0);
    BOOST_FILESYSTEM_DECL
    boost::uintmax_t hard_link_count(const path& p, system::error_code* ec=0);
    BOOST_FILESYSTEM_DECL
    std::time_t last_write_time(const path& p, system::error_code* ec=0);
    BOOST_FILESYSTEM_DECL
    void last_write_time(const path& p, const std::time_t new_time,
                         system::error_code* ec=0);
    BOOST_FILESYSTEM_DECL
    void permissions(const path& p, perms prms, system::error_code* ec=0);
    BOOST_FILESYSTEM_DECL
    path read_symlink(const path& p, system::error_code* ec=0);
    BOOST_FILESYSTEM_DECL
    path relative(const path& p, const path& base, system::error_code* ec = 0);
    BOOST_FILESYSTEM_DECL
    bool remove(const path& p, system::error_code* ec=0);
    BOOST_FILESYSTEM_DECL
    boost::uintmax_t remove_all(const path& p, system::error_code* ec=0);
    BOOST_FILESYSTEM_DECL
    void rename(const path& old_p, const path& new_p, system::error_code* ec=0);
    BOOST_FILESYSTEM_DECL
    void resize_file(const path& p, uintmax_t size, system::error_code* ec=0);
    BOOST_FILESYSTEM_DECL
    space_info space(const path& p, system::error_code* ec=0);
    BOOST_FILESYSTEM_DECL
    path system_complete(const path& p, system::error_code* ec=0);
    BOOST_FILESYSTEM_DECL
    path temp_directory_path(system::error_code* ec=0);
    BOOST_FILESYSTEM_DECL
    path unique_path(const path& p, system::error_code* ec=0);
    BOOST_FILESYSTEM_DECL
    path weakly_canonical(const path& p, system::error_code* ec = 0);
  }  // namespace detail

//--------------------------------------------------------------------------------------//
//                                                                                      //
//                             status query functions                                   //
//                                                                                      //
//--------------------------------------------------------------------------------------//

  inline
  file_status status(const path& p)    {return detail::status(p);}
  inline
  file_status status(const path& p, system::error_code& ec)
                                       {return detail::status(p, &ec);}
  inline
  file_status symlink_status(const path& p) {return detail::symlink_status(p);}
  inline
  file_status symlink_status(const path& p, system::error_code& ec)
                                       {return detail::symlink_status(p, &ec);}
  inline
  bool exists(const path& p)           {return exists(detail::status(p));}
  inline
  bool exists(const path& p, system::error_code& ec)
                                       {return exists(detail::status(p, &ec));}
  inline
  bool is_directory(const path& p)     {return is_directory(detail::status(p));}
  inline
  bool is_directory(const path& p, system::error_code& ec)
                                       {return is_directory(detail::status(p, &ec));}
  inline
  bool is_regular_file(const path& p)  {return is_regular_file(detail::status(p));}
  inline
  bool is_regular_file(const path& p, system::error_code& ec)
                                       {return is_regular_file(detail::status(p, &ec));}
  inline
  bool is_other(const path& p)         {return is_other(detail::status(p));}
  inline
  bool is_other(const path& p, system::error_code& ec)
                                       {return is_other(detail::status(p, &ec));}
  inline
  bool is_symlink(const path& p)       {return is_symlink(detail::symlink_status(p));}
  inline
  bool is_symlink(const path& p, system::error_code& ec)
                                       {return is_symlink(detail::symlink_status(p, &ec));}
# ifndef BOOST_FILESYSTEM_NO_DEPRECATED
  inline
  bool is_regular(const path& p)       {return is_regular(detail::status(p));}
  inline
  bool is_regular(const path& p, system::error_code& ec)
                                       {return is_regular(detail::status(p, &ec));}
# endif

  inline
  bool is_empty(const path& p)         {return detail::is_empty(p);}
  inline
  bool is_empty(const path& p, system::error_code& ec)
                                       {return detail::is_empty(p, &ec);}

//--------------------------------------------------------------------------------------//
//                                                                                      //
//                             operational functions                                    //
//                  in alphabetical order, unless otherwise noted                       //
//                                                                                      //
//--------------------------------------------------------------------------------------//

  //  forward declarations
  path current_path();  // fwd declaration
  path initial_path();

  BOOST_FILESYSTEM_DECL
  path absolute(const path& p, const path& base=current_path());
  //  If base.is_absolute(), throws nothing. Thus no need for ec argument

  inline
  path canonical(const path& p, const path& base=current_path())
                                       {return detail::canonical(p, base);}
  inline
  path canonical(const path& p, system::error_code& ec)
                                       {return detail::canonical(p, current_path(), &ec);}
  inline
  path canonical(const path& p, const path& base, system::error_code& ec)
                                       {return detail::canonical(p, base, &ec);}

# ifndef BOOST_FILESYSTEM_NO_DEPRECATED
  inline
  path complete(const path& p)
  {
    return absolute(p, initial_path());
  }

  inline
  path complete(const path& p, const path& base)
  {
    return absolute(p, base);
  }
# endif

  inline
  void copy(const path& from, const path& to) {detail::copy(from, to);}

  inline
  void copy(const path& from, const path& to, system::error_code& ec) BOOST_NOEXCEPT
                                       {detail::copy(from, to, &ec);}
  inline
  void copy_directory(const path& from, const path& to)
                                       {detail::copy_directory(from, to);}
  inline
  void copy_directory(const path& from, const path& to, system::error_code& ec) BOOST_NOEXCEPT
                                       {detail::copy_directory(from, to, &ec);}
  inline
  void copy_file(const path& from, const path& to,   // See ticket #2925
                 BOOST_SCOPED_ENUM_NATIVE(copy_option) option)
  {
    detail::copy_file(from, to, static_cast<detail::copy_option>(option));
  }
  inline
  void copy_file(const path& from, const path& to)
  {
    detail::copy_file(from, to, detail::fail_if_exists);
  }
  inline
  void copy_file(const path& from, const path& to,   // See ticket #2925
                 BOOST_SCOPED_ENUM_NATIVE(copy_option) option, system::error_code& ec) BOOST_NOEXCEPT
  {
    detail::copy_file(from, to, static_cast<detail::copy_option>(option), &ec);
  }
  inline
  void copy_file(const path& from, const path& to, system::error_code& ec) BOOST_NOEXCEPT
  {
    detail::copy_file(from, to, detail::fail_if_exists, &ec);
  }
  inline
  void copy_symlink(const path& existing_symlink,
                    const path& new_symlink) {detail::copy_symlink(existing_symlink, new_symlink);}

  inline
  void copy_symlink(const path& existing_symlink, const path& new_symlink,
                    system::error_code& ec) BOOST_NOEXCEPT
                                       {detail::copy_symlink(existing_symlink, new_symlink, &ec);}
  inline
  bool create_directories(const path& p) {return detail::create_directories(p);}

  inline
  bool create_directories(const path& p, system::error_code& ec) BOOST_NOEXCEPT
                                       {return detail::create_directories(p, &ec);}
  inline
  bool create_directory(const path& p) {return detail::create_directory(p);}

  inline
  bool create_directory(const path& p, system::error_code& ec) BOOST_NOEXCEPT
                                       {return detail::create_directory(p, &ec);}
  inline
  void create_directory_symlink(const path& to, const path& from)
                                       {detail::create_directory_symlink(to, from);}
  inline
  void create_directory_symlink(const path& to, const path& from, system::error_code& ec) BOOST_NOEXCEPT
                                       {detail::create_directory_symlink(to, from, &ec);}
  inline
  void create_hard_link(const path& to, const path& new_hard_link) {detail::create_hard_link(to, new_hard_link);}

  inline
  void create_hard_link(const path& to, const path& new_hard_link, system::error_code& ec) BOOST_NOEXCEPT
                                       {detail::create_hard_link(to, new_hard_link, &ec);}
  inline
  void create_symlink(const path& to, const path& new_symlink) {detail::create_symlink(to, new_symlink);}

  inline
  void create_symlink(const path& to, const path& new_symlink, system::error_code& ec) BOOST_NOEXCEPT
                                       {detail::create_symlink(to, new_symlink, &ec);}
  inline
  path current_path()                  {return detail::current_path();}

  inline
  path current_path(system::error_code& ec) BOOST_NOEXCEPT {return detail::current_path(&ec);}

  inline
  void current_path(const path& p)     {detail::current_path(p);}

  inline
  void current_path(const path& p, system::error_code& ec) BOOST_NOEXCEPT {detail::current_path(p, &ec);}

  inline
  bool equivalent(const path& p1, const path& p2) {return detail::equivalent(p1, p2);}

  inline
  bool equivalent(const path& p1, const path& p2, system::error_code& ec) BOOST_NOEXCEPT
                                       {return detail::equivalent(p1, p2, &ec);}
  inline
  boost::uintmax_t file_size(const path& p) {return detail::file_size(p);}

  inline
  boost::uintmax_t file_size(const path& p, system::error_code& ec) BOOST_NOEXCEPT
                                       {return detail::file_size(p, &ec);}
  inline
  boost::uintmax_t hard_link_count(const path& p) {return detail::hard_link_count(p);}

  inline
  boost::uintmax_t hard_link_count(const path& p, system::error_code& ec) BOOST_NOEXCEPT
                                       {return detail::hard_link_count(p, &ec);}
  inline
  path initial_path()                  {return detail::initial_path();}

  inline
  path initial_path(system::error_code& ec) {return detail::initial_path(&ec);}

  template <class Path>
  path initial_path() {return initial_path();}
  template <class Path>
  path initial_path(system::error_code& ec) {return detail::initial_path(&ec);}

  inline
  std::time_t last_write_time(const path& p) {return detail::last_write_time(p);}

  inline
  std::time_t last_write_time(const path& p, system::error_code& ec) BOOST_NOEXCEPT
                                       {return detail::last_write_time(p, &ec);}
  inline
  void last_write_time(const path& p, const std::time_t new_time)
                                       {detail::last_write_time(p, new_time);}
  inline
  void last_write_time(const path& p, const std::time_t new_time,
                       system::error_code& ec) BOOST_NOEXCEPT
                                       {detail::last_write_time(p, new_time, &ec);}
  inline
  void permissions(const path& p, perms prms)
                                       {detail::permissions(p, prms);}
  inline
  void permissions(const path& p, perms prms, system::error_code& ec) BOOST_NOEXCEPT
                                       {detail::permissions(p, prms, &ec);}

  inline
  path read_symlink(const path& p)     {return detail::read_symlink(p);}

  inline
  path read_symlink(const path& p, system::error_code& ec)
                                       {return detail::read_symlink(p, &ec);}
  inline
    // For standardization, if the committee doesn't like "remove", consider "eliminate"
  bool remove(const path& p)           {return detail::remove(p);}

  inline
  bool remove(const path& p, system::error_code& ec) BOOST_NOEXCEPT
                                       {return detail::remove(p, &ec);}

  inline
  boost::uintmax_t remove_all(const path& p) {return detail::remove_all(p);}

  inline
  boost::uintmax_t remove_all(const path& p, system::error_code& ec) BOOST_NOEXCEPT
                                       {return detail::remove_all(p, &ec);}
  inline
  void rename(const path& old_p, const path& new_p) {detail::rename(old_p, new_p);}

  inline
  void rename(const path& old_p, const path& new_p, system::error_code& ec) BOOST_NOEXCEPT
                                       {detail::rename(old_p, new_p, &ec);}
  inline  // name suggested by Scott McMurray
  void resize_file(const path& p, uintmax_t size) {detail::resize_file(p, size);}

  inline
  void resize_file(const path& p, uintmax_t size, system::error_code& ec) BOOST_NOEXCEPT
                                       {detail::resize_file(p, size, &ec);}
  inline
  path relative(const path& p, const path& base=current_path())
                                       {return detail::relative(p, base);}
  inline
  path relative(const path& p, system::error_code& ec)
                                       {return detail::relative(p, current_path(), &ec);}
  inline
  path relative(const path& p, const path& base, system::error_code& ec)
                                       {return detail::relative(p, base, &ec);}
  inline
  space_info space(const path& p)      {return detail::space(p);}

  inline
  space_info space(const path& p, system::error_code& ec) BOOST_NOEXCEPT
                                       {return detail::space(p, &ec);}

# ifndef BOOST_FILESYSTEM_NO_DEPRECATED
  inline bool symbolic_link_exists(const path& p)
                                       { return is_symlink(filesystem::symlink_status(p)); }
# endif

  inline
  path system_complete(const path& p)  {return detail::system_complete(p);}

  inline
  path system_complete(const path& p, system::error_code& ec)
                                       {return detail::system_complete(p, &ec);}
  inline
  path temp_directory_path()           {return detail::temp_directory_path();}

  inline
  path temp_directory_path(system::error_code& ec)
                                       {return detail::temp_directory_path(&ec);}
  inline
  path unique_path(const path& p="%%%%-%%%%-%%%%-%%%%")
                                       {return detail::unique_path(p);}
  inline
  path unique_path(const path& p, system::error_code& ec)
                                       {return detail::unique_path(p, &ec);}
  inline
  path weakly_canonical(const path& p)   {return detail::weakly_canonical(p);}

  inline
  path weakly_canonical(const path& p, system::error_code& ec)
                                       {return detail::weakly_canonical(p, &ec);}

//--------------------------------------------------------------------------------------//
//                                                                                      //
//                                 directory_entry                                      //
//                                                                                      //
//--------------------------------------------------------------------------------------//

//  GCC has a problem with a member function named path within a namespace or
//  sub-namespace that also has a class named path. The workaround is to always
//  fully qualify the name path when it refers to the class name.

class directory_entry
{
public:
  typedef boost::filesystem::path::value_type value_type;   // enables class path ctor taking directory_entry

  directory_entry() BOOST_NOEXCEPT {}
  explicit directory_entry(const boost::filesystem::path& p) :
    m_path(p), m_status(file_status()), m_symlink_status(file_status())
  {
  }
  directory_entry(const boost::filesystem::path& p,
    file_status st, file_status symlink_st = file_status()) :
    m_path(p), m_status(st), m_symlink_status(symlink_st)
  {
  }

  directory_entry(const directory_entry& rhs) :
    m_path(rhs.m_path), m_status(rhs.m_status), m_symlink_status(rhs.m_symlink_status)
  {
  }

  directory_entry& operator=(const directory_entry& rhs)
  {
    m_path = rhs.m_path;
    m_status = rhs.m_status;
    m_symlink_status = rhs.m_symlink_status;
    return *this;
  }

  //  As of October 2015 the interaction between noexcept and =default is so troublesome
  //  for VC++, GCC, and probably other compilers, that =default is not used with noexcept
  //  functions. GCC is not even consistent for the same release on different platforms.

#if !defined(BOOST_NO_CXX11_RVALUE_REFERENCES)
  directory_entry(directory_entry&& rhs) BOOST_NOEXCEPT :
    m_path(std::move(rhs.m_path)), m_status(std::move(rhs.m_status)), m_symlink_status(std::move(rhs.m_symlink_status))
  {
  }
  directory_entry& operator=(directory_entry&& rhs) BOOST_NOEXCEPT
  {
    m_path = std::move(rhs.m_path);
    m_status = std::move(rhs.m_status);
    m_symlink_status = std::move(rhs.m_symlink_status);
    return *this;
  }
#endif

  void assign(const boost::filesystem::path& p,
    file_status st = file_status(), file_status symlink_st = file_status())
  {
    m_path = p;
    m_status = st;
    m_symlink_status = symlink_st;
  }

  void replace_filename(const boost::filesystem::path& p,
    file_status st = file_status(), file_status symlink_st = file_status())
  {
    m_path.remove_filename();
    m_path /= p;
    m_status = st;
    m_symlink_status = symlink_st;
  }

# ifndef BOOST_FILESYSTEM_NO_DEPRECATED
  void replace_leaf(const boost::filesystem::path& p,
    file_status st, file_status symlink_st)
      { replace_filename(p, st, symlink_st); }
# endif

  const boost::filesystem::path&  path() const BOOST_NOEXCEPT {return m_path;}
  operator const boost::filesystem::path&() const BOOST_NOEXCEPT
                                                              {return m_path;}
  file_status   status() const                                {return m_get_status();}
  file_status   status(system::error_code& ec) const BOOST_NOEXCEPT
                                                              {return m_get_status(&ec); }
  file_status   symlink_status() const                        {return m_get_symlink_status();}
  file_status   symlink_status(system::error_code& ec) const BOOST_NOEXCEPT
                                                              {return m_get_symlink_status(&ec); }

  bool operator==(const directory_entry& rhs) const BOOST_NOEXCEPT {return m_path == rhs.m_path; }
  bool operator!=(const directory_entry& rhs) const BOOST_NOEXCEPT {return m_path != rhs.m_path;}
  bool operator< (const directory_entry& rhs) const BOOST_NOEXCEPT {return m_path < rhs.m_path;}
  bool operator<=(const directory_entry& rhs) const BOOST_NOEXCEPT {return m_path <= rhs.m_path;}
  bool operator> (const directory_entry& rhs) const BOOST_NOEXCEPT {return m_path > rhs.m_path;}
  bool operator>=(const directory_entry& rhs) const BOOST_NOEXCEPT {return m_path >= rhs.m_path;}

private:
  BOOST_FILESYSTEM_DECL file_status m_get_status(system::error_code* ec=0) const;
  BOOST_FILESYSTEM_DECL file_status m_get_symlink_status(system::error_code* ec=0) const;

private:
  boost::filesystem::path   m_path;
  mutable file_status       m_status;           // stat()-like
  mutable file_status       m_symlink_status;   // lstat()-like
}; // directory_entry


//--------------------------------------------------------------------------------------//
//                                                                                      //
//                            directory_entry overloads                                 //
//                                                                                      //
//--------------------------------------------------------------------------------------//

//  Without these functions, calling (for example) 'is_directory' with a 'directory_entry' results in:
//  - a conversion to 'path' using 'operator const boost::filesystem::path&()',
//  - then a call to 'is_directory(const path& p)' which recomputes the status with 'detail::status(p)'.
//  
//  These functions avoid a costly recomputation of the status if one calls 'is_directory(e)' instead of 'is_directory(e.status)'

inline file_status status         (const directory_entry& e) BOOST_NOEXCEPT { return e.status(); }
inline bool        type_present   (const directory_entry& e) BOOST_NOEXCEPT { return filesystem::type_present(e.status()); }
inline bool        status_known   (const directory_entry& e) BOOST_NOEXCEPT { return filesystem::status_known(e.status()); }
inline bool        exists         (const directory_entry& e) BOOST_NOEXCEPT { return filesystem::exists(e.status()); }
inline bool        is_regular_file(const directory_entry& e) BOOST_NOEXCEPT { return filesystem::is_regular_file(e.status()); }
inline bool        is_directory   (const directory_entry& e) BOOST_NOEXCEPT { return filesystem::is_directory(e.status()); }
inline bool        is_symlink     (const directory_entry& e) BOOST_NOEXCEPT { return filesystem::is_symlink(e.status()); }
inline bool        is_other       (const directory_entry& e) BOOST_NOEXCEPT { return filesystem::is_other(e.status()); }
#ifndef BOOST_FILESYSTEM_NO_DEPRECATED
inline bool        is_regular     (const directory_entry& e) BOOST_NOEXCEPT { return filesystem::is_regular(e.status()); }
#endif

//--------------------------------------------------------------------------------------//
//                                                                                      //
//                            directory_iterator helpers                                //
//                                                                                      //
//--------------------------------------------------------------------------------------//

class directory_iterator;

namespace detail
{
  BOOST_FILESYSTEM_DECL
  system::error_code dir_itr_close(// never throws()
    void *& handle
#   if     defined(BOOST_POSIX_API)
    , void *& buffer
#   endif
  );

  struct dir_itr_imp :
    public boost::intrusive_ref_counter< dir_itr_imp >
  {
    directory_entry  dir_entry;
    void*            handle;

#   ifdef BOOST_POSIX_API
    void*            buffer;  // see dir_itr_increment implementation
#   endif

    dir_itr_imp() BOOST_NOEXCEPT : handle(0)
#   ifdef BOOST_POSIX_API
      , buffer(0)
#   endif
    {}

    ~dir_itr_imp() BOOST_NOEXCEPT
    {
      dir_itr_close(handle
#       if defined(BOOST_POSIX_API)
         , buffer
#       endif
      );
    }
  };

  // see path::iterator: comment below
  BOOST_FILESYSTEM_DECL void directory_iterator_construct(directory_iterator& it,
    const path& p, system::error_code* ec);
  BOOST_FILESYSTEM_DECL void directory_iterator_increment(directory_iterator& it,
    system::error_code* ec);

}  // namespace detail

//--------------------------------------------------------------------------------------//
//                                                                                      //
//                                directory_iterator                                    //
//                                                                                      //
//--------------------------------------------------------------------------------------//

  class directory_iterator :
    public boost::iterator_facade<
      directory_iterator,
      directory_entry,
      boost::single_pass_traversal_tag
    >
  {
  public:

    directory_iterator() BOOST_NOEXCEPT {}  // creates the "end" iterator

    // iterator_facade derived classes don't seem to like implementations in
    // separate translation unit dll's, so forward to detail functions
    explicit directory_iterator(const path& p) :
      m_imp(new detail::dir_itr_imp())
    {
      detail::directory_iterator_construct(*this, p, 0);
    }

    directory_iterator(const path& p, system::error_code& ec) BOOST_NOEXCEPT :
      m_imp(new (std::nothrow) detail::dir_itr_imp())
    {
      if (BOOST_UNLIKELY(!m_imp))
      {
        ec = make_error_code(system::errc::not_enough_memory);
        return;
      }

      detail::directory_iterator_construct(*this, p, &ec);
    }

    BOOST_DEFAULTED_FUNCTION(directory_iterator(directory_iterator const& that), : m_imp(that.m_imp) {})
    BOOST_DEFAULTED_FUNCTION(directory_iterator& operator= (directory_iterator const& that), { m_imp = that.m_imp; return *this; })

#if !defined(BOOST_NO_CXX11_RVALUE_REFERENCES)
    directory_iterator(directory_iterator&& that) BOOST_NOEXCEPT :
      m_imp(std::move(that.m_imp))
    {
    }

    directory_iterator& operator= (directory_iterator&& that) BOOST_NOEXCEPT
    {
      m_imp = std::move(that.m_imp);
      return *this;
    }
#endif // !defined(BOOST_NO_CXX11_RVALUE_REFERENCES)

    directory_iterator& increment(system::error_code& ec) BOOST_NOEXCEPT
    {
      detail::directory_iterator_increment(*this, &ec);
      return *this;
    }

  private:
    friend struct detail::dir_itr_imp;
    friend BOOST_FILESYSTEM_DECL void detail::directory_iterator_construct(directory_iterator& it,
      const path& p, system::error_code* ec);
    friend BOOST_FILESYSTEM_DECL void detail::directory_iterator_increment(directory_iterator& it,
      system::error_code* ec);

    // intrusive_ptr provides the shallow-copy semantics required for single pass iterators
    // (i.e. InputIterators). The end iterator is indicated by !m_imp || !m_imp->handle
    boost::intrusive_ptr< detail::dir_itr_imp > m_imp;

    friend class boost::iterator_core_access;

    boost::iterator_facade<
      directory_iterator,
      directory_entry,
      boost::single_pass_traversal_tag
    >::reference dereference() const
    {
      BOOST_ASSERT_MSG(!!m_imp.get(), "attempt to dereference end directory iterator");
      return m_imp->dir_entry;
    }

    void increment() { detail::directory_iterator_increment(*this, 0); }

    bool equal(const directory_iterator& rhs) const
    {
      return m_imp == rhs.m_imp
        || (!m_imp && rhs.m_imp && !rhs.m_imp->handle)
        || (!rhs.m_imp && m_imp && !m_imp->handle);
    }
  };  // directory_iterator

  //  enable directory_iterator C++11 range-based for statement use  --------------------//

  //  begin() and end() are only used by a range-based for statement in the context of
  //  auto - thus the top-level const is stripped - so returning const is harmless and
  //  emphasizes begin() is just a pass through.
  inline
  const directory_iterator& begin(const directory_iterator& iter) BOOST_NOEXCEPT
    {return iter;}
  inline
  directory_iterator end(const directory_iterator&) BOOST_NOEXCEPT
    {return directory_iterator();}

  // enable C++14 generic accessors for range const iterators
  inline
  const directory_iterator& cbegin(const directory_iterator& iter) BOOST_NOEXCEPT
    {return iter;}
  inline
  directory_iterator cend(const directory_iterator&) BOOST_NOEXCEPT
    {return directory_iterator();}

  //  enable directory_iterator BOOST_FOREACH  -----------------------------------------//

  inline
  directory_iterator& range_begin(directory_iterator& iter) BOOST_NOEXCEPT
    {return iter;}
  inline
  directory_iterator range_begin(const directory_iterator& iter) BOOST_NOEXCEPT
    {return iter;}
  inline
  directory_iterator range_end(directory_iterator&) BOOST_NOEXCEPT
    {return directory_iterator();}
  inline
  directory_iterator range_end(const directory_iterator&) BOOST_NOEXCEPT
    {return directory_iterator();}
  }  // namespace filesystem

  //  namespace boost template specializations
  template<typename C, typename Enabler>
  struct range_mutable_iterator;

  template<>
  struct range_mutable_iterator<boost::filesystem::directory_iterator, void>
    { typedef boost::filesystem::directory_iterator type; };

  template<typename C, typename Enabler>
  struct range_const_iterator;

  template<>
  struct range_const_iterator<boost::filesystem::directory_iterator, void>
    { typedef boost::filesystem::directory_iterator type; };

namespace filesystem
{

//--------------------------------------------------------------------------------------//
//                                                                                      //
//                      recursive_directory_iterator helpers                            //
//                                                                                      //
//--------------------------------------------------------------------------------------//

  BOOST_SCOPED_ENUM_UT_DECLARE_BEGIN(symlink_option, unsigned int)
  {
    none,
    no_recurse = none,         // don't follow directory symlinks (default behavior)
    recurse,                   // follow directory symlinks
    _detail_no_push = recurse << 1 // internal use only
  }
  BOOST_SCOPED_ENUM_DECLARE_END(symlink_option)

  BOOST_BITMASK(BOOST_SCOPED_ENUM_NATIVE(symlink_option))

  namespace detail
  {
    struct recur_dir_itr_imp :
      public boost::intrusive_ref_counter< recur_dir_itr_imp >
    {
      typedef directory_iterator element_type;
      std::stack< element_type, std::vector< element_type > > m_stack;
      int m_level;
      // symlink_option values, declared as unsigned int for ABI compatibility
      unsigned int m_options;

      recur_dir_itr_imp() BOOST_NOEXCEPT : m_level(0), m_options(static_cast< unsigned int >(symlink_option::none)) {}
      explicit recur_dir_itr_imp(BOOST_SCOPED_ENUM_NATIVE(symlink_option) opt) BOOST_NOEXCEPT : m_level(0), m_options(static_cast< unsigned int >(opt)) {}

      // ec == 0 means throw on error
      //
      // Invariant: On return, the top of the iterator stack is the next valid (possibly
      // end) iterator, regardless of whether or not an error is reported, and regardless of
      // whether any error is reported by exception or error code. In other words, progress
      // is always made so a loop on the iterator will always eventually terminate
      // regardless of errors.
      BOOST_FILESYSTEM_DECL void increment(system::error_code* ec);

      // Returns: true if push occurs, otherwise false. Always returns false on error.
      BOOST_FILESYSTEM_DECL bool push_directory(system::error_code& ec) BOOST_NOEXCEPT;

      // ec == 0 means throw on error
      BOOST_FILESYSTEM_DECL void pop(system::error_code* ec);
    };

  } // namespace detail

//--------------------------------------------------------------------------------------//
//                                                                                      //
//                           recursive_directory_iterator                               //
//                                                                                      //
//--------------------------------------------------------------------------------------//

  class recursive_directory_iterator :
    public boost::iterator_facade<
      recursive_directory_iterator,
      directory_entry,
      boost::single_pass_traversal_tag
    >
  {
  public:

    recursive_directory_iterator() BOOST_NOEXCEPT {}  // creates the "end" iterator

    explicit recursive_directory_iterator(const path& dir_path) :  // throws if !exists()
      m_imp(new detail::recur_dir_itr_imp())
    {
      m_imp->m_stack.push(directory_iterator(dir_path));
      if (m_imp->m_stack.top() == directory_iterator())
        { m_imp.reset(); }
    }

    recursive_directory_iterator(const path& dir_path,
      BOOST_SCOPED_ENUM_NATIVE(symlink_option) opt) :  // throws if !exists()
      m_imp(new detail::recur_dir_itr_imp(opt))
    {
      m_imp->m_stack.push(directory_iterator(dir_path));
      if (m_imp->m_stack.top() == directory_iterator())
        { m_imp.reset (); }
    }

    recursive_directory_iterator(const path& dir_path,
      BOOST_SCOPED_ENUM_NATIVE(symlink_option) opt,
      system::error_code & ec) BOOST_NOEXCEPT :
      m_imp(new (std::nothrow) detail::recur_dir_itr_imp(opt))
    {
      if (BOOST_UNLIKELY(!m_imp))
      {
        ec = make_error_code(system::errc::not_enough_memory);
        return;
      }

      directory_iterator it(dir_path, ec);
      if (ec || it == directory_iterator())
      {
        m_imp.reset();
        return;
      }

      try
      {
#if !defined(BOOST_NO_CXX11_RVALUE_REFERENCES)
        m_imp->m_stack.push(std::move(it));
#else
        m_imp->m_stack.push(it);
#endif
      }
      catch (std::bad_alloc&)
      {
        ec = make_error_code(system::errc::not_enough_memory);
        m_imp.reset();
      }
    }

    recursive_directory_iterator(const path& dir_path,
      system::error_code & ec) BOOST_NOEXCEPT :
      m_imp(new (std::nothrow) detail::recur_dir_itr_imp())
    {
      if (BOOST_UNLIKELY(!m_imp))
      {
        ec = make_error_code(system::errc::not_enough_memory);
        return;
      }

      directory_iterator it(dir_path, ec);
      if (ec || it == directory_iterator())
      {
        m_imp.reset();
        return;
      }

      try
      {
#if !defined(BOOST_NO_CXX11_RVALUE_REFERENCES)
        m_imp->m_stack.push(std::move(it));
#else
        m_imp->m_stack.push(it);
#endif
      }
      catch (std::bad_alloc&)
      {
        ec = make_error_code(system::errc::not_enough_memory);
        m_imp.reset();
      }
    }

    BOOST_DEFAULTED_FUNCTION(recursive_directory_iterator(recursive_directory_iterator const& that), : m_imp(that.m_imp) {})
    BOOST_DEFAULTED_FUNCTION(recursive_directory_iterator& operator= (recursive_directory_iterator const& that), { m_imp = that.m_imp; return *this; })

#if !defined(BOOST_NO_CXX11_RVALUE_REFERENCES)
    recursive_directory_iterator(recursive_directory_iterator&& that) BOOST_NOEXCEPT :
      m_imp(std::move(that.m_imp))
    {
    }

    recursive_directory_iterator& operator= (recursive_directory_iterator&& that) BOOST_NOEXCEPT
    {
      m_imp = std::move(that.m_imp);
      return *this;
    }
#endif // !defined(BOOST_NO_CXX11_RVALUE_REFERENCES)

    recursive_directory_iterator& increment(system::error_code& ec) BOOST_NOEXCEPT
    {
      BOOST_ASSERT_MSG(!!m_imp.get(),
        "increment() on end recursive_directory_iterator");
      m_imp->increment(&ec);
      if (m_imp->m_stack.empty())
        m_imp.reset(); // done, so make end iterator
      return *this;
    }

    int depth() const BOOST_NOEXCEPT
    {
      BOOST_ASSERT_MSG(!!m_imp.get(),
        "depth() on end recursive_directory_iterator");
      return m_imp->m_level;
    }

    int level() const BOOST_NOEXCEPT { return depth(); }

    bool recursion_pending() const BOOST_NOEXCEPT
    {
      BOOST_ASSERT_MSG(!!m_imp.get(),
        "recursion_pending() on end recursive_directory_iterator");
      return (m_imp->m_options & static_cast< unsigned int >(symlink_option::_detail_no_push))
        == static_cast< unsigned int >(symlink_option::_detail_no_push);
    }

    bool no_push_pending() const BOOST_NOEXCEPT { return recursion_pending(); }

#   ifndef BOOST_FILESYSTEM_NO_DEPRECATED
    bool no_push_request() const BOOST_NOEXCEPT { return no_push_pending(); }
#   endif

    void pop()
    {
      BOOST_ASSERT_MSG(!!m_imp.get(),
        "pop() on end recursive_directory_iterator");
      m_imp->pop(0);
      if (m_imp->m_stack.empty())
        m_imp.reset(); // done, so make end iterator
    }

    void pop(system::error_code& ec) BOOST_NOEXCEPT
    {
      BOOST_ASSERT_MSG(!!m_imp.get(),
        "pop() on end recursive_directory_iterator");
      m_imp->pop(&ec);
      if (m_imp->m_stack.empty())
        m_imp.reset(); // done, so make end iterator
    }

    void disable_recursion_pending(bool value=true) BOOST_NOEXCEPT
    {
      BOOST_ASSERT_MSG(!!m_imp.get(),
        "disable_recursion_pending() on end recursive_directory_iterator");
      if (value)
        m_imp->m_options |= static_cast< unsigned int >(symlink_option::_detail_no_push);
      else
        m_imp->m_options &= ~static_cast< unsigned int >(symlink_option::_detail_no_push);
    }

    void no_push(bool value=true) BOOST_NOEXCEPT { disable_recursion_pending(value); }

    file_status status() const
    {
      BOOST_ASSERT_MSG(!!m_imp.get(),
        "status() on end recursive_directory_iterator");
      return m_imp->m_stack.top()->status();
    }

    file_status symlink_status() const
    {
      BOOST_ASSERT_MSG(!!m_imp.get(),
        "symlink_status() on end recursive_directory_iterator");
      return m_imp->m_stack.top()->symlink_status();
    }

  private:

    // intrusive_ptr provides the shallow-copy semantics required for single pass iterators
    // (i.e. InputIterators).
    // The end iterator is indicated by !m_imp || m_imp->m_stack.empty()
    boost::intrusive_ptr< detail::recur_dir_itr_imp >  m_imp;

    friend class boost::iterator_core_access;

    boost::iterator_facade<
      recursive_directory_iterator,
      directory_entry,
      boost::single_pass_traversal_tag >::reference
    dereference() const
    {
      BOOST_ASSERT_MSG(!!m_imp.get(),
        "dereference of end recursive_directory_iterator");
      return *m_imp->m_stack.top();
    }

    void increment()
    {
      BOOST_ASSERT_MSG(!!m_imp.get(),
        "increment of end recursive_directory_iterator");
      m_imp->increment(0);
      if (m_imp->m_stack.empty())
        m_imp.reset(); // done, so make end iterator
    }

    bool equal(const recursive_directory_iterator& rhs) const
    {
      return m_imp == rhs.m_imp
        || (!m_imp && rhs.m_imp && rhs.m_imp->m_stack.empty())
        || (!rhs.m_imp && m_imp && m_imp->m_stack.empty())        ;
    }
  };  // recursive directory iterator

  //  enable recursive directory iterator C++11 range-base for statement use  ----------//

  //  begin() and end() are only used by a range-based for statement in the context of
  //  auto - thus the top-level const is stripped - so returning const is harmless and
  //  emphasizes begin() is just a pass through.
  inline
  const recursive_directory_iterator&
    begin(const recursive_directory_iterator& iter) BOOST_NOEXCEPT
                                                  {return iter;}
  inline
  recursive_directory_iterator end(const recursive_directory_iterator&) BOOST_NOEXCEPT
                                                  {return recursive_directory_iterator();}

  // enable C++14 generic accessors for range const iterators
  inline
  const recursive_directory_iterator& cbegin(const recursive_directory_iterator& iter) BOOST_NOEXCEPT
    {return iter;}
  inline
  recursive_directory_iterator cend(const recursive_directory_iterator&) BOOST_NOEXCEPT
    {return recursive_directory_iterator();}

  //  enable recursive directory iterator BOOST_FOREACH  -------------------------------//

  inline
  recursive_directory_iterator&
    range_begin(recursive_directory_iterator& iter) BOOST_NOEXCEPT
                                                   {return iter;}
  inline
  recursive_directory_iterator
    range_begin(const recursive_directory_iterator& iter) BOOST_NOEXCEPT
                                                   {return iter;}
  inline
  recursive_directory_iterator range_end(recursive_directory_iterator&) BOOST_NOEXCEPT
                                                  {return recursive_directory_iterator();}
  inline
  recursive_directory_iterator range_end(const recursive_directory_iterator&) BOOST_NOEXCEPT
                                                  {return recursive_directory_iterator();}
  }  // namespace filesystem

  //  namespace boost template specializations
  template<>
  struct range_mutable_iterator<boost::filesystem::recursive_directory_iterator, void>
                        { typedef boost::filesystem::recursive_directory_iterator type; };
  template<>
  struct range_const_iterator<boost::filesystem::recursive_directory_iterator, void>
                        { typedef boost::filesystem::recursive_directory_iterator type; };

namespace filesystem
{

# if !defined(BOOST_FILESYSTEM_NO_DEPRECATED)
  typedef recursive_directory_iterator wrecursive_directory_iterator;
# endif

//  test helper  -----------------------------------------------------------------------//

//  Not part of the documented interface since false positives are possible;
//  there is no law that says that an OS that has large stat.st_size
//  actually supports large file sizes.

  namespace detail
  {
    BOOST_FILESYSTEM_DECL bool possible_large_file_size_support();
  }

  } // namespace filesystem
} // namespace boost

#include <boost/config/abi_suffix.hpp> // pops abi_prefix.hpp pragmas
#endif // BOOST_FILESYSTEM3_OPERATIONS_HPP
