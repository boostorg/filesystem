//  Copyright Jacob Schloss, 2013

//  Distributed under the Boost Software License, Version 1.0.
//  See http://www.boost.org/LICENSE_1_0.txt


#include <boost/thread.hpp>
#include <boost/filesystem.hpp>


int main(void)
{
        //std::string sPath("c:\\Development");
        std::string sPath(".");
        
        boost::thread_group tg;

        for(int i = 0; i < 2; i++)
        {
                tg.create_thread([&sPath](){
                        boost::this_thread::sleep(boost::posix_time::milliseconds(10));
                        boost::filesystem::path p(sPath);

                        boost::filesystem::directory_iterator di(p), end;
                        while(di != end)
                                std::cout << (*(di++)).path().string() << std::endl;
                });
        }

        tg.join_all();

        int a;
        std::cin >> a;

}
