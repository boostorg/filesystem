//  boost/filesystem/directory.hpp  ------------------------------------------//

// < ----------------------------------------------------------------------- > 
// <   Copyright © 2002 Beman Dawes.                                         >
// <   Copyright © 2002 Jan Langer.                                          >
// <   Copyright © 2001 Dietmar Kühl, All Rights Reserved                    >
// <                                                                         > 
// <   Permission to use, copy, modify, distribute and sell this             > 
// <   software for any purpose is hereby granted without fee, provided      > 
// <   that the above copyright notice appears in all copies and that        > 
// <   both that copyright notice and this permission notice appear in       > 
// <   supporting documentation. The authors make no representations about   > 
// <   the suitability of this software for any purpose. It is provided      > 
// <   "as is" without express or implied warranty.                          > 
// < ----------------------------------------------------------------------- > 

//  See http://www.boost.org for most recent version including documentation.

//----------------------------------------------------------------------------// 

#ifndef BOOST_FILESYSTEM_DIRECTORY_HPP
#define BOOST_FILESYSTEM_DIRECTORY_HPP

#include <boost/config.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/smart_ptr.hpp>
#include <boost/iterator_adaptors.hpp>

#include <string>

//----------------------------------------------------------------------------//

namespace boost
{
  namespace filesystem
  {

//  query functions  ---------------------------------------------------------//

    bool exists( const path & ph );

    bool is_directory( const path & ph );

    // VC++ 7.0 and earlier has a serious namespace bug that causes a clash
    // between boost::filesystem::is_empty and the unrelated type trait
    // boost::is_empty. The workaround for those who must use broken versions
    // of VC++ is to use the function _is_empty. All others should use the
    // correct is_empty name.
    bool _is_empty( const path & ph ); // deprecated

#   if !defined( BOOST_MSVC ) || BOOST_MSVC > 1300
    inline bool is_empty( const path & ph ) { return _is_empty( ph ); }
#   endif

//  operations  --------------------------------------------------------------//

    void create_directory( const path & directory_ph );

    void remove( const path & ph );

    void rename( const path & from_path,
                 const path & to_path );

    unsigned long remove_all( const path & ph );

    void copy_file( const path & from_file_ph,
                    const path & to_file_ph );

    const path & initial_directory();

//  directory_iterator details  ----------------------------------------------//

    namespace detail
    {
      const char * implementation_name();

      class directory_iterator_imp;
      
      struct directory_iterator_internals
      {
        typedef boost::shared_ptr< detail::directory_iterator_imp > dii_ptr;
        dii_ptr imp;

        const path & deref() const;
        void inc();
      };

      struct dii_policies : public default_iterator_policies
      {

        template <class IteratorAdaptor>
        typename IteratorAdaptor::reference
        dereference(const IteratorAdaptor& x) const
          { return x.base().deref(); }

        template <class IteratorAdaptor>
        void increment(IteratorAdaptor& x)
          { x.base().inc(); }

        template <class IteratorAdaptor1, class IteratorAdaptor2>
        bool equal(const IteratorAdaptor1& x, const IteratorAdaptor2& y) const
          { return x.base().imp == y.base().imp; }
      };
    }

//  directory_iterator  ------------------------------------------------------//

    class directory_iterator
      : public boost::iterator_adaptor<
          // because directory_iterator is an InputIterator, shallow copy-
          // semantics are required, and shared_ptr provides that.
          detail::directory_iterator_internals,
          detail::dii_policies,
          path, const path &, const path *,
          std::input_iterator_tag, std::ptrdiff_t >
    {
    public:
      directory_iterator();  // creates the "end" iterator
      explicit directory_iterator( const path & directory_path );

      // workaround iterator_adaptor / compiler interactions
      const boost::filesystem::path * operator->() const
        { return &base().deref(); }
    };

  } // namespace filesystem
} // namespace boost

#endif // BOOST_FILESYSTEM_DIRECTORY_HPP
