//  boost/filesystem/recursive_directory_iterator.hpp  -----------------------//

//  (C) Copyright Beman Dawes 2002. Permission to copy, use, modify, sell and
//  distribute this software is granted provided this copyright notice appears
//  in all copies. This software is provided "as is" without express or implied
//  warranty, and with no claim as to its suitability for any purpose.

//  See http://www.boost.org for most recent version including documentation.

#ifndef BOOST_FILESYSTEM_RECURSIVE_DIRECTORY_ITERATOR_HPP
#define BOOST_FILESYSTEM_RECURSIVE_DIRECTORY_ITERATOR_HPP

#include <boost/filesystem/operations.hpp>
#include <boost/iterator_adaptors.hpp>

#include <stack>
#include <vector>
//#include <iostream>

//----------------------------------------------------------------------------//

namespace boost
{
  namespace filesystem
  {
    namespace detail
    {
      const directory_iterator end_itr;

      template< typename Predicate >
      struct rdi_imp
      {
        std::stack< directory_iterator, std::vector<directory_iterator> >
          stac;
        Predicate pred;

        rdi_imp( const path & pth, Predicate pred )
          : pred(pred)
        {
          stac.push( directory_iterator( pth ) );
          while ( stac.top() != end_itr
            && !pred( *stac.top() ) ) ++stac.top();
        }

        rdi_imp()
          : pred( Predicate() )
        {
          stac.push( directory_iterator() );
        }

        void increment()
        {
          if ( is_directory( *stac.top() ) )
            stac.push( directory_iterator( *stac.top() ) );
          else ++stac.top();
          for (;;) // 'til end or pred true
          {
            while ( stac.top() != end_itr && !pred( *stac.top() ) )
              { ++stac.top(); }
            if ( stac.top() != end_itr || stac.size() == 1 ) return;
            stac.pop();
            ++stac.top();            
          }
        }
      };

      struct rdi_policies : public default_iterator_policies
      {

        template <class IteratorAdaptor>
        typename IteratorAdaptor::reference dereference(const IteratorAdaptor& x) const
        { 
          return *x.base()->stac.top();
        }

        template <class IteratorAdaptor>
        void increment(IteratorAdaptor& x) { x.base()->increment(); }

        template <class IteratorAdaptor1, class IteratorAdaptor2>
        bool equal(const IteratorAdaptor1& x, const IteratorAdaptor2& y) const
        { 
          return x.base()->stac.top() == y.base()->stac.top();
        }
      };

    } // namespace detail

    template< typename Predicate >
    class recursive_directory_iterator
      : public boost::iterator_adaptor< 
         boost::shared_ptr< detail::rdi_imp< Predicate > >,
         detail::rdi_policies,
         boost::filesystem::path,
         const boost::filesystem::path &,
         const boost::filesystem::path *,
         std::input_iterator_tag,
         std::ptrdiff_t >
    {
    public:
      recursive_directory_iterator( const path & pth, Predicate pred )
      {
        this->base().reset( new detail::rdi_imp<Predicate>( pth, pred ) );
      }
      recursive_directory_iterator() // the past-the-end iterator
      {
        this->base().reset( new detail::rdi_imp<Predicate> );
      }

      // workaround iterator_adaptor / compiler interactions
      const boost::filesystem::path * operator->() const
        { return &*this->base()->stac.top(); }
    };
  }
}

#endif // BOOST_FILESYSTEM_RECURSIVE_DIRECTORY_ITERATOR_HPP
