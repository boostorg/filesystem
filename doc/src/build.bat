@echo off
del tr2.html 2>nul
mmp TARGET=TR2 source.html tr2.html
del reference.html 2>nul
mmp TARGET=BOOST source.html reference.html
