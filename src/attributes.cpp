//  (C) Copyright Jan Langer 2002. Permission to copy, use, modify,
//  sell and distribute this software is granted provided this
//  copyright notice appears in all copies. This software is provided
//  "as is" without express or implied warranty, and with no claim as
//  to its suitability for any purpose.

#include "boost/config.hpp"

#if defined(BOOST_WINDOWS) || (!defined(BOOST_POSIX) && defined(_WIN32))
#  define BOOST_WINDOWS
#else
#  define BOOST_POSIX
#endif

#ifdef BOOST_POSIX // only POSIX API is available

#include "boost/filesystem/attributes.hpp"
#include "boost/filesystem/exception.hpp"

#include <sys/types.h>
#include <sys/stat.h>
#include <pwd.h>
#include <grp.h>
#include <unistd.h>

namespace boost 
{
    namespace filesystem
    {
        namespace detail
        {
	    // --------------------------------------------
	    // attribute for accessing the members of 
	    // POSIX's struct stat

            class stat_attr : 
		public attribute <true, struct ::stat, read_write_tag>
            {
                    typedef struct ::stat stat_type;
            
                public:
                    stat_attr (cache &pm);
                    ~stat_attr ();
            
                    value_type do_get ();
		    void do_put (value_type v);
		    void do_commit ();
   
                private:    
                    value_type stat_;

		    // remember which part of stat was changed
                    unsigned int flags_;

		    static unsigned int const valid = 1; 
		    static unsigned int const mod_owner = 2;
            };
	    
	    stat_attr::stat_attr (cache &pm) 
		    : base_type (pm), flags_ (0)
            {}

	    stat_attr::~stat_attr ()
	    {
		do_commit ();
	    }
	    
	    stat_attr::value_type stat_attr::do_get () 
            {
                if (!(flags_ & valid))
                {
                    path p = get_cache ().get_path ();
                    if (::stat (p.string ().c_str (), &stat_) != 0)
                        throw filesystem_error (	
				std::string("stat_attr::do_get (): ") + 
				p.string (), system_error);  
		    flags_ |= valid;
		}
                return stat_;
            }       
	    
	    void stat_attr::do_put (value_type v)
	    {
		do_get ();
		if (v.st_uid != stat_.st_uid || v.st_gid != stat_.st_gid)
		{	
		    flags_ |= mod_owner;
   		    stat_.st_uid = v.st_uid;
		    stat_.st_gid = v.st_gid;
		}
	    }

	    void stat_attr::do_commit ()
	    {
		if (flags_ & mod_owner)
		{
  	    	    path p = get_cache ().get_path ();
	    	    if (::chown (p.string ().c_str (), 
			         stat_.st_uid, 
				 stat_.st_gid) != 0)
                	throw filesystem_error (	
				std::string("stat_attr::do_put (): ") + 
				p.string (), system_error);
		    flags_ |= mod_owner;
		}
	    }
	    
	} // namespace detail
       
        // ------------------------------------------------
	// filename
	
	// just forward it to cache::get_path ()
	filename::value_type filename::do_get ()
	{
	    return get_cache ().get_path ();	
	}

        // ------------------------------------------------
        // size
	// implemented in terms of stat_attr

        size::value_type size::do_get ()
        {
	    detail::stat_attr::value_type s = get <detail::stat_attr> (get_cache ());
            return value_type (s.st_size);
        }

	// ------------------------------------------------
	// owner
	// convert from string to uid on the fly and use 
	// stat_attr to access uid

	owner::value_type owner::do_get ()
	{
	    detail::stat_attr::value_type s = get <detail::stat_attr> (get_cache ());
	    struct ::passwd *g = ::getpwuid (s.st_uid);
	    if (g == NULL)
	    {
		path p = get_cache ().get_path (); 
		throw filesystem_error (	
			std::string("owner::do_get (): ") + p.string (),
			system_error);
	    }
	    assert (g);
	    return value_type (g->pw_name);
	}

	void owner::do_put (value_type v)
	{
	    struct ::passwd *g = ::getpwnam (v.c_str ());
	    if (g == NULL)
	    {
		path p = get_cache ().get_path ();
	    	throw filesystem_error (                        
		        std::string("owner::do_put (): ") + p.string (), 
			system_error);
	    }
	    detail::stat_attr::value_type s = get <detail::stat_attr> (get_cache ());
	    s.st_uid = g->pw_uid;
	    put <detail::stat_attr> (get_cache (), s);
	}

	void owner::do_commit ()
	{
	    commit <detail::stat_attr> (get_cache ());
	}
	
	// ------------------------------------------------
	// group
	// convert from string to gid on the fly and use 
	// stat_attr to access gid

	group::value_type group::do_get ()
	{
	    detail::stat_attr::value_type s = get <detail::stat_attr> (get_cache ());
	    struct ::group *g = ::getgrgid (s.st_gid);
	    if (g == NULL) 
	    {
		path p = get_cache ().get_path ();
		throw filesystem_error (	
			std::string("group::do_get (): ") + p.string (),
			system_error);
	    }
	    return value_type (g->gr_name);
	}

	void group::do_put (value_type v)
	{
	    struct ::group *g = ::getgrnam (v.c_str ());
	    if (g == NULL)
	    {
		path p = get_cache ().get_path ();
	    	throw filesystem_error (                        
		        std::string("group::do_put (): ") + p.string (), 
			system_error);
	    }
	    detail::stat_attr::value_type s = get <detail::stat_attr> (get_cache ());
	    s.st_gid = g->gr_gid;
	    put <detail::stat_attr> (get_cache (), s);
	}

	void group::do_commit ()
	{
	    commit <detail::stat_attr> (get_cache ());
	}
	
    } // namespace filesystem  
} // namespace boost

#endif // BOOST_POSIX
