@echo off
echo Run regression tests, then create web-site format compiler status report
echo If no arguments supplied, "test" is assumed
echo If arguments with = supplied, enclose in quotes: "sTOOLS=gcc"

if %BOOST_ROOT%$==$ goto var_err
if "%TOOLS%"=="" goto var_err

pushd %BOOST_ROOT%\status
md bin 2>nul
if %1$==$ (
  bjam test >bin\jam_regr.log 2>&1
) else (
  bjam %1 %2 %3 %4 %5 %6 %7 %8 %9 >bin\jam_regr.log 2>&1
)
process_jam_log <bin\jam_regr.log
start bin\jam_regr.log
attrib -r cs-win32.html
compiler_status --no-links %BOOST_ROOT% >cs-win32.html
start cs-win32.html
popd
goto done

:var_err
echo Environment variables BOOST_ROOT and TOOLS must be set
echo For example, set BOOST_ROOT=C:\my_boost and set TOOLS=borland gcc
echo Note: no quotes around tool list

:done
 