//  (C) Copyright Jan Langer 2002. Permission to copy, use, modify,
//  sell and distribute this software is granted provided this
//  copyright notice appears in all copies. This software is provided
//  "as is" without express or implied warranty, and with no claim as
//  to its suitability for any purpose.

#include <iostream>
#include <exception>

#include "boost/filesystem/attributes.hpp"
#include "boost/test/minimal.hpp"

namespace fs = boost::filesystem;

namespace 
{
    class test_attr 
	    : public fs::attribute <true, int, 
    			fs::read_write_tag>
    {
        public:
	    test_attr (fs::cache &pm) 
		    : base_type (pm), v_ (17) 
	    {}
		
	    value_type do_get ()
	    {
		return v_;
	    }
	    void do_put (value_type v)
	    {
		v_ = v;
	    }
	    void do_commit ()
	    {
		std::string p = get_cache ().get_path ().string ();
		std::cout << "commit: " << v_ 
			  << "; file: " << p << '\n';
	    }
	
        private:	
  	    value_type v_;
    };
			
} // unnamed namespace 

int test_main (int argc, char **argv)
{
    fs::cache pm ("test");
    
    BOOST_TEST (get <test_attr> (pm) == 17);
    put <test_attr> (pm, 5);
    BOOST_TEST (get <test_attr> (pm) == 5);

    return 0;
}

