//  (C) Copyright Jan Langer 2002. Permission to copy, use, modify,
//  sell and distribute this software is granted provided this
//  copyright notice appears in all copies. This software is provided
//  "as is" without express or implied warranty, and with no claim as
//  to its suitability for any purpose.

#ifndef BOOST_FILESYSTEM_ATTRIBUTES_HPP
#define BOOST_FILESYSTEM_ATTRIBUTES_HPP

#include <string>

#include "boost/filesystem/cache.hpp"
#include "boost/filesystem/path.hpp"

namespace boost 
{
    namespace filesystem
    {
        // ------------------------------------------------
        // predefined attributes

    	// filename attribute	    
	class filename : public attribute <false, path, readable_tag>
	{
	    public:
	        filename (cache &pm) : base_type (pm) {}

		value_type do_get ();
	};
	
	// size of the file in bytes
        class size : 
		public attribute <false, unsigned int, readable_tag>
        {
            public:
	        size (cache &pm) : base_type (pm) {}
	    	
                value_type do_get ();
        };

	// owner (user) of the file as a string
	class owner : 
	    public attribute <false, std::string, read_write_tag>
	{
	    public:
	        owner (cache &pm) : base_type (pm) {}

		value_type do_get ();
	        void do_put (value_type v);
	        void do_commit ();
	};

	// group as a string, to which the file belongs
	class group :
	    public attribute <false, std::string, read_write_tag>
	{
	    public:
	        group (cache &pm) : base_type (pm) {}
		
		value_type do_get ();
	        void do_put (value_type v);
	        void do_commit ();
	};
    
    } // namespace filesystem
} // namespace boost

#endif // BOOST_FILESYSTEM_ATTRIBUTES_HPP

