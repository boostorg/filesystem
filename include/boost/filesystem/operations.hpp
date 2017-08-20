//  boost/filesystem/operations.hpp  ---------------------------------------------------//

//  Copyright Beman Dawes 2002-2009
//  Copyright Jan Langer 2002
//  Copyright Dietmar Kuehl 2001                                        
//  Copyright Vladimir Prus 2002
   
//  Distributed under the Boost Software License, Version 1.0.
//  See http://www.boost.org/LICENSE_1_0.txt

//  Library home page: http://www.boost.org/libs/filesystem

//--------------------------------------------------------------------------------------//

#ifndef BOOST_FILESYSTEM_OPERATIONS_HPP
#define BOOST_FILESYSTEM_OPERATIONS_HPP

#include <boost/config.hpp>

# if defined( BOOST_NO_STD_WSTRING )
#   error Configuration not supported: Boost.Filesystem V3 and later requires std::wstring support
# endif

#include <boost/filesystem/config.hpp>
#include <boost/filesystem/path.hpp>

#include <boost/detail/bitmask.hpp>
#include <boost/system/error_code.hpp>
#include <boost/system/system_error.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/utility/enable_if.hpp>
#include <boost/type_traits/is_same.hpp>
#include <boost/cstdint.hpp>
#include <boost/range/mutable_iterator.hpp>
#include <boost/range/const_iterator.hpp>
#include <boost/assert.hpp>
#include <string>
#include <utility> // for pair
#include <ctime>
#include <vector>
#include <stack>
#include <iosfwd>

#ifdef BOOST_WINDOWS_API
#  include <fstream>
#endif

#include <boost/config/abi_prefix.hpp> // must be the last #include

//--------------------------------------------------------------------------------------//

namespace boost
{
  namespace filesystem
  {
    typedef std::time_t file_time_type;

    //--------------------------------------------------------------------------------------//
    //                                                                                      //
    //                            class filesystem_error                                    //
    //                                                                                      //
    //--------------------------------------------------------------------------------------//

    class BOOST_SYMBOL_VISIBLE filesystem_error : public system::system_error
    {
      // see http://www.boost.org/more/error_handling.html for design rationale

      // all functions are inline to avoid issues with crossing dll boundaries

      // functions previously throw() are now BOOST_NOEXCEPT_OR_NOTHROW
      // functions previously without throw() are now BOOST_NOEXCEPT

    public:
      // compiler generates copy constructor and copy assignment

      filesystem_error(
        const std::string & what_arg, system::error_code ec) BOOST_NOEXCEPT
        : system::system_error(ec, what_arg)
      {
        try
        {
          m_imp_ptr.reset(new m_imp);
        }
        catch (...) { m_imp_ptr.reset(); }
      }

      filesystem_error(
        const std::string & what_arg, const path& path1_arg,
        system::error_code ec) BOOST_NOEXCEPT
        : system::system_error(ec, what_arg)
      {
        try
        {
          m_imp_ptr.reset(new m_imp);
          m_imp_ptr->m_path1 = path1_arg;
        }
        catch (...) { m_imp_ptr.reset(); }
      }

      filesystem_error(
        const std::string & what_arg, const path& path1_arg,
        const path& path2_arg, system::error_code ec) BOOST_NOEXCEPT
        : system::system_error(ec, what_arg)
      {
        try
        {
          m_imp_ptr.reset(new m_imp);
          m_imp_ptr->m_path1 = path1_arg;
          m_imp_ptr->m_path2 = path2_arg;
        }
        catch (...) { m_imp_ptr.reset(); }
      }

      ~filesystem_error() BOOST_NOEXCEPT_OR_NOTHROW{}

        const path& path1() const BOOST_NOEXCEPT
      {
        static const path empty_path;
        return m_imp_ptr.get() ? m_imp_ptr->m_path1 : empty_path;
      }
        const path& path2() const  BOOST_NOEXCEPT
      {
        static const path empty_path;
        return m_imp_ptr.get() ? m_imp_ptr->m_path2 : empty_path;
      }

        const char* what() const BOOST_NOEXCEPT_OR_NOTHROW
      {
        if (!m_imp_ptr.get())
        return system::system_error::what();

        try
        {
          if (m_imp_ptr->m_what.empty())
          {
            m_imp_ptr->m_what = system::system_error::what();
            if (!m_imp_ptr->m_path1.empty())
            {
              m_imp_ptr->m_what += ": \"";
              m_imp_ptr->m_what += m_imp_ptr->m_path1.string();
              m_imp_ptr->m_what += "\"";
            }
            if (!m_imp_ptr->m_path2.empty())
            {
              m_imp_ptr->m_what += ", \"";
              m_imp_ptr->m_what += m_imp_ptr->m_path2.string();
              m_imp_ptr->m_what += "\"";
            }
          }
          return m_imp_ptr->m_what.c_str();
        }
        catch (...)
        {
          return system::system_error::what();
        }
      }

    private:
      struct m_imp
      {
        path         m_path1; // may be empty()
        path         m_path2; // may be empty()
        std::string  m_what;  // not built until needed
      };
      boost::shared_ptr<m_imp> m_imp_ptr;
    };

//--------------------------------------------------------------------------------------//
//                                     file_type                                        //
//--------------------------------------------------------------------------------------//

    enum class file_type
    {
        none,
        not_found,
        regular,
        directory,
        // the following may not apply to some operating systems or file systems
        symlink,
        block,
        character,
        fifo,
        socket,
        reparse,  // Windows: FILE_ATTRIBUTE_REPARSE_POINT that is not a symlink
        unknown,  // file exists, but isn't one of the above types or
                  //   we don't have permission to find its type

#   ifndef BOOST_FILESYSTEM_NO_DEPRECATED
                  status_error = none,
                  status_unknown = none,
                  file_not_found = not_found,
                  regular_file = regular,
                  directory_file = directory,
                  symlink_file = symlink,
                  block_file = block,
                  character_file = character,
                  fifo_file = fifo,
                  socket_file = socket,
                  reparse_file = reparse,
                  type_unknown = unknown,
#   endif

                  _detail_directory_symlink  // internal use only; never exposed to users
    };

# ifndef BOOST_FILESYSTEM_NO_DEPRECATED
    static const file_type status_error = file_type::none;
    static const file_type status_unknown = file_type::none;
    static const file_type file_not_found = file_type::not_found;
    static const file_type regular_file = file_type::regular;
    static const file_type directory_file = file_type::directory;
    static const file_type symlink_file = file_type::symlink;
    static const file_type block_file = file_type::block;
    static const file_type character_file = file_type::character;
    static const file_type fifo_file = file_type::fifo;
    static const file_type socket_file = file_type::socket;
    static const file_type reparse_file = file_type::reparse;
    static const file_type type_unknown = file_type::unknown;
# endif

  template <class Char, class Traits>
  inline std::basic_ostream<Char, Traits>&
  operator<<(std::basic_ostream<Char, Traits>& os, file_type x)
  {
    return os << static_cast<int>(x);
  }
  
  template <class Char, class Traits>
  inline std::basic_istream<Char, Traits>&
  operator>>(std::basic_istream<Char, Traits>& is, file_type& x)
  {
    int tmp;
    is >> tmp;
    x = static_cast<file_type>(tmp);
    return is;
  }

//--------------------------------------------------------------------------------------//
//                                    copy_options                                      //
//--------------------------------------------------------------------------------------//

  enum class copy_options
  {
    none = 0,

    // copy_file function effects for existing target files; default: treat as error
    skip_existing = 1,        // do not overwrite existing file, do not report an error
    overwrite_existing = 2,   // overwrite the existing file
    update_existing = 4,      // overwrite existing file if older than replacement file

    // copy function effects for sub-directories; default: do not copy sub-directories
    recursive = 8,            // recursively copy sub-directories and their contents

    // copy function effects for symbolic links; default: follow symlinks
    copy_symlinks = 16,       // copy symlinks rather than the files they point to
    skip_symlinks = 32,       // ignore symbolic links

    // copy function effects for choosing the form of copying; default: copy contents
    directories_only = 64,    // copy directory structure but no copy of other file types
    create_symlinks = 128,    // make symbolic links instead of copies of files; source 
                              // path shall be an absolute path unless destination path
                              // is in the current directory
    create_hard_links = 256,  // make hard links instead of copies of files

    // extentions; not part of standard library filesystem
    copy_file_mask = skip_existing | overwrite_existing | update_existing,
    copy_sub_dirs_mask = recursive,
    copy_symlinks_mask = copy_symlinks | skip_symlinks,
    copy_form_mask = directories_only | create_symlinks | create_hard_links
  };

  BOOST_BITMASK(copy_options)

//--------------------------------------------------------------------------------------//
//                                       perms                                          //
//--------------------------------------------------------------------------------------//

  enum class perms
  {
    no_perms = 0,       // file_not_found is no_perms rather than perms_not_known

    // POSIX equivalent macros given in comments.
    // Values are from POSIX and are given in octal per the POSIX standard.

    // permission bits
    
    owner_read = 0400,  // S_IRUSR, Read permission, owner
    owner_write = 0200, // S_IWUSR, Write permission, owner
    owner_exe = 0100,   // S_IXUSR, Execute/search permission, owner
    owner_all = 0700,   // S_IRWXU, Read, write, execute/search by owner

    group_read = 040,   // S_IRGRP, Read permission, group
    group_write = 020,  // S_IWGRP, Write permission, group
    group_exe = 010,    // S_IXGRP, Execute/search permission, group
    group_all = 070,    // S_IRWXG, Read, write, execute/search by group

    others_read = 04,   // S_IROTH, Read permission, others
    others_write = 02,  // S_IWOTH, Write permission, others
    others_exe = 01,    // S_IXOTH, Execute/search permission, others
    others_all = 07,    // S_IRWXO, Read, write, execute/search by others

    all = 0777,         // owner_all|group_all|others_all

    // other POSIX bits

    set_uid = 04000,    // S_ISUID, Set-user-ID on execution
    set_gid = 02000,    // S_ISGID, Set-group-ID on execution
    sticky_bit = 01000, // S_ISVTX,
                        // (POSIX XSI) On directories, restricted deletion flag 
                        // (V7) 'sticky bit': save swapped text even after use 
                        // (SunOS) On non-directories: don't cache this file
                        // (SVID-v4.2) On directories: restricted deletion flag
                        // Also see http://en.wikipedia.org/wiki/Sticky_bit

    mask = 07777,       // all_all|set_uid_on_exe|set_gid_on_exe|sticky_bit

    unknown = 0xFFFF,   // permissions are not known, such as when a file_status object
                        // is created without specifying the permissions
  };

  BOOST_BITMASK(perms)

//--------------------------------------------------------------------------------------//
//                                   perm_options                                       //
//--------------------------------------------------------------------------------------//

  enum class perm_options
  {
    // one of:
    replace = 0,          // replace the file’s permission bits with perm arg
    add = 1,              // replace the file’s permission bits with the bitwise OR of
                          //   perm arg and the file’s current permission bits
    remove = 2,           // replace the file’s permission bits with the bitwise AND of
                          //   complement of perm arg and file’s current permission bits
    // optional bitmask:
    nofollow = 4          // change the permissions of a symbolic link itself rather than
                          //   the permissions of the file the link resolves to
  };

  BOOST_BITMASK(perm_options)

//--------------------------------------------------------------------------------------//
//                                 directory_options                                    //
//--------------------------------------------------------------------------------------//

  enum class directory_options
  {
    none = 0,                     // (default) Skip directory symlinks, permission denied
                                  //   is an error
    follow_directory_symlink = 1, // follow rather than skip directory symlinks
    skip_permission_denied = 2,   // skip directories that would otherwise result in a
                                  //   permission denied error

    _detail_no_push = 4           // internal use only
  };

  BOOST_BITMASK(directory_options)

//--------------------------------------------------------------------------------------//
//                                    file_status                                       //
//--------------------------------------------------------------------------------------//

  class BOOST_FILESYSTEM_DECL file_status
  {
  public:
             file_status() BOOST_NOEXCEPT
               : m_value(file_type::none), m_perms(perms::unknown) {}
    explicit file_status(file_type v) BOOST_NOEXCEPT
               : m_value(v), m_perms(perms::unknown)  {}
             file_status(file_type v, perms prms) BOOST_NOEXCEPT
               : m_value(v), m_perms(prms) {}

  //  As of October 2015 the interaction between noexcept and =default is so troublesome
  //  for VC++, GCC, and probably other compilers, that =default is not used with noexcept
  //  functions. GCC is not even consistent for the same release on different platforms.

    file_status(const file_status& rhs) BOOST_NOEXCEPT
      : m_value(rhs.m_value), m_perms(rhs.m_perms) {}
    file_status& operator=(const file_status& rhs) BOOST_NOEXCEPT
    {
      m_value = rhs.m_value;
      m_perms = rhs.m_perms;
      return *this;
    }

# if !defined(BOOST_NO_CXX11_RVALUE_REFERENCES)
    file_status(file_status&& rhs) BOOST_NOEXCEPT
    {
      m_value = std::move(rhs.m_value);
      m_perms = std::move(rhs.m_perms);
    }
    file_status& operator=(file_status&& rhs) BOOST_NOEXCEPT
    { 
      m_value = std::move(rhs.m_value);
      m_perms = std::move(rhs.m_perms);
      return *this;
    }
# endif


    // observers
    file_type  type() const BOOST_NOEXCEPT            { return m_value; }
    perms      permissions() const BOOST_NOEXCEPT     { return m_perms; } 

    // modifiers
    void       type(file_type v) BOOST_NOEXCEPT       { m_value = v; }
    void       permissions(perms prms) BOOST_NOEXCEPT { m_perms = prms; }

    bool operator==(const file_status& rhs) const BOOST_NOEXCEPT
      { return type() == rhs.type() && 
        permissions() == rhs.permissions(); }
    bool operator!=(const file_status& rhs) const BOOST_NOEXCEPT
      { return !(*this == rhs); }

  private:
    file_type         m_value;
    enum class perms  m_perms;
  };

  inline bool type_present(file_status f) BOOST_NOEXCEPT
                                          { return f.type() != file_type::none; }
  inline bool permissions_present(file_status f) BOOST_NOEXCEPT
                                          {return f.permissions() != perms::unknown;}
  inline bool status_known(file_status f) BOOST_NOEXCEPT
                                          { return type_present(f) && permissions_present(f); }
  inline bool exists(file_status f) BOOST_NOEXCEPT
                                          { return f.type() != file_type::none
                                                && f.type() != file_type::not_found; }
  inline bool is_block_file(file_status f) BOOST_NOEXCEPT
                                          { return f.type() == file_type::block; }
  inline bool is_character_file(file_status f) BOOST_NOEXCEPT
                                          { return f.type() == file_type::character; }
  inline bool is_fifo(file_status f) BOOST_NOEXCEPT
                                          { return f.type() == file_type::fifo; }
  inline bool is_socket(file_status f) BOOST_NOEXCEPT
                                          { return f.type() == file_type::socket; }
  inline bool is_regular_file(file_status f) BOOST_NOEXCEPT
                                          { return f.type() == file_type::regular; }
  inline bool is_directory(file_status f) BOOST_NOEXCEPT
                                          { return f.type() == file_type::directory; }
  inline bool is_symlink(file_status f) BOOST_NOEXCEPT
                                          { return f.type() == file_type::symlink; }
  inline bool is_other(file_status f) BOOST_NOEXCEPT
                                          { return exists(f) && !is_regular_file(f)
                                                && !is_directory(f) && !is_symlink(f); }

# ifndef BOOST_FILESYSTEM_NO_DEPRECATED
  inline bool is_regular(file_status f) BOOST_NOEXCEPT { return f.type() == file_type::regular; }
# endif

  struct space_info
  {
    // all values are byte counts
    boost::uintmax_t capacity;
    boost::uintmax_t free;      // <= capacity
    boost::uintmax_t available; // <= free
  };

//--------------------------------------------------------------------------------------//
//                             implementation details                                   //
//--------------------------------------------------------------------------------------//

  namespace detail
  {
    ////  We cannot pass a BOOST_SCOPED_ENUM to a compled function because it will cause
    ////  an undefined reference if the library is compled with -std=c++0x but the
    ////  use is compiled in C++03 mode, or visa versa. See tickets 6124, 6779, 10038.
    ////  The workaround is to pass a plain old enum.
    //enum copy_opts
    //{
    //  none=0,
    //  skip_existing = 1,      
    //  overwrite_existing = 2, 
    //  update_existing = 4,    
    //  recursive = 8,          
    //  copy_symlinks = 16,     
    //  skip_symlinks = 32,     
    //  directories_only = 64,  
    //  create_symlinks = 128,  
    //  create_hard_links = 256,
    //  copy_file_mask = skip_existing|overwrite_existing|update_existing,
    //  copy_sub_dirs_mask = recursive,
    //  copy_symlinks_mask = copy_symlinks|skip_symlinks,
    //  copy_form_mask = directories_only|create_symlinks|create_hard_links
    //};

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
                    copy_options options, system::error_code* ec=0);
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

    struct deacc;

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
  bool is_block_file(const path& p)    {return is_block_file(detail::status(p));}
  inline 
  bool is_block_file(const path& p, system::error_code& ec)
                                       {return is_block_file(detail::status(p, &ec));}
  inline 
  bool is_character_file(const path& p){return is_character_file(detail::status(p));}
  inline 
  bool is_character_file(const path& p, system::error_code& ec)
                                       {return is_character_file(detail::status(p, &ec));}
  inline 
  bool is_fifo(const path& p)          {return is_fifo(detail::status(p));}
  inline 
  bool is_fifo(const path& p, system::error_code& ec)
                                       {return is_fifo(detail::status(p, &ec));}
  inline 
  bool is_socket(const path& p)        {return is_socket(detail::status(p));}
  inline 
  bool is_socket(const path& p, system::error_code& ec)
                                       {return is_socket(detail::status(p, &ec));}
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
                 copy_options options)
  {
    detail::copy_file(from, to, options);
  }
  inline
  void copy_file(const path& from, const path& to)
  {
    detail::copy_file(from, to, copy_options::none);
  }
  inline
  void copy_file(const path& from, const path& to,   // See ticket #2925
                 copy_options options,
                 system::error_code& ec) BOOST_NOEXCEPT
  {
    detail::copy_file(from, to, options, &ec);
  }
  inline
  void copy_file(const path& from, const path& to, system::error_code& ec) BOOST_NOEXCEPT
  {
    detail::copy_file(from, to, copy_options::none, &ec);
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
                                       { return is_symlink(symlink_status(p)); }
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

//  TODO: is this really a GCC problem or is it a MSVC 2-phase lookup problem?
//  GCC has a problem with a member function named path within a namespace or 
//  sub-namespace that also has a class named path. The workaround is to always
//  fully qualify the name path when it refers to the class name.

#ifdef BOOST_WINDOWS_API
# define BOOST_FILESYSTEM_CACHE_STATUS
# define BOOST_FILESYSTEM_CACHE_SYMLINK_STATUS
# define BOOST_FILESYSTEM_CACHE_FILESIZE
# define BOOST_FILESYSTEM_CACHE_LAST_WRITE_TIME
#else
#endif

class BOOST_FILESYSTEM_DECL directory_entry
{
public:
  // enable class path ctor taking directory_entry
  typedef boost::filesystem::path::value_type value_type;   

  //  As of October 2015 the interaction between noexcept and =default is so troublesome
  //  for VC++, GCC, and probably other compilers, that =default is not used.
  //  GCC is not even consistent for the same release on different platforms.

  directory_entry() BOOST_NOEXCEPT {}

  explicit directory_entry(const boost::filesystem::path& p)
    : m_path(p) { refresh(); }
  directory_entry(const boost::filesystem::path& p, system::error_code& ec)
    : m_path(p) { refresh(ec); }

  //  compiler will generate copy constructor, move constructor, copy assignment,
  //  move assignment, and destructor

  // extension; the standard library only provides assign()
  directory_entry& operator=(const boost::filesystem::path& p)
  {
    m_path = p;
    refresh();
    return *this;
  }

  directory_entry& assign(const boost::filesystem::path& p)
  {
    m_path = p;
    refresh();
    return *this;
  }

  directory_entry& assign(const boost::filesystem::path& p, system::error_code& ec)
  {
    m_path = p;
    refresh(ec);
    return *this;
  }

  void replace_filename(const boost::filesystem::path& p)
  {
    m_path.remove_filename();
    m_path /= p;
    refresh();
  }

  void replace_filename(const boost::filesystem::path& p, system::error_code& ec)
  {
    m_path.remove_filename();
    m_path /= p;
    refresh(ec);
  }

  void refresh() { m_refresh(0); }

  void refresh(system::error_code& ec) BOOST_NOEXCEPT { m_refresh(&ec); }

  //  directory_entry observers --------------------------------------------------------//

  const boost::filesystem::path&  path() const BOOST_NOEXCEPT {return m_path;}
  operator const boost::filesystem::path&() const BOOST_NOEXCEPT {return m_path;}

  bool exists() const
    { return boost::filesystem::exists(this->status()); }
  bool exists(system::error_code& ec) const BOOST_NOEXCEPT 
    { return boost::filesystem::exists(this->status(ec)); }
  bool is_block_file() const
    { return boost::filesystem::is_block_file(this->status()); }
  bool is_block_file(system::error_code& ec) const BOOST_NOEXCEPT
    { return boost::filesystem::is_block_file(this->status(ec)); }
  bool is_character_file() const
    { return boost::filesystem::is_character_file(this->status()); }
  bool is_character_file(system::error_code& ec) const BOOST_NOEXCEPT
    { return boost::filesystem::is_character_file(this->status(ec)); }
  bool is_directory() const
    { return boost::filesystem::is_directory(this->status()); }
  bool is_directory(system::error_code& ec) const BOOST_NOEXCEPT
    { return boost::filesystem::is_directory(this->status(ec)); }
  bool is_fifo() const
    { return boost::filesystem::is_fifo(this->status()); }
  bool is_fifo(system::error_code& ec) const BOOST_NOEXCEPT
    { return boost::filesystem::is_fifo(this->status(ec)); }
  bool is_other() const
    { return boost::filesystem::is_other(this->status()); }
  bool is_other(system::error_code& ec) const BOOST_NOEXCEPT
    { return boost::filesystem::is_other(this->status(ec)); }
  bool is_regular_file() const
    { return boost::filesystem::is_regular_file(this->status()); }
  bool is_regular_file(system::error_code& ec) const BOOST_NOEXCEPT
    { return boost::filesystem::is_regular_file(this->status(ec)); }
  bool is_socket() const
    { return boost::filesystem::is_socket(this->status()); }
  bool is_socket(system::error_code& ec) const BOOST_NOEXCEPT
    { return boost::filesystem::is_socket(this->status(ec)); }
  bool is_symlink() const
    { return boost::filesystem::is_symlink(this->status()); }
  bool is_symlink(system::error_code& ec) const BOOST_NOEXCEPT
    { return boost::filesystem::is_symlink(this->status(ec)); }

# ifdef BOOST_FILESYSTEM_CACHE_STATUS
  file_status status() const { return m_status; }
  file_status status(system::error_code& ec) const { ec.clear(); return m_status; }
# else 
  file_status status() const { return boost::filesystem::status(m_path); }
  file_status status(system::error_code& ec) const
    { return boost::filesystem::status(m_path, ec); }
# endif

# ifdef BOOST_FILESYSTEM_CACHE_SYMLINK_STATUS
  file_status symlink_status() const { return m_symlink_status; }
  file_status symlink_status(system::error_code& ec) const
    { ec.clear(); return m_symlink_status; }
# else 
  file_status symlink_status() const { return boost::filesystem::symlink_status(m_path); }
  file_status symlink_status(system::error_code& ec) const
    { return boost::filesystem::symlink_status(m_path, ec); }
# endif

# ifdef BOOST_FILESYSTEM_CACHE_FILESIZE
  boost::uintmax_t file_size() const { return m_file_size; }
  boost::uintmax_t file_size(system::error_code& ec) const { ec.clear(); return m_file_size; }
# else 
  boost::uintmax_t file_size() const { return boost::filesystem::file_size(m_path); }
  boost::uintmax_t file_size(system::error_code& ec) const
    { return boost::filesystem::file_size(m_path, ec); }
# endif

# ifdef BOOST_FILESYSTEM_CACHE_LAST_WRITE_TIME
  std::time_t last_write_time() const { return m_last_write_time; }
  std::time_t last_write_time(system::error_code& ec) const { ec.clear(); return m_last_write_time; }
# else 
  std::time_t last_write_time() const { return boost::filesystem::last_write_time(m_path); }
  std::time_t last_write_time(system::error_code& ec) const
    { return boost::filesystem::last_write_time(m_path, ec); }
# endif

  bool operator==(const directory_entry& rhs) const BOOST_NOEXCEPT {return m_path == rhs.m_path; }
  bool operator!=(const directory_entry& rhs) const BOOST_NOEXCEPT {return m_path != rhs.m_path;} 
  bool operator< (const directory_entry& rhs) const BOOST_NOEXCEPT {return m_path < rhs.m_path;} 
  bool operator<=(const directory_entry& rhs) const BOOST_NOEXCEPT {return m_path <= rhs.m_path;} 
  bool operator> (const directory_entry& rhs) const BOOST_NOEXCEPT {return m_path > rhs.m_path;} 
  bool operator>=(const directory_entry& rhs) const BOOST_NOEXCEPT {return m_path >= rhs.m_path;} 

private:
  friend struct detail::deacc;

  void m_refresh(system::error_code* ec);

  boost::filesystem::path   m_path;  

  // platform specific cache; set by directory iteration or refresh()
# ifdef BOOST_FILESYSTEM_CACHE_STATUS
  file_status       m_status;           // stat()-like
# endif
# ifdef BOOST_FILESYSTEM_CACHE_SYMLINK_STATUS
  file_status       m_symlink_status;   // lstat()-like
# endif
# ifdef BOOST_FILESYSTEM_CACHE_FILESIZE
  boost::uintmax_t  m_file_size;        
# endif
# ifdef  BOOST_FILESYSTEM_CACHE_LAST_WRITE_TIME
  std::time_t       m_last_write_time;
# endif

}; // directory_entry

//--------------------------------------------------------------------------------------//
//                                                                                      //
//                            directory_iterator helpers                                //
//                                                                                      //
//--------------------------------------------------------------------------------------//

class directory_iterator;

namespace detail
{
  BOOST_FILESYSTEM_DECL
    system::error_code close_directory(// never throws()
    void *& handle
#   if     defined(BOOST_POSIX_API)
    , void *& buffer
#   endif
  ); 

  struct dir_itr_imp
  {
    directory_entry  dir_entry;
    void*            handle;

#   ifdef BOOST_POSIX_API
    void*            buffer;  // see dir_itr_increment implementation
#   endif

    dir_itr_imp() : handle(0)
#   ifdef BOOST_POSIX_API
      , buffer(0)
#   endif
    {}

    ~dir_itr_imp() // never throws
    {
      close_directory(handle
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

  class directory_iterator
    : public boost::iterator_facade< directory_iterator,
                                     directory_entry,
                                     boost::single_pass_traversal_tag >
  {
  public:

    directory_iterator() BOOST_NOEXCEPT {}  // creates the "end" iterator

    // iterator_facade derived classes don't seem to like implementations in
    // separate translation unit dll's, so forward to detail functions
    explicit directory_iterator(const path& p)
        : m_imp(new detail::dir_itr_imp)
          { detail::directory_iterator_construct(*this, p, 0); }

    directory_iterator(const path& p, system::error_code& ec) BOOST_NOEXCEPT
        : m_imp(new detail::dir_itr_imp)
          { detail::directory_iterator_construct(*this, p, &ec); }

   ~directory_iterator() {}

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

    // shared_ptr provides the shallow-copy semantics required for single pass iterators
    // (i.e. InputIterators). The end iterator is indicated by !m_imp || !m_imp->handle
    boost::shared_ptr< detail::dir_itr_imp >  m_imp;

    friend class boost::iterator_core_access;

    boost::iterator_facade<
      directory_iterator,
      directory_entry,
      boost::single_pass_traversal_tag >::reference dereference() const 
    {
      BOOST_ASSERT_MSG(m_imp.get(), "attempt to dereference end iterator");
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

  //  enable directory_iterator C++11 range-base for statement use  --------------------//

  //  begin() and end() are only used by a range-based for statement in the context of
  //  auto - thus the top-level const is stripped - so returning const is harmless and
  //  emphasizes begin() is just a pass through.
  inline
  const directory_iterator& begin(const directory_iterator& iter) BOOST_NOEXCEPT
    {return iter;}
  inline
  directory_iterator end(const directory_iterator&) BOOST_NOEXCEPT
    {return directory_iterator();}

  //  enable directory_iterator BOOST_FOREACH  -----------------------------------------//

  inline
  directory_iterator& range_begin(directory_iterator& iter) BOOST_NOEXCEPT
    {return iter;}
  inline
  directory_iterator range_begin(const directory_iterator& iter) BOOST_NOEXCEPT
    {return iter;}
  inline
  directory_iterator range_end(const directory_iterator&) BOOST_NOEXCEPT
    {return directory_iterator();}
  }  // namespace filesystem

  //  namespace boost template specializations
  template<>
  struct range_mutable_iterator<boost::filesystem::directory_iterator>
    { typedef boost::filesystem::directory_iterator type; };
  template<>
  struct range_const_iterator <boost::filesystem::directory_iterator>
    { typedef boost::filesystem::directory_iterator type; };

namespace filesystem
{

//--------------------------------------------------------------------------------------//
//                                                                                      //
//                      recursive_directory_iterator helpers                            //
//                                                                                      //
//--------------------------------------------------------------------------------------//

  //BOOST_SCOPED_ENUM_DECLARE_BEGIN(directory_options)
  //{
  //  none,
  //  no_recurse = none,         // don't follow directory symlinks (default behavior)
  //  recurse,                   // follow directory symlinks
  //  _detail_no_push = recurse << 1  // internal use only
  //}
  //BOOST_SCOPED_ENUM_DECLARE_END(directory_options)

  //BOOST_BITMASK(BOOST_SCOPED_ENUM_NATIVE(directory_options))

  namespace detail
  {
    struct recur_dir_itr_imp
    {
      typedef directory_iterator element_type;
      std::stack< element_type, std::vector< element_type > > m_stack;
      int  m_level;
      directory_options m_options;

      recur_dir_itr_imp() : m_level(0), m_options(directory_options::none) {}

      void increment(system::error_code* ec);  // ec == 0 means throw on error

      bool push_directory(system::error_code& ec) BOOST_NOEXCEPT;

      void pop();

    };

    //  Implementation is inline to avoid dynamic linking difficulties with m_stack:
    //  Microsoft warning C4251, m_stack needs to have dll-interface to be used by
    //  clients of struct 'boost::filesystem::detail::recur_dir_itr_imp'

    inline
    bool recur_dir_itr_imp::push_directory(system::error_code& ec) BOOST_NOEXCEPT
    // Returns: true if push occurs, otherwise false. Always returns false on error.
    {
      ec.clear();

      //  Discover if the iterator is for a directory that needs to be recursed into,
      //  taking symlinks and options into account.

      if ((m_options & directory_options::_detail_no_push) == directory_options::_detail_no_push)
      {
        m_options &= ~directory_options::_detail_no_push;
        return false;
      }

      file_status symlink_stat;

      // if we are not recursing into symlinks, we are going to have to know if the
      // stack top is a symlink, so get symlink_status and verify no error occurred 
      if ((m_options & directory_options::follow_directory_symlink) != directory_options::follow_directory_symlink)
      {
        symlink_stat = m_stack.top()->symlink_status(ec);
        if (ec)
          return false;
      }

      // Logic for following predicate was contributed by Daniel Aarno to handle cyclic
      // symlinks correctly and efficiently, fixing ticket #5652.
      //   if (((m_options & directory_options::follow_directory_symlink)
      //        == directory_options::follow_directory_symlink
      //         || !is_symlink(m_stack.top()->symlink_status()))
      //       && is_directory(m_stack.top()->status())) ...
      // The predicate code has since been rewritten to pass error_code arguments,
      // per ticket #5653.

      if ((m_options & directory_options::follow_directory_symlink) == directory_options::follow_directory_symlink
        || !is_symlink(symlink_stat))
      {
        file_status stat = m_stack.top()->status(ec);
        if (ec || !is_directory(stat))
          return false;

        directory_iterator next(m_stack.top()->path(), ec);
        if (!ec && next != directory_iterator())
        {
          m_stack.push(next);
          ++m_level;
          return true;
        }
      }
      return false;
    }

    inline
    void recur_dir_itr_imp::increment(system::error_code* ec)
    // ec == 0 means throw on error
    // 
    // Invariant: On return, the top of the iterator stack is the next valid (possibly
    // end) iterator, regardless of whether or not an error is reported, and regardless of
    // whether any error is reported by exception or error code. In other words, progress
    // is always made so a loop on the iterator will always eventually terminate
    // regardless of errors.
    {
      system::error_code ec_push_directory;

      //  if various conditions are met, push a directory_iterator into the iterator stack
      if (push_directory(ec_push_directory))
      {
        if (ec)
          ec->clear();
        return;
      }

      //  Do the actual increment operation on the top iterator in the iterator
      //  stack, popping the stack if necessary, until either the stack is empty or a
      //  non-end iterator is reached.
      while (!m_stack.empty() && ++m_stack.top() == directory_iterator())
      {
        m_stack.pop();
        --m_level;
      }

      // report errors if any
      if (ec_push_directory)
      {
        if (ec)
          *ec = ec_push_directory;
        else
        {
          BOOST_FILESYSTEM_THROW(filesystem_error(
            "filesystem::recursive_directory_iterator increment error",
            ec_push_directory));
        }
      }
      else if (ec)
        ec->clear();
    }

    inline
    void recur_dir_itr_imp::pop()
    {
      BOOST_ASSERT_MSG(m_level > 0,
        "pop() on recursive_directory_iterator with level < 1");

      do
      {
        m_stack.pop();
        --m_level;
      }
      while (!m_stack.empty() && ++m_stack.top() == directory_iterator());
    }
  } // namespace detail

//--------------------------------------------------------------------------------------//
//                                                                                      //
//                           recursive_directory_iterator                               //
//                                                                                      //
//--------------------------------------------------------------------------------------//

  class recursive_directory_iterator
    : public boost::iterator_facade<
        recursive_directory_iterator,
        directory_entry,
        boost::single_pass_traversal_tag >
  {
  public:

    recursive_directory_iterator() BOOST_NOEXCEPT {}  // creates the "end" iterator

    explicit recursive_directory_iterator(const path& dir_path)  // throws if !exists()
      : m_imp(new detail::recur_dir_itr_imp)
    {
      m_imp->m_options = directory_options::none;
      m_imp->m_stack.push(directory_iterator(dir_path));
      if (m_imp->m_stack.top() == directory_iterator())
        { m_imp.reset(); }
    }

    recursive_directory_iterator(const path& dir_path,
      directory_options opt)  // throws if !exists()
      : m_imp(new detail::recur_dir_itr_imp)
    {
      m_imp->m_options = opt;
      m_imp->m_stack.push(directory_iterator(dir_path));
      if (m_imp->m_stack.top() == directory_iterator())
        { m_imp.reset (); }
    }

    recursive_directory_iterator(const path& dir_path, directory_options opt,
      system::error_code & ec) BOOST_NOEXCEPT
    : m_imp(new detail::recur_dir_itr_imp)
    {
      m_imp->m_options = opt;
      m_imp->m_stack.push(directory_iterator(dir_path, ec));
      if (m_imp->m_stack.top() == directory_iterator())
        { m_imp.reset (); }
    }

    recursive_directory_iterator(const path& dir_path,
      system::error_code & ec) BOOST_NOEXCEPT
    : m_imp(new detail::recur_dir_itr_imp)
    {
      m_imp->m_options = directory_options::none;
      m_imp->m_stack.push(directory_iterator(dir_path, ec));
      if (m_imp->m_stack.top() == directory_iterator())
        { m_imp.reset (); }
    }

    recursive_directory_iterator& increment(system::error_code& ec) BOOST_NOEXCEPT
    {
      BOOST_ASSERT_MSG(m_imp.get(),
        "increment() on end recursive_directory_iterator");
      m_imp->increment(&ec);
      if (m_imp->m_stack.empty())
        m_imp.reset(); // done, so make end iterator
      return *this;
    }

    int depth() const BOOST_NOEXCEPT
    { 
      BOOST_ASSERT_MSG(m_imp.get(),
        "depth() on end recursive_directory_iterator");
      return m_imp->m_level;
    }
  
    int level() const BOOST_NOEXCEPT { return depth(); }

    bool recursion_pending() const BOOST_NOEXCEPT
    {
      BOOST_ASSERT_MSG(m_imp.get(),
        "is_no_push_requested() on end recursive_directory_iterator");
      return (m_imp->m_options & directory_options::_detail_no_push)
        == directory_options::_detail_no_push;
    }
  
    bool no_push_pending() const BOOST_NOEXCEPT { return recursion_pending(); }

#   ifndef BOOST_FILESYSTEM_NO_DEPRECATED
    bool no_push_request() const BOOST_NOEXCEPT { return no_push_pending(); }
#   endif

    void pop()
    { 
      BOOST_ASSERT_MSG(m_imp.get(),
        "pop() on end recursive_directory_iterator");
      m_imp->pop();
      if (m_imp->m_stack.empty()) m_imp.reset(); // done, so make end iterator
    }

    void disable_recursion_pending(bool value=true) BOOST_NOEXCEPT
    {
      BOOST_ASSERT_MSG(m_imp.get(),
        "no_push() on end recursive_directory_iterator");
      if (value)
        m_imp->m_options |= directory_options::_detail_no_push;
      else
        m_imp->m_options &= ~directory_options::_detail_no_push;
    }
  
    void no_push(bool value=true) BOOST_NOEXCEPT { disable_recursion_pending(value); }

    file_status status() const
    {
      BOOST_ASSERT_MSG(m_imp.get(),
        "status() on end recursive_directory_iterator");
      return m_imp->m_stack.top()->status();
    }

    file_status symlink_status() const
    {
      BOOST_ASSERT_MSG(m_imp.get(),
        "symlink_status() on end recursive_directory_iterator");
      return m_imp->m_stack.top()->symlink_status();
    }

  private:

    // shared_ptr provides the shallow-copy semantics required for single pass iterators
    // (i.e. InputIterators).
    // The end iterator is indicated by !m_imp || m_imp->m_stack.empty()
    boost::shared_ptr< detail::recur_dir_itr_imp >  m_imp;

    friend class boost::iterator_core_access;

    boost::iterator_facade< 
      recursive_directory_iterator,
      directory_entry,
      boost::single_pass_traversal_tag >::reference
    dereference() const 
    {
      BOOST_ASSERT_MSG(m_imp.get(),
        "dereference of end recursive_directory_iterator");
      return *m_imp->m_stack.top();
    }

    void increment()
    { 
      BOOST_ASSERT_MSG(m_imp.get(),
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
  recursive_directory_iterator range_end(const recursive_directory_iterator&) BOOST_NOEXCEPT
                                                  {return recursive_directory_iterator();}
  }  // namespace filesystem

  //  namespace boost template specializations
  template<>
  struct range_mutable_iterator<boost::filesystem::recursive_directory_iterator>
                        { typedef boost::filesystem::recursive_directory_iterator type; };
  template<>
  struct range_const_iterator <boost::filesystem::recursive_directory_iterator>
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
#endif // BOOST_FILESYSTEM_OPERATIONS_HPP
