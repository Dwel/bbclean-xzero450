rem this file is intended for usage from cmd.exe or from explorer (clicking on it)
@echo off

pushd %~dp0

echo "Cleanup of older builds..."
call build_clean.bat

echo "Building Vista+ 32/64 bit..."
call build_all_vs2012.bat

echo "Building XP 32/64 bit..."
call build_all_vs2012_xp.bat

echo "Building installer..."
call build_installer.bat

