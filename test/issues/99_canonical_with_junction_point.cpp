

#include <boost/filesystem.hpp>
#include <boost/core/lightweight_test.hpp>
#include <iostream>
#include <vector>

namespace fs = boost::filesystem;

struct TmpDir
{
  fs::path path;
  TmpDir(const fs::path& base): path(fs::absolute(base) / fs::unique_path())
  {
    fs::create_directories(path);
  }
  ~TmpDir()
  {
    boost::system::error_code ec;
    fs::remove_all(path, ec);
  }
};

int main()
{
  if(std::system("mklink /?") != 0)
  {
    std::cerr << "Junction points not supported. Skipping test" << std::endl;
    return boost::report_errors();
  }
  const fs::path cwd = fs::current_path();
  const TmpDir tmp(cwd);
  const fs::path junction = tmp.path / "junction";
  const fs::path real = tmp.path / "real";
  const fs::path subDir = "sub";
  fs::create_directories(real / subDir);
  fs::current_path(tmp.path);
  BOOST_TEST(std::system("mklink /j junction real") == 0);
  BOOST_TEST(fs::exists(junction));

  // Due to a bug there was a dependency on the current path so try the below for all:
  std::vector<fs::path> paths;
  paths.push_back(cwd);
  paths.push_back(junction);
  paths.push_back(real);
  paths.push_back(junction / subDir);
  paths.push_back(real / subDir);
  for (std::vector<fs::path>::iterator it = paths.begin(); it != paths.end(); ++it)
  {
    std::cout << "Testing in " << *it << std::endl;
    fs::current_path(*it);

    // Used by canonical, must work too
    BOOST_TEST(fs::read_symlink(junction) == real);

    BOOST_TEST(fs::canonical(junction) == real);
    BOOST_TEST(fs::canonical(junction / subDir) == real / subDir);
  }

  return boost::report_errors();
}
