//  boost/filesystem/operations.hpp  ---------------------------------------------------//

//  Copyright Beman Dawes 2002-2009
//  Copyright Jan Langer 2002
//  Copyright Dietmar Kuehl 2001
//  Copyright Vladimir Prus 2002
//  Copyright Andrey Semashev 2020-2026

//  Distributed under the Boost Software License, Version 1.0.
//  See http://www.boost.org/LICENSE_1_0.txt

//  Library home page: http://www.boost.org/libs/filesystem

//--------------------------------------------------------------------------------------//

#ifndef BOOST_FILESYSTEM_OPERATIONS_HPP
#define BOOST_FILESYSTEM_OPERATIONS_HPP

#include <boost/filesystem/config.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/filesystem/file_status.hpp>

#include <boost/detail/bitmask.hpp>
#include <boost/system/error_code.hpp>
#include <boost/cstdint.hpp>
#include <ctime>
#include <string>

#include <boost/filesystem/detail/header.hpp> // must be the last #include

//--------------------------------------------------------------------------------------//

namespace boost {
namespace filesystem {

//! Filesystem space information
struct space_info
{
    //! Total size of the filesystem, in bytes
    boost::uintmax_t capacity;
    //! Amount of free space, in bytes
    boost::uintmax_t free;
    //! Amount of free space available for unprivileged users, in bytes
    boost::uintmax_t available;
};

/*!
 * \brief Option flags for `copy` and `copy_file` operations.
 *
 * These options can be combined using bitwise OR and passed to `copy_file()` and `copy()` operations to customize
 * their behavior.
 *
 * \sa \ref copy_file, \ref copy.
 */
enum class copy_options : unsigned int
{
    none BOOST_FILESYSTEM_DETAIL_DOC_HIDDEN(= 0u), //!< Default. For `copy_file`: error if the target file exists. For `copy`: do not recurse, follow symlinks, copy file contents.

    // copy_file options:
    skip_existing BOOST_FILESYSTEM_DETAIL_DOC_HIDDEN(= 1u),                 //!< Don't overwrite the existing target file, don't report an error
    overwrite_existing BOOST_FILESYSTEM_DETAIL_DOC_HIDDEN(= 1u << 1u),      //!< Overwrite existing file
    update_existing BOOST_FILESYSTEM_DETAIL_DOC_HIDDEN(= 1u << 2u),         //!< Overwrite existing file if its last write time is older than the replacement file
    synchronize_data BOOST_FILESYSTEM_DETAIL_DOC_HIDDEN(= 1u << 3u),        //!< Flush all buffered data written to the target file to permanent storage
    synchronize BOOST_FILESYSTEM_DETAIL_DOC_HIDDEN(= 1u << 4u),             //!< Flush all buffered data and attributes written to the target file to permanent storage
    ignore_attribute_errors BOOST_FILESYSTEM_DETAIL_DOC_HIDDEN(= 1u << 5u), //!< Ignore errors of copying file attributes

    // copy options:
    recursive BOOST_FILESYSTEM_DETAIL_DOC_HIDDEN(= 1u << 8u),               //!< Recurse into sub-directories
    copy_symlinks BOOST_FILESYSTEM_DETAIL_DOC_HIDDEN(= 1u << 9u),           //!< Copy symlinks as symlinks instead of copying the referenced file
    skip_symlinks BOOST_FILESYSTEM_DETAIL_DOC_HIDDEN(= 1u << 10u),          //!< Don't copy symlinks
    directories_only BOOST_FILESYSTEM_DETAIL_DOC_HIDDEN(= 1u << 11u),       //!< Only copy directory structure, do not copy non-directory files
    create_symlinks BOOST_FILESYSTEM_DETAIL_DOC_HIDDEN(= 1u << 12u),        //!< Create symlinks instead of copying files
    create_hard_links BOOST_FILESYSTEM_DETAIL_DOC_HIDDEN(= 1u << 13u),      //!< Create hard links instead of copying files
#if !defined(BOOST_FILESYSTEM_DOXYGEN)
    _detail_recursing = 1u << 14u       // Internal use only, do not use
#endif
};

BOOST_BITMASK(copy_options)

//--------------------------------------------------------------------------------------//
//                             implementation details                                   //
//--------------------------------------------------------------------------------------//

namespace detail {

BOOST_FILESYSTEM_DECL
path absolute_v3(path const& p, path const& base, system::error_code* ec = nullptr);
BOOST_FILESYSTEM_DECL
path absolute_v4(path const& p, path const& base, system::error_code* ec = nullptr);
BOOST_FILESYSTEM_DECL
file_status status(path const& p, system::error_code* ec = nullptr);
BOOST_FILESYSTEM_DECL
file_status symlink_status(path const& p, system::error_code* ec = nullptr);
BOOST_FILESYSTEM_DECL
bool is_empty(path const& p, system::error_code* ec = nullptr);
BOOST_FILESYSTEM_DECL
path initial_path(system::error_code* ec = nullptr);
BOOST_FILESYSTEM_DECL
path canonical_v3(path const& p, path const& base, system::error_code* ec = nullptr);
BOOST_FILESYSTEM_DECL
path canonical_v4(path const& p, path const& base, system::error_code* ec = nullptr);
BOOST_FILESYSTEM_DECL
void copy(path const& from, path const& to, copy_options options, system::error_code* ec = nullptr);
BOOST_FILESYSTEM_DECL
bool copy_file(path const& from, path const& to, copy_options options, system::error_code* ec = nullptr);
BOOST_FILESYSTEM_DECL
void copy_symlink(path const& existing_symlink, path const& new_symlink, system::error_code* ec = nullptr);
BOOST_FILESYSTEM_DECL
bool create_directories(path const& p, system::error_code* ec = nullptr);
BOOST_FILESYSTEM_DECL
bool create_directory(path const& p, const path* existing, system::error_code* ec = nullptr);
BOOST_FILESYSTEM_DECL
void create_directory_symlink(path const& to, path const& from, system::error_code* ec = nullptr);
BOOST_FILESYSTEM_DECL
void create_hard_link(path const& to, path const& from, system::error_code* ec = nullptr);
BOOST_FILESYSTEM_DECL
void create_symlink(path const& to, path const& from, system::error_code* ec = nullptr);
BOOST_FILESYSTEM_DECL
path current_path(system::error_code* ec = nullptr);
BOOST_FILESYSTEM_DECL
void current_path(path const& p, system::error_code* ec = nullptr);
BOOST_FILESYSTEM_DECL
bool equivalent_v3(path const& p1, path const& p2, system::error_code* ec = nullptr);
BOOST_FILESYSTEM_DECL
bool equivalent_v4(path const& p1, path const& p2, system::error_code* ec = nullptr);
BOOST_FILESYSTEM_DECL
boost::uintmax_t file_size(path const& p, system::error_code* ec = nullptr);
BOOST_FILESYSTEM_DECL
boost::uintmax_t hard_link_count(path const& p, system::error_code* ec = nullptr);
BOOST_FILESYSTEM_DECL
std::time_t creation_time(path const& p, system::error_code* ec = nullptr);
BOOST_FILESYSTEM_DECL
std::time_t last_write_time(path const& p, system::error_code* ec = nullptr);
BOOST_FILESYSTEM_DECL
void last_write_time(path const& p, const std::time_t new_time, system::error_code* ec = nullptr);
BOOST_FILESYSTEM_DECL
void permissions(path const& p, perms prms, system::error_code* ec = nullptr);
BOOST_FILESYSTEM_DECL
path read_symlink(path const& p, system::error_code* ec = nullptr);
BOOST_FILESYSTEM_DECL
path relative(path const& p, path const& base, system::error_code* ec = nullptr);
BOOST_FILESYSTEM_DECL
bool remove(path const& p, system::error_code* ec = nullptr);
BOOST_FILESYSTEM_DECL
boost::uintmax_t remove_all(path const& p, system::error_code* ec = nullptr);
BOOST_FILESYSTEM_DECL
void rename(path const& old_p, path const& new_p, system::error_code* ec = nullptr);
BOOST_FILESYSTEM_DECL
void resize_file(path const& p, uintmax_t size, system::error_code* ec = nullptr);
BOOST_FILESYSTEM_DECL
space_info space(path const& p, system::error_code* ec = nullptr);
BOOST_FILESYSTEM_DECL
path system_complete(path const& p, system::error_code* ec = nullptr);
BOOST_FILESYSTEM_DECL
path temp_directory_path(system::error_code* ec = nullptr);
BOOST_FILESYSTEM_DECL
path unique_path(path const& p, system::error_code* ec = nullptr);
BOOST_FILESYSTEM_DECL
path weakly_canonical_v3(path const& p, path const& base, system::error_code* ec = nullptr);
BOOST_FILESYSTEM_DECL
path weakly_canonical_v4(path const& p, path const& base, system::error_code* ec = nullptr);

} // namespace detail

//--------------------------------------------------------------------------------------//
//                                                                                      //
//                             status query functions                                   //
//                                                                                      //
//--------------------------------------------------------------------------------------//

//! \name Status query functions
//! @{

/*!
 * \brief Returns `file_status` for the file identified by `p`, following symbolic link resolution.
 *
 * \effects
 * Determines the attributes and access permissions of the file `p` resolves to, as if by ISO/IEC 9945
 * `stat()`.
 *
 * \note If `p` resolves to a symbolic link, the behavior is as if the operation restarts with `p` being
 *       equal to the contents of the symbolic link. There may be a platform-specific limit on the number
 *       of symbolic link resolutions performed or other conditions for detecting symbolic link cycles. If
 *       such limit or conditions are reached, the operation returns with an error indication. Otherwise,
 *       if `p` does not resolve to a symbolic link, the effects are equivalent to `symlink_status()`.
 *
 * \param p Path to the file to obtain information about.
 * \param ec Error code returned in case of failure.
 *
 * \returns If symbolic link resolution in `p` succeeds (i.e. produces a path that does not resolve to
 *          a symbolic link), returns `symlink_status()` on the resolved path. Otherwise, returns
 *          `file_status(status_error)`.
 *
 * \error_reporting
 * The overload that is not taking the `ec` argument throws `filesystem_error` only if the operation would have
 * returned `file_status(status_error)`. Other unsuccessful cases are indicated with the `file_status` returned
 * normally.
 *
 * The overload that takes the `ec` argument sets `ec` to a non-empty `error_code` if the operation fails
 * to determine the file attributes and access permissions (e.g. if the path does not resolve to an existing file).
 *
 * \sa \ref symlink_status.
 */
inline file_status status(path const& p, system::error_code& ec) noexcept
{
    return detail::status(p, &ec);
}

/*! \overload */
inline file_status status(path const& p)
{
    return detail::status(p);
}

/*!
 * \brief Returns `file_status` for the file identified by `p`.
 *
 * \effects
 * Determines the attributes and access permissions of the file `p` resolves to, as if by ISO/IEC 9945
 * `lstat()`. If determining succeeded:
 *
 * - Based on the determined attributes, select a `file_type` enum value `ft` as follows:
 *
 *     - If the attributes indicate a symbolic link, as if by ISO/IEC 9945
 *       [`S_IFLNK()`](http://www.opengroup.org/onlinepubs/000095399/basedefs/sys/stat.h.html),
 *       then `ft = symlink_file`.
 *     - If the attributes indicate a regular file, as if by ISO/IEC 9945
 *       [`S_ISREG()`](http://www.opengroup.org/onlinepubs/000095399/basedefs/sys/stat.h.html),
 *       then `ft = regular_file`.
 *     - Otherwise, if the attributes indicate a directory, as if by ISO/IEC 9945
 *       [`S_ISDIR()`](http://www.opengroup.org/onlinepubs/000095399/basedefs/sys/stat.h.html),
 *       then `ft = directory_file`.
 *     - Otherwise, if the attributes indicate a block special file, as if by ISO/IEC 9945
 *       [`S_ISBLK()`](http://www.opengroup.org/onlinepubs/000095399/basedefs/sys/stat.h.html),
 *       then `ft = block_file`.
 *     - Otherwise, if the attributes indicate a character special file, as if by ISO/IEC 9945
 *       [`S_ISCHR()`](http://www.opengroup.org/onlinepubs/000095399/basedefs/sys/stat.h.html),
 *       then `ft = character_file`.
 *     - Otherwise, if the attributes indicate a fifo or pipe file, as if by ISO/IEC 9945
 *       [`S_ISFIFO()`](http://www.opengroup.org/onlinepubs/000095399/basedefs/sys/stat.h.html),
 *       then `ft = fifo_file`.
 *     - Otherwise, if the attributes indicate a socket, as if by ISO/IEC 9945
 *       [`S_ISSOCK()`](http://www.opengroup.org/onlinepubs/000095399/basedefs/sys/stat.h.html),
 *       then `ft = socket_file`.
 *     - Otherwise, on Windows, if the attributes indicate a reparse point, then `ft = reparse_file`.
 *     - Otherwise, `ft = type_unknown`.
 *
 *     \remark `regular_file` implies appropriate `<fstream>` operations would succeed, assuming no hardware,
 *             permission, access, or file system race errors. Lack of `regular_file` does not necessarily imply
 *             `<fstream>` operations would fail on a directory. `directory_file` implies `directory_iterator(p)`
 *             would succeed.
 *
 *     \note On Windows, symbolic links are implemented as reparse points. The library recognizes this
 *           kind of reparse points, as well as junction reparse points, and reports such reparse points as
 *           `symlink_file`. `reparse_file` file type is reported for reparse points of unsupported types
 *           or when the reparse point doesn't fit into one of the more specialized categories. For ISO/IEC
 *           9945, reparse points are not supported and never reported.
 *
 * - Based on the determined file access permissions, compute `perms` value `prm` as a combination of the
 *   following bits:
 *
 *   | Permission              | `perms` bit      | [ISO/IEC 9945 equivalent bit](http://www.opengroup.org/onlinepubs/000095399/basedefs/sys/stat.h.html) |
 *   |-------------------------|------------------|-----------|
 *   | Read by owner           | `owner_read`     | `S_IRUSR` |
 *   | Write by owner          | `owner_write`    | `S_IWUSR` |
 *   | Execute by owner        | `owner_exe`      | `S_IXUSR` |
 *   | Read by group           | `group_read`     | `S_IRGRP` |
 *   | Write by group          | `group_write`    | `S_IWGRP` |
 *   | Execute by group        | `group_exe`      | `S_IXGRP` |
 *   | Read by others          | `others_read`    | `S_IROTH` |
 *   | Write by others         | `others_write`   | `S_IWOTH` |
 *   | Execute by others       | `others_exe`     | `S_IXOTH` |
 *   | Set-user-ID on execute  | `set_uid_on_exe` | `S_ISUID` |
 *   | Set-group-ID on execute | `set_gid_on_exe` | `S_ISGID` |
 *   | Sticky bit              | `sticky_bit`     | `S_ISVTX` |
 *
 *   \remark Support and semantics of the sticky bit may vary across platforms.
 *
 *   \note On Windows, some permissions may not be reported accurately. Currently, all files are reported
 *         as readable by all users. Files not marked as read-only are reported as writable for all users.
 *         For certain well known executable file formats, based on the file name, the library reports
 *         executable permission for all users. The library does not use ACL to produce or verify these
 *         permissions.
 *
 * - The returned object is constructed from `ft` and `prm` as if by calling `file_status(ft, prm)`.
 *
 * Otherwise, if determining file attributes failed:
 *
 * - If the specific error indicates that `p` cannot be resolved because some element of the path does not
 *   exist, return `file_status(file_not_found)`.
 *   \note ISO/IEC 9945 errors that indicate this are `ENOENT` or `ENOTDIR`.
 *         Windows equivalents include `ERROR_FILE_NOT_FOUND`, `ERROR_PATH_NOT_FOUND`, `ERROR_INVALID_NAME`,
 *         `ERROR_INVALID_PARAMETER`, `ERROR_BAD_PATHNAME`, and `ERROR_BAD_NETPATH`. This list of error codes
 *         may not be exhaustive.
 * - Otherwise, if the specific error indicates that `p` can be resolved but the attributes cannot be determined,
 *   return `file_status(type_unknown)`.
 *   \note For example, Windows `ERROR_SHARING_VIOLATION` errors. For ISO/IEC 9945, the case never arises.
 * - Otherwise, return `file_status(status_error)`.
 *
 * \remark These semantics distinguish between `p` being known not to exist, `p` existing but not being able
 *         to determine its attributes, and there being an error that prevents even knowing if `p` exists. These
 *         distinctions are important to some use cases.
 *
 * \param p Path to the file to obtain information about.
 * \param ec Error code returned in case of failure.
 *
 * \returns The `file_status` object constructed as described above.
 *
 * \error_reporting
 * The overload that is not taking the `ec` argument throws `filesystem_error` only if the operation would have
 * returned `file_status(status_error)`. Other unsuccessful cases are indicated with the `file_status` returned
 * normally.
 *
 * The overload that takes the `ec` argument sets `ec` to a non-empty `error_code` if the operation fails
 * to determine the file attributes and access permissions (e.g. if the path does not resolve to an existing file).
 */
inline file_status symlink_status(path const& p, system::error_code& ec) noexcept
{
    return detail::symlink_status(p, &ec);
}

/*! \overload */
inline file_status symlink_status(path const& p)
{
    return detail::symlink_status(p);
}

/*!
 * \brief Checks if file exists.
 *
 * \effects Determines `file_status` of `p`, as if by `status(p, ec)`.
 *
 * \param p The path to query.
 * \param ec The error code to set on failure.
 *
 * \returns `true` if `status(p, ec).type()` is not `status_error` or `file_not_found`, `false` otherwise.
 */
inline bool exists(path const& p, system::error_code& ec) noexcept
{
    return filesystem::exists(detail::status(p, &ec));
}

/*! \overload */
inline bool exists(path const& p)
{
    return filesystem::exists(detail::status(p));
}

/*!
 * \brief Checks if file is a regular file.
 *
 * \effects Determines `file_status` of `p`, as if by `status(p, ec)`.
 *
 * \param p The path to query.
 * \param ec The error code to set on failure.
 *
 * \returns `true` if `status(p, ec).type() == regular_file`, `false` otherwise.
 */
inline bool is_regular_file(path const& p, system::error_code& ec) noexcept
{
    return filesystem::is_regular_file(detail::status(p, &ec));
}

/*! \overload */
inline bool is_regular_file(path const& p)
{
    return filesystem::is_regular_file(detail::status(p));
}

/*!
 * \brief Checks if file is a directory.
 *
 * \effects Determines `file_status` of `p`, as if by `status(p, ec)`.
 *
 * \param p The path to query.
 * \param ec The error code to set on failure.
 *
 * \returns `true` if `status(p, ec).type() == directory_file`, `false` otherwise.
 */
inline bool is_directory(path const& p, system::error_code& ec) noexcept
{
    return filesystem::is_directory(detail::status(p, &ec));
}

/*! \overload */
inline bool is_directory(path const& p)
{
    return filesystem::is_directory(detail::status(p));
}

/*!
 * \brief Checks if file is a symlink.
 *
 * \effects Determines `file_status` of `p`, as if by `symlink_status(p, ec)`.
 *
 * \param p The path to query.
 * \param ec The error code to set on failure.
 *
 * \returns `true` if `symlink_status(p, ec).type() == symlink_file`, `false` otherwise.
 */
inline bool is_symlink(path const& p, system::error_code& ec) noexcept
{
    return filesystem::is_symlink(detail::symlink_status(p, &ec));
}

/*! \overload */
inline bool is_symlink(path const& p)
{
    return filesystem::is_symlink(detail::symlink_status(p));
}

/*!
 * \brief Checks if file is a block special file.
 *
 * \effects Determines `file_status` of `p`, as if by `status(p, ec)`.
 *
 * \param p The path to query.
 * \param ec The error code to set on failure.
 *
 * \returns `true` if `status(p, ec).type() == block_file`, `false` otherwise.
 */
inline bool is_block_file(path const& p, system::error_code& ec) noexcept
{
    return filesystem::is_block_file(detail::status(p, &ec));
}

/*! \overload */
inline bool is_block_file(path const& p)
{
    return filesystem::is_block_file(detail::status(p));
}

/*!
 * \brief Checks if file is a character special file.
 *
 * \effects Determines `file_status` of `p`, as if by `status(p, ec)`.
 *
 * \param p The path to query.
 * \param ec The error code to set on failure.
 *
 * \returns `true` if `status(p, ec).type() == character_file`, `false` otherwise.
 */
inline bool is_character_file(path const& p, system::error_code& ec) noexcept
{
    return filesystem::is_character_file(detail::status(p, &ec));
}

/*! \overload */
inline bool is_character_file(path const& p)
{
    return filesystem::is_character_file(detail::status(p));
}

/*!
 * \brief Checks if file is a FIFO or pipe file.
 *
 * \effects Determines `file_status` of `p`, as if by `status(p, ec)`.
 *
 * \param p The path to query.
 * \param ec The error code to set on failure.
 *
 * \returns `true` if `status(p, ec).type() == fifo_file`, `false` otherwise.
 */
inline bool is_fifo(path const& p, system::error_code& ec) noexcept
{
    return filesystem::is_fifo(detail::status(p, &ec));
}

/*! \overload */
inline bool is_fifo(path const& p)
{
    return filesystem::is_fifo(detail::status(p));
}

/*!
 * \brief Checks if file is a socket file.
 *
 * \effects Determines `file_status` of `p`, as if by `status(p, ec)`.
 *
 * \param p The path to query.
 * \param ec The error code to set on failure.
 *
 * \returns `true` if `status(p, ec).type() == socket_file`, `false` otherwise.
 */
inline bool is_socket(path const& p, system::error_code& ec) noexcept
{
    return filesystem::is_socket(detail::status(p, &ec));
}

/*! \overload */
inline bool is_socket(path const& p)
{
    return filesystem::is_socket(detail::status(p));
}

/*!
 * \brief Checks if file is a reparse point file.
 *
 * \effects Determines `file_status` of `p`, as if by `symlink_status(p, ec)`.
 *
 * \param p The path to query.
 * \param ec The error code to set on failure.
 *
 * \returns `true` if `symlink_status(p, ec).type() == reparse_file`, `false` otherwise.
 */
inline bool is_reparse_file(path const& p, system::error_code& ec) noexcept
{
    return filesystem::is_reparse_file(detail::symlink_status(p, &ec));
}

/*! \overload */
inline bool is_reparse_file(path const& p)
{
    return filesystem::is_reparse_file(detail::symlink_status(p));
}

/*!
 * \brief Checks if file is an other type.
 *
 * \effects Determines `file_status` `s` of `p`, as if by `s = status(p, ec)`.
 *
 * \param p The path to query.
 * \param ec The error code to set on failure.
 *
 * \returns `true` if `exists(s) && !is_regular_file(s) && !is_directory(s)`, `false` otherwise.
 */
inline bool is_other(path const& p, system::error_code& ec) noexcept
{
    return filesystem::is_other(detail::status(p, &ec));
}

/*! \overload */
inline bool is_other(path const& p)
{
    return filesystem::is_other(detail::status(p));
}

/*!
 * \brief Checks if file or directory is empty.
 *
 * \param p The path to query.
 * \param ec The error code to set on failure.
 *
 * \returns `is_directory(p, ec) ? directory_iterator(p, ec) == directory_iterator() : file_size(p, ec) == 0`.
 */
inline bool is_empty(path const& p, system::error_code& ec)
{
    return detail::is_empty(p, &ec);
}

/*! \overload */
inline bool is_empty(path const& p)
{
    return detail::is_empty(p);
}

//! @}

//--------------------------------------------------------------------------------------//
//                                                                                      //
//                   attributes querying and manipulation functions                     //
//                                                                                      //
//--------------------------------------------------------------------------------------//

//! \name File attributes querying and manipulation
//! @{

/*!
 * \brief Returns the file size.
 *
 * \param p The path to the file.
 * \param ec The error code to set on failure.
 *
 * \returns If `exists(p) && is_regular_file(p)`, the size in bytes of the file `p` resolves to, determined as if
 *          by the value of the ISO/IEC 9945 [`stat`](http://www.opengroup.org/onlinepubs/000095399/basedefs/sys/stat.h.html)
 *          structure member `st_size` obtained as if by ISO/IEC 9945
 *          [`stat()`](http://www.opengroup.org/onlinepubs/000095399/functions/stat.html). Otherwise,
 *          indicates error and the overload taking `ec` returns `static_cast<uintmax_t>(-1)`.
 */
inline boost::uintmax_t file_size(path const& p, system::error_code& ec) noexcept
{
    return detail::file_size(p, &ec);
}

/*! \overload */
inline boost::uintmax_t file_size(path const& p)
{
    return detail::file_size(p);
}

/*!
 * \brief Resizes a file.
 *
 * \effects
 * Sets the size of the file to which `p` resolves to `size`, as if by ISO/IEC 9945
 * [`truncate()`](http://www.opengroup.org/onlinepubs/000095399/functions/truncate.html).
 *
 * \post `file_size() == size` is `true`.
 *
 * \param p The path to the file.
 * \param size The new size of the file.
 * \param ec The error code to set on failure.
 */
inline void resize_file(path const& p, uintmax_t size, system::error_code& ec) noexcept
{
    detail::resize_file(p, size, &ec);
}

/*! \overload */
inline void resize_file(path const& p, uintmax_t size)
{
    detail::resize_file(p, size);
}

/*!
 * \brief Returns the number of hard links.
 *
 * \param p The path to the file.
 * \param ec The error code to set on failure.
 *
 * \returns The number of hard links for `p`, determined as if by the value of the ISO/IEC 9945
 *          [`stat`](http://www.opengroup.org/onlinepubs/000095399/basedefs/sys/stat.h.html)
 *          structure member `st_nlink` obtained as if by ISO/IEC 9945
 *          [`stat()`](http://www.opengroup.org/onlinepubs/000095399/functions/stat.html). Otherwise,
 *          indicates error and the overload taking `ec` returns `static_cast<uintmax_t>(-1)`.
 */
inline boost::uintmax_t hard_link_count(path const& p, system::error_code& ec) noexcept
{
    return detail::hard_link_count(p, &ec);
}

/*! \overload */
inline boost::uintmax_t hard_link_count(path const& p)
{
    return detail::hard_link_count(p);
}

/*!
 * \brief Returns the creation time of a file.
 *
 * \note Not all platforms support querying file creation time. Where not supported, the operation will fail with
 *       `errc::function_not_supported` error code.
 *
 * \param p The path to the file.
 * \param ec The error code to set on failure.
 *
 * \returns The time of creation of the file to which `p` resolves. In case of error, the overload taking `ec`
 *          returns `std::numeric_limits<std::time_t>::min()`.
 */
inline std::time_t creation_time(path const& p, system::error_code& ec) noexcept
{
    return detail::creation_time(p, &ec);
}

/*! \overload */
inline std::time_t creation_time(path const& p)
{
    return detail::creation_time(p);
}

/*!
 * \brief Returns the last write time of a file.
 *
 * \param p The path to the file.
 * \param ec The error code to set on failure.
 *
 * \returns The time of last data modification of `p`, determined as if by the value of the ISO/IEC 9945
 *          [`stat`](http://www.opengroup.org/onlinepubs/000095399/basedefs/sys/stat.h.html) structure member
 *          `st_mtime` obtained as if by ISO/IEC 9945
 *          [`stat()`](http://www.opengroup.org/onlinepubs/000095399/functions/stat.html). In case of error,
 *          the overload taking `ec` returns `std::numeric_limits<std::time_t>::min()`.
 */
inline std::time_t last_write_time(path const& p, system::error_code& ec) noexcept
{
    return detail::last_write_time(p, &ec);
}

/*! \overload */
inline std::time_t last_write_time(path const& p)
{
    return detail::last_write_time(p);
}

/*!
 * \brief Sets the last write time of a file.
 *
 * \effects
 * Sets the time of last data modification of the file resolved to by `p` to `new_time`, as if by ISO/IEC 9945
 * [`stat()`](http://www.opengroup.org/onlinepubs/000095399/functions/stat.html) followed by ISO/IEC 9945
 * [`utime()`](http://www.opengroup.org/onlinepubs/000095399/functions/utime.html).
 *
 * \note A postcondition of `last_write_time(p) == new_time` is not specified since it might not hold for file systems
 *       with coarse time granularity.
 *
 * \param p The path to the file.
 * \param new_time The new last write time.
 * \param ec The error code to set on failure.
 */
inline void last_write_time(path const& p, const std::time_t new_time, system::error_code& ec) noexcept
{
    detail::last_write_time(p, new_time, &ec);
}

/*! \overload */
inline void last_write_time(path const& p, const std::time_t new_time)
{
    detail::last_write_time(p, new_time);
}

/*!
 * \brief Applies file access permissions.
 *
 * \effects
 * Applies the effective permission bits from `prms` to the file `p` resolves to, as if by ISO/IEC 9945
 * [`fchmodat()`](http://pubs.opengroup.org/onlinepubs/9699919799/functions/fchmodat.html). The effective permission
 * bits are determined as specified by the following table.
 *
 * | bits present in `prms`                 | Effective bits applied                           |
 * |----------------------------------------|--------------------------------------------------|
 * | Neither `add_perms` nor `remove_perms` | `prms & perms_mask`                              |
 * | `add_perms`                            | `status(p).permissions() | (prms & perms_mask)`  |
 * | `remove_perms`                         | `status(p).permissions() & ~(prms & perms_mask)` |
 *
 * \note Conceptually permissions are viewed as bits, but the actual implementation may use some other mechanism.
 *
 * \pre `!((prms & add_perms) && (prms & remove_perms))`.
 *
 * \param p The path to the file.
 * \param prms The permissions to apply.
 * \param ec The error code to set on failure.
 */
inline void permissions(path const& p, perms prms, system::error_code& ec) noexcept
{
    detail::permissions(p, prms, &ec);
}

/*! \overload */
inline void permissions(path const& p, perms prms)
{
    detail::permissions(p, prms);
}


//--------------------------------------------------------------------------------------//
//                                                                                      //
//                          path manipulation functions                                 //
//                                                                                      //
//--------------------------------------------------------------------------------------//

//! \name Path manipulation functions
//! @{

/*!
 * \brief Returns `current_path()` as of first call to `initial_path()`.
 *
 * \note `initial_path()` is not thread safe, and may return an undesirable result if called subsequent to a change
 *       to the current directory. These problems can be avoided by calling `initial_path()` immediately on entry to
 *       `main()`.
 *
 * \param ec The error code to set on failure.
 *
 * \returns The current working directory path, as it was on the first call to `initial_path()`. If an error occurs,
 *          the overload taking `ec` returns an empty path.
 */
inline path initial_path(system::error_code& ec)
{
    return detail::initial_path(&ec);
}

/*! \overload */
inline path initial_path()
{
    return detail::initial_path();
}

#if BOOST_FILESYSTEM_VERSION < 4 && !defined(BOOST_FILESYSTEM_NO_DEPRECATED) && !defined(BOOST_FILESYSTEM_DOXYGEN)

template< class Path >
BOOST_FILESYSTEM_DETAIL_DEPRECATED("Use the non-templated `initial_path` overload")
path initial_path()
{
    return initial_path();
}

template< class Path >
BOOST_FILESYSTEM_DETAIL_DEPRECATED("Use the non-templated `initial_path` overload")
path initial_path(system::error_code& ec)
{
    return detail::initial_path(&ec);
}

#endif // BOOST_FILESYSTEM_VERSION < 4 && !defined(BOOST_FILESYSTEM_NO_DEPRECATED) && !defined(BOOST_FILESYSTEM_DOXYGEN)

/*!
 * \brief Returns the current working directory path.
 *
 * \param ec The error code to set on failure.
 *
 * \returns The current working directory path, as if by ISO/IEC 9945
 *          [`getcwd()`](http://www.opengroup.org/onlinepubs/000095399/functions/getcwd.html).
 *          `is_absolute()` is `true` for the returned path. An empty path is returned in case of error.
 *
 * \remark The `current_path()` name was chosen to emphasize that the return is a path, not just a single directory name.
 *
 * \note The current path as returned by many operating systems is a dangerous global variable. It may be changed
 *       unexpectedly by a third-party or system library functions, or by another thread.
 */
inline path current_path(system::error_code& ec)
{
    return detail::current_path(&ec);
}

/*!
 * \brief Returns the current working directory path.
 *
 * \returns The current working directory path, as if by ISO/IEC 9945
 *          [`getcwd()`](http://www.opengroup.org/onlinepubs/000095399/functions/getcwd.html).
 *          `is_absolute()` is `true` for the returned path.
 *
 * \remark The `current_path()` name was chosen to emphasize that the return is a path, not just a single directory name.
 *
 * \note The current path as returned by many operating systems is a dangerous global variable. It may be changed
 *       unexpectedly by a third-party or system library functions, or by another thread.
 */
inline path current_path()
{
    return detail::current_path();
}

/*!
 * \brief Sets the current path.
 *
 * \effects Establishes the postcondition, as if by ISO/IEC 9945
 *          [`chdir()`](http://www.opengroup.org/onlinepubs/000095399/functions/chdir.html).
 *
 * \post `equivalent(p, current_path(ec))` is `true`.
 *
 * \param p The path to set as the current path.
 * \param ec The error code to set on failure.
 *
 * \note The current path as returned by many operating systems is a dangerous global variable. It may be changed
 *       unexpectedly by a third-party or system library functions, or by another thread.
 */
inline void current_path(path const& p, system::error_code& ec) noexcept
{
    detail::current_path(p, &ec);
}

/*!
 * \brief Sets the current path.
 *
 * \effects Establishes the postcondition, as if by ISO/IEC 9945
 *          [`chdir()`](http://www.opengroup.org/onlinepubs/000095399/functions/chdir.html).
 *
 * \post `equivalent(p, current_path())` is `true`.
 *
 * \param p The path to set as the current path.
 *
 * \note The current path as returned by many operating systems is a dangerous global variable. It may be changed
 *       unexpectedly by a third-party or system library functions, or by another thread.
 */
inline void current_path(path const& p)
{
    detail::current_path(p);
}

/*!
 * \brief Returns the temporary directory path.
 *
 * \remark The `temp_directory_path()` name was chosen to emphasize that the return is a path, not just a single
 *         directory name.
 *
 * \overview
 * The specifics of how the temporary directory path is determined are platform-dependent:
 *
 * - ISO/IEC 9945: The path supplied by the first environment variable found in the list `TMPDIR`, `TMP`, `TEMP`,
 *   `TEMPDIR`. If none of these are found, "/data/local/tmp" on Android, otherwise "/tmp".
 * - Windows: The path reported by the `GetTempPath` API function.
 *
 * \param ec The error code to set on failure.
 *
 * \returns A directory path suitable for temporary files under the conventions of the operating system. An error shall
 *          be reported if `!exists(p) || !is_directory(p)`, where `p` is the path to be returned. In case of error,
 *          the overload taking `ec` returns an empty path.
 */
inline path temp_directory_path(system::error_code& ec)
{
    return detail::temp_directory_path(&ec);
}

/*! \overload */
inline path temp_directory_path()
{
    return detail::temp_directory_path();
}

/*!
 * \brief Generates a unique path.
 *
 * \overview
 * The `unique_path` function generates a path name suitable for creating temporary files, including directories.
 * The name is based on a model that uses the percent sign character to specify replacement by a random hexadecimal
 * digit.
 *
 * \note The more bits of randomness in the generated path name, the less likelihood of prior existence or being
 *       guessed. Each replacement hexadecimal digit in the model adds four bits of randomness. The default model thus
 *       provides 64 bits of randomness. This is sufficient for most applications.
 *
 * \note Implementations are encouraged to obtain the required randomness via a cryptographically secure pseudo-random
 *       number generator, such as one provided by the operating system. Such generators may block until sufficient
 *       entropy develops.
 *
 * \param model The path model to use for generating the unique path. "%%%%-%%%%-%%%%-%%%%" if not specified.
 * \param ec The error code to set on failure.
 *
 * \returns A path identical to `model`, except that each occurrence of a percent sign character ('\%') is replaced
 *          by a random hexadecimal digit character in the range 0-9, a-f. In case of error, the overload taking `ec`
 *          returns an empty path.
 */
inline path unique_path(path const& model, system::error_code& ec)
{
    return detail::unique_path(model, &ec);
}

/*! \overload */
inline path unique_path(system::error_code& ec)
{
    return detail::unique_path
    (
#if defined(BOOST_FILESYSTEM_WINDOWS_API)
        L"%%%%-%%%%-%%%%-%%%%",
#else
        "%%%%-%%%%-%%%%-%%%%",
#endif
        &ec
    );
}

/*! \overload */
inline path unique_path(path const& model =
#if defined(BOOST_FILESYSTEM_WINDOWS_API)
    L"%%%%-%%%%-%%%%-%%%%"
#else
    "%%%%-%%%%-%%%%-%%%%"
#endif
)
{
    return detail::unique_path(model);
}

/*!
 * \brief Returns a relative path.
 *
 * \overview
 * Returns `p` made relative to `base`. Treats empty or identical paths as corner cases, not errors. Resolves symlinks
 * and normalizes both `p` and `base` before other processing.
 *
 * \post The returned path is in normal form.
 *
 * \param p The path to be made relative.
 * \param base The base path to make `p` relative to. `current_path()` if not specified.
 * \param ec The error code to set on failure.
 *
 * \returns `weakly_canonical(p).lexically_relative(weakly_canonical(base))`. Overloads taking `ec` return an empty
 *          path in case of error.
 */
inline path relative(path const& p, path const& base, system::error_code& ec)
{
    return detail::relative(p, base, &ec);
}

/*! \overload */
inline path relative(path const& p, system::error_code& ec)
{
    path base = current_path(ec);
    if (ec)
        return path();
    return detail::relative(p, base, &ec);
}

/*! \overload */
inline path relative(path const& p, path const& base = current_path())
{
    return detail::relative(p, base);
}

/*!
 * \brief Returns the system complete path.
 *
 * \effects
 * Composes an absolute path from `p`, using the same rules used by the operating system to resolve a path passed as
 * the filename argument to standard library open functions.
 *
 * \note For ISO/IEC 9945, `system_complete(p)` has the same semantics as `absolute(p, current_path())`. For Windows,
 *       `system_complete(p)` has the same semantics as `absolute(p, current_path())` if
 *       `p.is_absolute() || !p.has_root_name()` or `p` and `base` have the same `root_name()`. Otherwise it acts like
 *       `absolute(p, cur)`, where `cur` is the current directory for the `p.root_name()` drive. This will be
 *       the current directory of that drive the last time it was set, and thus **may be residue left over from a prior
 *       program run by the command processor**. Although these semantics are often useful, they are also very
 *       error-prone.
 *
 * \post For the returned path `rp`, `rp.is_absolute()` is `true`.
 *
 * \param p The path to be completed.
 * \param ec The error code to set on failure.
 *
 * \returns The composed path. In case of error, the overload taking `ec` returns an empty path.
 */
inline path system_complete(path const& p, system::error_code& ec)
{
    return detail::system_complete(p, &ec);
}

/*! \overload */
inline path system_complete(path const& p)
{
    return detail::system_complete(p);
}

#if !defined(BOOST_FILESYSTEM_DOXYGEN)
namespace BOOST_FILESYSTEM_VERSION_NAMESPACE {
#endif // !defined(BOOST_FILESYSTEM_DOXYGEN)

/*!
 * \brief Constructs an absolute path from `p` and `base`.
 *
 * \overview
 * The absolute path is composed according to the following table:
 *
 * |                      | `p.has_root_directory()` | `!p.has_root_directory()` |
 * |----------------------|--------------------------|-------------------------|
 * | `p.has_root_name()`  | `return p`               | `return p.root_name() / absolute(base).root_directory() / absolute(base).relative_path() / p.relative_path()` |
 * | `!p.has_root_name()` | `return absolute(base).root_name() / p` | `return absolute(base) / p` |
 *
 * \post For the returned path `rp`, `rp.is_absolute()` is `true`.
 *
 * \param p The path to make absolute.
 * \param base The path to use as a base for `p`. `current_path()` if not specified.
 * \param ec The error code to set on failure.
 *
 * \returns If `p.is_absolute()` is `true` then `p`, otherwise an absolute path composed according to the table above.
 *          In case of error, the overloads taking `ec` return an empty path.
 */
inline path absolute(path const& p, path const& base, system::error_code& ec)
{
    return BOOST_FILESYSTEM_VERSIONED_SYM(detail::absolute)(p, base, &ec);
}

/*! \overload */
inline path absolute(path const& p, system::error_code& ec)
{
    path base = current_path(ec);
    if (ec)
        return path();
    return BOOST_FILESYSTEM_VERSIONED_SYM(detail::absolute)(p, base, &ec);
}

/*! \overload */
inline path absolute(path const& p, path const& base = current_path())
{
    return BOOST_FILESYSTEM_VERSIONED_SYM(detail::absolute)(p, base);
}

/*!
 * \brief Converts `p` to canonical path relative to `base`.
 *
 * \overview
 * Converts `p` to an absolute path that has no symbolic link, dot, or dot-dot elements. Testing path elements for
 * symbolic links is done as if by calling `is_symlink()` and requires `absolute(p, base)` to exist.
 *
 * \note `!exists(absolute(p, base))` is an error.
 *
 * \note Canonical pathnames allow security checking of a path (e.g. does this path live in "/home/goodguy" or
 *       "/home/badguy"?).
 *
 * \param p The path to canonicalize.
 * \param base The path to use as a base for `p`. `current_path()` if not specified.
 * \param ec The error code to set on failure.
 *
 * \returns A canonical path that refers to the same file system object as `absolute(p, base)`. In case of error,
 *          the overloads taking `ec` return an empty path.
 */
inline path canonical(path const& p, path const& base, system::error_code& ec)
{
    return BOOST_FILESYSTEM_VERSIONED_SYM(detail::canonical)(p, base, &ec);
}

/*! \overload */
inline path canonical(path const& p, system::error_code& ec)
{
    path base = current_path(ec);
    if (ec)
        return path();
    return BOOST_FILESYSTEM_VERSIONED_SYM(detail::canonical)(p, base, &ec);
}

/*! \overload */
inline path canonical(path const& p, path const& base = current_path())
{
    return BOOST_FILESYSTEM_VERSIONED_SYM(detail::canonical)(p, base);
}

/*!
 * \brief Returns path with symlinks resolved and result normalized.
 *
 * \effects:
 * Let `head` be the path composed of the leading elements of `p` that exist and `tail` - from the rest of `p`.
 * Calls `canonical(head, base)` or `canonical(head, base, ec)`, appends `tail` to the returned path as if by using
 * `operator/`. The result is then normalized and returned.
 *
 * \note Uses `operator/=` to compose the returned path. Uses the `status()` function to determine existence.
 *
 * \remark Implementations are encouraged to avoid unnecessary normalization such as when `canonical()` has already
 *         been called on the entirety of `p`.
 *
 * \post The returned path is in normal form.
 *
 * \param p The path to canonicalize.
 * \param base The path to use as a base for `p`. `current_path()` if not specified.
 * \param ec The error code to set on failure.
 *
 * \returns The canonicalized path. In case of error, the overloads taking `ec` return an empty path.
 *
 * \sa \ref canonical.
 */
inline path weakly_canonical(path const& p, path const& base, system::error_code& ec)
{
    return BOOST_FILESYSTEM_VERSIONED_SYM(detail::weakly_canonical)(p, base, &ec);
}

/*! \overload */
inline path weakly_canonical(path const& p, system::error_code& ec)
{
    path base = current_path(ec);
    if (ec)
        return path();
    return BOOST_FILESYSTEM_VERSIONED_SYM(detail::weakly_canonical)(p, base, &ec);
}

/*! \overload */
inline path weakly_canonical(path const& p, path const& base = current_path())
{
    return BOOST_FILESYSTEM_VERSIONED_SYM(detail::weakly_canonical)(p, base);
}

#if !defined(BOOST_FILESYSTEM_DOXYGEN)
} // namespace BOOST_FILESYSTEM_VERSION_NAMESPACE
#endif // !defined(BOOST_FILESYSTEM_DOXYGEN)

//! @}

//--------------------------------------------------------------------------------------//
//                                                                                      //
//                             operational functions                                    //
//                                                                                      //
//--------------------------------------------------------------------------------------//

//! \name Operational functions
//! @{

/*!
 * \brief Copies files or directories.
 *
 * \effects
 * In the following description, assume any errors reported by the mentioned calls propagate immediately to the caller,
 * either as an exception or by setting the `ec` argument to the error code, as described in the Error reporting section.
 *
 * Let `f` and `t` be `file_status` objects obtained the following way:
 *
 * - If `(options & (copy_options::create_symlinks | copy_options::skip_symlinks)) != copy_options::none` then
 *   `f = symlink_status(from)` and `t = symlink_status(to)`;
 * - Otherwise, if `(options & copy_options::copy_symlinks) != copy_options::none` then `f = symlink_status(from)`
 *   and `t = status(to)`;
 * - Otherwise, `f = status(from)` and `t = status(to)`.
 *
 * Then, report an error if:
 *
 * - `!exists(f)`, or
 * - `equivalent(from, to, ec)`, or
 * - `is_other(f) || is_other(t)`, or
 * - `is_directory(f) && is_regular_file(t)`
 *
 * Otherwise, if `is_symlink(f)`, then:
 *
 * - If `(options & copy_options::skip_symlinks) != copy_options::none` then return;
 * - Otherwise if `!exists(to) && (options & copy_options::copy_symlinks) != copy_options::none` then
 *   `copy_symlink(from, to)`;
 * - Otherwise report error.
 *
 * Otherwise, if `is_regular_file(f)`, then:
 *
 * - If `(options & copy_options::directories_only) != copy_options::none` then return;
 * - Otherwise if `(options & copy_options::create_symlinks) != copy_options::none` then `create_symlink(link, to)`,
 *   where `link` is determined as follows:
 *   - If `from.is_absolute()` then `link` equals to `from`;
 *   - Otherwise, `link` is equivalent to `relative(absolute(to.parent_path()), absolute(from.parent_path())) / from.filename()`.
 * - Otherwise if `(options & copy_options::create_hard_links) != copy_options::none` then `create_hard_link(from, to)`;
 * - Otherwise if `is_directory(to)` then `copy_file(from, to / from.filename(), options)`;
 * - Otherwise `copy_file(from, to, options)`;
 *
 * Otherwise, if `is_directory(f)`, then:
 *
 * - If `(options & copy_options::create_symlinks) != copy_options::none` then report error with error code equal to
 *   `make_error_code(system::errc::is_a_directory);`
 * - Otherwise if `(options & copy_options::recursive) != copy_options::none`, or `options == copy_options::none` and
 *   this call to `copy` is not a recursive call from `copy` then:
 *   - If `!exists(t)`, then `create_directory(to, from)`.
 *   - Then, iterate over files in `from` and for each `directory_entry x` obtained during iteration invoke
 *     `copy(x.path(), to / x.path().filename(), options)`;
 * - Otherwise, return.
 *
 * Otherwise, for all unsupported file types of `f` report error.
 *
 * \pre `options` must contain at most one option from each of the following groups:
 * - `copy_options::skip_existing`, `copy_options::overwrite_existing` or
 *   `copy_options::update_existing;`
 * - `copy_options::synchronize_data` or `copy_options::synchronize;`
 * - `copy_options::ignore_attribute_errors;`
 * - `copy_options::recursive;`
 * - `copy_options::copy_symlinks` or `copy_options::skip_symlinks;`
 * - `copy_options::directories_only`, `copy_options::create_symlinks` or
 *   `copy_options::create_hard_links;`
 *
 * \param from The path to the file or directory to be copied.
 * \param to The path to the destination.
 * \param options Copy options to control the behavior of the operation. `copy_options::none` if not specified.
 * \param ec The error code to set on failure.
 */
inline void copy(path const& from, path const& to, copy_options options, system::error_code& ec) noexcept
{
    detail::copy(from, to, options, &ec);
}

/*! \overload */
inline void copy(path const& from, path const& to, system::error_code& ec) noexcept
{
    detail::copy(from, to, copy_options::none, &ec);
}

/*! \overload */
inline void copy(path const& from, path const& to, copy_options options)
{
    detail::copy(from, to, options);
}

/*! \overload */
inline void copy(path const& from, path const& to)
{
    detail::copy(from, to, copy_options::none);
}

/*!
 * \brief Copies a file.
 *
 * \effects
 * In the following description, assume any errors reported by the mentioned calls propagate immediately to the caller,
 * either as an exception or by setting the `ec` argument to the error code, as described in the Error reporting section.

 * Report an error if:
 *
 * - `!is_regular_file(from)`, or
 * - `exists(to) && !is_regular_file(to)`, or
 * - `exists(to) && equivalent(from, to)`, or
 * - `exists(to) && (options & (copy_options::skip_existing | copy_options::overwrite_existing)) == copy_options::none`;
 *
 * Otherwise, return successfully with no effect if:
 *
 * - `exists(to) && (options & copy_options::skip_existing) != copy_options::none`, or
 * - `exists(to) && (options & copy_options::update_existing) != copy_options::none` and last write time of `to`
 *   is more recent than that of `from`;
 *
 * Otherwise:
 *
 * - The contents and attributes of the file `from` resolves to are copied to the file `to` resolves to. If copying file
 *   attributes (but not contents) fails with an error and `(options & copy_options::ignore_attribute_errors) != copy_options::none`
 *   then that error is ignored. After that,
 * - If `(options & copy_options::synchronize) != copy_options::none`, the written data and attributes are synchronized
 *   with the permanent storage; otherwise
 * - If `(options & copy_options::synchronize_data) != copy_options::none`, the written data is synchronized with
 *   the permanent storage.
 *
 * \note When `copy_options::update_existing` is specified, checking the write times of `from` and `to` may not be
 *       atomic with the copy operation. Another process may create or modify the file identified by `to` after
 *       the file modification times have been checked but before copying starts. In this case the target file
 *       will be overwritten.
 *
 * \note The `copy_options::synchronize_data` and `copy_options::synchronize` options may have a significant
 *       performance impact. The `copy_options::synchronize_data` option may be less expensive than
 *       `copy_options::synchronize`. However, without these options, upon returning from `copy_file` it is not
 *       guaranteed that the copied file is completely written and preserved in case of a system failure. Any delayed
 *       write operations may fail after the function returns, at the point of physically writing the data to
 *       the underlying media, and this error will not be reported to the caller.
 *
 * \note The `copy_options::ignore_attribute_errors` option can be used when the caller does not require file attributes
 *       to be copied. The implementation is permitted to make an attempt to copy the file attributes, but still
 *       succeed the file copying operation if that attempt fails. This option may be useful with file systems that
 *       do not fully support operations of file attributes.
 *
 * \pre `options` must contain at most one option from each of the following groups:
 * - `copy_options::skip_existing`, `copy_options::overwrite_existing` or
 *   `copy_options::update_existing;`
 * - `copy_options::synchronize_data` or `copy_options::synchronize;`
 * - `copy_options::ignore_attribute_errors;`
 *
 * \param from The path to the file to be copied.
 * \param to The path to the destination file.
 * \param options Copy options to control the behavior of the operation. `copy_options::none` if not specified.
 * \param ec The error code to set on failure.
 *
 * \returns `true` if the file was copied without error, `false` otherwise.
 */
inline bool copy_file(path const& from, path const& to, copy_options options, system::error_code& ec) noexcept
{
    return detail::copy_file(from, to, options, &ec);
}

/*! \overload */
inline bool copy_file(path const& from, path const& to, system::error_code& ec) noexcept
{
    return detail::copy_file(from, to, copy_options::none, &ec);
}

/*! \overload */
inline bool copy_file(path const& from, path const& to, copy_options options)
{
    return detail::copy_file(from, to, options);
}

/*! \overload */
inline bool copy_file(path const& from, path const& to)
{
    return detail::copy_file(from, to, copy_options::none);
}

/*!
 * \brief Copies a symbolic link.
 *
 * \effects `create_symlink(read_symlink(existing_symlink, ec), new_symlink, ec)`.
 *
 * \param existing_symlink The path to an existing symbolic link.
 * \param new_symlink The path to the new symbolic link to be created.
 * \param ec The error code to set on failure.
 */
inline void copy_symlink(path const& existing_symlink, path const& new_symlink, system::error_code& ec) noexcept
{
    detail::copy_symlink(existing_symlink, new_symlink, &ec);
}

/*! \overload */
inline void copy_symlink(path const& existing_symlink, path const& new_symlink)
{
    detail::copy_symlink(existing_symlink, new_symlink);
}

/*!
 * \brief Creates directories.
 *
 * \effects Establishes the postcondition by calling `create_directory()` for any element of `p` that does not exist.
 *
 * \post `is_directory(p)` is `true`.
 *
 * \complexity `O(n+1)` where `n` is the number of elements of `p` that do not exist.
 *
 * \param p The path to the directory to be created.
 * \param ec The error code to set on failure.
 *
 * \returns `true` if a new directory was created, `false` otherwise.
 */
inline bool create_directories(path const& p, system::error_code& ec) noexcept
{
    return detail::create_directories(p, &ec);
}

/*! \overload */
inline bool create_directories(path const& p)
{
    return detail::create_directories(p);
}

/*!
 * \brief Creates a directory.
 *
 * \effects
 * If `p` resolves to an existing directory, returns with no effect.
 *
 * Otherwise, establishes the postcondition by attempting to create the directory `p` resolves to, as if by ISO/IEC
 * 9945 [`mkdir()`](http://www.opengroup.org/onlinepubs/000095399/functions/mkdir.html). For overloads without
 * the `existing` argument, the new directory is created with read, write and file iteration permissions for all users
 * (`S_IRWXU|S_IRWXG|S_IRWXO` mode in ISO/IEC 9945). Overloads with `existing` argument obtain permissions from
 * `existing`, which must be a path to an existing directory.
 *
 * \remark On Windows, an implementation may call `CreateDirectoryW(p.c_str(), nullptr)` when `existing` is not
 *         specified and `CreateDirectoryExW(existing.c_str(), p.c_str(), nullptr)` otherwise.
 *
 * \post `is_directory(p)` is `true`.
 *
 * \param p The path to the directory to be created.
 * \param existing The path to an existing directory to obtain permissions from.
 * \param ec The error code to set on failure.
 *
 * \returns `true` if a new directory was created, `false` otherwise.
 */
inline bool create_directory(path const& p, path const& existing, system::error_code& ec) noexcept
{
    return detail::create_directory(p, &existing, &ec);
}

/*! \overload */
inline bool create_directory(path const& p, system::error_code& ec) noexcept
{
    return detail::create_directory(p, nullptr, &ec);
}

/*! \overload */
inline bool create_directory(path const& p, path const& existing)
{
    return detail::create_directory(p, &existing);
}

/*! \overload */
inline bool create_directory(path const& p)
{
    return detail::create_directory(p, nullptr);
}

/*!
 * \brief Creates a directory symbolic link.
 *
 * \effects Establishes the postcondition, as if by ISO/IEC 9945
 *          [`symlink()`](http://www.opengroup.org/onlinepubs/000095399/functions/symlink.html).
 *
 * \post `new_symlink` resolves to a symbolic link file that contains an unspecified representation of `to`.
 *
 * \note Some operating systems, such as Windows, require symlink creation to identify that the link is to a directory.
 *       Portable code should use `create_directory_symlink()` to create directory symlinks rather than
 *      `create_symlink()`.
 *
 * \note Some operating systems do not support symbolic links at all or support them only for regular files. Some
 *       file systems do not support symbolic links regardless of the operating system - the FAT file system used on
 *       memory cards and flash drives, for example.
 *
 * \param to The target path that the symbolic link will point to.
 * \param new_symlink The path to the new symbolic link to be created.
 * \param ec The error code to set on failure.
 */
inline void create_directory_symlink(path const& to, path const& new_symlink, system::error_code& ec) noexcept
{
    detail::create_directory_symlink(to, new_symlink, &ec);
}

/*! \overload */
inline void create_directory_symlink(path const& to, path const& new_symlink)
{
    detail::create_directory_symlink(to, new_symlink);
}

/*!
 * \brief Creates a hard link.
 *
 * \effects Establishes the postcondition, as if by ISO/IEC 9945
 *          [`link()`](http://www.opengroup.org/onlinepubs/000095399/functions/link.html).
 *
 * \post `exists(to) && exists(new_hard_link) && equivalent(to, new_hard_link)` is `true`.
 *       The contents of the file or directory `to` resolves to are unchanged.
 *
 * \note Some operating systems do not support hard links at all or support them only for regular files. Some
 *       file systems do not support hard links regardless of the operating system - the FAT file system used on memory
 *       cards and flash drives, for example. Some file systems limit the number of links per file.
 *
 * \param to The path to the existing file.
 * \param new_hard_link The path to the new hard link to be created.
 * \param ec The error code to set on failure.
 */
inline void create_hard_link(path const& to, path const& new_hard_link, system::error_code& ec) noexcept
{
    detail::create_hard_link(to, new_hard_link, &ec);
}

/*! \overload */
inline void create_hard_link(path const& to, path const& new_hard_link)
{
    detail::create_hard_link(to, new_hard_link);
}

/*!
 * \brief Creates a file symbolic link.
 *
 * \effects Establishes the postcondition, as if by ISO/IEC 9945
 *          [`symlink()`](http://www.opengroup.org/onlinepubs/000095399/functions/symlink.html).
 *
 * \post `new_symlink` resolves to a symbolic link file that contains an unspecified representation of `to`.
 *
 * \note Some operating systems do not support symbolic links at all or support them only for regular files. Some
 *       file systems do not support symbolic links regardless of the operating system - the FAT system used on memory
 *       cards and flash drives, for example.
 *
 * \param to The target path that the symbolic link will point to.
 * \param new_symlink The path to the new symbolic link to be created.
 * \param ec The error code to set on failure.
 */
inline void create_symlink(path const& to, path const& new_symlink, system::error_code& ec) noexcept
{
    detail::create_symlink(to, new_symlink, &ec);
}

/*! \overload */
inline void create_symlink(path const& to, path const& new_symlink)
{
    detail::create_symlink(to, new_symlink);
}

/*!
 * \brief Reads contents of a symbolic link.
 *
 * \param p The path to the symbolic link.
 * \param ec The error code to set on failure.
 *
 * \returns If `p` resolves to a symbolic link, a `path` object containing the contents of that symbolic link.
 *          Otherwise an empty `path` object.
 *
 * \note It is an error if `p` does not resolve to a symbolic link.
 */
inline path read_symlink(path const& p, system::error_code& ec)
{
    return detail::read_symlink(p, &ec);
}

/*! \overload */
inline path read_symlink(path const& p)
{
    return detail::read_symlink(p);
}

/*!
 * \brief Removes a file or directory.
 *
 * \effects
 * If `exists(symlink_status(p, ec))`, it is removed as if by ISO/IEC 9945
 * [`remove()`](http://www.opengroup.org/onlinepubs/000095399/functions/remove.html).
 *
 * \note A symbolic link is itself removed, rather than the file it resolves to being removed.
 *
 * \post `!exists(p)` is `true`.
 *
 * \param p The path to the file or directory to be removed.
 * \param ec The error code to set on failure.
 *
 * \returns `false` if `p` did not exist, `true` otherwise.
 */
inline bool remove(path const& p, system::error_code& ec) noexcept
{
    return detail::remove(p, &ec);
}

/*! \overload */
inline bool remove(path const& p)
{
    return detail::remove(p);
}

/*!
 * \brief Recursively removes files and directories.
 *
 * \effects
 * Recursively deletes the contents of `p` if it exists, then deletes file `p` itself, as if by ISO/IEC 9945
 * [`remove()`](http://www.opengroup.org/onlinepubs/000095399/functions/remove.html).
 *
 * \note A symbolic link is itself removed, rather than the file it resolves to being removed.
 *
 * \post `!exists(p)` is `true`.
 *
 * \param p The path to the file or directory to be removed.
 * \param ec The error code to set on failure.
 *
 * \returns The number of files removed.
 */
inline boost::uintmax_t remove_all(path const& p, system::error_code& ec) noexcept
{
    return detail::remove_all(p, &ec);
}

/*! \overload */
inline boost::uintmax_t remove_all(path const& p)
{
    return detail::remove_all(p);
}

/*!
 * \brief Renames a file or directory.
 *
 * \effects
 * Renames `old_p` to `new_p`, as if by ISO/IEC 9945
 * [`rename()`](http://www.opengroup.org/onlinepubs/000095399/functions/rename.html).
 *
 * \note If `old_p` and `new_p` resolve to the same existing file, no action is taken. Otherwise, if `new_p` resolves
 *       to an existing non-directory file, it is removed, while if `new_p` resolves to an existing directory, it is
 *       removed if empty on ISO/IEC 9945 but is an error on Windows. A symbolic link is itself renamed, rather than
 *       the file it resolves to being renamed. Renaming across different file systems is an error on ISO/IEC 9945 but
 *       may succeed on Windows, resulting in a copy to `new_p` and removal of `old_p`. Portable programs should avoid
 *       renaming files across file systems.
 *
 * \param old_p The current path of the file or directory.
 * \param new_p The new path for the file or directory.
 * \param ec The error code to set on failure.
 */
inline void rename(path const& old_p, path const& new_p, system::error_code& ec) noexcept
{
    detail::rename(old_p, new_p, &ec);
}

/*! \overload */
inline void rename(path const& old_p, path const& new_p)
{
    detail::rename(old_p, new_p);
}

/*!
 * \brief Returns filesystem space information.
 *
 * \param p The path to any file in the filesystem.
 * \param ec The error code to set on failure.
 *
 * \returns An object of type `space_info`. The value of the `space_info` object is determined as if by using
 *          ISO/IEC 9945 [`statvfs()`](http://www.opengroup.org/onlinepubs/000095399/functions/statvfs.html) to obtain
 *          an ISO/IEC 9945 struct [`statvfs`](http://www.opengroup.org/onlinepubs/000095399/basedefs/sys/statvfs.h.html),
 *          and then multiplying its `f_blocks`, `f_bfree`, and `f_bavail` members by its `f_frsize` member, and
 *          assigning the results to the `capacity`, `free`, and `available` members respectively. Any members
 *          for which the value cannot be determined shall be set to -1.
 */
inline space_info space(path const& p, system::error_code& ec) noexcept
{
    return detail::space(p, &ec);
}

/*! \overload */
inline space_info space(path const& p)
{
    return detail::space(p);
}

#if !defined(BOOST_FILESYSTEM_DOXYGEN)
namespace BOOST_FILESYSTEM_VERSION_NAMESPACE {
#endif // !defined(BOOST_FILESYSTEM_DOXYGEN)

/*!
 * \brief Checks if two paths resolve to the same file.
 *
 * \overview
 * Two paths are considered to resolve to the same file system entity if two candidate entities reside on the same
 * filesystem at the same location. This is determined as if by the values of the ISO/IEC 9945
 * [`stat`](http://www.opengroup.org/onlinepubs/000095399/basedefs/sys/stat.h.html) structure,
 * obtained as if by [`stat()`](http://www.opengroup.org/onlinepubs/000095399/functions/stat.html)
 * for the two paths, having equal `st_dev` values and equal `st_ino` values.
 *
 * \remark ISO/IEC 9945 requires that "`st_dev` must be unique within a Local Area Network". Conservative ISO/IEC 9945
 *         implementations may also wish to check for equal `st_size` and `st_mtime` values. Windows implementations
 *         may use `GetFileInformationByHandle()` as a surrogate for `stat()`, and consider "same" to be equal values
 *         for `dwVolumeSerialNumber`, `nFileIndexHigh`, `nFileIndexLow`, `nFileSizeHigh`, `nFileSizeLow`,
 *         `ftLastWriteTime.dwLowDateTime`, and `ftLastWriteTime.dwHighDateTime`.
 *
 * \filesystem_v3 `!exists(p1) && !exists(p2)` is an error. If only one of the paths doesn't exist, `false` is returned
 *                without error.
 *
 * \filesystem_v4 `!exists(p1) || !exists(p2)` is an error.
 *
 * \param p1 First path to test for equivalence.
 * \param p2 Second path to test for equivalence.
 * \param ec The error code to set on failure.
 *
 * \returns `true`, if `p1` and `p2` resolve to the same file system entity, else `false`.
 */
inline bool equivalent(path const& p1, path const& p2, system::error_code& ec) noexcept
{
    return BOOST_FILESYSTEM_VERSIONED_SYM(detail::equivalent)(p1, p2, &ec);
}

/*! \overload */
inline bool equivalent(path const& p1, path const& p2)
{
    return BOOST_FILESYSTEM_VERSIONED_SYM(detail::equivalent)(p1, p2);
}

#if !defined(BOOST_FILESYSTEM_DOXYGEN)
} // namespace BOOST_FILESYSTEM_VERSION_NAMESPACE

using BOOST_FILESYSTEM_VERSION_NAMESPACE::absolute;
using BOOST_FILESYSTEM_VERSION_NAMESPACE::canonical;
using BOOST_FILESYSTEM_VERSION_NAMESPACE::equivalent;
using BOOST_FILESYSTEM_VERSION_NAMESPACE::weakly_canonical;
#endif // !defined(BOOST_FILESYSTEM_DOXYGEN)

//! @}

//  test helper  -----------------------------------------------------------------------//

//  Not part of the documented interface since false positives are possible;
//  there is no law that says that an OS that has large stat.st_size
//  actually supports large file sizes.

namespace detail {

BOOST_FILESYSTEM_DECL bool possible_large_file_size_support();

} // namespace detail

} // namespace filesystem
} // namespace boost

#include <boost/filesystem/detail/footer.hpp>

#endif // BOOST_FILESYSTEM_OPERATIONS_HPP
