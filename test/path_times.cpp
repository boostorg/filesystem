//  Boost Filesystem path_times.cpp  ---------------------------------------------------//

//  Copyright Beman Dawes 2013

//  Distributed under the Boost Software License, Version 1.0.
//  See http://www.boost.org/LICENSE_1_0.txt

//  Library home page: http://www.boost.org/libs/filesystem

#include <boost/config/warning_disable.hpp>

#ifndef BOOST_FILESYSTEM_NO_DEPRECATED 
#  define BOOST_FILESYSTEM_NO_DEPRECATED
#endif
#ifndef BOOST_SYSTEM_NO_DEPRECATED 
#  define BOOST_SYSTEM_NO_DEPRECATED
#endif

#include <boost/timer/timer.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/cstdint.hpp>

#include <boost/config.hpp>
# if defined( BOOST_NO_STD_WSTRING )
#   error Configuration not supported: Boost.Filesystem V3 and later requires std::wstring support
# endif

#define BOOST_LIGHTWEIGHT_MAIN
#ifndef BOOST_LIGHTWEIGHT_MAIN
#  include <boost/test/prg_exec_monitor.hpp>
#else
#  include <boost/detail/lightweight_main.hpp>
#endif

namespace fs = boost::filesystem;

#include <fstream>
#include <iostream>

using std::cout;
using std::endl;

namespace
{
  template <class STD_STRING>
   boost::int64_t time_ctor(const STD_STRING& s)
  {
    boost::timer::nanosecond_type timeout(1000000000);
//    typename STD_STRING::size_type len(0);
    boost::timer::auto_cpu_timer tmr;
    boost::int64_t count = 0;
    do
    {
      fs::path p(s);
//      len += p.native().size();
      ++count;
    } while (tmr.elapsed().user < timeout);

//    cout << " len = " << len << endl;
    cout << " count = " << count << endl;
    return count;
  }
}  // unnamed namespace

//--------------------------------------------------------------------------------------//
//                                      main                                            //
//--------------------------------------------------------------------------------------//

int cpp_main(int argc, char* argv[])
{
  cout << "time_ctor with string" << endl;
  boost::int64_t s = time_ctor(std::string("/foo/bar/baz"));
   
  cout << "time_ctor with wstring" << endl;
  boost::int64_t w = time_ctor(std::wstring(L"/foo/bar/baz"));

  if (s > w)
    cout << "narrow/wide ratio = " << long double(s)/w << endl;
  else
    cout << "wide/narrow ratio = " << long double(w)/s << endl;

  cout << "returning from main()" << endl;
  return 0;
}
