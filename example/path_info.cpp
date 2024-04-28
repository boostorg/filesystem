//  path_info.cpp  ---------------------------------------------------------------------//

//  Copyright Beman Dawes 2009

//  Distributed under the Boost Software License, Version 1.0.
//  See http://www.boost.org/LICENSE_1_0.txt

//  Library home page: http://www.boost.org/libs/filesystem

//[example_path_info
#include <ios>
#include <iostream>
#include <boost/filesystem.hpp>

using namespace boost::filesystem;

int main(int argc, char* argv[])
{
    if (argc < 2)
    {
        std::cout << "Usage: path_info path-element [path-element...]\n"
                "Composes a path via operator/= from one or more path-element arguments\n"
                "Example: path_info foo/bar baz\n"
                "         would report info about the composed path "
#ifdef BOOST_POSIX_API
                "foo/bar/baz"
#else // BOOST_WINDOWS_API
                "foo/bar\\baz"
#endif
                << std::endl;
        return 1;
    }

    path p;
    for (int i = 1; i < argc; ++i)
        p /= argv[i]; // compose path p from the command line arguments

    std::cout << "\ncomposed path:" << std::endl;
    std::cout << "  operator<<()---------: " << p << std::endl;
    std::cout << "  make_preferred()-----: " << p.make_preferred() << std::endl;

    std::cout << "\nelements:" << std::endl;
    for (auto element : p)
        std::cout << "  " << element << std::endl;

    std::cout << "\nobservers, native format:" << std::endl;
#ifdef BOOST_POSIX_API
    std::cout << "  native()-------------: " << p.native() << std::endl;
    std::cout << "  c_str()--------------: " << p.c_str() << std::endl;
#else // BOOST_WINDOWS_API
    std::wcout << L"  native()-------------: " << p.native() << std::endl;
    std::wcout << L"  c_str()--------------: " << p.c_str() << std::endl;
#endif
    std::cout << "  string()-------------: " << p.string() << std::endl;
    std::wcout << L"  wstring()------------: " << p.wstring() << std::endl;

    std::cout << "\nobservers, generic format:" << std::endl;
    std::cout << "  generic_string()-----: " << p.generic_string() << std::endl;
    std::wcout << L"  generic_wstring()----: " << p.generic_wstring() << std::endl;

    std::cout << "\ndecomposition:" << std::endl;
    std::cout << "  root_name()----------: " << p.root_name() << std::endl;
    std::cout << "  root_directory()-----: " << p.root_directory() << std::endl;
    std::cout << "  root_path()----------: " << p.root_path() << std::endl;
    std::cout << "  relative_path()------: " << p.relative_path() << std::endl;
    std::cout << "  parent_path()--------: " << p.parent_path() << std::endl;
    std::cout << "  filename()-----------: " << p.filename() << std::endl;
    std::cout << "  stem()---------------: " << p.stem() << std::endl;
    std::cout << "  extension()----------: " << p.extension() << std::endl;

    std::cout << std::boolalpha;

    std::cout << "\nquery:" << std::endl;
    std::cout << "  empty()--------------: " << p.empty() << std::endl;
    std::cout << "  is_absolute()--------: " << p.is_absolute() << std::endl;
    std::cout << "  has_root_name()------: " << p.has_root_name() << std::endl;
    std::cout << "  has_root_directory()-: " << p.has_root_directory() << std::endl;
    std::cout << "  has_root_path()------: " << p.has_root_path() << std::endl;
    std::cout << "  has_relative_path()--: " << p.has_relative_path() << std::endl;
    std::cout << "  has_parent_path()----: " << p.has_parent_path() << std::endl;
    std::cout << "  has_filename()-------: " << p.has_filename() << std::endl;
    std::cout << "  has_stem()-----------: " << p.has_stem() << std::endl;
    std::cout << "  has_extension()------: " << p.has_extension() << std::endl;

    return 0;
}
//]
