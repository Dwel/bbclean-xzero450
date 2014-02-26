rem this file is intended for usage from cmd.exe or from explorer (clicking on it)
@echo off

set PATH=C:\Program Files (x86)\CMake 2.8\bin;%PATH%

set INSTDIR=c:/_builds/vs_vista_64
mkdir _projects.vs11.64
cd _projects.vs11.64
call "C:\Program Files (x86)\Microsoft Visual Studio 11.0\VC\vcvarsall.bat" x86_amd64
if %errorlevel% neq 0 goto TERM
cmake -G "Visual Studio 11 Win64" -DCMAKE_INSTALL_PREFIX:PATH=%INSTDIR% ..
if %errorlevel% neq 0 goto TERM
devenv BlackBoxZero.sln /build RelWithDebInfo /project INSTALL
if %errorlevel% neq 0 goto TERM

cd ..

set INSTDIR=c:/_builds/vs_vista_64_dbg
mkdir _projects.vs11.64.dbg
cd _projects.vs11.64.dbg
call "C:\Program Files (x86)\Microsoft Visual Studio 11.0\VC\vcvarsall.bat" x86_amd64
if %errorlevel% neq 0 goto TERM
cmake -G "Visual Studio 11 Win64" -DCMAKE_INSTALL_PREFIX:PATH=%INSTDIR% ..
if %errorlevel% neq 0 goto TERM
devenv BlackBoxZero.sln /build Debug /project INSTALL
if %errorlevel% neq 0 goto TERM

cd ..

set INSTDIR=c:/_builds/vs_vista_32
mkdir _projects.vs11.32
cd _projects.vs11.32
call "C:\Program Files (x86)\Microsoft Visual Studio 11.0\VC\vcvarsall.bat" x86
if %errorlevel% neq 0 goto TERM
cmake -G "Visual Studio 11" -DCMAKE_INSTALL_PREFIX:PATH=%INSTDIR% ..
if %errorlevel% neq 0 goto TERM
devenv BlackBoxZero.sln /build  RelWithDebInfo /project INSTALL
if %errorlevel% neq 0 goto TERM

cd ..

set INSTDIR=c:/_builds/vs_vista_32_dbg
mkdir _projects.vs11.32.dbg
cd _projects.vs11.32.dbg
call "C:\Program Files (x86)\Microsoft Visual Studio 11.0\VC\vcvarsall.bat" x86
if %errorlevel% neq 0 goto TERM
cmake -G "Visual Studio 11" -DCMAKE_INSTALL_PREFIX:PATH=%INSTDIR% ..
if %errorlevel% neq 0 goto TERM
devenv BlackBoxZero.sln /build Debug /project INSTALL
if %errorlevel% neq 0 goto TERM

cd ..

goto NOPAUSE

:TERM
pause

:NOPAUSE
