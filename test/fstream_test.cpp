//  fstream_test.cpp  --------------------------------------------------------//

//  (C) Copyright Beman Dawes 2002. Permission to copy, use, modify, sell and
//  distribute this software is granted provided this copyright notice appears
//  in all copies. This software is provided "as is" without express or implied
//  warranty, and with no claim as to its suitability for any purpose.

//  See http://www.boost.org for most recent version including documentation.

#include "boost/filesystem/fstream.hpp"
#include <string>

namespace fs = boost::filesystem;

#define BOOST_INCLUDE_MAIN
#include "boost/test/test_tools.hpp"

int test_main( int, char*[] )
{
  { // basic_filebuf runtime results are ignored; as long as they don't crash
    // or throw we are satisfied
    fs::basic_filebuf<char> bfb;
    fs::filebuf cfb;
    fs::wfilebuf wfb;

    bfb.open( "fstream_test_bffoo", std::ios_base::in );
    cfb.open( "fstream_test_bffoo", std::ios_base::in );
    wfb.open( "fstream_test_bffoo", std::ios_base::in );
  }

  std::remove( "fstream_test_bfoo" );
  std::remove( "fstream_test_cfoo" );
  std::remove( "fstream_test_wfoo" );

  {
    fs::basic_ofstream<char> bofs( "fstream_test_bfoo" );
    fs::ofstream cofs( "fstream_test_cfoo" );
    fs::wofstream wofs( "fstream_test_wfoo" );

    BOOST_TEST( bofs.is_open() );
    BOOST_TEST( cofs.is_open() );
    BOOST_TEST( wofs.is_open() );

    bofs << "fstream_test_bfoo";
    cofs << "fstream_test_cfoo";
    wofs << L"fstream_test_wfoo";

    // these will fail, but they still test the interface
    bofs.open( "fstream_test_bfoo" );
    cofs.open( "fstream_test_cfoo" );
    wofs.open( "fstream_test_wfoo" );
  }

  {
    fs::basic_ifstream<char> bifs( "fstream_test_bfoo" );
    fs::ifstream cifs( "fstream_test_cfoo" );
    fs::wifstream wifs( "fstream_test_wfoo" );

    BOOST_TEST( bifs.is_open() );
    BOOST_TEST( cifs.is_open() );
    BOOST_TEST( wifs.is_open() );

    std::string b;
    std::string c;
    std::wstring w;

    bifs >> b;
    cifs >> c;
    wifs >> w;

    BOOST_TEST( b == "fstream_test_bfoo" );
    BOOST_TEST( c == "fstream_test_cfoo" );
    BOOST_TEST( w == L"fstream_test_wfoo" );

    std::cout << "b is \"" << b << "\"\n";
    std::cout << "c is \"" << c << "\"\n";
    std::wcout << L"w is \"" << w << L"\"\n";

    // these will fail, but they still test the interface
    bifs.open( "fstream_test_bfoo" );
    cifs.open( "fstream_test_cfoo" );
    wifs.open( "fstream_test_wfoo" );
  }

  {
    fs::basic_fstream<char> bfs( "fstream_test_bfoo" );
    fs::fstream cfs( "fstream_test_cfoo" );
    fs::wfstream wfs( "fstream_test_wfoo" );

    BOOST_TEST( bfs.is_open() );
    BOOST_TEST( cfs.is_open() );
    BOOST_TEST( wfs.is_open() );

    std::string b;
    std::string c;
    std::wstring w;

    bfs >> b;
    cfs >> c;
    wfs >> w;

    BOOST_TEST( b == "fstream_test_bfoo" );
    BOOST_TEST( c == "fstream_test_cfoo" );
    BOOST_TEST( w == L"fstream_test_wfoo" );

    // these will fail, but they still test the interface
    bfs.open( "fstream_test_bfoo" );
    cfs.open( "fstream_test_cfoo" );
    wfs.open( "fstream_test_wfoo" );
  }

//  std::remove( "fstream_test_bfoo" );
//  std::remove( "fstream_test_cfoo" );
//  std::remove( "fstream_test_wfoo" );

  return 0;
}
