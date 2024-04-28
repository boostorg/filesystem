//  boost/filesystem/file_status.hpp  --------------------------------------------------//

//  Copyright Beman Dawes 2002-2009
//  Copyright Jan Langer 2002
//  Copyright Dietmar Kuehl 2001
//  Copyright Vladimir Prus 2002
//  Copyright Andrey Semashev 2019

//  Distributed under the Boost Software License, Version 1.0.
//  See http://www.boost.org/LICENSE_1_0.txt

//  Library home page: http://www.boost.org/libs/filesystem

//--------------------------------------------------------------------------------------//

#ifndef BOOST_FILESYSTEM_FILE_STATUS_HPP
#define BOOST_FILESYSTEM_FILE_STATUS_HPP

#include <boost/filesystem/config.hpp>
#include <boost/detail/bitmask.hpp>

#include <boost/filesystem/detail/header.hpp> // must be the last #include

//--------------------------------------------------------------------------------------//

namespace boost {
namespace filesystem {

//--------------------------------------------------------------------------------------//
//                                     file_type                                        //
//--------------------------------------------------------------------------------------//

/*!
 * \brief Specifies constants used to identify file types.
 *
 * \note Not all file types may be supported on a given platform.
 */
enum file_type
{
    /*!
     * \brief An error occurred while trying to obtain the status of the file.
     *
     * \note The file simply not being found is **not** considered a status error.
     */
    status_error,
    //! The file could not be found.
    file_not_found,
    //! Regular file.
    regular_file,
    //! Directory file.
    directory_file,
    // the following may not apply to some operating systems or file systems
    //! Symbolic link file.
    symlink_file,
    //! Block special file.
    block_file,
    //! Character special file.
    character_file,
    //! FIFO or pipe file.
    fifo_file,
    //! Socket file.
    socket_file,
    /*!
     * \brief Reparse point file.
     *
     * \note This type is Windows-specific. It is indicated for files of type `FILE_ATTRIBUTE_REPARSE_POINT`
     *       that are not recognized as a symlink.
     */
    reparse_file,
    //! The file exists, but it is of a system specific type not covered by any of the above cases.
    type_unknown
};

//--------------------------------------------------------------------------------------//
//                                       perms                                          //
//--------------------------------------------------------------------------------------//

/*!
 * \brief Specifies bitmask constants uses to identify file permissions.
 *
 * \remark ISO/IEC 9945(POSIX) specifies actual values, and those values have been adopted here because they are very
 *         familiar and ingrained for many POSIX users.
 *
 * \remark On Windows, all permissions except write are currently ignored. There is only a single write permission;
 *         setting write permission for owner, group, or others sets write permission for all, and removing write
 *         permission for owner, group, or others removes write permission for all.
 */
enum perms
{
    //! \name Permission bits
    //! @{

    /*!
     * \brief There are no permissions set for the file.
     *
     * \note `file_not_found` is `no_perms` rather than `perms_not_known`.
     */
    no_perms = 0,

    // POSIX equivalent macros given in comments.
    // Values are from POSIX and are given in octal per the POSIX standard.

    owner_read = 0400,  //!< Read permission for owner (`S_IRUSR`).
    owner_write = 0200, //!< Write permission for owner (`S_IWUSR`).
    owner_exe = 0100,   //!< Execute/search permission for owner (`S_IXUSR`).
    owner_all = 0700,   //!< Read, write, execute/search by owner (`S_IRWXU`).

    group_read = 040,  //!< Read permission for group members (`S_IRGRP`).
    group_write = 020, //!< Write permissionfor group members (`S_IWGRP`).
    group_exe = 010,   //!< Execute/search permission for group members (`S_IXGRP`).
    group_all = 070,   //!< Read, write, execute/search by group members (`S_IRWXG`).

    others_read = 04,  //!< Read permission for other users (`S_IROTH`).
    others_write = 02, //!< Write permission for other users (`S_IWOTH`).
    others_exe = 01,   //!< Execute/search permission for other users (`S_IXOTH`).
    others_all = 07,   //!< Read, write, execute/search by other users (`S_IRWXO`).

    all_all = 0777, //!< `owner_all | group_all | others_all`

    // other POSIX bits

    set_uid_on_exe = 04000, //!< Set-user-ID on execution (`S_ISUID`).
    set_gid_on_exe = 02000, //!< Set-group-ID on execution (`S_ISGID`).
    /*!
     * \brief Sticky bit (`S_ISVTX`).
     *
     * Support and semantics vary between platforms. For example:
     *
     * - POSIX XSI: On directories, restricted deletion flag
     * - V7: 'sticky bit': save swapped text even after use
     * - SunOS: On non-directories: don't cache this file
     * - SVID-v4.2: On directories: restricted deletion flag
     *
     * Also see http://en.wikipedia.org/wiki/Sticky_bit.
     */
    sticky_bit = 01000,

    perms_mask = 07777, //!< `all_all | set_uid_on_exe | set_gid_on_exe | sticky_bit`

    //! @}

    //! \name Special values
    //! @{

    //! Special value present when `directory_entry` cache is not loaded.
    perms_not_known BOOST_FILESYSTEM_DETAIL_DOC_HIDDEN(= 0xFFFF),

    //! @}

    //! \name Options for `permissions()` function
    //! @{

    /*!
     * \brief Special value that indicates that `permissions()` should add the given permission bits to the current bits.
     *
     * Must not be combined with `remove_perms`.
     */
    add_perms BOOST_FILESYSTEM_DETAIL_DOC_HIDDEN(= 0x1000),
    /*!
     * \brief Special value that indicates that `permissions()` should remove the given permission bits from the current bits.
     *
     * Must not be combined with `add_perms`.
     */
    remove_perms BOOST_FILESYSTEM_DETAIL_DOC_HIDDEN(= 0x2000),
    /*!
     * \brief Special value that indicates that `permissions()` should not resolve symbolic links.
     *
     * \remark This option is implied on Windows.
     */
    symlink_perms BOOST_FILESYSTEM_DETAIL_DOC_HIDDEN(= 0x4000),

    //! @}

#if !defined(BOOST_FILESYSTEM_DOXYGEN)
    // BOOST_BITMASK op~ casts to int32_least_t, producing invalid enum values
    _detail_extend_perms_32_1 = 0x7fffffff,
    _detail_extend_perms_32_2 = -0x7fffffff - 1
#endif // !defined(BOOST_FILESYSTEM_DOXYGEN)
};

BOOST_BITMASK(perms)

//--------------------------------------------------------------------------------------//
//                                    file_status                                       //
//--------------------------------------------------------------------------------------//

//! An object of type `file_status` stores information about the type and access permissions of a file.
class file_status
{
public:
    /*!
     * \brief Default constructor.
     *
     * \post
     * | Expression            | Value             |
     * |-----------------------|-------------------|
     * | `type()`              | `status_error`    |
     * | `permissions()`       | `perms_not_known` |
     */
    BOOST_CONSTEXPR file_status() noexcept :
        m_value(status_error),
        m_perms(perms_not_known)
    {
    }
    /*!
     * \brief Constructor with file type.
     *
     * \post
     * | Expression            | Value  |
     * |-----------------------|--------|
     * | `type()`              | `ft`   |
     * | `permissions()`       | `prms` |
     *
     * \param v The file type.
     */
    explicit BOOST_CONSTEXPR file_status(file_type v) noexcept :
        m_value(v),
        m_perms(perms_not_known)
    {
    }
    /*!
     * \brief Constructor with file type and permissions.
     *
     * \post
     * | Expression            | Value  |
     * |-----------------------|--------|
     * | `type()`              | `v`    |
     * | `permissions()`       | `prms` |
     *
     * \param v The file type.
     * \param prms The permissions.
     */
    BOOST_CONSTEXPR file_status(file_type v, perms prms) noexcept :
        m_value(v),
        m_perms(prms)
    {
    }

    /*!
     * \brief Copy constructor.
     *
     * \effects Copies the other `file_status` object.
     *
     * \param rhs Object to copy from.
     */
    BOOST_CONSTEXPR file_status(file_status const& rhs) noexcept :
        m_value(rhs.m_value),
        m_perms(rhs.m_perms)
    {
    }
    /*!
     * \brief Copy assignment operator.
     *
     * \effects Copies the other `file_status` object.
     *
     * \param rhs Object to copy from.
     *
     * \returns `*this`
     */
    BOOST_CXX14_CONSTEXPR file_status& operator=(file_status const& rhs) noexcept
    {
        m_value = rhs.m_value;
        m_perms = rhs.m_perms;
        return *this;
    }

    // Note: std::move is not constexpr in C++11, that's why we're not using it here
    /*!
     * \brief Move constructor.
     *
     * \effects Moves from the other `file_status` object.
     *
     * \param rhs Object to move from.
     */
    BOOST_CONSTEXPR file_status(file_status&& rhs) noexcept :
        m_value(static_cast< file_type&& >(rhs.m_value)),
        m_perms(static_cast< perms&& >(rhs.m_perms))
    {
    }
    /*!
     * \brief Move assignment operator.
     *
     * \effects Moves from the other `file_status` object.
     *
     * \param rhs Object to move from.
     *
     * \returns `*this`
     */
    BOOST_CXX14_CONSTEXPR file_status& operator=(file_status&& rhs) noexcept
    {
        m_value = static_cast< file_type&& >(rhs.m_value);
        m_perms = static_cast< perms&& >(rhs.m_perms);
        return *this;
    }

    //! \name observers
    //! @{

    /*!
     * \returns The value of `type()` specified by the postconditions of the most recent call to a constructor,
     *          `operator=`, or `type(file_type)` function.
     */
    BOOST_CONSTEXPR file_type type() const noexcept { return m_value; }
    /*!
     * \returns The value of `permissions()` specified by the postconditions of the most recent call to a constructor,
     *          `operator=`, or `permissions(perms)` function.
     */
    BOOST_CONSTEXPR perms permissions() const noexcept { return m_perms; }

    //! @}

    //! \name modifiers
    //! @{

    /*!
     * \brief Sets the file type.
     *
     * \post `type() == v`
     *
     * \param v File type to set.
     */
    BOOST_CXX14_CONSTEXPR void type(file_type v) noexcept { m_value = v; }
    /*!
     * \brief Sets the file access permission bits.
     *
     * \post `permissions() == prms`
     *
     * \param prms File access permission bits to set.
     */
    BOOST_CXX14_CONSTEXPR void permissions(perms prms) noexcept { m_perms = prms; }

    /*!
     * \returns `type() == rhs.type() && permissions() == rhs.permissions()`
     */
    BOOST_CONSTEXPR bool operator==(file_status const& rhs) const noexcept
    {
        return type() == rhs.type() && permissions() == rhs.permissions();
    }
    /*!
     * \returns `!(*this == rhs)`
     */
    BOOST_CONSTEXPR bool operator!=(file_status const& rhs) const noexcept
    {
        return !(*this == rhs);
    }

    //! @}

    //! \cond
private:
    file_type m_value;
    perms m_perms;
    //! \endcond
};

/*!
 * \returns `f.type() != status_error`.
 */
inline BOOST_CONSTEXPR bool type_present(file_status f) noexcept
{
    return f.type() != filesystem::status_error;
}

/*!
 * \returns `f.permissions() != perms_not_known`.
 */
inline BOOST_CONSTEXPR bool permissions_present(file_status f) noexcept
{
    return f.permissions() != filesystem::perms_not_known;
}

/*!
 * \returns `type_present(f) && permissions_present(f)`.
 */
inline BOOST_CONSTEXPR bool status_known(file_status f) noexcept
{
    return filesystem::type_present(f) && filesystem::permissions_present(f);
}

/*!
 * \returns `status_known(f) && f.type() != file_not_found`.
 */
inline BOOST_CONSTEXPR bool exists(file_status f) noexcept
{
    return f.type() != filesystem::status_error && f.type() != filesystem::file_not_found;
}

/*!
 * \returns `f.type() == regular_file`.
 */
inline BOOST_CONSTEXPR bool is_regular_file(file_status f) noexcept
{
    return f.type() == filesystem::regular_file;
}

/*!
 * \returns `f.type() == directory_file`.
 */
inline BOOST_CONSTEXPR bool is_directory(file_status f) noexcept
{
    return f.type() == filesystem::directory_file;
}

/*!
 * \returns `f.type() == symlink_file`.
 */
inline BOOST_CONSTEXPR bool is_symlink(file_status f) noexcept
{
    return f.type() == filesystem::symlink_file;
}

/*!
 * \returns `f.type() == block_file`.
 */
inline BOOST_CONSTEXPR bool is_block_file(file_status f) noexcept
{
    return f.type() == filesystem::block_file;
}

/*!
 * \returns `f.type() == character_file`.
 */
inline BOOST_CONSTEXPR bool is_character_file(file_status f) noexcept
{
    return f.type() == filesystem::character_file;
}

/*!
 * \returns `f.type() == fifo_file`.
 */
inline BOOST_CONSTEXPR bool is_fifo(file_status f) noexcept
{
    return f.type() == filesystem::fifo_file;
}

/*!
 * \returns `f.type() == socket_file`.
 */
inline BOOST_CONSTEXPR bool is_socket(file_status f) noexcept
{
    return f.type() == filesystem::socket_file;
}

/*!
 * \returns `f.type() == reparse_file`.
 */
inline BOOST_CONSTEXPR bool is_reparse_file(file_status f) noexcept
{
    return f.type() == filesystem::reparse_file;
}

/*!
 * \returns `exists(f) && !is_regular_file(f) && !is_directory(f) && !is_symlink(f)`.
 */
inline BOOST_CONSTEXPR bool is_other(file_status f) noexcept
{
    return filesystem::exists(f) && !filesystem::is_regular_file(f) && !filesystem::is_directory(f) && !filesystem::is_symlink(f);
}

} // namespace filesystem
} // namespace boost

#include <boost/filesystem/detail/footer.hpp> // pops abi_prefix.hpp pragmas

#endif // BOOST_FILESYSTEM_FILE_STATUS_HPP
