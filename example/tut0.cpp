//  filesystem tut0.cpp  ---------------------------------------------------------------//

//  Copyright Beman Dawes 2009

//  Distributed under the Boost Software License, Version 1.0.
//  See http://www.boost.org/LICENSE_1_0.txt

//  Library home page: http://www.boost.org/libs/filesystem

//[example_tut0
#include <iostream>
#include <boost/filesystem.hpp>

namespace fs = boost::filesystem;

int main(int argc, char* argv[])
{
    if (argc < 2)
    {
        std::cout << "Usage: tut0 path" << std::endl;
        return 1;
    }
    std::cout << argv[1] << std::endl;
    return 0;
}
//]
