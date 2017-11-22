//  Copyright Daniel Kruegler 2017

//  Distributed under the Boost Software License, Version 1.0.
//  See http://www.boost.org/LICENSE_1_0.txt

//  Library home page: http://www.boost.org/libs/filesystem

#include <boost/filesystem.hpp>
#include <boost/thread.hpp>

namespace fs = boost::filesystem;

struct StaticInitializer {
  StaticInitializer() {
    static const fs::path path = fs::current_path();
    struct {
      void operator()() { fs::status(path); }
    } func;
    const int max_count = 50;
    boost::thread ths[max_count];
    for (int i = 0; i < max_count; ++i) {
      new (&ths[i]) boost::thread(func);
    }
    for (int i = 0; i < max_count; ++i) {
      ths[i].join();
    }
  }
} instance;

int main()
{
}
