//  (C) Copyright Jan Langer 2002. Permission to copy, use, modify,
//  sell and distribute this software is granted provided this
//  copyright notice appears in all copies. This software is provided
//  "as is" without express or implied warranty, and with no claim as
//  to its suitability for any purpose.

#include <iostream>
#include <exception>

#include "boost/filesystem/attributes.hpp"
#include "boost/filesystem/path.hpp"
#include "boost/test/minimal.hpp"

namespace fs = boost::filesystem;

int test_main (int argc, char **argv)
{
    BOOST_REQUIRE (argc == 4);

    fs::path file (argv [1], fs::native);
    fs::cache pm (file);
    std::cout << get <fs::filename> (pm).string () 
              << ": " << get <fs::owner> (pm) 
	      << "." << get <fs::group> (pm);
	
    put <fs::owner> (pm, argv [2]); 	
    put <fs::group> (pm, argv [3]); 	

    commit <fs::group> (pm);
	
    std::cout << " -> " << get <fs::owner> (pm) 
	      << "." << get <fs::group> (pm) << '\n';

    return 0;
}

