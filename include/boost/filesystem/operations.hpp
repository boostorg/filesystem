//  boost/filesystem/directory.hpp  ------------------------------------------//

// < ----------------------------------------------------------------------- > 
// <   Copyright © 2002, 2003 Beman Dawes.                                   >
// <   Copyright © 2002 Jan Langer.                                          >
// <   Copyright © 2001 Dietmar Kühl                                         >
// <                                                                         > 
// <   See accompanying license for terms and conditions of use.             > 
// < ----------------------------------------------------------------------- > 

//  See http://www.boost.org/libs/filesystem for documentation.

//----------------------------------------------------------------------------// 

#ifndef BOOST_FILESYSTEM_DIRECTORY_HPP
#define BOOST_FILESYSTEM_DIRECTORY_HPP

#include <boost/config.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/iterator.hpp>

#include <string>
#include <ctime>

# ifdef BOOST_NO_STDC_NAMESPACE
    namespace std { using ::time_t; }
# endif

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

    std::time_t last_write_time( const path & ph );
    void last_write_time( const path & ph, const std::time_t new_time );

//  operations  --------------------------------------------------------------//

    void create_directory( const path & directory_ph );

    bool remove( const path & ph );
    unsigned long remove_all( const path & ph );

    void rename( const path & from_path,
                 const path & to_path );

    void copy_file( const path & from_file_ph,
                    const path & to_file_ph );

    path          current_path();
    const path &  initial_path();

    path          system_complete( const path & ph );
    path          complete( const path & ph, const path & base = initial_path() );

//  directory_iterator  ------------------------------------------------------//

    class directory_iterator : public boost::iterator_facade<
      directory_iterator,
      path,
      boost::readable_iterator_tag,
      boost::single_pass_traversal_tag >
    {
    public:
      directory_iterator();  // creates the "end" iterator
      explicit directory_iterator( const path & p );

/*
The *r++ requirement doesn't appear to apply to the new single_pass_traversal category
Thus I'm leaving the proxy out pending confirmation from the N1477 authors
struct path_proxy // allows *r++ to work, as required by 24.1.1
      {
        path pv;
        explicit path_proxy( const path & p ) : pv(p) {}
        path operator*() const { return pv; }
      };

      path_proxy operator++(int)
      {
        path_proxy pp( m_deref() );
        ++*this;
        return pp;
      }
*/

    private:
      class dir_itr_imp;
      // shared_ptr provides shallow-copy semantics required for InputIterators
      typedef boost::shared_ptr< dir_itr_imp > m_imp_ptr;
      m_imp_ptr  m_imp;

      friend class boost::iterator_core_access;
      reference dereference() const;
      void increment();
      bool equal( const directory_iterator & rhs ) const
        { return m_imp == rhs.m_imp; }
    };
  } // namespace filesystem
} // namespace boost

#endif // BOOST_FILESYSTEM_DIRECTORY_HPP
