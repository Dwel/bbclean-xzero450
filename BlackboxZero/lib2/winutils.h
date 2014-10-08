#pragma once
#include "bbapi.h"

BBLIB_EXPORT void dbg_printf (TCHAR const *fmt, ...);
BBLIB_EXPORT void dbg_window (HWND hwnd, TCHAR const * fmt, ...);
BBLIB_EXPORT void BitBltRect (HDC hdc_to, HDC hdc_from, RECT const * r);
BBLIB_EXPORT HWND GetRootWindow (HWND hwnd);
BBLIB_EXPORT int is_bbwindow (HWND hwnd);
BBLIB_EXPORT int get_fontheight (HFONT hFont);
BBLIB_EXPORT int get_filetime (const TCHAR * fn, FILETIME * ft);
BBLIB_EXPORT int diff_filetime (const TCHAR * fn, FILETIME * ft0);
BBLIB_EXPORT unsigned long getfileversion (const TCHAR * path);
BBLIB_EXPORT TCHAR const * replace_environment_strings_alloc (TCHAR * & out, TCHAR const * src);
BBLIB_EXPORT TCHAR * replace_environment_strings (TCHAR * src, int max_size);
BBLIB_EXPORT TCHAR * win_error (TCHAR * msg, int msgsize);
BBLIB_EXPORT void ForceForegroundWindow (HWND theWin);
BBLIB_EXPORT void SetOnTop (HWND hwnd);
BBLIB_EXPORT int is_frozen (HWND hwnd);
BBLIB_EXPORT HWND window_under_mouse ();
/* ------------------------------------------------------------------------- */
/* Function: BBWait */
/* Purpose: wait for some obj and/or delay, dispatch messages in between */
/* ------------------------------------------------------------------------- */
BBLIB_EXPORT int BBWait (int delay, unsigned nObj, HANDLE * pObj);
/* ------------------------------------------------------------------------- */
/* API: BBSleep */
/* Purpose: pause for the given delay while blackbox remains responsive */
/* ------------------------------------------------------------------------- */
BBLIB_EXPORT void BBSleep (unsigned millisec);
/* ------------------------------------------------------------------------- */
/* Function: run_process */
/* Purpose: low level process spawn, optionally wait for completion */
/* ------------------------------------------------------------------------- */
#define RUN_SHOWERRORS  0
#define RUN_NOERRORS    1
#define RUN_WAIT        2
#define RUN_HIDDEN      4
#define RUN_NOARGS      8
#define RUN_NOSUBST    16
#define RUN_ISPIDL     32
#define RUN_WINDIR     64
BBLIB_EXPORT int run_process (const TCHAR * cmd, const TCHAR * dir, int flags);

BBLIB_EXPORT int load_imp (void * pp, const TCHAR * dll, const char * proc);
BBLIB_EXPORT int _load_imp (void * pp, const TCHAR * dll, const char * proc);
inline bool have_imp (void const * pp) { return (DWORD_PTR)pp > 1; }

