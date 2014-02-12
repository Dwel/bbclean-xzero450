rem this batch creates packed builds in OUTDIR
@echo off
set PATH="C:\Program Files\WinRAR\";%path%

set OUTDIR=c:/_builds

set INSTDIR=vs_vista_32
winrar a -z%OUTDIR%/comment.txt -afzip -df %OUTDIR%/%BUILD%.zip %OUTDIR%/%BUILD%
if %errorlevel% neq 0 goto TERM

set INSTDIR=vs_vista_64
winrar a -z%OUTDIR%/comment.txt -afzip -df %OUTDIR%/%BUILD%.zip %OUTDIR%/%BUILD%
if %errorlevel% neq 0 goto TERM

set INSTDIR=vs_xp_32
winrar a -z%OUTDIR%/comment.txt -afzip -df %OUTDIR%/%BUILD%.zip %OUTDIR%/%BUILD%
if %errorlevel% neq 0 goto TERM

set INSTDIR=vs_xp_64
winrar a -z%OUTDIR%/comment.txt -afzip -df %OUTDIR%/%BUILD%.zip %OUTDIR%/%BUILD%
if %errorlevel% neq 0 goto TERM

goto NOPAUSE

:TERM
pause

:NOPAUSE
