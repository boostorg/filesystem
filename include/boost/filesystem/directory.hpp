//  boost/filesystem/directory.hpp  ---------------------------------------------------//

//  Copyright Beman Dawes 2002-2009
//  Copyright Jan Langer 2002
//  Copyright Dietmar Kuehl 2001
//  Copyright Vladimir Prus 2002
//  Copyright Andrey Semashev 2019-2026

//  Distributed under the Boost Software License, Version 1.0.
//  See http://www.boost.org/LICENSE_1_0.txt

//  Library home page: http://www.boost.org/libs/filesystem

//--------------------------------------------------------------------------------------//

#ifndef BOOST_FILESYSTEM_DIRECTORY_HPP
#define BOOST_FILESYSTEM_DIRECTORY_HPP

#include <boost/filesystem/config.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/filesystem/file_status.hpp>
#include <boost/filesystem/detail/path_traits.hpp>

#include <cstddef>
#include <string>
#include <vector>

#include <boost/assert.hpp>
#include <boost/detail/bitmask.hpp>
#include <boost/system/error_code.hpp>
#include <boost/smart_ptr/intrusive_ptr.hpp>
#include <boost/smart_ptr/intrusive_ref_counter.hpp>
#include <boost/iterator/iterator_facade.hpp>
#include <boost/iterator/iterator_categories.hpp>

#include <boost/filesystem/detail/header.hpp> // must be the last #include

#if defined(BOOST_GCC) && (__GNUC__ == 12)
#pragma GCC diagnostic push
// 'function' redeclared without dllimport attribute: previous dllimport ignored
// gcc bug on MinGW-w64 and Cygwin: https://gcc.gnu.org/bugzilla/show_bug.cgi?id=106395
#pragma GCC diagnostic ignored "-Wattributes"
#endif

//--------------------------------------------------------------------------------------//

namespace boost {
namespace filesystem {

/*!
 * \brief Option flags for directory iteration.
 *
 * These options can be combined using bitwise OR and passed to `directory_iterator` and `recursive_directory_iterator`
 * constructors to customize directory iteration behavior.
 *
 * \sa \ref directory_iterator, \ref recursive_directory_iterator.
 */
enum class directory_options : unsigned int
{
    //! Default directory iteration options.
    none BOOST_FILESYSTEM_DETAIL_DOC_HIDDEN(= 0u),
    //! If a directory cannot be opened because of insufficient permissions, pretend that the directory is empty.
    skip_permission_denied BOOST_FILESYSTEM_DETAIL_DOC_HIDDEN(= 1u),
    //! For `recursive_directory_iterator`: follow directory symlinks.
    follow_directory_symlink BOOST_FILESYSTEM_DETAIL_DOC_HIDDEN(= 1u << 1u),
    //! Non-standard extension for `recursive_directory_iterator`: don't follow dangling directory symlinks.
    skip_dangling_symlinks BOOST_FILESYSTEM_DETAIL_DOC_HIDDEN(= 1u << 2u),
    /*!
     * Non-standard extension for `recursive_directory_iterator`: instead of producing an end iterator on errors,
     * repeatedly invoke `pop()` until it succeeds or the iterator becomes equal to end iterator.
     */
    pop_on_error BOOST_FILESYSTEM_DETAIL_DOC_HIDDEN(= 1u << 3u),
#if !defined(BOOST_FILESYSTEM_DOXYGEN)
    _detail_no_follow = 1u << 4u,        // internal use only
    _detail_no_push = 1u << 5u           // internal use only
#endif
};

BOOST_BITMASK(directory_options)

class directory_iterator;
class recursive_directory_iterator;

namespace detail {

struct directory_iterator_params;

BOOST_FILESYSTEM_DECL
void directory_iterator_construct(directory_iterator& it, path const& p, directory_options opts, directory_iterator_params* params, system::error_code* ec);
BOOST_FILESYSTEM_DECL
void directory_iterator_increment(directory_iterator& it, system::error_code* ec);

struct recur_dir_itr_imp;

BOOST_FILESYSTEM_DECL
void recursive_directory_iterator_construct(recursive_directory_iterator& it, path const& dir_path, directory_options opts, system::error_code* ec);
BOOST_FILESYSTEM_DECL
void recursive_directory_iterator_increment(recursive_directory_iterator& it, system::error_code* ec);
BOOST_FILESYSTEM_DECL
void recursive_directory_iterator_pop(recursive_directory_iterator& it, system::error_code* ec);

enum class directory_entry_update_mask : unsigned int
{
    none = 0u,
    symlink_status = 1u,
    status = 1u << 1u,
    all = 3u
};

BOOST_BITMASK(directory_entry_update_mask)

} // namespace detail

//--------------------------------------------------------------------------------------//
//                                                                                      //
//                                 directory_entry                                      //
//                                                                                      //
//--------------------------------------------------------------------------------------//

//  GCC has a problem with a member function named path within a namespace or
//  sub-namespace that also has a class named path. The workaround is to always
//  fully qualify the name path when it refers to the class name.

/*!
 * \brief Directory entry.
 *
 * A `directory_entry` object stores a `path` object, as well as some amount of cached information about the file
 * identified by the path. Currently, the cached information includes a `file_status` object for non-symbolic link
 * status and a `file_status` object for symbolic link status.
 *
 * \note
 * \parblock
 * Because `status()` on a pathname may be a relatively expensive operation, some operating systems provide
 * status information as a byproduct of directory iteration. Caching such status information can result in
 * significant time savings. Cached and non-cached results may differ in the presence of file system races.
 *
 * As an example, actual cold-boot timing of iteration over a directory with 15,047 entries was six seconds
 * for non-cached status queries versus one second for cached status queries. Windows XP, 3.0 GHz processor,
 * with a moderately fast hard-drive. Similar speedups are expected on Linux and BSD-derived systems that
 * provide status as a by-product of directory iteration.
 *
 * The exact set of cached information may vary from one Boost.Filesystem version to another, and also between
 * different operating systems and underlying file systems. Users' code must not rely on whether a certain piece
 * of information is cached or not. This means that calling most observers and modifiers of `directory_entry` may
 * or may not result in a filesystem query that may potentially fail. Information caching is exclusively
 * a performance feature aimed at reducing the amount of such queries.
 * \endparblock
 */
class directory_entry
{
#if !defined(BOOST_FILESYSTEM_DOXYGEN)
    friend BOOST_FILESYSTEM_DECL
    void detail::directory_iterator_construct(directory_iterator& it, path const& p, directory_options opts, detail::directory_iterator_params* params, system::error_code* ec);
    friend BOOST_FILESYSTEM_DECL
    void detail::directory_iterator_increment(directory_iterator& it, system::error_code* ec);

    friend BOOST_FILESYSTEM_DECL
    void detail::recursive_directory_iterator_increment(recursive_directory_iterator& it, system::error_code* ec);
#endif // !defined(BOOST_FILESYSTEM_DOXYGEN)

public:
    //! \cond
    using value_type = boost::filesystem::path::value_type; // enables class path ctor taking directory_entry, not part of public interface
    //! \endcond

    /*!
     * \brief Default constructor.
     *
     * \post
     * | Expression         | Value           |
     * |--------------------|-----------------|
     * | `path().empty()`   | `true`          |
     * | `status()`         | `file_status()` |
     * | `symlink_status()` | `file_status()` |
     */
    directory_entry() noexcept {}

    /*!
     * \brief Constructs a `directory_entry` for the given path.
     *
     * \filesystem_v3
     * Equivalent to calling `directory_entry(p, file_status(), file_status())`.
     *
     * \note The cached file statuses will be updated when queried by the caller or by an explicit call to `refresh()`.
     *
     * \filesystem_v4
     * Equivalent to calling `directory_entry(p, ec)`, where `ec` is an instance of `system::error_code`.
     * Throws `filesystem_error` with `ec` if the constructor fails.
     *
     * \param p Path to initialize the directory entry from.
     */
    explicit directory_entry(boost::filesystem::path const& p);

#if BOOST_FILESYSTEM_VERSION >= 4 || defined(BOOST_FILESYSTEM_DOXYGEN)
    /*!
     * \brief Constructs a `directory_entry` for the given path.
     *
     * \effects
     * Initializes the stored path from `p` and calls `refresh()` or `refresh(ec)`. If the call fails,
     * the stored path is replaced with an empty path.
     *
     * \note This overload is not available in **v3**.
     *
     * \post `path() == p` if no error occurs, otherwise `path().empty() == true`.
     *
     * \param p Path to initialize the directory entry from.
     * \param ec Error code returned in case of failure.
     */
    directory_entry(boost::filesystem::path const& p, system::error_code& ec) :
        m_path(p)
    {
        refresh_impl(update_mask::all, &ec);
        if (ec)
            m_path.clear();
    }
#endif
#if BOOST_FILESYSTEM_VERSION < 4 || defined(BOOST_FILESYSTEM_DOXYGEN)
    /*!
     * \brief Constructs a `directory_entry` for the given path and file statuses.
     *
     * \note This overload is not available in **v4**.
     *
     * \post
     * | Expression         | Value        |
     * |--------------------|--------------|
     * | `path()`           | `p`          |
     * | `status()`         | `st`         |
     * | `symlink_status()` | `symlink_st` |
     *
     * \param p Path to initialize the directory entry from.
     * \param st File status, as if acquired from `status(p)`.
     * \param symlink_st File symlink status, as if acquired from `symlink_status(p)`.
     */
    directory_entry(boost::filesystem::path const& p, file_status st, file_status symlink_st = file_status()) :
        m_path(p), m_status(st), m_symlink_status(symlink_st)
    {
    }
#endif

    /*!
     * \brief Copy constructor.
     *
     * \param rhs Directory entry object to copy from.
     */
    directory_entry(directory_entry const& rhs) :
        m_path(rhs.m_path), m_status(rhs.m_status), m_symlink_status(rhs.m_symlink_status)
    {
    }

    /*!
     * \brief Copy assignment operator.
     *
     * \param rhs Directory entry object to copy from.
     *
     * \returns `*this`.
     */
    directory_entry& operator=(directory_entry const& rhs)
    {
        m_path = rhs.m_path;
        m_status = rhs.m_status;
        m_symlink_status = rhs.m_symlink_status;
        return *this;
    }

    /*!
     * \brief Move constructor.
     *
     * \param rhs Directory entry object to move from.
     */
    directory_entry(directory_entry&& rhs) noexcept :
        m_path(static_cast< boost::filesystem::path&& >(rhs.m_path)),
        m_status(static_cast< file_status&& >(rhs.m_status)),
        m_symlink_status(static_cast< file_status&& >(rhs.m_symlink_status))
    {
    }

    /*!
     * \brief Move assignment operator.
     *
     * \param rhs Directory entry object to move from.
     *
     * \returns `*this`.
     */
    directory_entry& operator=(directory_entry&& rhs) noexcept
    {
        m_path = static_cast< boost::filesystem::path&& >(rhs.m_path);
        m_status = static_cast< file_status&& >(rhs.m_status);
        m_symlink_status = static_cast< file_status&& >(rhs.m_symlink_status);
        return *this;
    }

    /*!
     * \brief Assigns a path and file statuses to the directory entry.
     *
     * \filesystem_v3
     * Equivalent to calling `assign(p, file_status(), file_status())`.
     *
     * \note The cached file statuses will be updated when queried by the caller or by an explicit call to `refresh()`.
     *
     * \filesystem_v4
     * Equivalent to calling `assign(p, ec)`, where `ec` is an instance of `system::error_code`.
     * Throws `filesystem_error` with `ec` if the call fails.
     *
     * \param p Path to assign from.
     */
    void assign(boost::filesystem::path const& p);

    /*!
     * \brief Assigns a path and file statuses to the directory entry.
     *
     * \filesystem_v3
     * Equivalent to calling `assign(std::move(p), file_status(), file_status())`.
     *
     * \note The cached file statuses will be updated when queried by the caller or by an explicit call to `refresh()`.
     *
     * \filesystem_v4
     * Equivalent to calling `assign(std::move(p), ec)`, where `ec` is an instance of `system::error_code`.
     * Throws `filesystem_error` with `ec` if the call fails.
     *
     * \param p Path to assign from.
     */
    void assign(boost::filesystem::path&& p);

#if BOOST_FILESYSTEM_VERSION >= 4 || defined(BOOST_FILESYSTEM_DOXYGEN)
    /*!
     * \brief Assigns a path to the directory entry.
     *
     * \effects
     * Assigns `p` to the stored path and calls `refresh(ec)`. If an error occurs, the value of the cached data
     * is unspecified.
     *
     * \note This overload is not available in **v3**.
     *
     * \param p Path to assign from.
     * \param ec Error code returned in case of failure.
     */
    void assign(boost::filesystem::path const& p, system::error_code& ec)
    {
        m_path = p;
        refresh_impl(update_mask::all, &ec);
    }

    /*!
     * \brief Assigns a path to the directory entry.
     *
     * \effects
     * Assigns `p` to the stored path and calls `refresh(ec)`. If an error occurs, the value of the cached data
     * is unspecified.
     *
     * \note This overload is not available in **v3**.
     *
     * \param p Path to assign from.
     * \param ec Error code returned in case of failure.
     */
    void assign(boost::filesystem::path&& p, system::error_code& ec)
    {
        m_path = static_cast< boost::filesystem::path&& >(p);
        refresh_impl(update_mask::all, &ec);
    }
#endif
#if BOOST_FILESYSTEM_VERSION < 4 || defined(BOOST_FILESYSTEM_DOXYGEN)
    /*!
     * \brief Assigns a path and file statuses to the directory entry.
     *
     * \effects
     * Assigns `p`, `st` and `symlink_st` to the directory entry.
     *
     * \post
     * | Expression         | Value        |
     * |--------------------|--------------|
     * | `path()`           | `p`          |
     * | `status()`         | `st`         |
     * | `symlink_status()` | `symlink_st` |
     *
     * \note This overload is not available in **v4**.
     *
     * \param p Path to assign from.
     * \param st File status, as if acquired from `status(p)`.
     * \param symlink_st File symlink status, as if acquired from `symlink_status(p)`.
     */
    void assign(boost::filesystem::path const& p, file_status st, file_status symlink_st = file_status())
    {
        assign_with_status(p, st, symlink_st);
    }

    /*!
     * \brief Assigns a path and file statuses to the directory entry.
     *
     * \effects
     * Assigns `p`, `st` and `symlink_st` to the directory entry.
     *
     * \post
     * | Expression         | Value        |
     * |--------------------|--------------|
     * | `path()`           | `p`          |
     * | `status()`         | `st`         |
     * | `symlink_status()` | `symlink_st` |
     *
     * \note This overload is not available in **v4**.
     *
     * \param p Path to assign from.
     * \param st File status, as if acquired from `status(p)`.
     * \param symlink_st File symlink status, as if acquired from `symlink_status(p)`.
     */
    void assign(boost::filesystem::path&& p, file_status st, file_status symlink_st = file_status())
    {
        assign_with_status(static_cast< boost::filesystem::path&& >(p), st, symlink_st);
    }
#endif

    /*!
     * \brief Replaces the filename component of the path.
     *
     * \filesystem_v3
     * Equivalent to calling `replace_filename(p, file_status(), file_status())`.
     *
     * \note The cached file statuses will be updated when queried by the caller or by an explicit call to `refresh()`.
     *
     * \filesystem_v4
     * Equivalent to calling `replace_filename(p, ec)`, where `ec` is an instance of `system::error_code`.
     * Throws `filesystem_error` with `ec` if the call fails.
     *
     * \param p Path to assign from.
     */
    void replace_filename(boost::filesystem::path const& p);

#if BOOST_FILESYSTEM_VERSION >= 4 || defined(BOOST_FILESYSTEM_DOXYGEN)
    /*!
     * \brief Replaces the filename component of the path.
     *
     * \effects
     * On the stored path `m_path`, calls `m_path.replace_filename(p)` and then `refresh(ec)`. If an error occurs,
     * the value of the cached data is unspecified.
     *
     * \note This overload is not available in **v3**.
     *
     * \param p The replacement filename.
     * \param ec Error code returned in case of failure.
     */
    void replace_filename(boost::filesystem::path const& p, system::error_code& ec)
    {
        m_path.replace_filename(p);
        refresh_impl(update_mask::all, &ec);
    }
#endif
#if BOOST_FILESYSTEM_VERSION < 4 || defined(BOOST_FILESYSTEM_DOXYGEN)
    /*!
     * \brief Replaces the filename component of the path.
     *
     * \effects
     * On the stored path `m_path`, calls `m_path.replace_filename(p)` and then stores `st` and `symlink_st` as
     * the cached file statuses.
     *
     * \post
     * | Expression         | Value                        |
     * |--------------------|------------------------------|
     * | `path()`           | `m_path.replace_filename(p)` |
     * | `status()`         | `st`                         |
     * | `symlink_status()` | `symlink_st`                 |
     *
     * \note This overload is not available in **v4**.
     *
     * \param p The replacement filename.
     * \param st File status, as if acquired from `status(p)`.
     * \param symlink_st File symlink status, as if acquired from `symlink_status(p)`.
     */
    void replace_filename(boost::filesystem::path const& p, file_status st, file_status symlink_st = file_status())
    {
        replace_filename_with_status(p, st, symlink_st);
    }

#if !defined(BOOST_FILESYSTEM_NO_DEPRECATED) && !defined(BOOST_FILESYSTEM_DOXYGEN)
    BOOST_FILESYSTEM_DETAIL_DEPRECATED("Use directory_entry::replace_filename() instead")
    void replace_leaf(boost::filesystem::path const& p, file_status st, file_status symlink_st)
    {
        replace_filename_with_status(p, st, symlink_st);
    }
#endif // !defined(BOOST_FILESYSTEM_NO_DEPRECATED) && !defined(BOOST_FILESYSTEM_DOXYGEN)
#endif

    /*!
     * \brief Returns the path stored in the directory entry.
     *
     * \returns The stored path.
     */
    boost::filesystem::path const& path() const noexcept { return m_path; }

    /*!
     * \brief Returns the path stored in the directory entry.
     *
     * \returns The stored path.
     */
    operator boost::filesystem::path const&() const noexcept { return m_path; }

    /*!
     * \brief Updates any cached data by querying the filesystem.
     *
     * \effects
     * Updates any cached data by querying the filesystem about the file identified by the stored path. If an error
     * occurs, the value of the cached data is unspecified.
     *
     * \param ec Error code returned in case of failure.
     */
    void refresh(system::error_code& ec) noexcept { refresh_impl(update_mask::all, &ec); }

    /*! \overload */
    void refresh() { refresh_impl(update_mask::all); }

    /*!
     * \brief Returns the file status.
     *
     * \effects
     * For the cached file status `m_status`, if `!status_known(m_status)`, refreshes the cache to update it.
     * Then returns `m_status`.
     *
     * \note The implementation does not query the filesystem after the file status has been cached.
     *       Filesystem changes after the file status has been cached will not be reflected in the result.
     *
     * \param ec Error code returned in case of failure.
     *
     * \returns The file status.
     */
    file_status status(system::error_code& ec) const noexcept
    {
        ec.clear();

        if (!filesystem::status_known(m_status))
            refresh_impl(update_mask::all, &ec);
        return m_status;
    }

    /*! \overload */
    file_status status() const
    {
        if (!filesystem::status_known(m_status))
            refresh_impl(update_mask::all);
        return m_status;
    }

    /*!
     * \brief Returns the symlink file status.
     *
     * \effects
     * For the cached symlink file status `m_symlink_status`, if `!status_known(m_symlink_status)`, refreshes
     * the cache to update it. Then returns `m_symlink_status`.
     *
     * \note The implementation does not query the filesystem after the symlink file status has been cached.
     *       Filesystem changes after the symlink file status has been cached will not be reflected in the result.
     *
     * \param ec Error code returned in case of failure.
     *
     * \returns The symlink file status.
     */
    file_status symlink_status(system::error_code& ec) const noexcept
    {
        ec.clear();

        if (!filesystem::status_known(m_symlink_status))
            refresh_impl(update_mask::symlink_status, &ec);
        return m_symlink_status;
    }

    /*! \overload */
    file_status symlink_status() const
    {
        if (!filesystem::status_known(m_symlink_status))
            refresh_impl(update_mask::symlink_status);
        return m_symlink_status;
    }

    /*!
     * \brief Returns the file type.
     *
     * \effects Equivalent to `status(ec).type()` or `status().type()`, respectively.
     *
     * \note The implementation may be more efficient than calling `status`, if the information about the file type
     *       is cached, but permissions are not.
     *
     * \param ec Error code returned in case of failure.
     *
     * \returns The file type. If an error occurs, the overload taking `ec` returns `file_type::status_error`.
     *
     * \sa \ref directory_entry::status.
     */
    filesystem::file_type file_type(system::error_code& ec) const noexcept
    {
        ec.clear();

        if (!filesystem::type_present(m_status))
            refresh_impl(update_mask::all, &ec);
        return m_status.type();
    }

    /*! \overload */
    filesystem::file_type file_type() const
    {
        if (!filesystem::type_present(m_status))
            refresh_impl(update_mask::all);
        return m_status.type();
    }

    /*!
     * \brief Returns the symlink file type.
     *
     * \effects Equivalent to `symlink_status(ec).type()` or `symlink_status().type()`, respectively.
     *
     * \note The implementation may be more efficient than calling `symlink_status`, if the information about
     *       the symlink file type is cached, but permissions are not.
     *
     * \param ec Error code returned in case of failure.
     *
     * \returns The symlink file type. If an error occurs, the overload taking `ec` returns `file_type::status_error`.
     *
     * \sa \ref directory_entry::symlink_status.
     */
    filesystem::file_type symlink_file_type(system::error_code& ec) const noexcept
    {
        ec.clear();

        if (!filesystem::type_present(m_symlink_status))
            refresh_impl(update_mask::symlink_status, &ec);
        return m_symlink_status.type();
    }

    /*! \overload */
    filesystem::file_type symlink_file_type() const
    {
        if (!filesystem::type_present(m_symlink_status))
            refresh_impl(update_mask::symlink_status);
        return m_symlink_status.type();
    }

    /*!
     * \brief Checks if the file exists.
     *
     * \effects Equivalent to `exists(status(ec))` or `exists(status())`, respectively.
     *
     * \note The implementation may be more efficient than calling `status`, if the information about the file type
     *       is cached, but permissions are not.
     *
     * \param ec Error code returned in case of failure.
     *
     * \returns `true` if the file exists, according to the cached file type, otherwise `false`.
     *          If an error occurs, the overload taking `ec` returns `false`.
     *
     * \sa \ref directory_entry::status.
     */
    bool exists(system::error_code& ec) const noexcept
    {
        filesystem::file_type ft = this->file_type(ec);
        return ft != filesystem::status_error && ft != filesystem::file_not_found;
    }

    /*! \overload */
    bool exists() const
    {
        filesystem::file_type ft = this->file_type();
        return ft != filesystem::status_error && ft != filesystem::file_not_found;
    }

    /*!
     * \brief Checks if the file is a regular file.
     *
     * \effects Equivalent to `is_regular_file(status(ec))` or `is_regular_file(status())`, respectively.
     *
     * \note The implementation may be more efficient than calling `status`, if the information about the file type
     *       is cached, but permissions are not.
     *
     * \param ec Error code returned in case of failure.
     *
     * \returns `true` if the file is a regular file, according to the cached file type, otherwise `false`.
     *          If an error occurs, the overload taking `ec` returns `false`.
     *
     * \sa \ref directory_entry::status.
     */
    bool is_regular_file(system::error_code& ec) const noexcept
    {
        return this->file_type(ec) == filesystem::regular_file;
    }

    /*! \overload */
    bool is_regular_file() const
    {
        return this->file_type() == filesystem::regular_file;
    }

    /*!
     * \brief Checks if the file is a directory.
     *
     * \effects Equivalent to `is_directory(status(ec))` or `is_directory(status())`, respectively.
     *
     * \note The implementation may be more efficient than calling `status`, if the information about the file type
     *       is cached, but permissions are not.
     *
     * \param ec Error code returned in case of failure.
     *
     * \returns `true` if the file is a directory, according to the cached file type, otherwise `false`.
     *          If an error occurs, the overload taking `ec` returns `false`.
     *
     * \sa \ref directory_entry::status.
     */
    bool is_directory(system::error_code& ec) const noexcept
    {
        return this->file_type(ec) == filesystem::directory_file;
    }

    /*! \overload */
    bool is_directory() const
    {
        return this->file_type() == filesystem::directory_file;
    }

    /*!
     * \brief Checks if the file is a symbolic link.
     *
     * \effects Equivalent to `is_symlink(symlink_status(ec))` or ``is_symlink(symlink_status())`, respectively.
     *
     * \note The implementation may be more efficient than calling `symlink_status`, if the information about
     *       the symlink file type is cached, but permissions are not.
     *
     * \param ec Error code returned in case of failure.
     *
     * \returns `true` if the file is a symbolic link, according to the cached symlink file type, otherwise `false`.
     *          If an error occurs, the overload taking `ec` returns `false`.
     *
     * \sa \ref directory_entry::symlink_status.
     */
    bool is_symlink(system::error_code& ec) const noexcept
    {
        return this->symlink_file_type(ec) == filesystem::symlink_file;
    }

    /*! \overload */
    bool is_symlink() const
    {
        return this->symlink_file_type() == filesystem::symlink_file;
    }

    /*!
     * \brief Checks if the file is a block special file.
     *
     * \effects Equivalent to `is_block_file(status(ec))` or `is_block_file(status())`, respectively.
     *
     * \note The implementation may be more efficient than calling `status`, if the information about the file type
     *       is cached, but permissions are not.
     *
     * \param ec Error code returned in case of failure.
     *
     * \returns `true` if the file is a block special file, according to the cached file type, otherwise `false`.
     *          If an error occurs, the overload taking `ec` returns `false`.
     *
     * \sa \ref directory_entry::status.
     */
    bool is_block_file(system::error_code& ec) const noexcept
    {
        return this->file_type(ec) == filesystem::block_file;
    }

    /*! \overload */
    bool is_block_file() const
    {
        return this->file_type() == filesystem::block_file;
    }

    /*!
     * \brief Checks if the file is a character special file.
     *
     * \effects Equivalent to `is_character_file(status(ec))` or `is_character_file(status())`, respectively.
     *
     * \note The implementation may be more efficient than calling `status`, if the information about the file type
     *       is cached, but permissions are not.
     *
     * \param ec Error code returned in case of failure.
     *
     * \returns `true` if the file is a character special file, according to the cached file type, otherwise `false`.
     *          If an error occurs, the overload taking `ec` returns `false`.
     *
     * \sa \ref directory_entry::status.
     */
    bool is_character_file(system::error_code& ec) const noexcept
    {
        return this->file_type(ec) == filesystem::character_file;
    }

    /*! \overload */
    bool is_character_file() const
    {
        return this->file_type() == filesystem::character_file;
    }

    /*!
     * \brief Checks if the file is a FIFO or pipe file.
     *
     * \effects Equivalent to `is_fifo(status(ec))` or `is_fifo(status())`, respectively.
     *
     * \note The implementation may be more efficient than calling `status`, if the information about the file type
     *       is cached, but permissions are not.
     *
     * \param ec Error code returned in case of failure.
     *
     * \returns `true` if the file is a FIFO or pipe file, according to the cached file type, otherwise `false`.
     *          If an error occurs, the overload taking `ec` returns `false`.
     *
     * \sa \ref directory_entry::status.
     */
    bool is_fifo(system::error_code& ec) const noexcept
    {
        return this->file_type(ec) == filesystem::fifo_file;
    }

    /*! \overload */
    bool is_fifo() const
    {
        return this->file_type() == filesystem::fifo_file;
    }

    /*!
     * \brief Checks if the file is a socket file.
     *
     * \effects Equivalent to `is_socket(status(ec))` or `is_socket(status())`, respectively.
     *
     * \note The implementation may be more efficient than calling `status`, if the information about the file type
     *       is cached, but permissions are not.
     *
     * \param ec Error code returned in case of failure.
     *
     * \returns `true` if the file is a socket file, according to the cached file type, otherwise `false`.
     *          If an error occurs, the overload taking `ec` returns `false`.
     *
     * \sa \ref directory_entry::status.
     */
    bool is_socket(system::error_code& ec) const noexcept
    {
        return this->file_type(ec) == filesystem::socket_file;
    }

    /*! \overload */
    bool is_socket() const
    {
        return this->file_type() == filesystem::socket_file;
    }

    /*!
     * \brief Checks if the file is a reparse file.
     *
     * \effects Equivalent to `is_reparse_file(symlink_status(ec))` or `is_reparse_file(symlink_status())`, respectively.
     *
     * \note The implementation may be more efficient than calling `symlink_status`, if the information about
     *       the symlink file type is cached, but permissions are not.
     *
     * \param ec Error code returned in case of failure.
     *
     * \returns `true` if the file is a reparse file, according to the cached symlink file type, otherwise `false`.
     *          If an error occurs, the overload taking `ec` returns `false`.
     *
     * \sa \ref directory_entry::symlink_status.
     */
    bool is_reparse_file(system::error_code& ec) const noexcept
    {
        return this->symlink_file_type(ec) == filesystem::reparse_file;
    }

    /*! \overload */
    bool is_reparse_file() const
    {
        return this->symlink_file_type() == filesystem::reparse_file;
    }

    /*!
     * \brief Checks if the file is of an unknown or other type.
     *
     * \effects Equivalent to `is_other(status(ec))` or `is_other(status())`, respectively.
     *
     * \note The implementation may be more efficient than calling `status`, if the information about the file type
     *       is cached, but permissions are not.
     *
     * \param ec Error code returned in case of failure.
     *
     * \returns `true` if the file is of an unknown or other type, according to the cached file type, otherwise
     *          `false`. If an error occurs, the overload taking `ec` returns `false`.
     *
     * \sa \ref directory_entry::status.
     */
    bool is_other(system::error_code& ec) const noexcept
    {
        filesystem::file_type ft = this->file_type(ec);
        return ft != filesystem::status_error && ft != filesystem::file_not_found &&
            ft != filesystem::regular_file && ft != filesystem::directory_file;
    }

    /*! \overload */
    bool is_other() const
    {
        filesystem::file_type ft = this->file_type();
        return ft != filesystem::status_error && ft != filesystem::file_not_found &&
            ft != filesystem::regular_file && ft != filesystem::directory_file;
    }

    /*!
     * \brief Compares two directory entries for equivalence.
     *
     * \param rhs Directory entry to compare with.
     *
     * \returns `this->path() == rhs.path()`.
     */
    bool operator==(directory_entry const& rhs) const { return m_path == rhs.m_path; }

    /*!
     * \brief Compares two directory entries for inequivalence.
     *
     * \param rhs Directory entry to compare with.
     *
     * \returns `this->path() != rhs.path()`.
     */
    bool operator!=(directory_entry const& rhs) const { return m_path != rhs.m_path; }

    /*!
     * \brief Compares two directory entries for less.
     *
     * \param rhs Directory entry to compare with.
     *
     * \returns `this->path() < rhs.path()`.
     */
    bool operator<(directory_entry const& rhs) const { return m_path < rhs.m_path; }

    /*!
     * \brief Compares two directory entries for less or equal.
     *
     * \param rhs Directory entry to compare with.
     *
     * \returns `this->path() <= rhs.path()`.
     */
    bool operator<=(directory_entry const& rhs) const { return m_path <= rhs.m_path; }

    /*!
     * \brief Compares two directory entries for greater.
     *
     * \param rhs Directory entry to compare with.
     *
     * \returns `this->path() > rhs.path()`.
     */
    bool operator>(directory_entry const& rhs) const { return m_path > rhs.m_path; }

    /*!
     * \brief Compares two directory entries for greater or equal.
     *
     * \param rhs Directory entry to compare with.
     *
     * \returns `this->path() >= rhs.path()`.
     */
    bool operator>=(directory_entry const& rhs) const { return m_path >= rhs.m_path; }

#if !defined(BOOST_FILESYSTEM_DOXYGEN)
private:
    using update_mask = detail::directory_entry_update_mask;

    BOOST_FILESYSTEM_DECL void refresh_impl(update_mask mask, system::error_code* ec = nullptr) const;

    void assign_with_status(boost::filesystem::path&& p, file_status st, file_status symlink_st)
    {
        m_path = static_cast< boost::filesystem::path&& >(p);
        m_status = static_cast< file_status&& >(st);
        m_symlink_status = static_cast< file_status&& >(symlink_st);
    }

    void assign_with_status(boost::filesystem::path const& p, file_status st, file_status symlink_st)
    {
        m_path = p;
        m_status = static_cast< file_status&& >(st);
        m_symlink_status = static_cast< file_status&& >(symlink_st);
    }

    void replace_filename_with_status(boost::filesystem::path const& p, file_status st, file_status symlink_st)
    {
        m_path.replace_filename(p);
        m_status = static_cast< file_status&& >(st);
        m_symlink_status = static_cast< file_status&& >(symlink_st);
    }

private:
    boost::filesystem::path m_path;
    mutable file_status m_status;         // stat()-like
    mutable file_status m_symlink_status; // lstat()-like
#endif // !defined(BOOST_FILESYSTEM_DOXYGEN)
};

#if !defined(BOOST_FILESYSTEM_SOURCE) && !defined(BOOST_FILESYSTEM_DOXYGEN)

inline directory_entry::directory_entry(boost::filesystem::path const& p) :
    m_path(p)
{
#if BOOST_FILESYSTEM_VERSION >= 4
    refresh_impl(update_mask::all);
#endif
}

inline void directory_entry::assign(boost::filesystem::path&& p)
{
    m_path = static_cast< boost::filesystem::path&& >(p);
#if BOOST_FILESYSTEM_VERSION >= 4
    refresh_impl(update_mask::all);
#else
    m_status = file_status();
    m_symlink_status = file_status();
#endif
}

inline void directory_entry::assign(boost::filesystem::path const& p)
{
    m_path = p;
#if BOOST_FILESYSTEM_VERSION >= 4
    refresh_impl(update_mask::all);
#else
    m_status = file_status();
    m_symlink_status = file_status();
#endif
}

inline void directory_entry::replace_filename(boost::filesystem::path const& p)
{
    m_path.replace_filename(p);
#if BOOST_FILESYSTEM_VERSION >= 4
    refresh_impl(update_mask::all);
#else
    m_status = file_status();
    m_symlink_status = file_status();
#endif
}

#endif // !defined(BOOST_FILESYSTEM_SOURCE) && !defined(BOOST_FILESYSTEM_DOXYGEN)

namespace detail {
namespace path_traits {

// Dispatch function for integration with path class
template< typename Callback >
BOOST_FORCEINLINE typename Callback::result_type dispatch(directory_entry const& de, Callback cb, const codecvt_type* cvt, directory_entry_tag)
{
    boost::filesystem::path::string_type const& source = de.path().native();
    return cb(source.data(), source.data() + source.size(), cvt);
}

} // namespace path_traits
} // namespace detail

//--------------------------------------------------------------------------------------//
//                                                                                      //
//                            directory_entry overloads                                 //
//                                                                                      //
//--------------------------------------------------------------------------------------//

//! \name File Status Queries for Directory Entries
//! @{

//  Without these functions, calling (for example) 'is_directory' with a 'directory_entry' results in:
//  - a conversion to 'path' using 'operator boost::filesystem::path const&()',
//  - then a call to 'is_directory(path const& p)' which recomputes the status with 'detail::status(p)'.
//
//  These functions avoid a costly recomputation of the status if one calls 'is_directory(e)' instead of 'is_directory(e.status())'

/*!
 * \brief Returns the file status.
 *
 * \param e Directory entry.
 * \param ec Error code returned in case of failure.
 *
 * \returns `e.status(ec)` or `e.status(ec)`, respectively.
 */
inline file_status status(directory_entry const& e, system::error_code& ec) noexcept
{
    return e.status(ec);
}

/*! \overload */
inline file_status status(directory_entry const& e)
{
    return e.status();
}

/*!
 * \brief Returns the symlink file status.
 *
 * \param e Directory entry.
 * \param ec Error code returned in case of failure.
 *
 * \returns `e.symlink_status(ec)` or `e.symlink_status()`, respectively.
 */
inline file_status symlink_status(directory_entry const& e, system::error_code& ec) noexcept
{
    return e.symlink_status(ec);
}

/*! \overload */
inline file_status symlink_status(directory_entry const& e)
{
    return e.symlink_status();
}

/*!
 * \brief Checks if file type information is present.
 *
 * \param e Directory entry.
 * \param ec Error code returned in case of failure.
 *
 * \returns `e.file_type(ec) != filesystem::status_error` or `e.file_type() != filesystem::status_error`, respectively.
 */
inline bool type_present(directory_entry const& e, system::error_code& ec) noexcept
{
    return e.file_type(ec) != filesystem::status_error;
}

/*! \overload */
inline bool type_present(directory_entry const& e)
{
    return e.file_type() != filesystem::status_error;
}

/*!
 * \brief Checks if file status information is known.
 *
 * \param e Directory entry.
 * \param ec Error code returned in case of failure.
 *
 * \returns `filesystem::status_known(e.status(ec))` or `filesystem::status_known(e.status())`, respectively.
 */
inline bool status_known(directory_entry const& e, system::error_code& ec) noexcept
{
    return filesystem::status_known(e.status(ec));
}

/*! \overload */
inline bool status_known(directory_entry const& e)
{
    return filesystem::status_known(e.status());
}

/*!
 * \brief Checks if the file exists.
 *
 * \param e Directory entry.
 * \param ec Error code returned in case of failure.
 *
 * \returns `e.exists(ec)` or `e.exists()`, respectively.
 */
inline bool exists(directory_entry const& e, system::error_code& ec) noexcept
{
    return e.exists(ec);
}

/*! \overload */
inline bool exists(directory_entry const& e)
{
    return e.exists();
}

/*!
 * \brief Checks if the file is a regular file.
 *
 * \param e Directory entry.
 * \param ec Error code returned in case of failure.
 *
 * \returns `e.is_regular_file(ec)` or `e.is_regular_file()`, respectively.
 */
inline bool is_regular_file(directory_entry const& e, system::error_code& ec) noexcept
{
    return e.is_regular_file(ec);
}

/*! \overload */
inline bool is_regular_file(directory_entry const& e)
{
    return e.is_regular_file();
}

/*!
 * \brief Checks if the file is a directory.
 *
 * \param e Directory entry.
 * \param ec Error code returned in case of failure.
 *
 * \returns `e.is_directory(ec)` or `e.is_directory()`, respectively.
 */
inline bool is_directory(directory_entry const& e, system::error_code& ec) noexcept
{
    return e.is_directory(ec);
}

/*! \overload */
inline bool is_directory(directory_entry const& e)
{
    return e.is_directory();
}

/*!
 * \brief Checks if the file is a symbolic link.
 *
 * \param e Directory entry.
 * \param ec Error code returned in case of failure.
 *
 * \returns `e.is_symlink(ec)` or `e.is_symlink()`, respectively.
 */
inline bool is_symlink(directory_entry const& e, system::error_code& ec) noexcept
{
    return e.is_symlink(ec);
}

/*! \overload */
inline bool is_symlink(directory_entry const& e)
{
    return e.is_symlink();
}

/*!
 * \brief Checks if the file is a block special file.
 *
 * \param e Directory entry.
 * \param ec Error code returned in case of failure.
 *
 * \returns `e.is_block_file(ec)` or `e.is_block_file()`, respectively.
 */
inline bool is_block_file(directory_entry const& e, system::error_code& ec) noexcept
{
    return e.is_block_file(ec);
}

/*! \overload */
inline bool is_block_file(directory_entry const& e)
{
    return e.is_block_file();
}

/*!
 * \brief Checks if the file is a character special file.
 *
 * \param e Directory entry.
 * \param ec Error code returned in case of failure.
 *
 * \returns `e.is_character_file(ec)` or `e.is_character_file()`, respectively.
 */
inline bool is_character_file(directory_entry const& e, system::error_code& ec) noexcept
{
    return e.is_character_file(ec);
}

/*! \overload */
inline bool is_character_file(directory_entry const& e)
{
    return e.is_character_file();
}

/*!
 * \brief Checks if the file is a FIFO or pipe file.
 *
 * \param e Directory entry.
 * \param ec Error code returned in case of failure.
 *
 * \returns `e.is_fifo(ec)` or `e.is_fifo()`, respectively.
 */
inline bool is_fifo(directory_entry const& e, system::error_code& ec) noexcept
{
    return e.is_fifo(ec);
}

/*! \overload */
inline bool is_fifo(directory_entry const& e)
{
    return e.is_fifo();
}

/*!
 * \brief Checks if the file is a socket file.
 *
 * \param e Directory entry.
 * \param ec Error code returned in case of failure.
 *
 * \returns `e.is_socket(ec)` or `e.is_socket()`, respectively.
 */
inline bool is_socket(directory_entry const& e, system::error_code& ec) noexcept
{
    return e.is_socket(ec);
}

/*! \overload */
inline bool is_socket(directory_entry const& e)
{
    return e.is_socket();
}

/*!
 * \brief Checks if the file is a reparse file.
 *
 * \param e Directory entry.
 * \param ec Error code returned in case of failure.
 *
 * \returns `e.is_reparse_file(ec)` or `e.is_reparse_file()`, respectively.
 */
inline bool is_reparse_file(directory_entry const& e, system::error_code& ec) noexcept
{
    return e.is_reparse_file(ec);
}

/*! \overload */
inline bool is_reparse_file(directory_entry const& e)
{
    return e.is_reparse_file();
}

/*!
 * \brief Checks if the file is of an unknown or other type.
 *
 * \param e Directory entry.
 * \param ec Error code returned in case of failure.
 *
 * \returns `e.is_other(ec)` or `e.is_other()`, respectively.
 */
inline bool is_other(directory_entry const& e, system::error_code& ec) noexcept
{
    return e.is_other(ec);
}

/*! \overload */
inline bool is_other(directory_entry const& e)
{
    return e.is_other();
}

//! @}

//--------------------------------------------------------------------------------------//
//                                                                                      //
//                                directory_iterator                                    //
//                                                                                      //
//--------------------------------------------------------------------------------------//

namespace detail {

struct dir_itr_imp :
    public boost::intrusive_ref_counter< dir_itr_imp >
{
#ifdef BOOST_FILESYSTEM_WINDOWS_API
    bool close_handle;
    unsigned char extra_data_format;
    std::size_t current_offset;
#endif
    directory_entry dir_entry;
    void* handle;

    dir_itr_imp() noexcept :
#ifdef BOOST_FILESYSTEM_WINDOWS_API
        close_handle(false),
        extra_data_format(0u),
        current_offset(0u),
#endif
        handle(nullptr)
    {
    }
    BOOST_FILESYSTEM_DECL ~dir_itr_imp() noexcept;

    BOOST_FILESYSTEM_DECL static void* operator new(std::size_t class_size, std::size_t extra_size) noexcept;
    BOOST_FILESYSTEM_DECL static void operator delete(void* p, std::size_t extra_size) noexcept;
    BOOST_FILESYSTEM_DECL static void operator delete(void* p) noexcept;
};

} // namespace detail

/*!
 * \brief Objects of type `directory_iterator` provide standard library compliant iteration over the contents of a directory.
 *
 * `directory_iterator` satisfies the requirements of an input iterator (C++ Std, 24.2.1, Input iterators
 * [input.iterators]).
 *
 * A `directory_iterator` reads successive elements from the directory for which it was constructed, as if by calling
 * ISO/IEC 9945 [`readdir()`](https://pubs.opengroup.org/onlinepubs/000095399/functions/readdir.html) or
 * [`readdir_r()`](https://pubs.opengroup.org/onlinepubs/000095399/functions/readdir_r.html). After
 * a `directory_iterator` is constructed, and every time `operator++` is called, it reads a directory element and
 * stores information about it in an object of type \ref directory_entry.
 *
 * `operator++` is not equality preserving; that is, `i == j` does not imply that `++i == ++j`.
 *
 * \note The practical consequence of not preserving equality is that directory iterators can only be used for
 *       single-pass algorithms.
 *
 * If the end of the directory elements is reached, the iterator shall become equal to the end iterator value.
 * The constructor `directory_iterator()` with no arguments always constructs an end iterator object, which shall be
 * the only valid iterator for the end condition. Invoking `operator*` on an end iterator has undefined behavior. For
 * any other iterator value a `const directory_entry&` is returned. Similarly, invoking `operator->` on an end iterator
 * results in undefined behavior. For any other iterator value a `const directory_entry*` is returned.
 *
 * Two end iterators are always equal. An end iterator is never equal to a non-end iterator.
 *
 * The result of calling the `path()` member of the `directory_entry` object obtained by dereferencing
 * a `directory_iterator` is a reference to a path object composed of the directory argument from which the iterator
 * was constructed with filename of the directory entry appended as if by `operator/=`.
 *
 * Directory iteration shall not yield directory entries for the current (dot) and parent (dot dot) directories.
 *
 * The order of directory entries obtained by dereferencing successive increments of a `directory_iterator` is
 * unspecified. The order may also be different between different iterations over the same directory.
 *
 * \remark
 * \parblock
 * Programs performing directory iteration may wish to test if the path obtained by dereferencing a directory iterator
 * actually exists. It could be a symbolic link to a non-existent file. Programs recursively walking directory trees
 * for purposes of removing and renaming entries may wish to avoid following symbolic links.
 *
 * If a file is removed from or added to a directory after the construction of a `directory_iterator` for the
 * directory, it is unspecified whether or not subsequent incrementing of the iterator will ever result in an iterator
 * whose value is the removed or added directory entry. See ISO/IEC 9945
 * [`readdir()`](https://pubs.opengroup.org/onlinepubs/000095399/functions/readdir.html).
 * \endparblock
 *
 * `directory_iterator` objects maintain an internal shared state. When a `directory_iterator` is copied, the copy
 * refers to the same state as the original. Dereferencing the copy produces the same directory entry as the original.
 * It is unspecified whether modifying a directory iterator or its directory entry affects other directory iterators
 * referring to the same shared state. Concurrently accessing multiple iterators referring to the same shared state
 * results in undefined behavior.
 *
 * \note Dereferencing a `directory_iterator` returns a reference to `directory_entry`, which may be stored in
 *       the shared state. Many methods of `directory_entry` may update cached information stored in the entry,
 *       which would constitute a data race if done concurrently in multiple threads.
 *
 * \remark
 * \parblock
 * Until C++17, iterators are required to be copyable. In C++20 and later, iterator concepts no longer require
 * copyability, but much of the existing code still assumes iterators are copyable. On the other hand, ISO/IEC 9945
 * [`readdir()`](https://pubs.opengroup.org/onlinepubs/000095399/functions/readdir.html) and the equivalent
 * Windows APIs don't support deep copying of the directory iteration state. This necessitates the shared state
 * design, along with the caveats described above. Future operating systems may support copying directory iteration
 * state, which would allow for a simpler and more natural design of the iterator.
 *
 * Users are advised to avoid using multiple copies of the same directory iterator and relying on the shared state
 * effects. Prefer move operations for passing directory iterators and construct separate directory iterators from
 * paths when independent iterators are needed.
 * \endparblock
 *
 * \sa \ref recursive_directory_iterator.
 */
class directory_iterator :
    public boost::iterator_facade<
        directory_iterator,
        directory_entry,
        boost::single_pass_traversal_tag
    >
{
#if !defined(BOOST_FILESYSTEM_DOXYGEN)
    friend class boost::iterator_core_access;

    // iterator_facade derived classes don't seem to like implementations in
    // separate translation unit dll's, so forward to detail functions in the implementation below
    friend BOOST_FILESYSTEM_DECL void detail::directory_iterator_construct
    (
        directory_iterator& it,
        path const& p,
        directory_options opts,
        detail::directory_iterator_params* params,
        system::error_code* ec
    );
    friend BOOST_FILESYSTEM_DECL void detail::directory_iterator_increment(directory_iterator& it, system::error_code* ec);

    friend BOOST_FILESYSTEM_DECL void detail::recursive_directory_iterator_increment(recursive_directory_iterator& it, system::error_code* ec);
#endif // !defined(BOOST_FILESYSTEM_DOXYGEN)

public:
    /*!
     * \brief Default constructor.
     *
     * \effects Constructs the end iterator.
     */
    directory_iterator() noexcept {}

    /*!
     * \brief Constructs an iterator representing the first entry in the directory.
     *
     * \effects
     * Constructs an iterator representing the first entry in the directory `p` resolves to, if any; otherwise,
     * the end iterator.
     *
     * If opening the directory fails with a `permission_denied` error and
     * `(opts & directory_options::skip_permission_denied) != 0`, constructs the end iterator and ignores the error.
     *
     * For the overloads taking `ec`, an end iterator is constructed in case of error.
     *
     * \post Unless the end iterator was constructed, `*this` points to the first entry.
     *
     * \param p Path to directory to iterate over.
     * \param opts Directory iteration options. `directory_options::none` if not specified.
     * \param ec Error code returned in case of failure.
     *
     * \remark To iterate over the current directory, use `directory_iterator(".")` rather than
     *         `directory_iterator("")`.
     */
    directory_iterator(path const& p, directory_options opts, system::error_code& ec) noexcept
    {
        detail::directory_iterator_construct(*this, p, opts, nullptr, &ec);
    }

    /*! \overload */
    directory_iterator(path const& p, system::error_code& ec) noexcept
    {
        detail::directory_iterator_construct(*this, p, directory_options::none, nullptr, &ec);
    }

    /*! \overload */
    explicit directory_iterator(path const& p, directory_options opts = directory_options::none)
    {
        detail::directory_iterator_construct(*this, p, opts, nullptr, nullptr);
    }

    /*!
     * \brief Copy constructor.
     *
     * \effects
     * If `that` is an end iterator, creates an end iterator. Otherwise, creates an iterator referring to the same
     * shared state as `that`.
     *
     * \post `*this == that`.
     *
     * \param that Iterator to copy from.
     */
    directory_iterator(directory_iterator const& that) = default;

    /*!
     * \brief Copy assignment operator.
     *
     * \effects
     * If `that` is an end iterator, makes `*this` equal to end iterator. Otherwise, makes `*this` refer to the same
     * shared state as `that`.
     *
     * \post `*this == that`.
     *
     * \param that Iterator to copy from.
     *
     * \returns `*this`.
     */
    directory_iterator& operator=(directory_iterator const& that) = default;

    /*!
     * \brief Move constructor.
     *
     * \effects
     * If `that` is an end iterator, creates an end iterator. Otherwise, creates an iterator referring to the same
     * shared state as `that`.
     *
     * \post `*this` is equal to the original value of `that`, `that == directory_iterator()`.
     *
     * \param that Iterator to move from.
     */
    directory_iterator(directory_iterator&& that) noexcept :
        m_imp(static_cast< boost::intrusive_ptr< detail::dir_itr_imp >&& >(that.m_imp))
    {
    }

    /*!
     * \brief Move assignment operator.
     *
     * \effects
     * If `that` is an end iterator, makes `*this` equal to end iterator. Otherwise, makes `*this` refer to the same
     * shared state as `that`.
     *
     * \post `*this` is equal to the original value of `that`, `that == directory_iterator()`.
     *
     * \param that Iterator to move from.
     *
     * \returns `*this`.
     */
    directory_iterator& operator=(directory_iterator&& that) noexcept
    {
        m_imp = static_cast< boost::intrusive_ptr< detail::dir_itr_imp >&& >(that.m_imp);
        return *this;
    }

    /*!
     * \brief Advances the iterator to the next directory entry.
     *
     * \effects
     * As specified by the C++ Standard, 24.1.1 Input iterators [input.iterators]. In case of error the iterator
     * is left in the end state.
     *
     * \param ec Error code returned in case of failure.
     *
     * \returns `*this`.
     */
    directory_iterator& increment(system::error_code& ec)
    {
        detail::directory_iterator_increment(*this, &ec);
        return *this;
    }

#if defined(BOOST_FILESYSTEM_DOXYGEN)
    /*!
     * \brief Advances the iterator to the next directory entry.
     *
     * \effects
     * As specified by the C++ Standard, 24.1.1 Input iterators [input.iterators].
     *
     * \returns `*this`.
     *
     * \sa \ref directory_iterator::increment.
     */
    directory_iterator& operator++();

    /*!
     * \brief Dereferences the iterator.
     *
     * \returns The directory entry referenced by the iterator.
     */
    directory_entry const& operator*() const noexcept;

    /*!
     * \brief Indirection operator of the iterator.
     *
     * \returns The directory entry referenced by the iterator.
     */
    const directory_entry* operator->() const noexcept;

    /*!
     * \brief Compares two directory iterators for equivalence.
     *
     * \param that Directory iterator to compare with.
     *
     * \returns `true` if both `*this` and `that` are end iterators or refer to the same shared state,
     *          otherwise `false`.
     */
    bool operator== (directory_iterator const& that) const noexcept;

    /*!
     * \brief Compares two directory iterators for inequivalence.
     *
     * \param that Directory iterator to compare with.
     *
     * \returns `!(*this == that)`.
     */
    bool operator!= (directory_iterator const& that) const noexcept;
#endif // defined(BOOST_FILESYSTEM_DOXYGEN)

#if !defined(BOOST_FILESYSTEM_DOXYGEN)
private:
    boost::iterator_facade<
        directory_iterator,
        directory_entry,
        boost::single_pass_traversal_tag
    >::reference dereference() const
    {
        BOOST_ASSERT_MSG(!is_end(), "attempt to dereference end directory iterator");
        return m_imp->dir_entry;
    }

    void increment() { detail::directory_iterator_increment(*this, nullptr); }

    bool equal(directory_iterator const& rhs) const noexcept
    {
        return m_imp == rhs.m_imp || (is_end() && rhs.is_end());
    }

    bool is_end() const noexcept
    {
        // Note: The check for handle is needed because the iterator can be copied and the copy
        // can be incremented to end while the original iterator still refers to the same dir_itr_imp.
        return !m_imp || !m_imp->handle;
    }

private:
    // intrusive_ptr provides the shallow-copy semantics required for single pass iterators
    // (i.e. InputIterators). The end iterator is indicated by is_end().
    boost::intrusive_ptr< detail::dir_itr_imp > m_imp;
#endif // !defined(BOOST_FILESYSTEM_DOXYGEN)
};

//  enable directory_iterator C++11 range-based for statement use  --------------------//

// begin() and end() are only used by a range-based for statement in the context of
// auto - thus the top-level const is stripped - so returning const is harmless and
// emphasizes begin() is just a pass through.

//! \name Range Accessors and Range-based Loop Support
//! @{

/*!
 * \brief Returns beginning of the range.
 *
 * \param iter Directory iterator representing the iteration range.
 *
 * \returns `iter`.
 */
inline directory_iterator const& begin(directory_iterator const& iter) noexcept
{
    return iter;
}

/*!
 * \brief Returns ending of the range.
 *
 * \returns `directory_iterator()`.
 */
inline directory_iterator end(directory_iterator const&) noexcept
{
    return directory_iterator();
}

/*!
 * \brief Returns beginning of the immutable range.
 *
 * \param iter Directory iterator representing the iteration range.
 *
 * \returns `iter`.
 */
inline directory_iterator const& cbegin(directory_iterator const& iter) noexcept
{
    return iter;
}

/*!
 * \brief Returns ending of the immutable range.
 *
 * \returns `directory_iterator()`.
 */
inline directory_iterator cend(directory_iterator const&) noexcept
{
    return directory_iterator();
}

//  enable directory_iterator BOOST_FOREACH  -----------------------------------------//

/*!
 * \brief Returns beginning of the iteration range.
 *
 * \remark This function is part of integration with `BOOST_FOREACH`.
 *
 * \param iter Directory iterator representing the iteration range.
 *
 * \returns `iter`.
 */
inline directory_iterator& range_begin(directory_iterator& iter) noexcept
{
    return iter;
}

/*! \overload */
inline directory_iterator range_begin(directory_iterator const& iter) noexcept
{
    return iter;
}

/*!
 * \brief Returns ending of the iteration range.
 *
 * \remark This function is part of integration with `BOOST_FOREACH`.
 *
 * \returns `directory_iterator()`.
 */
inline directory_iterator range_end(directory_iterator&) noexcept
{
    return directory_iterator();
}

/*! \overload */
inline directory_iterator range_end(directory_iterator const&) noexcept
{
    return directory_iterator();
}

//! @}

} // namespace filesystem

//! Boost.Range type trait to deduce mutable iterator type from a range type
template< typename C, typename Enabler >
struct range_mutable_iterator;

/*!
 * \brief `range_mutable_iterator` trait specialization.
 *
 * \remark This specialization is part of integration with Boost.Range.
 *
 * The type trait produces `directory_iterator` as the iterator type for `directory_iterator` mutable range type.
 */
template< >
struct range_mutable_iterator< boost::filesystem::directory_iterator, void >
{
    using type = boost::filesystem::directory_iterator;
};

//! Boost.Range type trait to deduce immutable iterator type from a range type
template< typename C, typename Enabler >
struct range_const_iterator;

/*!
 * \brief `range_const_iterator` trait specialization.
 *
 * \remark This specialization is part of integration with Boost.Range.
 *
 * The type trait produces `directory_iterator` as the iterator type for `directory_iterator` immutable range type.
 */
template< >
struct range_const_iterator< boost::filesystem::directory_iterator, void >
{
    using type = boost::filesystem::directory_iterator;
};

namespace filesystem {

//--------------------------------------------------------------------------------------//
//                                                                                      //
//                           recursive_directory_iterator                               //
//                                                                                      //
//--------------------------------------------------------------------------------------//

namespace detail {

struct recur_dir_itr_imp :
    public boost::intrusive_ref_counter< recur_dir_itr_imp >
{
    using element_type = directory_iterator;
    std::vector< element_type > m_stack;
    directory_options m_options;

    explicit recur_dir_itr_imp(directory_options opts) noexcept : m_options(opts) {}
};

} // namespace detail

/*!
 * \brief Objects of type `recursive_directory_iterator` provide standard library compliant iteration over the contents
 *        of a directory, including recursion into its sub-directories.
 *
 * The behavior of a `recursive_directory_iterator` is the same as a `directory_iterator` unless otherwise specified.
 * The main differences are:
 *
 * - Incrementing a `recursive_directory_iterator` pointing to a directory causes that directory itself to be iterated
 *   over, as specified by the `operator++` and `increment` functions.
 * - When a `recursive_directory_iterator` reaches the end of the directory currently being iterated over, or when
 *   `pop()` is called, iteration depth is decremented, and iteration of the parent directory continues.
 *
 * \note Like `directory_iterator`, `recursive_directory_iterator` objects also maintain internal shared state, so
 *       the same caveats as for `directory_iterator` apply.
 *
 * \sa \ref directory_iterator
 */
class recursive_directory_iterator :
    public boost::iterator_facade<
        recursive_directory_iterator,
        directory_entry,
        boost::single_pass_traversal_tag
    >
{
#if !defined(BOOST_FILESYSTEM_DOXYGEN)
    friend class boost::iterator_core_access;

    friend BOOST_FILESYSTEM_DECL void detail::recursive_directory_iterator_construct
    (
        recursive_directory_iterator& it,
        path const& dir_path,
        directory_options opts,
        system::error_code* ec
    );
    friend BOOST_FILESYSTEM_DECL void detail::recursive_directory_iterator_increment(recursive_directory_iterator& it, system::error_code* ec);
    friend BOOST_FILESYSTEM_DECL void detail::recursive_directory_iterator_pop(recursive_directory_iterator& it, system::error_code* ec);
#endif // !defined(BOOST_FILESYSTEM_DOXYGEN)

public:
    /*!
     * \brief Default constructor.
     *
     * \effects Constructs the end iterator.
     */
    recursive_directory_iterator() noexcept {}

    /*!
     * \brief Constructs an iterator representing the first entry in the directory.
     *
     * \effects
     * Constructs an iterator representing the first entry in the directory `p` resolves to, if any; otherwise,
     * the end iterator.
     *
     * If opening the directory fails with a `permission_denied` error and
     * `(opts & directory_options::skip_permission_denied) != 0`, constructs the end iterator and ignores the error.
     *
     * For the overloads taking `ec`, an end iterator is constructed in case of error.
     *
     * \post Unless the end iterator was constructed, `*this` points to the first entry and
     *       `depth() == 0 && recursion_pending() == true`.
     *
     * \param p Path to directory to iterate over.
     * \param opts Directory iteration options. `directory_options::none` if not specified.
     * \param ec Error code returned in case of failure.
     *
     * \remark To iterate over the current directory, use `recursive_directory_iterator(".")` rather than
     *         `recursive_directory_iterator("")`.
     *
     * \remark By default, `recursive_directory_iterator` does not follow directory symlinks. To follow directory
     *         symlinks, specify `directory_options::follow_directory_symlink`.
     */
    recursive_directory_iterator(path const& p, directory_options opts, system::error_code& ec)
    {
        detail::recursive_directory_iterator_construct(*this, p, opts, &ec);
    }

    /*! \overload */
    recursive_directory_iterator(path const& p, system::error_code& ec)
    {
        detail::recursive_directory_iterator_construct(*this, p, directory_options::none, &ec);
    }

    /*! \overload */
    recursive_directory_iterator(path const& p, directory_options opts)
    {
        detail::recursive_directory_iterator_construct(*this, p, opts, nullptr);
    }

    /*! \overload */
    explicit recursive_directory_iterator(path const& p)
    {
        detail::recursive_directory_iterator_construct(*this, p, directory_options::none, nullptr);
    }

    /*!
     * \brief Copy constructor.
     *
     * \effects
     * If `that` is an end iterator, creates an end iterator. Otherwise, creates an iterator referring to the same
     * shared state as `that`.
     *
     * \post `*this == that`.
     *
     * \param that Iterator to copy from.
     */
    recursive_directory_iterator(recursive_directory_iterator const& that) = default;

    /*!
     * \brief Copy assignment operator.
     *
     * \effects
     * If `that` is an end iterator, makes `*this` equal to end iterator. Otherwise, makes `*this` refer to the same
     * shared state as `that`.
     *
     * \post `*this == that`.
     *
     * \param that Iterator to copy from.
     *
     * \returns `*this`.
     */
    recursive_directory_iterator& operator=(recursive_directory_iterator const& that) = default;

    /*!
     * \brief Move constructor.
     *
     * \effects
     * If `that` is an end iterator, creates an end iterator. Otherwise, creates an iterator referring to the same
     * shared state as `that`.
     *
     * \post `*this` is equal to the original value of `that`, `that == recursive_directory_iterator()`.
     *
     * \param that Iterator to move from.
     */
    recursive_directory_iterator(recursive_directory_iterator&& that) noexcept :
        m_imp(static_cast< boost::intrusive_ptr< detail::recur_dir_itr_imp >&& >(that.m_imp))
    {
    }

    /*!
     * \brief Move assignment operator.
     *
     * \effects
     * If `that` is an end iterator, makes `*this` equal to end iterator. Otherwise, makes `*this` refer to the same
     * shared state as `that`.
     *
     * \post `*this` is equal to the original value of `that`, `that == recursive_directory_iterator()`.
     *
     * \param that Iterator to move from.
     *
     * \returns `*this`.
     */
    recursive_directory_iterator& operator=(recursive_directory_iterator&& that) noexcept
    {
        m_imp = static_cast< boost::intrusive_ptr< detail::recur_dir_itr_imp >&& >(that.m_imp);
        return *this;
    }

    /*!
     * \brief Advances the iterator to the next entry.
     *
     * \effects
     * As specified by the C++ Standard, 24.1.1 Input iterators [input.iterators], except:
     *
     * - if `recursion_pending() && is_directory(this->status())` then directory is recursively iterated into
     *   and `depth()` is incremented.
     * - if opening the directory fails with a `permission_denied` error and
     *   `(m_options & directory_options::skip_permission_denied) != 0`, increments on the current level and ignores
     *   the error.
     * - if there are no more directory entries at this level then `depth()` is decremented and iteration of the parent
     *   directory resumes.
     * - If the operation completes with an error, then if `(m_options & directory_options::pop_on_error) != 0`,
     *   the iterator is left in a state as if after repeatedly calling `pop()` until it succeeds or the iterator
     *   becomes equal to an end iterator. Otherwise, the iterator is left equal to an end iterator.
     *
     * \post `recursion_pending() == true`.
     *
     * \param ec Error code returned in case of failure.
     *
     * \returns `*this`
     */
    recursive_directory_iterator& increment(system::error_code& ec) noexcept
    {
        detail::recursive_directory_iterator_increment(*this, &ec);
        return *this;
    }

#if defined(BOOST_FILESYSTEM_DOXYGEN)
    /*!
     * \brief Advances the iterator to the next entry.
     *
     * \effects
     * Equivalent to calling `increment(ec)`, where `ec` is an instance of `system::error_code`. Throws
     * `filesystem_error` with `ec` if the call fails.
     *
     * \returns `*this`.
     *
     * \sa \ref recursive_directory_iterator::increment.
     */
    recursive_directory_iterator& operator++();

    /*!
     * \brief Dereferences the iterator.
     *
     * \returns The directory entry referenced by the iterator.
     */
    directory_entry const& operator*() const noexcept;

    /*!
     * \brief Indirection operator of the iterator.
     *
     * \returns The directory entry referenced by the iterator.
     */
    const directory_entry* operator->() const noexcept;

    /*!
     * \brief Compares two recursive directory iterators for equivalence.
     *
     * \param that Recursive directory iterator to compare with.
     *
     * \returns `true` if both `*this` and `that` are end iterators or refer to the same shared state,
     *          otherwise `false`.
     */
    bool operator== (recursive_directory_iterator const& that) const noexcept;

    /*!
     * \brief Compares two recursive directory iterators for inequivalence.
     *
     * \param that Recursive directory iterator to compare with.
     *
     * \returns `!(*this == that)`.
     */
    bool operator!= (recursive_directory_iterator const& that) const noexcept;
#endif // defined(BOOST_FILESYSTEM_DOXYGEN)

    /*!
     * \brief Returns the depth of the current directory entry.
     *
     * The top level directory with path to which the iterator was constructed has depth 0. Immediate subdirectories
     * of the top level directory have depth 1, and so on.
     *
     * \pre `*this != recursive_directory_iterator()`.
     *
     * \returns Current iteration depth.
     */
    int depth() const noexcept
    {
        BOOST_ASSERT_MSG(!is_end(), "depth() on end recursive_directory_iterator");
        return static_cast< int >(m_imp->m_stack.size() - 1u);
    }

    /*!
     * \brief Checks if recursion is pending.
     *
     * \pre `*this != recursive_directory_iterator()`.
     *
     * \returns `true` if `disable_recursion_pending(true)` has not been called after the prior construction or
     *          increment operation, otherwise `false`.
     */
    bool recursion_pending() const noexcept
    {
        BOOST_ASSERT_MSG(!is_end(), "recursion_pending() on end recursive_directory_iterator");
        return (m_imp->m_options & directory_options::_detail_no_push) == directory_options::none;
    }

    /*!
     * \brief Disables or enables recursion into the current directory entry.
     *
     * \pre `*this != recursive_directory_iterator()`.
     * \post `recursion_pending() == value`.
     *
     * \param value Indicates whether the subsequent increment operation should attempt to recurse into subdirectory.
     */
    void disable_recursion_pending(bool value = true) noexcept
    {
        BOOST_ASSERT_MSG(!is_end(), "disable_recursion_pending() on end recursive_directory_iterator");
        if (value)
            m_imp->m_options |= directory_options::_detail_no_push;
        else
            m_imp->m_options &= ~directory_options::_detail_no_push;
    }

    /*!
     * \brief Pops the current directory from the iteration stack.
     *
     * \pre `*this != recursive_directory_iterator()`.
     *
     * \effects
     * If `depth() == 0`, sets `*this` to `recursive_directory_iterator()`. Otherwise, decrements `depth()`, ceases
     * iteration of the directory currently being iterated over, and continues iteration over the parent directory.
     * If the operation completes with an error, then if `directory_options::pop_on_error` was specified in options
     * on the iterator construction, the iterator is left in a state as if after repeatedly calling `pop()` until
     * it succeeds or the iterator becomes equal to an end iterator. Otherwise, the iterator is left equal
     * to an end iterator.
     *
     * \param ec Error code returned in case of failure.
     */
    void pop(system::error_code& ec) noexcept
    {
        detail::recursive_directory_iterator_pop(*this, &ec);
    }

    /*! \overload */
    void pop()
    {
        detail::recursive_directory_iterator_pop(*this, nullptr);
    }

    /*!
     * \brief Returns file status of the current directory entry.
     *
     * \note Use iterator dereferencing operators instead, e.g. `it->status()`.
     *
     * \returns `(*this)->status()`.
     *
     * \sa \ref directory_entry::status.
     */
    file_status status() const
    {
        BOOST_ASSERT_MSG(!is_end(), "status() on end recursive_directory_iterator");
        return m_imp->m_stack.back()->status();
    }

    /*!
     * \brief Returns symlink file status of the current directory entry.
     *
     * \note Use iterator dereferencing operators instead, e.g. `it->symlink_status()`.
     *
     * \returns `(*this)->symlink_status()`.
     *
     * \sa \ref directory_entry::symlink_status.
     */
    file_status symlink_status() const
    {
        BOOST_ASSERT_MSG(!is_end(), "symlink_status() on end recursive_directory_iterator");
        return m_imp->m_stack.back()->symlink_status();
    }

#if !defined(BOOST_FILESYSTEM_DOXYGEN)
private:
    boost::iterator_facade<
        recursive_directory_iterator,
        directory_entry,
        boost::single_pass_traversal_tag
    >::reference dereference() const
    {
        BOOST_ASSERT_MSG(!is_end(), "dereference of end recursive_directory_iterator");
        return *m_imp->m_stack.back();
    }

    void increment() { detail::recursive_directory_iterator_increment(*this, nullptr); }

    bool equal(recursive_directory_iterator const& rhs) const noexcept
    {
        return m_imp == rhs.m_imp || (is_end() && rhs.is_end());
    }

    bool is_end() const noexcept
    {
        // Note: The check for m_stack.empty() is needed because the iterator can be copied and the copy
        // can be incremented to end while the original iterator still refers to the same recur_dir_itr_imp.
        return !m_imp || m_imp->m_stack.empty();
    }

private:
    // intrusive_ptr provides the shallow-copy semantics required for single pass iterators
    // (i.e. InputIterators). The end iterator is indicated by is_end().
    boost::intrusive_ptr< detail::recur_dir_itr_imp > m_imp;
#endif // !defined(BOOST_FILESYSTEM_DOXYGEN)
};

//  enable recursive directory iterator C++11 range-base for statement use  ----------//

// begin() and end() are only used by a range-based for statement in the context of
// auto - thus the top-level const is stripped - so returning const is harmless and
// emphasizes begin() is just a pass through.

//! \name Range Accessors and Range-based Loop Support
//! @{

/*!
 * \brief Returns beginning of the range.
 *
 * \param iter Recursive directory iterator representing the iteration range.
 *
 * \returns `iter`.
 */
inline recursive_directory_iterator const& begin(recursive_directory_iterator const& iter) noexcept
{
    return iter;
}

/*!
 * \brief Returns ending of the range.
 *
 * \returns `recursive_directory_iterator()`.
 */
inline recursive_directory_iterator end(recursive_directory_iterator const&) noexcept
{
    return recursive_directory_iterator();
}

/*!
 * \brief Returns beginning of the immutable range.
 *
 * \param iter Directory iterator representing the iteration range.
 *
 * \returns `iter`.
 */
inline recursive_directory_iterator const& cbegin(recursive_directory_iterator const& iter) noexcept
{
    return iter;
}

/*!
 * \brief Returns ending of the immutable range.
 *
 * \returns `recursive_directory_iterator()`.
 */
inline recursive_directory_iterator cend(recursive_directory_iterator const&) noexcept
{
    return recursive_directory_iterator();
}

//  enable recursive directory iterator BOOST_FOREACH  -------------------------------//

/*!
 * \brief Returns beginning of the iteration range.
 *
 * \remark This function is part of integration with `BOOST_FOREACH`.
 *
 * \param iter Recursive directory iterator representing the iteration range.
 *
 * \returns `iter`.
 */
inline recursive_directory_iterator& range_begin(recursive_directory_iterator& iter) noexcept
{
    return iter;
}

/*! \overload */
inline recursive_directory_iterator range_begin(recursive_directory_iterator const& iter) noexcept
{
    return iter;
}

/*!
 * \brief Returns ending of the iteration range.
 *
 * \remark This function is part of integration with `BOOST_FOREACH`.
 *
 * \returns `recursive_directory_iterator()`.
 */
inline recursive_directory_iterator range_end(recursive_directory_iterator&) noexcept
{
    return recursive_directory_iterator();
}

/*! \overload */
inline recursive_directory_iterator range_end(recursive_directory_iterator const&) noexcept
{
    return recursive_directory_iterator();
}

//! @}

} // namespace filesystem

/*!
 * \brief `range_mutable_iterator` trait specialization.
 *
 * \remark This specialization is part of integration with Boost.Range.
 *
 * The type trait produces `recursive_directory_iterator` as the iterator type for `recursive_directory_iterator`
 * mutable range type.
 */
template< >
struct range_mutable_iterator< boost::filesystem::recursive_directory_iterator, void >
{
    typedef boost::filesystem::recursive_directory_iterator type;
};

/*!
 * \brief `range_const_iterator` trait specialization.
 *
 * \remark This specialization is part of integration with Boost.Range.
 *
 * The type trait produces `recursive_directory_iterator` as the iterator type for `recursive_directory_iterator`
 * immutable range type.
 */
template< >
struct range_const_iterator< boost::filesystem::recursive_directory_iterator, void >
{
    typedef boost::filesystem::recursive_directory_iterator type;
};

} // namespace boost

#if defined(BOOST_GCC) && (__GNUC__ == 12)
#pragma GCC diagnostic pop
#endif

#include <boost/filesystem/detail/footer.hpp>

#endif // BOOST_FILESYSTEM_DIRECTORY_HPP
