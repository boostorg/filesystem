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

#include "boost/config.hpp"
#include "boost/filesystem/path.hpp"
#include "boost/smart_ptr.hpp"
#include "boost/iterator.hpp"
#include "boost/operators.hpp"

#include <string>

//----------------------------------------------------------------------------//

namespace boost
{
  namespace filesystem
  {

//  query functions  ---------------------------------------------------------//

    bool exists( const path & any_path );

    bool directory( const path & any_path );

    bool empty( const path & any_path );

//  operations  --------------------------------------------------------------//

    void create_directory( const path & directory_path );

    void remove( const path & any_path );

    void rename( const path & from_path,
                 const path & to_path );

    unsigned long remove_all( const path & any_path );

    void copy_file( const path & from_file_path,
                    const path & to_file_path );

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
      explicit directory_iterator( const path & directory_path );

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
