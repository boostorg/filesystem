//  (C) Copyright Jan Langer 2002. Permission to copy, use, modify,
//  sell and distribute this software is granted provided this
//  copyright notice appears in all copies. This software is provided
//  "as is" without express or implied warranty, and with no claim as
//  to its suitaibility for any purpose.
//
//  based on previous work of Dietmar Kuehl

#ifndef BOOST_FILESYSTEM_CACHE_HPP
#define BOOST_FILESYSTEM_CACHE_HPP

#include <map>
#include <cassert>

#include "boost/property_map.hpp"
#include "boost/shared_ptr.hpp"
#include "boost/any.hpp"
#include "boost/filesystem/path.hpp"
#include "boost/filesystem/detail/type_property_map.hpp"

namespace boost 
{
    namespace filesystem
    {
	// ------------------------------------------------
        // attribute cache class
	// this class acts as a property map. the key
	// to the map is the template parameter of the 
	// at() member function. and the value is an object
	// of this type, which was stores in the map.
	// 
        class cache
	{
            public:
		// category of the property map
                typedef read_write_property_map_tag category;

		// takes the name of the file for which the
		// cache is established
                cache (path const &name);

		// destroys the stored attributes
	        ~cache (); 

		// access on the path of the cache
		path get_path () const
		{
		    return path_;
		}	    

		// access on a certain value
		// at is used because operator[] is not
		// possible without an object as key 
		template <typename T>
		T &at ()
	        {
		    // fails access on the attributes
	            // during destruction of the cache object
	            assert (!destruction_);
                    
		    assert (map_);
		    any &a = map_->template at <T> ();
		    // no object of this type exists yet
		    // -> construct one
		    if (a.empty ())
		        a = T (*this);

		    // conversion to required type
		    T *v = any_cast <T> (&a);
	            assert (v);
                    return *v;
                }
		
            private:
	    	typedef detail::type_property_map <any> map_type;
		shared_ptr <map_type> map_;
	        path path_;
		bool destruction_;	
        };
	
	// ------------------------------------------------
	// base class for all other attributes
	// this class exists only for convenience. not every
	// attribute should implement this interface by 
	// its own. 
	//
	template <bool HasState,      
	          typename ValueType, 
	          typename Category>
        class attribute
        {
	    public:
		// data type of the property which is 
		// accessed by this attribute
		typedef ValueType value_type;
		// category of this attribute (readable,
		// writable, read_write)
		typedef Category category;
		
	        // attribute will be stored in the cache
		static bool const has_state = HasState;
	
	    private:
	        cache &a_;
	
	    protected:
		// allow easier initilization of this class 
		// in derived attributes
		typedef attribute <HasState, ValueType, Category>
			  base_type;

		// takes the cache the attribute belongs to
		explicit attribute (cache &a) : a_ (a)
		{}
		~attribute () 
		{}
				
		// access the cache to which the attribute 
		// belongs
  	        cache &get_cache ()
		{
		    return a_;
		}
	};

	// ------------------------------------------------
	// access the cache property map 
	// the key is the template parameter 
	// the value type equals the key
		  
        template <typename T>
        typename T::value_type get (cache &pmap)
	{
	    // stores attributes in map only if
	    // it wants to be stored
	    if (T::has_state)
	    {
                T &a = pmap.template at <T> ();
                return a.do_get ();
	    }
	    else // otherwise use a temporary
		return T (pmap).do_get ();
        }

        template <typename T>
        void put (cache &pmap, typename T::value_type v)
        {
	    // stores attributes in map only if
	    // it wants to be stored
	    if (T::has_state)
	    {
                T &a = pmap.template at <T> ();
                a.do_put (v);
	    }
	    else // otherwise use a temporary
		T (pmap).do_put (v);
        }

	// add commit function to "flush" this attribute
	// to the filesystem 
        template <typename T>
        void commit (cache &pmap)
        {
	    // stores attributes in map only if
	    // it wants to be stored
	    if (T::has_state)
	    {
                T &a = pmap.template at <T> ();
                a.do_commit ();
	    }
	    else // otherwise use a temporary
		T (pmap).do_commit ();
        }
	
	// ------------------------------------------------
	// convenience access functions
	
        template <typename T>
        typename T::value_type get (path const &p)
        {
            cache a (p);
            return get <T> (a);
        }

        template <typename T>
        void put (path const &p, typename T::value_type v)
        {
            cache a (p);
            put <T> (a, v);
        }

        // ------------------------------------------------
        // attribute categories
        // 
        struct readable_tag {};    
        struct writable_tag {};    
        struct read_write_tag 
		: public readable_tag
		, public writable_tag {};

    } // namespace filesystem
} // namespace boost

// putting get and put in global namespace
using boost::filesystem::get;
using boost::filesystem::put;
using boost::filesystem::commit;

#endif // BOOST_FILESYSTEM_CACHE_HPP

