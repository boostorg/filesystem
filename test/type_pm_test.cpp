//  (C) Copyright Jan Langer 2002. Permission to copy, use, modify,
//  sell and distribute this software is granted provided this
//  copyright notice appears in all copies. This software is provided
//  "as is" without express or implied warranty, and with no claim as
//  to its suitability for any purpose.

#include <iostream>
#include <exception>

#include "boost/filesystem/detail/type_property_map.hpp"
#include "boost/test/minimal.hpp"

int test_main (int argc, char **argv)
{
    boost::filesystem::detail::
	    type_property_map <char> pm;

    put <int> (pm, 'i');
    put <char> (pm, 'c');

    BOOST_TEST (get <char> (pm) == 'c');
    BOOST_TEST (get <int> (pm) == 'i');
    BOOST_TEST (get <long> (pm) == long ());

    return 0;
}

