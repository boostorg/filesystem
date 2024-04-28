//  boost/filesystem/exception.hpp  -----------------------------------------------------//

//  Copyright Beman Dawes 2003
//  Copyright Andrey Semashev 2019-2026

//  Distributed under the Boost Software License, Version 1.0.
//  See http://www.boost.org/LICENSE_1_0.txt

//  Library home page: http://www.boost.org/libs/filesystem

#ifndef BOOST_FILESYSTEM_EXCEPTION_HPP
#define BOOST_FILESYSTEM_EXCEPTION_HPP

#include <boost/filesystem/config.hpp>
#include <boost/filesystem/path.hpp>

#include <string>
#include <boost/system/error_code.hpp>
#include <boost/system/system_error.hpp>
#include <boost/smart_ptr/intrusive_ptr.hpp>
#include <boost/smart_ptr/intrusive_ref_counter.hpp>

#include <boost/filesystem/detail/header.hpp> // must be the last #include

namespace boost {
namespace filesystem {

//--------------------------------------------------------------------------------------//
//                                                                                      //
//                            class filesystem_error                                    //
//                                                                                      //
//--------------------------------------------------------------------------------------//

/*!
 * \brief Exception type used to report file system errors from library functions.
 */
class BOOST_SYMBOL_VISIBLE filesystem_error :
    public system::system_error
{
    // see http://www.boost.org/more/error_handling.html for design rationale

public:
    /*!
     * \brief Constructs the exception.
     *
     * \post
     * | Expression                | Value                                  |
     * |---------------------------|----------------------------------------|
     * | `runtime_error::what()`   | A string containing `what_arg.c_str()` |
     * | `code()`                  | `ec`                                   |
     * | `path1().empty()`         | `true`                                 |
     * | `path2().empty()`         | `true`                                 |
     *
     * \param what_arg The explanatory string.
     * \param ec The error code.
     */
    BOOST_FILESYSTEM_DECL filesystem_error(const char* what_arg, system::error_code ec);

    /*!
     * \brief Constructs the exception.
     *
     * \post
     * | Expression                | Value                                  |
     * |---------------------------|----------------------------------------|
     * | `runtime_error::what()`   | A string containing `what_arg.c_str()` |
     * | `code()`                  | `ec`                                   |
     * | `path1().empty()`         | `true`                                 |
     * | `path2().empty()`         | `true`                                 |
     *
     * \param what_arg The explanatory string.
     * \param ec The error code.
     */
    BOOST_FILESYSTEM_DECL filesystem_error(std::string const& what_arg, system::error_code ec);

    /*!
     * \brief Constructs the exception.
     *
     * \post
     * | Expression                | Value                                  |
     * |---------------------------|----------------------------------------|
     * | `runtime_error::what()`   | A string containing `what_arg.c_str()` |
     * | `code()`                  | `ec`                                   |
     * | `path1()`                 | Reference to stored copy of `p1`       |
     * | `path2().empty()`         | `true`                                 |
     *
     * \param what_arg The explanatory string.
     * \param path1_arg The first path.
     * \param ec The error code.
     */
    BOOST_FILESYSTEM_DECL filesystem_error(const char* what_arg, path const& path1_arg, system::error_code ec);

    /*!
     * \brief Constructs the exception.
     *
     * \post
     * | Expression                | Value                                  |
     * |---------------------------|----------------------------------------|
     * | `runtime_error::what()`   | A string containing `what_arg.c_str()` |
     * | `code()`                  | `ec`                                   |
     * | `path1()`                 | Reference to stored copy of `p1`       |
     * | `path2().empty()`         | `true`                                 |
     *
     * \param what_arg The explanatory string.
     * \param path1_arg The first path.
     * \param ec The error code.
     */
    BOOST_FILESYSTEM_DECL filesystem_error(std::string const& what_arg, path const& path1_arg, system::error_code ec);

    /*!
     * \brief Constructs the exception.
     *
     * \post
     * | Expression                | Value                                  |
     * |---------------------------|----------------------------------------|
     * | `runtime_error::what()`   | A string containing `what_arg.c_str()` |
     * | `code()`                  | `ec`                                   |
     * | `path1()`                 | Reference to stored copy of `p1`       |
     * | `path2()`                 | Reference to stored copy of `p2`       |
     *
     * \param what_arg The explanatory string.
     * \param path1_arg The first path.
     * \param path2_arg The second path.
     * \param ec The error code.
     */
    BOOST_FILESYSTEM_DECL filesystem_error(const char* what_arg, path const& path1_arg, path const& path2_arg, system::error_code ec);

    /*!
     * \brief Constructs the exception.
     *
     * \post
     * | Expression                | Value                                  |
     * |---------------------------|----------------------------------------|
     * | `runtime_error::what()`   | A string containing `what_arg.c_str()` |
     * | `code()`                  | `ec`                                   |
     * | `path1()`                 | Reference to stored copy of `p1`       |
     * | `path2()`                 | Reference to stored copy of `p2`       |
     *
     * \param what_arg The explanatory string.
     * \param path1_arg The first path.
     * \param path2_arg The second path.
     * \param ec The error code.
     */
    BOOST_FILESYSTEM_DECL filesystem_error(std::string const& what_arg, path const& path1_arg, path const& path2_arg, system::error_code ec);

    /*!
     * \brief Copy constructor.
     *
     * \param that Exception object to copy from.
     */
    BOOST_FILESYSTEM_DECL filesystem_error(filesystem_error const& that);

    /*!
     * \brief Copy assignment operator.
     *
     * \param that Exception object to copy from.
     *
     * \returns `*this`.
     */
    BOOST_FILESYSTEM_DECL filesystem_error& operator=(filesystem_error const& that);

    /*!
     * \brief Destructor.
     */
    BOOST_FILESYSTEM_DECL ~filesystem_error() noexcept;

    /*!
     * \returns Reference to copy of `p1` stored by the constructor, or, if none, an empty path.
     */
    path const& path1() const noexcept
    {
        return m_imp_ptr.get() ? m_imp_ptr->m_path1 : get_empty_path();
    }
    /*!
     * \returns Reference to copy of `p2` stored by the constructor, or, if none, an empty path.
     */
    path const& path2() const noexcept
    {
        return m_imp_ptr.get() ? m_imp_ptr->m_path2 : get_empty_path();
    }

    /*!
     * \returns A string containing `runtime_error::what()`. The exact format is unspecified. The implementation may
     *          (but is not required to) include `path1.native_string()` if not empty, `path2.native_string()`
     *          if not empty, and `system_error::what()` strings in the returned string.
     */
    BOOST_FILESYSTEM_DECL const char* what() const noexcept override;

    //! \cond
private:
    BOOST_FILESYSTEM_DECL static path const& get_empty_path() noexcept;

private:
    struct impl :
        public boost::intrusive_ref_counter< impl >
    {
        path m_path1;       // may be empty()
        path m_path2;       // may be empty()
        std::string m_what; // not built until needed

        impl() = default;
        explicit impl(path const& path1) :
            m_path1(path1)
        {
        }
        impl(path const& path1, path const& path2) :
            m_path1(path1), m_path2(path2)
        {
        }
    };
    boost::intrusive_ptr< impl > m_imp_ptr;

    //! \endcond
};

} // namespace filesystem
} // namespace boost

#include <boost/filesystem/detail/footer.hpp>

#endif // BOOST_FILESYSTEM_EXCEPTION_HPP
