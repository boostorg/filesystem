//  (C) Copyright Jan Langer 2002. Permission to copy, use, modify,
//  sell and distribute this software is granted provided this
//  copyright notice appears in all copies. This software is provided
//  "as is" without express or implied warranty, and with no claim as
//  to its suitability for any purpose.

#ifndef BOOST_FILESYSTEM_DETAIL_TYPE_PROPERTY_MAP_HPP
#define BOOST_FILESYSTEM_DETAIL_TYPE_PROPERTY_MAP_HPP

#include <map>

#include "boost/property_map.hpp"

// use loki's TypeInfo
#include "boost/filesystem/detail/type_info.hpp"

namespace boost
{
 namespace filesystem
 {
  namespace detail
  {
    // ------------------------------------------------
    // type_property_map class

    template <typename Value>
    class type_property_map
    {
            typedef std::map <boost::filesystem::detail::type_info, 
	                      Value> map_type;

        public:
            typedef lvalue_property_map_tag category;
	    typedef Value value_type;

            // operator [] is not good if argument is no key
	    template <typename Key>
            value_type &at () 
            {
		value_type &v = map_ [typeid (Key)];
                return v;
            }

        private:
            map_type map_;
    };

    // ------------------------------------------------
    // access functions for type_property_map

    template <typename Key, typename Value>
    void put (type_property_map <Value> &pmap, Value v)
    {
        pmap.template at <Key> () = v;
    }

    template <typename Key, typename Value>
    Value &get (type_property_map <Value> &pmap)
    {
        return pmap.template at <Key> ();
    }
    
  } // namespace detail
 } // namespace filesystem
} // namespace boost

using boost::filesystem::detail::get;
using boost::filesystem::detail::put;

#endif // BOOST_FILESYSTEM_DETAIL_TYPE_PROPERTY_MAP_HPP
