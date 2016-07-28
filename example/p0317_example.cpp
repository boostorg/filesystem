//  p0317_example.cpp  -----------------------------------------------------------------//

//  Copyright Beman Dawes 2016

//  Distributed under the Boost Software License, Version 1.0.
//  See http://www.boost.org/LICENSE_1_0.txt

//  Library home page: http://www.boost.org/libs/filesystem

//  ------------------------------------------------------------------------------------//


#include <boost/config/warning_disable.hpp>

#include <boost/filesystem.hpp>
#include <iostream>

using namespace boost::filesystem;

void lengthy_function(const path& p)
{
  //  ... whatever
}

int main()
{
  // use possibly cached last write time to minimize disk accesses
  for (auto&& x : directory_iterator("."))
  {
    std::cout << x.path() << " " << x.last_write_time() << std::endl;
  }

  // call refresh() to refresh a stale cache
  for (auto&& x : directory_iterator("."))
  {
    lengthy_function(x.path());  // cache becomes stale
    x.refresh();
    std::cout << x.path() << " " << x.last_write_time() << std::endl;
  }
 }
