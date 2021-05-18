//  atomic.hpp  ------------------------------------------------------------------------//

//  Copyright 2021 Andrey Semashev

//  Distributed under the Boost Software License, Version 1.0.
//  See http://www.boost.org/LICENSE_1_0.txt

//  See library home page at http://www.boost.org/libs/filesystem

//--------------------------------------------------------------------------------------//

#ifndef BOOST_FILESYSTEM_SRC_ATOMIC_HPP_
#define BOOST_FILESYSTEM_SRC_ATOMIC_HPP_

#include <boost/filesystem/config.hpp>

#if !defined(BOOST_NO_CXX11_HDR_ATOMIC)

#include <atomic>

namespace atomic_ns = std;

#else // !defined(BOOST_NO_CXX11_HDR_ATOMIC)

#include <boost/memory_order.hpp>
#include <boost/atomic/atomic.hpp>

namespace atomic_ns = boost;

#endif // !defined(BOOST_NO_CXX11_HDR_ATOMIC)

#endif // BOOST_FILESYSTEM_SRC_ATOMIC_HPP_
