//  recursive_dir_itr_test program  ------------------------------------------//

//  (C) Copyright Beman Dawes 2002. Permission to copy,
//  use, modify, sell and distribute this software is granted provided this
//  copyright notice appears in all copies. This software is provided "as is"
//  without express or implied warranty, and with no claim as to its
//  suitability for any purpose.

//  See http://www.boost.org for most recent version including documentation.

#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/recursive_directory_iterator.hpp>
#include <iostream>
#include <cassert>

namespace fs = boost::filesystem;

namespace
{

  bool dummy( const fs::path & /*pth*/ )
  {
//    std::cout << "predicate called with " << pth.generic_path() << '\n';
    return true;
  }

} // unnamed namespace

int main( int argc, char * argv[] )
{
  fs::path dir( argc > 1
    ? fs::path( argv[1], fs::native )
    : fs::initial_path() );

  std::cout << "Directory being inspected: " << dir.string () << std::endl;

  typedef bool(*pred_type)(const fs::path&);

  fs::recursive_directory_iterator<pred_type> rdi( dir , dummy );
  fs::recursive_directory_iterator<pred_type> rdi_end;

  for ( ; rdi != rdi_end; ++rdi )
  {
    std::cout << (*rdi).string() << std::endl;
    std::cout << rdi->string() << std::endl;
  }

  return 0;
}
