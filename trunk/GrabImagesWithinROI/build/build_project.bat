@echo off

set OLDPATH=%PATH%

call "C:\Program Files (x86)\Microsoft Visual Studio 10.0\VC\vcvarsall.bat"

set PATH=C:\Program Files (x86)\CMake 2.8\bin;%PATH%
set GIWR_INCLUDES=D:\libraries\icode\google-icode-mda\trunk\GrabImagesWithinROI\src

cmake -G "Visual Studio 10" -DGIWR_INCLUDES=%GIWR_INCLUDES% ../src/

set PATH=%OLDPATH%
set OLDPATH=
