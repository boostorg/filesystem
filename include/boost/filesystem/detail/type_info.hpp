////////////////////////////////////////////////////////////////////////////////
// The Loki Library
// Copyright (c) 2001 by Andrei Alexandrescu
// This code accompanies the book:
// Alexandrescu, Andrei. "Modern C++ Design: Generic Programming and Design 
//     Patterns Applied". Copyright (c) 2001. Addison-Wesley.
// Permission to use, copy, modify, distribute and sell this software for any 
//     purpose is hereby granted without fee, provided that the above copyright 
//     notice appear in all copies and that both that copyright notice and this 
//     permission notice appear in supporting documentation.
// The author or Addison-Wesley Longman make no representations about the 
//     suitability of this software for any purpose. It is provided "as is" 
//     without express or implied warranty.
////////////////////////////////////////////////////////////////////////////////

// Last update: June 20, 2001

// boostified by Jan Langer (jan@langernetz.de), 12/2002

#ifndef BOOST_FILESYSTEM_DETAIL_TYPEINFO_HPP
#define BOOST_FILESYSTEM_DETAIL_TYPEINFO_HPP

#include <typeinfo>
#include <cassert>

namespace boost
{
 namespace filesystem
 {	 
  namespace detail
  {
	  
////////////////////////////////////////////////////////////////////////////////
// class type_info
// Purpose: offer a first-class, comparable wrapper over std::type_info
////////////////////////////////////////////////////////////////////////////////

    class type_info
    {
    public:
        // Constructors
        type_info (); // needed for containers
        type_info (const std::type_info&); // non-explicit

        // Access for the wrapped std::type_info
        const std::type_info& get() const;
        // Compatibility functions
        bool before(const type_info& rhs) const;
        const char* name() const;

    private:
        const std::type_info* pInfo_;
    };
    
// Implementation
    
    inline type_info::type_info()
    {
        class Nil {};
        pInfo_ = &typeid(Nil);
        assert(pInfo_);
    }
    
    inline type_info::type_info(const std::type_info& ti)
    : pInfo_(&ti)
    { assert(pInfo_); }
    
    inline bool type_info::before(const type_info& rhs) const
    {
        assert(pInfo_);
        return pInfo_->before(*rhs.pInfo_);
    }

    inline const std::type_info& type_info::get() const
    {
        assert(pInfo_);
        return *pInfo_;
    }
    
    inline const char* type_info::name() const
    {
        assert(pInfo_);
        return pInfo_->name();
    }

// Comparison operators
    
    inline bool operator==(const type_info& lhs, const type_info& rhs)
    { return lhs.get() == rhs.get(); }

    inline bool operator<(const type_info& lhs, const type_info& rhs)
    { return lhs.before(rhs); }

    inline bool operator!=(const type_info& lhs, const type_info& rhs)
    { return !(lhs == rhs); }    
    
    inline bool operator>(const type_info& lhs, const type_info& rhs)
    { return rhs < lhs; }
    
    inline bool operator<=(const type_info& lhs, const type_info& rhs)
    { return !(lhs > rhs); }
     
    inline bool operator>=(const type_info& lhs, const type_info& rhs)
    { return !(lhs < rhs); }
    
  } // namespace detail
 } // namespace filesystem
} // namespace boost

////////////////////////////////////////////////////////////////////////////////
// Change log:
// June 20, 2001: ported by Nick Thurn to gcc 2.95.3. Kudos, Nick!!!
////////////////////////////////////////////////////////////////////////////////

#endif // BOOST_FILESYSTEM_DETAIL_TYPEINFO_HPP
