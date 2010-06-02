#!/usr/bin/env bash

#  Change to version 3

#  Copyright Beman Dawes 2010

#  Distributed under the Boost Software License, Version 1.0.
#  See http://www.boost.org/LICENSE_1_0.txt

echo Change to version 3

if [ -f "src/unique_path.cpp" ]; then
  echo Error: version 3 already in use
  exit 1
fi

if [ ! -d "v2" ];then
  echo Making a copy of version 2 files...
  mkdir v2
  pushd v2
  mkdir boost
  mkdir libs
  popd
  cp ../../boost/filesystem.hpp v2/boost
  cp -r ../../boost/filesystem v2/boost
  cp index.html v2/libs
  cp -r build v2/libs
  cp -r doc v2/libs
  cp -r example v2/libs
  cp -r src v2/libs
  cp -r test v2/libs
fi
 
echo Deleting v2 files...

rm ../../boost/filesystem.hpp index.html
rm -r ../../boost/filesystem
rm -r build doc example src test

echo Copying v3 files...

cp v3/boost/filesystem.hpp ../../boost
cp -r v3/boost/filesystem ../../boost
cp v3/libs/filesystem/index.html .
cp -r v3/libs/filesystem/build .
cp -r v3/libs/filesystem/doc .
cp -r v3/libs/filesystem/example .
cp -r v3/libs/filesystem/src .
cp -r v3/libs/filesystem/test .

echo Copying complete. Version 3 files now ready to use.

echo You must now rebuild Boost.Filesystem object libraries.
