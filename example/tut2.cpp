//  filesystem tut2.cpp  ---------------------------------------------------------------//

//  Copyright Beman Dawes 2009

//  Distributed under the Boost Software License, Version 1.0.
//  See http://www.boost.org/LICENSE_1_0.txt

//  Library home page: http://www.boost.org/libs/filesystem

//[example_tut2
#include <iostream>
#include <boost/filesystem.hpp>

using namespace boost::filesystem;

int main(int argc, char* argv[])
{
    if (argc < 2)
    {
        std::cout << "Usage: tut2 path" << std::endl;
        return 1;
    }

    path p(argv[1]); // avoid repeated path construction below

    if (exists(p)) // does path p actually exist?
    {
        if (is_regular_file(p)) // is path p a regular file?
            std::cout << p << " size is " << file_size(p) << std::endl;
        else if (is_directory(p)) // is path p a directory?
            std::cout << p << " is a directory" << std::endl;
        else
            std::cout << p << " exists, but is not a regular file or directory" << std::endl;
    }
    else
        std::cout << p << " does not exist" << std::endl;

    return 0;
}
//]
