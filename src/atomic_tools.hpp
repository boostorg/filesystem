//  atomic_tools.hpp  ------------------------------------------------------------------//

//  Copyright 2021 Andrey Semashev

//  Distributed under the Boost Software License, Version 1.0.
//  See http://www.boost.org/LICENSE_1_0.txt

//  See library home page at http://www.boost.org/libs/filesystem

//--------------------------------------------------------------------------------------//

#ifndef BOOST_FILESYSTEM_SRC_ATOMIC_TOOLS_HPP_
#define BOOST_FILESYSTEM_SRC_ATOMIC_TOOLS_HPP_

#include <boost/filesystem/config.hpp>

#if !defined(BOOST_FILESYSTEM_SINGLE_THREADED)

#include "atomic.hpp"

#define BOOST_FILESYSTEM_ATOMIC_VAR(type, name, init) atomic_ns::atomic< type > name(init)
#define BOOST_FILESYSTEM_ATOMIC_LOAD_RELAXED(a) a.load(atomic_ns::memory_order_relaxed)
#define BOOST_FILESYSTEM_ATOMIC_STORE_RELAXED(a, x) a.store(x, atomic_ns::memory_order_relaxed)

#else // !defined(BOOST_FILESYSTEM_SINGLE_THREADED)

#define BOOST_FILESYSTEM_ATOMIC_VAR(type, name, init) type name = init
#define BOOST_FILESYSTEM_ATOMIC_LOAD_RELAXED(a) a
#define BOOST_FILESYSTEM_ATOMIC_STORE_RELAXED(a, x) a = x

#endif // !defined(BOOST_FILESYSTEM_SINGLE_THREADED)

#endif // BOOST_FILESYSTEM_SRC_ATOMIC_TOOLS_HPP_
