@echo off
rem  Change to version 3

rem  Copyright Beman Dawes 2010

rem  Distributed under the Boost Software License, Version 1.0.
rem  See http://www.boost.org/LICENSE_1_0.txt

echo Change to version 3

if not exist src\unique_path.cpp goto versionok
echo Error: version 3 already in use
goto done
:versionok

if exist v2 goto v2copydone

echo Making a copy of version 2 files...

md v2
pushd v2
md boost
md libs
popd
xcopy ..\..\boost\filesystem.hpp v2\boost
xcopy /i ..\..\boost\filesystem v2\boost\filesystem
xcopy index.html v2\libs
xcopy /i /s build v2\libs\build
xcopy /i /s doc v2\libs\doc
xcopy /i /s example v2\libs\example
xcopy /i /s src v2\libs\src
xcopy /i /s test v2\libs\test
:v2copydone
 
echo Deleting v2 files...

del ..\..\boost\filesystem.hpp index.html
del /q ..\..\boost\filesystem
del /q /s build doc example src test

echo Copying v3 files...

xcopy v3\boost\filesystem.hpp ..\..\boost
xcopy /i v3\boost\filesystem ..\..\boost\filesystem
xcopy v3\libs\index.html .
xcopy /i /s v3\libs\filesystem\build build
xcopy /i /s v3\libs\filesystem\doc doc
xcopy /i /s v3\libs\filesystem\example example
xcopy /i /s v3\libs\filesystem\src src
xcopy /i /s v3\libs\filesystem\test test

echo Copying complete. Version 3 files now ready to use.

echo You must now rebuild Boost.Filesystem object libraries.
:done