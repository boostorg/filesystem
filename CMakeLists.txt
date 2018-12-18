# Copyright 2018 Peter Dimov
# Copyright 2018 Andrey Semashev
# Distributed under the Boost Software License, Version 1.0.
# See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt

# Partial (add_subdirectory only) and experimental CMake support
# Subject to change; please do not rely on the contents of this file yet.
# This file is NOT supposed to be used to build the library, use Boost.Build for that.

cmake_minimum_required(VERSION 3.5)

project(BoostFilesystem LANGUAGES CXX)

add_library(boost_filesystem INTERFACE)
add_library(Boost::filesystem ALIAS boost_filesystem)

target_include_directories(boost_filesystem INTERFACE include)

target_link_libraries(boost_filesystem
    INTERFACE
        Boost::assert
        Boost::config
        Boost::container_hash
        Boost::core
        Boost::detail
        Boost::io
        Boost::iterator
        Boost::smart_ptr
        Boost::system
        Boost::type_traits
)
