@echo off
#  Change to version 2

#  Copyright Beman Dawes 2010

#  Distributed under the Boost Software License, Version 1.0.
#  See http://www.boost.org/LICENSE_1_0.txt

echo Change to version 2

if [ ! -f "src/unique_path.cpp" ]; then
  echo Error: version 2 already in use
  exit 1
fi
 
echo Deleting v3 files...

rm ../../boost/filesystem.hpp index.html
rm -r ../../boost/filesystem
rm -r build doc example src test

echo Copying v2 files...

cp v2/boost/filesystem.hpp ../../boost
cp -r v2/boost/filesystem ../../boost/filesystem
cp v2/libs/index.html .
cp -r v2/libs/build .
cp -r v2/libs/doc .
cp -r v2/libs/example .
cp -r v2/libs/src .
cp -r v2/libs/test .

echo Copying complete. Version 2 files now ready to use.

echo You must now rebuild Boost.Filesystem object libraries.
