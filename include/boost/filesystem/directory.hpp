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
// <   "as is" without expressed or implied warranty.                        > 
// < ----------------------------------------------------------------------- > 

//  See http://www.boost.org for most recent version including documentation.

//----------------------------------------------------------------------------// 

#ifndef BOOST_FILESYSTEM_DIRECTORY_HPP
#define BOOST_FILESYSTEM_DIRECTORY_HPP

#include "boost/config.hpp"
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

    bool exists( const std::string & path );

    bool is_directory( const std::string & path );

    bool is_file( const std::string & path );

    //  TODO: consider adding is_existing_directory() and is_existing_file()
    //  functions. They don't fatten the interface much, and may be more
    //  efficient internally than user coded exists(foo) && is_directory(foo).
    //  But how useful would they be?  I've needed both in practice, but not
    //  enough to die for.

    bool empty( const std::string & path );
    //  Rationale for name: This function should be named "is_empty" for
    //  consistency with other is_xxx names.  However, there a serious VC++
    //  bug (version 7.0 and earlier that causes a name clash between
    //  the boost::is_empty type trait and boost::filesystem::is_empty.
    //  (Jason Shirk says the bug is fixed in later versions).
    //  Should the filesystem library be proposed for standardization, the
    //  "is_empty" name should be used; "empty" is just a workaround name.

#if !defined(BOOST_MSVC) || _MSC_VER > 1300 // VC++ 7.0 or earlier
    inline bool is_empty( const std::string & path ) { return empty( path ); }
#endif

//  operations  --------------------------------------------------------------//

    void create_directory( const std::string & directory_path );

    void remove_file( const std::string & file_path );

    void remove_directory( const std::string & directory_path );

    unsigned long remove_all( const std::string & path );

    void rename( const std::string & from_path,
                 const std::string & to_path );

    void copy_file( const std::string & from_file_path,
                    const std::string & to_file_path );

    const std::string & initial_directory();
    //  Effects: The first time called, stores an absolute directory path.
    //  The path is determined by the first rule which is possible for the
    //  platform:
    //    * The path of the initial working directory when main()
    //      was called.
    //    * The current working directory.
    //    * A reasonable directory (such as the root directory on
    //      a single rooted filesystem).
    //    * Compile or link error.
    //    * Throws an exception.
    //  Returns: The stored path.
    //  Rationale for not simply specifying the return as "initial working
    //  directory when main() was called": That directory isn't known on all
    //  platforms; indeed there may not even be a concept of "current working
    //  directory". The semantics are chosen to come as close to that as
    //  possible (and encourage but not require language runtime support).
    //  Note: It would be good practice in a program dependent on
    //  initial_directory() to call it immediately on entering main().
    //  That protects against a called function altering the current working
    //  directory (using a native platform function) and then calling
    //  initial_directory().

    namespace detail
    {
      class directory_entry_imp;
      class directory_iterator_imp;
    }

//  directory_entry  ---------------------------------------------------------//

////////////////////////////////////////////////////////////////////////////////
//
//  WARNING: directory_entry is FAR MORE LIKELY TO CHANGE RADICALLY than the
//  other portions of this header. As of May 16, 2002, I've got a tentative
//  design for a path_string class which would replace directory_entry, and
//  would be the directory_iterator value_type.  --Beman
//
////////////////////////////////////////////////////////////////////////////////

    class directory_entry
    {
    public:
      directory_entry();

      // unlike directory_iterator, deep copy semantics are required:
      //    directory_iterator itr( "foo" );
      //    directory_entry first_entry( *itr );
      //    ++itr;  // must not modify first_entry (or its internal imp type)
      directory_entry( const directory_entry & rhs );
      directory_entry & operator=( const directory_entry & rhs );

      ~directory_entry();

      bool is_directory() const; // may be cached
      bool is_file() const { return !is_directory(); }
      const std::string & name() const;
      const std::string & directory_path() const;
      const std::string & path() const;
      // Return: compose( directory_path(), name() )
      // Rationale for providing path(): makes much user code simpler.

      // Rationale for  operator== not being providing: it is not always
      // possible (see design docs for details) to tell if two paths are
      // the same.

    private:
      boost::scoped_ptr<detail::directory_entry_imp>  m_imp;
      friend class directory_iterator;
    };

//  directory_iterator  ------------------------------------------------//

    class directory_iterator:
      public boost::iterator< std::input_iterator_tag,
                              directory_entry >,
      public boost::equality_comparable< directory_iterator >,
      public boost::dereferenceable< directory_iterator,
                                     const directory_entry * >,
      public boost::incrementable< directory_iterator >
    {
    public:
      typedef boost::iterator<
        std::input_iterator_tag,
        directory_entry >             base;
      typedef base::value_type        value_type;
      typedef base::difference_type   difference_type;
      typedef base::pointer           pointer;
      typedef base::reference         reference;
      typedef base::iterator_category iterator_category;

      directory_iterator(); // creates the "end" iterator
      explicit directory_iterator( const std::string & directory_path );

      ~directory_iterator();
      // May throw, except that will not throw if it represents the
      // "end" iterator.

      // compiler generates copy constructor and copy assignment
      
      bool operator==( const directory_iterator & ) const;

      directory_entry const & operator*() const;
      directory_iterator & operator++();

    private:
      // because directory_iterator is an InputIterator, shallow copy
      // semantics are required, and shared_ptr provides that.
      boost::shared_ptr<detail::directory_iterator_imp> m_imp;
    };

  } // namespace filesystem
} // namespace boost

#endif // BOOST_FILESYSTEM_DIRECTORY_HPP
