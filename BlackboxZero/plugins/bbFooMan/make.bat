rem Compiled with Pelles C for Windows
cls
@echo off
call C:\PellesC\Projects\povars32.bat
%PellesCDir%\BIN\cc.exe -c -Os -Ox -Ze -Zx -Zl -Gd -Gn -Tx86-coff bbfooman.c
%PellesCDir%\BIN\polink.exe /DLL /NOENTRY /NODEFAULTLIB /SUBSYSTEM:windows kernel32.lib user32.lib msvcrt.lib crt.lib gdiplus.lib Blackbox.lib bbfooman.obj
del bbfooman.obj bbfooman.lib bbfooman.exp
copy bbfooman.dll "C:\bbLean\bbfooman.dll"