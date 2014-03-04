/* Misc. functions */

#include <ctype.h>
#include <stdlib.h>

#define _WIN32_WINNT 0x0500
#define WIN32_LEAN_AND_MEAN

#include <tchar.h>

#include <BBApi.h> // BBApi-C.h

#include "misc.h"




#ifdef UNICODE
#define SUFFIX "W"
#else
#define SUFFIX "A"
#endif




static HMODULE g_User32Module;

// note: this does not work, you cannot use cdecl with variable (mojmir)
#if 0
#define DECLARE(TYPE, NAME, ARGS) \
        typedef TYPE NAME##_f ARGS; \
        static TYPE (*p##NAME) ARGS;
DECLARE(HMONITOR WINAPI, MonitorFromWindow, (HWND, DWORD))
DECLARE(BOOL WINAPI, GetMonitorInfo, (HMONITOR, LPMONITORINFO))
#undef DECLARE
#endif

typedef HMONITOR WINAPI MonitorFromWindow_f (HWND, DWORD);
static HMONITOR (*pMonitorFromWindow) (HWND, DWORD);

typedef BOOL WINAPI GetMonitorInfo_f (HMONITOR, LPMONITORINFO);
static BOOL (*pGetMonitorInfo) (HMONITOR, LPMONITORINFO);



BOOL
misc_begin(void)
{
	#define LOAD(F, S) \
	    if ((p##F=(F##_f*)GetProcAddress(g_User32Module, #F S)) == NULL) \
	        goto E_GetProcAddress;

	g_User32Module = LoadLibrary(T("USER32"));
	if (g_User32Module == NULL)
		goto E_LoadLibrary;

	LOAD(MonitorFromWindow, "");
	LOAD(GetMonitorInfo, SUFFIX);

	return TRUE;

E_GetProcAddress:
	FreeLibrary(g_User32Module);
	g_User32Module = NULL;
E_LoadLibrary:
	return FALSE;
	#undef LOAD
}


void
misc_end(void)
{
	FreeLibrary(g_User32Module);
	g_User32Module = NULL;
}




BOOL
is_2k_or_later(void)
{
	OSVERSIONINFO os;

	os.dwOSVersionInfoSize = sizeof(os);
	GetVersionEx(&os);

	return 5 <= os.dwMajorVersion;
}


static BOOL
is_98_or_2k_or_later(void)
{
	OSVERSIONINFO os;

	os.dwOSVersionInfoSize = sizeof(os);
	GetVersionEx(&os);

	return (5 <= os.dwMajorVersion)
	       || ((4 <= os.dwMajorVersion) && (10 <= os.dwMinorVersion));
}




DLL_EXPORT void
resize(HWND hwnd, int width, int height)
{
	SetWindowPos( hwnd, NULL, 0, 0, width, height,
	              SWP_NOACTIVATE | SWP_NOMOVE
	                | SWP_NOOWNERZORDER | SWP_NOZORDER );
}


DLL_EXPORT void
move(HWND hwnd, int x, int y)
{
	SetWindowPos( hwnd, NULL, x, y, 0, 0,
	              SWP_NOACTIVATE | SWP_NOOWNERZORDER
	                | SWP_NOSIZE | SWP_NOZORDER );
}




static RECT g_ScreenRect;
static int g_MonitorCount;


void
update_screen_rect(void)
{
	if (is_98_or_2k_or_later()) {
		g_MonitorCount = GetSystemMetrics(SM_CMONITORS);

		if (!(1 < g_MonitorCount)) {
		  g_ScreenRect.left = GetSystemMetrics(SM_XVIRTUALSCREEN);
		  g_ScreenRect.top = GetSystemMetrics(SM_YVIRTUALSCREEN);
		  g_ScreenRect.right = GetSystemMetrics(SM_CXVIRTUALSCREEN);
		  g_ScreenRect.bottom = GetSystemMetrics(SM_CYVIRTUALSCREEN);
		}
	} else {
		g_MonitorCount = 1;
		g_ScreenRect.left = 0;
		g_ScreenRect.top = 0;
		g_ScreenRect.right = GetSystemMetrics(SM_CXSCREEN);
		g_ScreenRect.bottom = GetSystemMetrics(SM_CYSCREEN);
	}
}


DLL_EXPORT void
get_screen_rect(HWND active_window, RECT* rect)
{
	if (1 < g_MonitorCount) {
		HMONITOR hmonitor;
		MONITORINFO info;

		hmonitor = (*pMonitorFromWindow)( active_window,
		                                  MONITOR_DEFAULTTONEAREST );
		info.cbSize = sizeof(info);
		(*pGetMonitorInfo)(hmonitor, &info);

		memcpy(rect, &(info.rcMonitor), sizeof(RECT));
	} else {
		memcpy(rect, &g_ScreenRect, sizeof(RECT));
	}
}




void
my_FreeLibrary(HMODULE hmodule)
{
	if (hmodule != NULL)
		FreeLibrary(hmodule);
}




BOOL
get_module_dir(LPTSTR buf, int size, HMODULE hmodule)
{
	LPTSTR bs;

	if (!GetModuleFileName(hmodule, buf, size))
		return FALSE;
	buf[size - 1] = T('\0');

	bs = _tcsrchr(buf, T('\\'));
	if (bs != NULL)
		bs[1] = T('\0');

	return TRUE;
}


BOOL
get_file_path(LPTSTR buf, int size, LPCTSTR filename, HMODULE hmodule)
{
	TCHAR dir[MAX_PATH];

	if (!get_module_dir(dir, NUMBER_OF(dir), hmodule))
		return FALSE;

	if ((int)size < (int)(_tcslen(dir) + _tcslen(filename) + 1)) {
		SetLastError(ERROR_BUFFER_OVERFLOW);
		return FALSE;
	}

	_tcscpy(buf, dir);
	_tcscat(buf, filename);

	return TRUE;
}




DLL_EXPORT LPTSTR
my_strncpy(LPTSTR dest, LPCTSTR src, int n)
{
	if (0 < n) {
		strncpy(dest, src, n-1);
		dest[n - 1] = T('\0');
	}

	return dest;
}


DLL_EXPORT LPTSTR
my_strndup(LPCTSTR s, int n)
{
	LPTSTR p;

	if (n < 0)
		return NULL;

	p = (TCHAR*)malloc(sizeof(TCHAR) * (n+1));
	if (p != NULL)
		my_strncpy(p, s, n+1);

	return p;
}


DLL_EXPORT int
my_strncmp(LPCTSTR sz, LPCTSTR nsz, int n)
{
	int r;

	if ((r = strncmp(sz, nsz, n)))
		return r;
	else
		return (sz[n] == T('\0') ? 0 : 1);
}


DLL_EXPORT void
tokenize( LPCTSTR source,
          int max,
          Token token[],
          int* token_num,
          LPCTSTR* rest )
{
	LPCTSTR WS = T(" \t");  /* WhiteSpace */
	LPCTSTR head;
	LPCTSTR tail;
	int n;

	head = source + strspn(source, WS);
	n = 0;
	while ((*head != T('\0')) && (n < max)) {
		head += strspn(head, WS);

		if (*head == T('\'')) {
			head++;
			tail = head + strcspn(head, T("'"));
		} else if (*head == T('"')) {
			head++;
			tail = head;
			while ((*tail != T('\0')) && (*tail != T('"'))) {
				if ((tail[0] == T('\\')) && (tail[1] != T('\0')))
					tail++;
				tail++;
			}
		} else {
			tail = head + strcspn(head, WS);
		}

		token[n].string = head;
		token[n].length = (tail - head) / sizeof(TCHAR);

		head = tail + ((*tail == T('\'')) || (*tail == T('"')) ? 1 : 0);
		n++;
	}

	*token_num = n;
	*rest = head + strspn(head, WS);
}


void
descape(LPTSTR s)
{
	LPTSTR i;
	LPTSTR o;

	i = o = s;
	while (*i != T('\0')) {
		if (*i == T('\\')) {
			i++;
			if (*i == T('\0')) {
				*o = T('\\');
				break;
			} else if (*i == T('n')) {
				*i = T('\n');
			}
		}

		*o = *i;
		o++;
		i++;
	}

	*o = T('\0');
}


DLL_EXPORT int
my_atoi(LPCTSTR s, int n)
{
	int result;
	int i;

	if (!(0 < n))
		return 0;

	result = 0;
	i = (s[0] == T('-') ? 1 : 0);
	while ((i < n) && isdigit(s[i])) {
		result = result*10 + (s[i] - T('0'));
		i++;
	}

	return (s[0] == T('-') ? -result : result);
}




DLL_EXPORT Padding
Padding_FromTokens(const Token token[], int token_num)
{
	Padding p;

	switch (token_num) {
	case 4:
		p.left = my_atoi(token[3].string, token[3].length);
	case 3:
		p.bottom = my_atoi(token[2].string, token[2].length);
	case 2:
		p.right = my_atoi(token[1].string, token[1].length);
	case 1:
		p.top = my_atoi(token[0].string, token[0].length);
	default:
		break;
	}

	switch (token_num) {
	default:
		p.top = 0;
	case 1:
		p.right = p.top;
	case 2:
		p.bottom = p.top;
	case 3:
		p.left = p.right;
	case 4:
		break;
	}

	return p;
}


DLL_EXPORT int
StyleNumber_FromToken(Token token)
{
	const struct {
		LPCTSTR name;
		int number;
	} STYLE_TABLE[] = {
		{T("toolbar"), SN_TOOLBAR},
		{T("toolbar.label"), SN_TOOLBARLABEL},
		{T("toolbar.windowlabel"), SN_TOOLBARWINDOWLABEL},
		{T("toolbar.clock"), SN_TOOLBARCLOCK},
		{T("menu.title"), SN_MENUTITLE},
		{T("menu.frame"), SN_MENUFRAME},
		{T("menu.hilite"), SN_MENUHILITE},
	};
	int i;

	for (i = 0; i < (int)NUMBER_OF(STYLE_TABLE); i++) {
		if (!my_strncmp(STYLE_TABLE[i].name,token.string,token.length))
			return STYLE_TABLE[i].number;
	}

	return STYLE_TABLE[0].number;
}


DLL_EXPORT BOOL
BOOL_FromToken(Token token)
{
	return !my_strncmp(T("true"), token.string, token.length);
}




DLL_EXPORT void
notice(LPCTSTR caption, LPCTSTR format, ...)
{
	TCHAR buf[80 * 25];
	va_list va;

	va_start(va, format);
	_vsntprintf(buf, NUMBER_OF(buf), format, va);
	buf[NUMBER_OF(buf) - 1] = T('\0');
	va_end(va);

	MessageBox( NULL, buf, caption,
	            MB_OK | MB_SETFOREGROUND | MB_TOPMOST );
}


DLL_EXPORT void
notice_with_last_error(LPCTSTR caption, LPCTSTR text)
{
	DWORD error;
	LPVOID error_detail;

	error = GetLastError();

	FormatMessage( FORMAT_MESSAGE_ALLOCATE_BUFFER
	                 | FORMAT_MESSAGE_FROM_SYSTEM,
	               NULL,
	               error,
	               MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
	               (LPTSTR)&error_detail,
	               0,
	               NULL );
	notice(caption, T("[FAILED] %s\n%s"), text, (LPCTSTR)error_detail);

	LocalFree(error_detail);
}


DLL_EXPORT void
broam_error(LPCTSTR caption, LPCTSTR broam, LPCTSTR format, ...)
{
	TCHAR buf[80 * 25];
	va_list va;

	va_start(va, format);
	_vsntprintf(buf, NUMBER_OF(buf), format, va);
	buf[NUMBER_OF(buf) - 1] = T('\0');
	va_end(va);

	notice(caption, T("%s\n\n%s"), broam, buf);
}


DLL_EXPORT BOOL
tokenize_check( LPCTSTR caption,
                int min, int max, BOOL allow_rest,
                int token_num, LPCTSTR rest,
                LPCTSTR broam )
{
	if ((min == max) && (token_num != min)) {
		broam_error( caption, broam,
		             T("%d arg%s required (%d given)"),
		             min, (1 < min ? T("s") : T("")), token_num );
		return FALSE;
	}

	if ((token_num < min) || (max < token_num)) {
		broam_error( caption, broam,
		             T("%d or %d arg%s required (%d given)"),
		             min, max, (1 < max ? T("s") : T("")), token_num );
		return FALSE;
	}

	if ((!allow_rest) && (rest[0] != T('\0'))) {
		broam_error(caption, broam, T("Trailing characters"));
		return FALSE;
	}

	return TRUE;
}




/* __END__ */
