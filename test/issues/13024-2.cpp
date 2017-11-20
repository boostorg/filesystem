//  Copyright Daniel Kruegler 2017

//  Distributed under the Boost Software License, Version 1.0.
//  See http://www.boost.org/LICENSE_1_0.txt

//  Library home page: http://www.boost.org/libs/filesystem

#define BOOST_TEST_MODULE issue_13024
#include <boost/test/included/unit_test.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/data/monomorphic.hpp>

#include <boost/filesystem.hpp>
#include <iostream>
#include <vector>

static std::vector<boost::filesystem::path> get_files()
{
    std::vector<boost::filesystem::path> result;
    boost::system::error_code error;
    boost::filesystem::directory_iterator it("..", error);
    for (boost::filesystem::directory_iterator const eit; it != eit; ++it)
        if (is_regular_file(it->status()))
            result.push_back(it->path());
    return result;
}

BOOST_DATA_TEST_CASE(test, get_files(), file_name)
{
  std::cout << file_name << std::endl;
}
