rem this file is intended for usage from cmd.exe or from explorer (clicking on it)
@echo off
pushd %~dp0

echo 64bit...
mkdir _projects.vs11.64_dev
cd _projects.vs11.64_dev
cmake -G "Visual Studio 11 Win64" -DDEVEL_BUILD:STRING=1 -DCMAKE_BUILD_TYPE=RelWithDebInfo ..
if %errorlevel% neq 0 goto TERM
echo "generated projects are in ./_projects.vs11.64_dev"

cd ..
rem goto NOPAUSE

:TERM
pause

:NOPAUSE
popd
