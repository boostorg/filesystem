//  (C) Copyright Jan Langer 2002. Permission to copy, use, modify,
//  sell and distribute this software is granted provided this
//  copyright notice appears in all copies. This software is provided
//  "as is" without express or implied warranty, and with no claim as
//  to its suitability for any purpose.

#include "boost/filesystem/cache.hpp"

namespace boost 
{
    namespace filesystem
    {

        cache::cache (path const &name) 
		: map_ (new map_type)
		, path_ (name)
		, destruction_ (false)  
        {}

	cache::~cache ()
	{
            // force destruction of stored attributes
 	    // earlier than the rest of the cache
	    destruction_ = true;
	    map_.reset ();
	} 
                
    } // namespace filesystem  
} // namespace boost
