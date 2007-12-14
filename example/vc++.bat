set BOOST_ROOT=..\..\..
rem A more robust script would test for BOOST_ROOT already set in the environment.

cl -EHsc -I%BOOST_ROOT% %* -link -LIBPATH:%BOOST_ROOT%\lib

