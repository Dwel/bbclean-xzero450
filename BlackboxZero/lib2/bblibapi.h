#pragma once

#ifdef BBLIB_COMPILING
# ifndef WINVER
#  define WINVER 0x0500
#  define _WIN32_WINNT 0x0501
#  define _WIN32_IE 0x0501
# endif
# define WIN32_LEAN_AND_MEAN
# include <windows.h>
# include <cstdlib>
# include <cstdio>
# include <tchar.h>
# ifndef BBLIB_STATIC
#  define BBLIB_EXPORT __declspec(dllexport)
# endif
#endif

#ifndef BBLIB_EXPORT
# define BBLIB_EXPORT
#endif

#ifndef __BBCORE__
# define dbg_printf _dbg_printf
#endif

