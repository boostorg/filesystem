//  detail/source_value_type.hpp  ------------------------------------------------------//

//  Copyright Beman Dawes 2014

//  Distributed under the Boost Software License, Version 1.0.
//  See http://www.boost.org/LICENSE_1_0.txt

#if !defined(BOOST_FILESYSTEM_SOURCE_VALUE_TYPE)
# define BOOST_FILESYSTEM_SOURCE_VALUE_TYPE

#include <boost/filesystem/detail/is_iterator.hpp>
//#include <boost/utility/enable_if.hpp>
//#include <boost/mpl/logical.hpp>
//#include <boost/type_traits/is_same.hpp>
#include <iterator>

namespace boost
{
  namespace BOOST_FILESYSTEM_NAMESPACE
  {
    namespace detail
    {
      template <class T, bool Pred> struct source_value_type_helper;

      // for iterator, use std::iterator_traits<T>::value_type
      template <class T> struct source_value_type_helper<T, true>
      { 
        typedef typename std::iterator_traits<T>::value_type type;
      };

      // non-iterators must be containers, so use T::value_type
      template <class T> struct source_value_type_helper<T, false>
      { 
        typedef typename T::value_type type;
      };

      template <class T>
      struct source_value_type
      {
        typedef
          typename source_value_type_helper<T, is_iterator<T>::value>::type type;
      };

    } // namespace detail
  } // namespace BOOST_FILESYSTEM_NAMESPACE
} // namespace boost


#endif  // include guard
