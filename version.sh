#!/usr/bin/env bash

#  Display version number

#  Copyright Beman Dawes 2010

#  Distributed under the Boost Software License, Version 1.0.
#  See http://www.boost.org/LICENSE_1_0.txt

if [ -f "src/unique_path.cpp" ]; then
  echo Version 3
else
  echo Version 2
fi
