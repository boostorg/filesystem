//  deprecated_test program --------------------------------------------------//

//  Copyright Beman Dawes 2002
//  Copyright Vladimir Prus 2002

//  Use, modification, and distribution is subject to the Boost Software
//  License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

//  See library home page at http://www.boost.org/libs/filesystem

//  This test verifies that various deprecated names still compile. This is
//  important to preserve existing code that uses the old names.

#include <boost/filesystem.hpp>

namespace fs = boost::filesystem;

int main()
{
  fs::directory_entry de( "foo/bar" );

  de.replace_leaf( "", fs::file_status(), fs::file_status() );

  de.leaf();
  de.string();

  return 0;
}
