//  windows_attributes  ----------------------------------------------------------------//

//  Copyright Beman Dawes 2010
   
//  Distributed under the Boost Software License, Version 1.0.
//  See http://www.boost.org/LICENSE_1_0.txt

//  Library home page: http://www.boost.org/libs/filesystem

//--------------------------------------------------------------------------------------//

//                   Useful for debugging status related issues                         //

//--------------------------------------------------------------------------------------//

#include <windows.h>
#include <map>
#include <utility>
#include <iostream>
#include <string>

using std::make_pair;

int main( int argc, char* argv[])
{
  typedef std::map<DWORD, std::string> decode_type;
  decode_type table;

  table.insert(make_pair<DWORD, std::string>(FILE_ATTRIBUTE_ARCHIVE, "FILE_ATTRIBUTE_ARCHIVE"));
  table.insert(make_pair<DWORD, std::string>(FILE_ATTRIBUTE_COMPRESSED, "FILE_ATTRIBUTE_COMPRESSED"));
  table.insert(make_pair<DWORD, std::string>(FILE_ATTRIBUTE_DEVICE, "FILE_ATTRIBUTE_DEVICE"));
  table.insert(make_pair<DWORD, std::string>(FILE_ATTRIBUTE_DIRECTORY, "FILE_ATTRIBUTE_DIRECTORY"));
  table.insert(make_pair<DWORD, std::string>(FILE_ATTRIBUTE_ENCRYPTED, "FILE_ATTRIBUTE_ENCRYPTED"));
  table.insert(make_pair<DWORD, std::string>(FILE_ATTRIBUTE_HIDDEN, "FILE_ATTRIBUTE_HIDDEN"));
  table.insert(make_pair<DWORD, std::string>(FILE_ATTRIBUTE_NOT_CONTENT_INDEXED, "FILE_ATTRIBUTE_NOT_CONTENT_INDEXED"));
  table.insert(make_pair<DWORD, std::string>(FILE_ATTRIBUTE_OFFLINE, "FILE_ATTRIBUTE_OFFLINE"));
  table.insert(make_pair<DWORD, std::string>(FILE_ATTRIBUTE_READONLY, "FILE_ATTRIBUTE_READONLY"));
  table.insert(make_pair<DWORD, std::string>(FILE_ATTRIBUTE_REPARSE_POINT, "FILE_ATTRIBUTE_REPARSE_POINT"));
  table.insert(make_pair<DWORD, std::string>(FILE_ATTRIBUTE_SPARSE_FILE, "FILE_ATTRIBUTE_SPARSE_FILE"));
  table.insert(make_pair<DWORD, std::string>(FILE_ATTRIBUTE_SYSTEM, "FILE_ATTRIBUTE_SYSTEM"));
  table.insert(make_pair<DWORD, std::string>(FILE_ATTRIBUTE_TEMPORARY, "FILE_ATTRIBUTE_TEMPORARY"));
  table.insert(make_pair<DWORD, std::string>(FILE_ATTRIBUTE_VIRTUAL, "FILE_ATTRIBUTE_VIRTUAL"));

  if (argc < 2)
  {
    std::cout << "Usage: windows_attributes path\n";
    return 1;
  }

  DWORD at(::GetFileAttributesA(argv[1]));
  if (at == INVALID_FILE_ATTRIBUTES)
  {
    std::cout << "GetFileAttributes(\"" << argv[1]
              << "\") returned INVALID_FILE_ATTRIBUTES\n";
    return 0;
  }

  std::cout << "GetFileAttributes(\"" << argv[1]
            << "\") returned ";

  bool bar = false;
  for (decode_type::iterator it = table.begin(); it != table.end(); ++it)
  {
    if (!(it->first & at))
      continue;
    if (bar)
      std::cout << " | ";
    bar = true;
    std::cout << it->second;
    at &= ~it->first;
  }
  std::cout << std::endl;

  if (at)
    std::cout << "plus unknown attributes " << at << std::endl; 

  return 0;
}
