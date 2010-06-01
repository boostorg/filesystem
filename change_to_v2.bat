@echo off
rem  Change to version 2

rem  Copyright Beman Dawes 2010

rem  Distributed under the Boost Software License, Version 1.0.
rem  See http://www.boost.org/LICENSE_1_0.txt

echo Change to version 2

if exist src\unique_path.cpp goto versionok
echo Error: version 2 already in use
goto done
:versionok
 
echo Deleting v3 files...

del ..\..\boost\filesystem.hpp index.html
del /q ..\..\boost\filesystem
del /q /s build doc example src test

echo Copying v2 files...

xcopy v2\boost\filesystem.hpp ..\..\boost
xcopy /i v2\boost\filesystem ..\..\boost\filesystem
xcopy v2\libs\index.html .
xcopy /i /s v2\libs\build build
xcopy /i /s v2\libs\doc doc
xcopy /i /s v2\libs\example example
xcopy /i /s v2\libs\src src
xcopy /i /s v2\libs\test test

echo Copying complete. Version 2 files now ready to use.

echo You must now rebuild Boost.Filesystem object libraries.

:done
