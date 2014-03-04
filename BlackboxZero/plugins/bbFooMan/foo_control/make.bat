rem Compiled with Pelles C for Windows
cls
@echo off
call C:\PellesC\Projects\povars32.bat
rem Use -Gz options for __stdcall prototype default convention in foo_control!!!
%PellesCDir%\BIN\cc.exe -c -Os -Ox -Ze -Zx -Zl -Gz -Gn -Tx86-coff foo_control.c
%PellesCDir%\BIN\polink.exe /DLL /NOENTRY /NODEFAULTLIB /SUBSYSTEM:windows kernel32.lib user32.lib msvcrt.lib crt.lib foo_control.obj
del foo_control.obj foo_control.lib foo_control.exp
copy foo_control.dll "E:\Program Files\foobar2000\components\foo_control.dll"