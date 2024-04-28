//  filesystem tut3.cpp  ---------------------------------------------------------------//

//  Copyright Beman Dawes 2009

//  Distributed under the Boost Software License, Version 1.0.
//  See http://www.boost.org/LICENSE_1_0.txt

//  Library home page: http://www.boost.org/libs/filesystem

//[example_tut3
#include <iostream>
#include <boost/filesystem.hpp>

using namespace boost::filesystem;

int main(int argc, char* argv[])
{
    if (argc < 2)
    {
        std::cout << "Usage: tut3 path" << std::endl;
        return 1;
    }

    path p(argv[1]);

    try
    {
        if (exists(p))
        {
            if (is_regular_file(p))
            {
                std::cout << p << " size is " << file_size(p) << std::endl;
            }
            else if (is_directory(p))
            {
                std::cout << p << " is a directory containing:" << std::endl;

                for (directory_entry const& x : directory_iterator(p))
                    std::cout << "    " << x.path() << std::endl;
            }
            else
                std::cout << p << " exists, but is not a regular file or directory" << std::endl;
        }
        else
            std::cout << p << " does not exist" << std::endl;
    }
    catch (filesystem_error& ex)
    {
        std::cout << ex.what() << std::endl;
    }

    return 0;
}
//]
