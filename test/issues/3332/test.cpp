#include <boost/filesystem.hpp>
#include <boost/filesystem/detail/utf8_codecvt_facet.hpp>
#include <iostream>
#include <string>
#include <locale>

namespace fs = boost::filesystem;

int main(void) {

  std::locale global_loc = std::locale();
  std::locale loc(global_loc, new fs::detail::utf8_codecvt_facet);
  fs::path::imbue(loc);

  std::cout << "HEADS UP! PIPE OUTPUT TO FILE AND INSPECT WITH EDITOR.\n"
    "WINDOWS COMMAND PROMPT FONTS DON'T SUPPORT CHINESE,\n"
    "EVEN WITH chcp 65001 AND EVEN AS OF WIN 10 TECH PREVIEW." << std::endl;

  fs::recursive_directory_iterator end;
  fs::recursive_directory_iterator iter
    ("C:/boost/modular/develop/libs/filesystem/test/issues/3332/test_folder");

  while (iter != end)
  {
    if (fs::is_directory(*iter))
    {
      std::cout << "[directory] " << iter->path() << std::endl;
    }
    else if (fs::is_regular(*iter))
    {
      std::cout << " [file] " << iter->path() << std::endl;
    }
    ++iter;
  }
  return 0;
}
