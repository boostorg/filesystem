//  boost/type_traits/is_iterator.hpp

//  (C) Copyright Beman Dawes and Howard Hinnant 2011
//  (C) Copyright Steve Cleary, Beman Dawes, Howard Hinnant & John Maddock 2000.

//  Distributed under the Boost Software License, Version 1.0.
//  See http://www.boost.org/LICENSE_1_0.txt.
//
//  See http://www.boost.org/libs/type_traits for most recent version including documentation.

#ifndef BOOST_TT_IS_ITERATOR_HPP
#define BOOST_TT_IS_ITERATOR_HPP

#include <boost/type_traits/is_pointer.hpp>
#include <boost/type_traits/detail/ice_or.hpp>
#include <boost/type_traits/detail/ice_and.hpp>
#include <boost/config.hpp>

// should be the last #include
#include <boost/type_traits/detail/bool_trait_def.hpp>

namespace boost {

namespace detail {

template <class T>
class has_iterator_category
{
  struct size2 {char a[2]; };
  template <class U> static size2 sfinae(...);
  template <class U> static char sfinae(typename U::iterator_category*);
public:
  BOOST_STATIC_CONSTANT(bool, value = sizeof(sfinae<T>(0)) == 1);
};

template <class T>
class has_difference_type
{
  struct size2 {char a[2]; };
  template <class U> static size2 sfinae(...);
  template <class U> static char sfinae(typename U::difference_type*);
public:
  BOOST_STATIC_CONSTANT(bool, value = sizeof(sfinae<T>(0)) == 1);
};

template <class T>
class has_value_type
{
  struct size2 {char a[2]; };
  template <class U> static size2 sfinae(...);
  template <class U> static char sfinae(typename U::value_type*);
public:
  BOOST_STATIC_CONSTANT(bool, value = sizeof(sfinae<T>(0)) == 1);
};

template <class T>
struct is_iterator_impl
{ 
    BOOST_STATIC_CONSTANT(bool, value = 
        (::boost::type_traits::ice_or< 
            ::boost::is_pointer<T>::value,
            ::boost::type_traits::ice_and<
              ::boost::detail::has_iterator_category<T>::value,
              ::boost::detail::has_value_type<T>::value,
              ::boost::detail::has_difference_type<T>::value
            >::value // ice_and
          >::value // ice_or
        ));
//  It would further reduce the chance of accidental conformance if it was
//  required that T::iterator_category was convertible to std::input_iterator_tag
//  or std::output_iterator_tag.
};

} // namespace detail

BOOST_TT_AUX_BOOL_TRAIT_DEF1(is_iterator,T,::boost::detail::is_iterator_impl<T>::value)

} // namespace boost

#include <boost/type_traits/detail/bool_trait_undef.hpp>

#endif // BOOST_TT_IS_ITERATOR_HPP
