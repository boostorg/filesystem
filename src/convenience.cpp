//  libs/filesystem/src/convenience.cpp  -------------------------------------//

//  Copyright Beman Dawes, 2002
//  Copyright Vladimir Prus, 2002
//  See accompanying license for terms and conditions of use.

//  See http://www.boost.org/libs/filesystem for documentation.

#include <boost/filesystem/convenience.hpp>

namespace boost
{
  namespace filesystem
  {

//  create_directories (contributed by Vladimir Prus)  -----------------------//

     void create_directories(const path& ph)
     {
         if (ph.empty() || exists(ph)) return;

         // First create branch, by calling ourself recursively
         create_directories(ph.branch_path());
         // Now that parent's path exists, create the directory
         create_directory(ph);
     }

    std::string extension(const path& ph)
    {
      std::string leaf = ph.leaf();

      std::string::size_type n = leaf.rfind('.');
      if (n != std::string::npos)
        return leaf.substr(n);
      else
        return std::string();
    }

    std::string basename(const path& ph)
    {
      std::string leaf = ph.leaf();

      std::string::size_type n = leaf.rfind('.');
      return leaf.substr(0, n);
    }

    path change_extension(const path& ph, const std::string& new_extension)
    {
      return ph.branch_path() / (basename(ph) + new_extension);
    }


  } // namespace filesystem
} // namespace boost
