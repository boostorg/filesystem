//  boost/interop/detail/iterator_value.hpp  -------------------------------------------//

//  Copyright Beman Dawes 2011

//  Distributed under the Boost Software License, Version 1.0.
//  See http://www.boost.org/LICENSE_1_0.txt

#if !defined(BOOST_INTEROP_ITERATOR_VALUE_HPP)
# define BOOST_INTEROP_ITERATOR_VALUE_HPP

#include <boost/interop/detail/is_iterator.hpp>
#include <boost/utility/enable_if.hpp>
#include <boost/mpl/logical.hpp>
#include <boost/type_traits/is_same.hpp>
#include <iterator>

//--------------------------------------------------------------------------------------//
//                                                                                      //
//  iterator_value<T>::type is equivalent to std::iterator_traits<T>::value_type, but   //
//  works correctly in a SFINAE context such as enable_if.                              //
//                                                                                      //
//  std::iterator_traits<T>::value_type doesn't work in a SFINAE context because, as    //
//  Jonathan Wakely explains, "If T::difference_type doesn't exist you get an error     //
//  that is not in the immediate context, and Substitution Failure Outside The          //
//  Immediate Context Is An Error."                                                     //
//                                                                                      //
//--------------------------------------------------------------------------------------//

namespace boost
{
  namespace xop
  {
    namespace detail
    {
      template <class T, bool Pred> struct iterator_value_helper;
      template <class T> struct iterator_value_helper<T, true>
      { 
        typedef typename std::iterator_traits<T>::value_type type;
      };
      struct not_an_iterator;
      template <class T> struct iterator_value_helper<T, false>
      { 
        typedef not_an_iterator type;
      };

      template <class T>
      struct iterator_value
      {
        typedef
          typename iterator_value_helper<T, is_iterator<T>::value >::type type;
      };

    } // namespace detail
  } // namespace xop
} // namespace boost


#endif  // BOOST_INTEROP_ITERATOR_VALUE_HPP
