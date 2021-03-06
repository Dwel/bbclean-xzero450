rem this file is intended for usage from cmd.exe or from explorer (clicking on it)
@echo off

pushd %~dp0
set PATH=C:\Program Files (x86)\CMake 2.8\bin;%PATH%

echo 32b xp...

set INSTDIR=c:/_builds/vs_xp_32
mkdir _projects.vs11.32_xp
cd _projects.vs11.32_xp
call "C:\Program Files (x86)\Microsoft Visual Studio 11.0\VC\vcvarsall.bat" x86
if %errorlevel% neq 0 goto TERM
cmake -G "Visual Studio 11" -T "v110_xp" -DCMAKE_INSTALL_PREFIX:PATH=%INSTDIR% ..
if %errorlevel% neq 0 goto TERM
devenv BlackBoxZero.sln /build  RelWithDebInfo /project INSTALL
if %errorlevel% neq 0 goto TERM

cd ..

echo 64b xp...

set INSTDIR=c:/_builds/vs_xp_64
mkdir _projects.vs11.64_xp
cd _projects.vs11.64_xp
call "C:\Program Files (x86)\Microsoft Visual Studio 11.0\VC\vcvarsall.bat" x86_amd64
if %errorlevel% neq 0 goto TERM
cmake -G "Visual Studio 11 Win64" -T "v110_xp" -DCMAKE_INSTALL_PREFIX:PATH=%INSTDIR% ..
if %errorlevel% neq 0 goto TERM
devenv BlackBoxZero.sln /build  RelWithDebInfo /project INSTALL
if %errorlevel% neq 0 goto TERM

cd ..
 
goto NOPAUSE

:TERM
pause

:NOPAUSE
popd
