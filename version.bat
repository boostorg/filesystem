@echo off
rem  Display version number

rem  Copyright Beman Dawes 2010

rem  Distributed under the Boost Software License, Version 1.0.
rem  See http://www.boost.org/LICENSE_1_0.txt

if exist src\unique_path.cpp (
  echo Version 3
) else (
  echo Version 2
)
