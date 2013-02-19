#include <iostream>
#include <boost/thread.hpp>
#include <boost/filesystem/path.hpp>

using std::cout;
using std::endl;

namespace
{
  void f1()
  { 
    cout << "f1()" << endl;

  } 
  void f2()
  {
    cout << "f2()" << endl;
  }

}

int main()
{
  boost::thread t1(f1);
  boost::thread t2(f2);

  cout << "t1.join()" << endl;
  t1.join();
  cout << "t2.join()" << endl;
  t2.join();
  cout << "all done" << endl;
}