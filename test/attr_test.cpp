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
    for (int i = 1; i < argc; ++i)
    {
	fs::path file (argv [i], fs::native);
	fs::cache pm (file);
        std::cout << "name: " << get <fs::filename> (pm).string () 
                  << "; size: " << get <fs::size> (pm) 
                  << "; owner: " << get <fs::owner> (pm)
	          << "; group: " << get <fs::group> (pm) << "\n";
    }

    return 0;
}

