rem this file is intended for usage from cmd.exe or from explorer (clicking on it)
@echo off

pushd %~dp0

echo Removing generated and old build files...

rmdir _projects.vs11.64_xp /S /Q
rmdir _projects.vs11.32_xp /S /Q
rmdir _projects.vs11.64 /S /Q
rmdir _projects.vs11.32 /S /Q

echo Removing old build ...

set INSTDIR="c:\_builds\vs_xp_64"
rmdir %INSTDIR% /S /Q
set INSTDIR="c:\_builds\vs_xp_32"
rmdir %INSTDIR% /S /Q

set INSTDIR="c:\_builds\vs_vista_32"
rmdir %INSTDIR% /S /Q
set INSTDIR="c:\_builds\vs_vista_64"
rmdir %INSTDIR% /S /Q
set INSTDIR="c:\_builds\redist"
rmdir %INSTDIR% /S /Q

del /F /Q c:\_builds\*.*
