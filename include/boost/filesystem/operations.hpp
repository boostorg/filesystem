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
#include <boost/iterator.hpp>
#include <boost/operators.hpp>

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
    //  of VC++ is to use the function _is_empty. All others should use the
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

    namespace detail
    {
      class directory_iterator_imp;
    }

//  directory_iterator  ------------------------------------------------------//

    class directory_iterator:
      public boost::iterator< std::input_iterator_tag,
                              path >,
      public boost::equality_comparable< directory_iterator >,
      public boost::dereferenceable< directory_iterator,
                                     const path * >,
      public boost::incrementable< directory_iterator >
    {
    public:
      typedef boost::iterator<
        std::input_iterator_tag,
        path >                        base;
      typedef base::value_type        value_type;
      typedef base::difference_type   difference_type;
      typedef base::pointer           pointer;
      typedef base::reference         reference;
      typedef base::iterator_category iterator_category;

      directory_iterator(); // creates the "end" iterator
      explicit directory_iterator( const path & directory_ph );

      ~directory_iterator();

      // compiler generates copy constructor and copy assignment
      
      bool operator==( const directory_iterator & ) const;

      const path & operator*() const;

      directory_iterator & operator++();

    private:
      // because directory_iterator is an InputIterator, shallow copy
      // semantics are required, and shared_ptr provides that.
      boost::shared_ptr<detail::directory_iterator_imp> m_imp;
    };

  } // namespace filesystem
} // namespace boost

#endif // BOOST_FILESYSTEM_DIRECTORY_HPP
