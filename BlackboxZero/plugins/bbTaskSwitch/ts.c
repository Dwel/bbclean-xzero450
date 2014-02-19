/* TS - Alt-Tab replacement engine

Notation
--------

A-X        Press X while holding Alt.

S-A-X      Press X while holding Shift and Alt.

U-X        Release X.

A-NonX     Press non-X key while holding Alt.


Known issues
------------

[TSEngine_HOOK_PROBLEM]
	TS will set itself as the foreground window.
	Because TS rarely fails to switch task.
	(This problem appears only rxvt of Cygwin as far as I tested.)

[TSEngine_SIMPLE_PROBLEM]
	TS sometimes misses key-strokes, especially U-Alt.
	So TS checks U-Alt verbosely at the end of TSM_STARTSEL.

	I guess that missings are occured
	between the beginning of task selecting
	and the calling SetForegroundWindow.
	If so, TS cannot receive the messages -- cannot check U-Alt.

	For example, let's imagine the following situation:

		  Key        Progress of TS
		  -------    --------------
		  D-A-Alt

		  D-A-Tab
		             TSM_NEXT
		             {
		                 TSM_STARTSEL (SendMessage'ed from TSM_NEXT)
		                 {
		* U-Alt
		* U-Tab
		                     SetForegroundWindow (in TSM_STARTSEL)
		                 }
		             }

	The key events marked with `*' are sent to another window.
*/
/*
#define UNICODE
#define _UNICODE
*/

#include <assert.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <tchar.h>

#include "ts.h"




#define NUMBER_OF(a) (sizeof(a) / sizeof((a)[0]))

#define T(s) TEXT(s)

#define HWND_FROM_TS(ts) ((HWND)(ts))
#define TS_FROM_HWND(hwnd) ((TS*)(hwnd))




typedef struct _TSTaskInfo TSTaskInfo;
struct _TSTaskInfo {
	HWND window;
	TCHAR title[80];
	ULONG_PTR extra[1];
};

struct _TSTaskList {
	TSTaskInfo** list;
	int count;
	int selected_task_index;
};

/* struct _TS -- not defined.  See TS_New/TS_Del. */




static const TCHAR TS_CLASS_NAME[] = T("TS/TaskSwitcher");
#define TS_WINDOW_NAME NULL

#define TSM_STARTSEL (WM_USER+0x0101)
#define TSM_ENDSEL   (WM_USER+0x0102)
#define TSM_SHOW     (WM_USER+0x0103)
#define TSM_HIDE     (WM_USER+0x0104)
#define TSM_REDRAW   (WM_USER+0x0105)

#define IDHK_NEXT 0x0001
#define IDHK_PREV 0x0002

/* FIXME: Should these be available as options? */
#define SHOW_HEAVY_THINGS FALSE
#define USE_INVESTIGATE FALSE




#define SHARED(X) X __attribute__((section("TS_shared"), shared))

static HWND SHARED(g_TSWindow) = NULL;
static HHOOK SHARED(g_HKeyHook) = NULL;

static TSHandler* g_OnStartSelecting = TS_DefaultOnStartSelecting;
static TSHandler* g_OnPaint = TS_DefaultOnPaint;
static TSHandler* g_OnSwitch = TS_DefaultOnSwitch;
static TSHandler* g_OnCancel = TS_DefaultOnCancel;
static TSSieveProcedure* g_SieveProcedure = TS_DefaultSieveProcedure;
static BOOL g_Activeness = FALSE;
static BOOL SHARED(g_Selecting) = FALSE;
static TSTaskList* g_TaskList = NULL;
static UINT SHARED(g_HotKey) = VK_TAB;
static TSEngine g_Engine = TSEngine_NONE;
static HWND g_DebugWindow = NULL;








/* --------------------------------------------------------- */
/* Misc. functions (which are weakly-related to TS) */

#define DEBUG_PRINTF(x) ((g_DebugWindow != NULL) && (debug_printf x, 0))
#define FAILURE(f) DEBUG_PRINTF((T("(FAILED) %s"), f))

static void
debug_printf(LPCTSTR format, ...)
{
	va_list va;
	TCHAR buf[80*25];

	va_start(va, format);
	_vsntprintf(buf, NUMBER_OF(buf), format, va);
	buf[NUMBER_OF(buf) - 1] = T('\0');
	va_end(va);

	SendMessage(g_DebugWindow, EM_REPLACESEL, 0, (LPARAM)buf);
	SendMessage(g_DebugWindow, EM_REPLACESEL, 0, (LPARAM)T("\r\n"));
}


#if USE_INVESTIGATE
static void
investigate(HWND hwnd)
{
	LONG_PTR exstyle = GetWindowLongPtr(hwnd, GWL_EXSTYLE);
	LONG_PTR style = GetWindowLongPtr(hwnd, GWL_STYLE);
	TCHAR buf[80];

	GetWindowText(hwnd, buf, NUMBER_OF(buf));
	DEBUG_PRINTF(( T("inv: %p <- %p (%p) '%s'"),
	               hwnd, GetParent(hwnd), GetLastActivePopup(hwnd), buf ));

	buf[0] = T('\0');
	_tcscat(buf, (exstyle&WS_EX_ACCEPTFILES ? T("Af") : T(".,")));
	_tcscat(buf, (exstyle&WS_EX_APPWINDOW ? T("Aw") : T(".,")));
	_tcscat(buf, (exstyle&WS_EX_CLIENTEDGE ? T("Ce") : T(".,")));
	_tcscat(buf, (exstyle&WS_EX_COMPOSITED ? T("Cm") : T(".,")));
	_tcscat(buf, (exstyle&WS_EX_CONTEXTHELP ? T("Ch") : T(".,")));
	_tcscat(buf, (exstyle&WS_EX_CONTROLPARENT ? T("Cp") : T(".,")));
	_tcscat(buf, (exstyle&WS_EX_DLGMODALFRAME ? T("Dm") : T(".,")));
	_tcscat(buf, (exstyle&WS_EX_LAYERED ? T("Ly") : T(".,")));
	_tcscat(buf, (exstyle&WS_EX_LAYOUTRTL ? T("Lr") : T(".,")));
	_tcscat(buf, (exstyle&WS_EX_LEFT ? T("Le") : T(".,")));
	_tcscat(buf, (exstyle&WS_EX_LEFTSCROLLBAR ? T("Ls") : T(".,")));
	_tcscat(buf, (exstyle&WS_EX_LTRREADING ? T("Lr") : T(".,")));
	_tcscat(buf, (exstyle&WS_EX_MDICHILD ? T("Md") : T(".,")));
	_tcscat(buf, (exstyle&WS_EX_NOACTIVATE ? T("Na") : T(".,")));
	_tcscat(buf, (exstyle&WS_EX_NOINHERITLAYOUT ? T("Ni"):T(".,")));
	_tcscat(buf, (exstyle&WS_EX_NOPARENTNOTIFY ? T("Np") : T(".,")));
	_tcscat(buf, (exstyle&WS_EX_OVERLAPPEDWINDOW ? T("Ol"):T(".,")));
	_tcscat(buf, (exstyle&WS_EX_PALETTEWINDOW ? T("Pw") : T(".,")));
	_tcscat(buf, (exstyle&WS_EX_RIGHT ? T("Ri") : T(".,")));
	_tcscat(buf, (exstyle&WS_EX_RIGHTSCROLLBAR ? T("Rs") : T(".,")));
	_tcscat(buf, (exstyle&WS_EX_RTLREADING ? T("Rr") : T(".,")));
	_tcscat(buf, (exstyle&WS_EX_STATICEDGE ? T("Se") : T(".,")));
	_tcscat(buf, (exstyle&WS_EX_TOOLWINDOW ? T("Tw") : T(".,")));
	_tcscat(buf, (exstyle&WS_EX_TOPMOST ? T("Tm") : T(".,")));
	_tcscat(buf, (exstyle&WS_EX_TRANSPARENT ? T("Tp") : T(".,")));
	_tcscat(buf, (exstyle&WS_EX_WINDOWEDGE ? T("We") : T(".,")));
	DEBUG_PRINTF((T("inv: ex '%s'"), buf));

	buf[0] = T('\0');
	_tcscat(buf, (style&WS_BORDER ? T("Br") : T(".,")));
	_tcscat(buf, (style&WS_CAPTION ? T("Cp") : T(".,")));
	_tcscat(buf, (style&WS_CHILD ? T("Ch") : T(".,")));
	_tcscat(buf, (style&WS_CHILDWINDOW ? T("Cw") : T(".,")));
	_tcscat(buf, (style&WS_CLIPCHILDREN ? T("Cc") : T(".,")));
	_tcscat(buf, (style&WS_CLIPSIBLINGS ? T("Cs") : T(".,")));
	_tcscat(buf, (style&WS_DISABLED ? T("Di") : T(".,")));
	_tcscat(buf, (style&WS_DLGFRAME ? T("Df") : T(".,")));
	_tcscat(buf, (style&WS_GROUP ? T("Gr") : T(".,")));
	_tcscat(buf, (style&WS_HSCROLL ? T("Hs") : T(".,")));
	_tcscat(buf, (style&WS_ICONIC ? T("Ic") : T(".,")));
	_tcscat(buf, (style&WS_MAXIMIZE ? T("Mx") : T(".,")));
	_tcscat(buf, (style&WS_MAXIMIZEBOX ? T("Mb") : T(".,")));
	_tcscat(buf, (style&WS_MINIMIZE ? T("Mn") : T(".,")));
	_tcscat(buf, (style&WS_MINIMIZEBOX ? T("Mb") : T(".,")));
	_tcscat(buf, (style&WS_OVERLAPPED ? T("Ol") : T(".,")));
	_tcscat(buf, (style&WS_OVERLAPPEDWINDOW ? T("Ow") : T(".,")));
	_tcscat(buf, (style&WS_POPUP ? T("Po") : T(".,")));
	_tcscat(buf, (style&WS_POPUPWINDOW ? T("Pw") : T(".,")));
	_tcscat(buf, (style&WS_SIZEBOX ? T("Sb") : T(".,")));
	_tcscat(buf, (style&WS_SYSMENU ? T("Sm") : T(".,")));
	_tcscat(buf, (style&WS_TABSTOP ? T("Ts") : T(".,")));
	_tcscat(buf, (style&WS_THICKFRAME ? T("Tf") : T(".,")));
	_tcscat(buf, (style&WS_TILED ? T("Ti") : T(".,")));
	_tcscat(buf, (style&WS_TILEDWINDOW ? T("Tw") : T(".,")));
	_tcscat(buf, (style&WS_VISIBLE ? T("Vi") : T(".,")));
	_tcscat(buf, (style&WS_VSCROLL ? T("Vs") : T(".,")));
	_tcscat(buf, (style&WS_ACTIVECAPTION ? T("Ac") : T(".,")));
	DEBUG_PRINTF((T("inv: st '%s'"), buf));
}
#endif




static BOOL
is_shift_vk(UINT vk)
{
	switch (vk) {
	default:
		return FALSE;

	case VK_SHIFT:
	case VK_LSHIFT:
	case VK_RSHIFT:
		return TRUE;
	}
}


static BOOL
is_alt_vk(UINT vk)
{
	switch (vk) {
	default:
		return FALSE;

	case VK_MENU:
	case VK_LMENU:
	case VK_RMENU:
		return TRUE;
	}
}


static BOOL
is_pressed_now(UINT vk)
{
	return (1 << (sizeof(SHORT) * CHAR_BIT - 1)) & GetAsyncKeyState(vk);
}




static HWND
get_last_active_window(void)
{
	HWND last_active_window;
	HWND t;

	last_active_window = GetForegroundWindow();

	while ((t = GetParent(last_active_window)) != NULL)
		last_active_window = t;

	while ((t = GetWindow(last_active_window, GW_OWNER)) != NULL)
		last_active_window = t;

	return GetLastActivePopup(last_active_window);
}




static void
resize(HWND hwnd, int width, int height)
{
	SetWindowPos( hwnd, NULL, 0, 0, width, height,
		      SWP_NOACTIVATE | SWP_NOMOVE
			  | SWP_NOOWNERZORDER | SWP_NOZORDER );
}


static void
move(HWND hwnd, int x, int y)
{
	SetWindowPos( hwnd, NULL, x, y, 0, 0,
		      SWP_NOACTIVATE | SWP_NOOWNERZORDER
	                  | SWP_NOSIZE | SWP_NOZORDER );
}




static int
get_screen_width(void)
{
	int w;

	/* FIXME: Don't consider multimonitor env. */
	w = GetSystemMetrics(SM_CXSCREEN);
	return (0 < w) ? w : 640;
}


static int
get_screen_height(void)
{
	int h;

	/* FIXME: Don't consider multimonitor env. */
	h = GetSystemMetrics(SM_CYSCREEN);
	return (0 < h) ? h : 640;
}




static BOOL CALLBACK
_delete_property_procedure(HWND hwnd, LPCTSTR key, HANDLE data)
{ 
	RemoveProp(hwnd, key); 
	return TRUE; 
}


static int
RemoveAllProps(HWND hwnd)
{
	return EnumProps(hwnd, _delete_property_procedure);
}




static HWND
DebugWnd_New(HINSTANCE hinstance)
{
	HWND self;
	HFONT hfont;

	self = CreateWindowEx( WS_EX_TOOLWINDOW,
	                       T("EDIT"),
	                       NULL,
	                       WS_POPUP | WS_VISIBLE | WS_VSCROLL
	                           | ES_LEFT | ES_MULTILINE | ES_AUTOVSCROLL,
	                       0, 0, 512, 512,
	                       NULL,
	                       NULL,
	                       hinstance,
	                       NULL );
	if (self == NULL)
		return NULL;

	SendMessage(self, WM_SETTEXT, 0, (LPARAM)T(""));

	hfont = CreateFont( 12, 0,
	                    0, 0,
	                    FW_NORMAL,
	                    FALSE,
	                    FALSE,
	                    FALSE,
	                    ANSI_CHARSET,
	                    OUT_DEFAULT_PRECIS,
	                    CLIP_DEFAULT_PRECIS,
	                    DEFAULT_QUALITY,
	                    FF_DONTCARE,
	                    T("MS Gothic") );
	SendMessage(self, WM_SETFONT, (WPARAM)hfont, TRUE);

	return self;
}


static void
DebugWnd_Del(HWND self)
{
	HGDIOBJ hfont;

	hfont = (HGDIOBJ)SendMessage(self, WM_GETFONT, 0, 0);
	DestroyWindow(self);
	DeleteObject(hfont);
}








/* --------------------------------------------------------- */
/* Misc. functions (which are strongly-related to TS) */

static TSTaskInfo*
TSTaskInfo_New(HWND hwnd, ULONG_PTR extra[], int extra_count)
{
	TSTaskInfo* self;

	self = malloc(sizeof(*self) + sizeof(extra[0]) * (extra_count-1));
	if (self == NULL)
		return NULL;

	self->window = hwnd;
	GetWindowText(hwnd, self->title, NUMBER_OF(self->title));
	if ((extra != NULL) && (0 < extra_count))
		memcpy(self->extra, extra, sizeof(extra[0]) * extra_count);

	return self;
}


static void
TSTaskInfo_Del(TSTaskInfo* self)
{
	free(self);
}




static TSTaskList*
TSTaskList_New(void)
{
	TSTaskList* self;

	self = malloc(sizeof(*self));
	if (self == NULL)
		return NULL;

	self->count = 0;
	self->list = NULL;
	self->selected_task_index = -1;

	return self;
}


static void
TSTaskList_Del(TSTaskList* self)
{
	int i;

	for (i = 0; i < self->count; i++)
		TSTaskInfo_Del(self->list[i]);

	free(self->list);
	free(self);
}


static BOOL
TSTaskList_Append( TSTaskList* self,
                   HWND hwnd, ULONG_PTR extra[], int extra_count )
{
	TSTaskInfo* ti;
	TSTaskInfo** tmp;

	ti = TSTaskInfo_New(hwnd, extra, extra_count);
	if (ti == NULL) {
		FAILURE(T("TSTaskList_Append/TSTaskInfo_New"));
		return FALSE;
	}

	tmp = realloc(self->list, sizeof(self->list[0]) * (self->count + 1));
	if (tmp == NULL) {
		TSTaskInfo_Del(ti);
		FAILURE(T("TSTaskList_Append/realloc"));
		return FALSE;
	}

	self->list = tmp;
	self->list[self->count] = ti;
	self->count++;

	return TRUE;
}




typedef struct {
	TSTaskList* tasklist;
	TSSieveProcedure* sieve_procedure;
} EWPArg;

static BOOL CALLBACK
enumwindowsproc(HWND hwnd, LPARAM lp)
{
	EWPArg* arg = (EWPArg*)lp;

	return (*(arg->sieve_procedure))(hwnd, arg->tasklist);
}

/* BUGS: Enumerated windows by EnumWindows are already sorted by Z-order? */
static TSTaskList*
EnumerateTasks(TSSieveProcedure* sieve_procedure)
{
	TSTaskList* tl;
	EWPArg arg;

	tl = TSTaskList_New();
	if (tl == NULL) {
		FAILURE(T("EnumerateTasks/TSTaskList_New"));
		return NULL;
	}

	arg.tasklist = tl;
	arg.sieve_procedure = sieve_procedure;

	if (!EnumWindows(enumwindowsproc, (LPARAM)&arg)) {
		TSTaskList_Del(tl);
		FAILURE(T("EnumerateTasks/EnumWindows"));
		return NULL;
	}

	return tl;
}




#define IS_WINDOW_TOPMOST(hwnd) \
            (GetWindowLongPtr((hwnd), GWL_EXSTYLE) & WS_EX_TOPMOST)

static int
get_current_task_index(const TSTaskList* tasklist, HWND last_active_window)
{
	int i;

	for (i = 0; i < tasklist->count; i++)
		if (tasklist->list[i]->window == last_active_window)
			return i;

	return tasklist->count - 1;  /* BUGS: Assumes 0 < tasklist->count */
}




static LRESULT CALLBACK
TS_LowLevelKeyboardProc(int ncode, WPARAM wp, LPARAM lp)
{
	if (0 <= ncode && g_Selecting) {
		const KBDLLHOOKSTRUCT* data = (const KBDLLHOOKSTRUCT*)lp;

		switch (wp) {
		default:
			break;

		case WM_SYSKEYDOWN:
		case WM_KEYDOWN:
			if ( (data->vkCode != g_HotKey)
			     && (!is_alt_vk(data->vkCode))
			     && (!is_shift_vk(data->vkCode)) )
			{
				SendMessage(g_TSWindow, TSM_CANCEL, 0, 0);
			}
			break;

		case WM_KEYUP:
		case WM_SYSKEYUP:
			if (is_alt_vk(data->vkCode)) {
				SendMessage(g_TSWindow, TSM_SWITCH, 0, 0);
			}
			break;
		}
	}

	return CallNextHookEx(g_HKeyHook, ncode, wp, lp);
}




static BOOL
Engine_Activate(TSEngine engine)
{
	switch (engine) {
	default:
		assert(FALSE);
		return FALSE;

	case TSEngine_NONE:
		return TRUE;

	case TSEngine_SIMPLE:
		if (!RegisterHotKey(g_TSWindow, IDHK_NEXT, MOD_ALT, g_HotKey))
			goto E1;
		if (!RegisterHotKey( g_TSWindow, IDHK_PREV,
		                     MOD_ALT|MOD_SHIFT, g_HotKey ))
			goto E2;
		return TRUE;
	E2:
		UnregisterHotKey(g_TSWindow, IDHK_NEXT);
	E1:
		return FALSE;

	case TSEngine_HOOK: {
		HINSTANCE hinstance;

		hinstance = (HINSTANCE)GetWindowLongPtr( g_TSWindow,
		                                         GWLP_HINSTANCE );
		g_HKeyHook = SetWindowsHookEx( WH_KEYBOARD_LL,
		                               TS_LowLevelKeyboardProc,
		                               hinstance, 0 );
		if (g_HKeyHook == NULL)
			goto E3;
		if (!RegisterHotKey(g_TSWindow, IDHK_NEXT, MOD_ALT, g_HotKey))
			goto E4;
		if (!RegisterHotKey( g_TSWindow, IDHK_PREV,
		                     MOD_ALT|MOD_SHIFT, g_HotKey ))
			goto E5;
		return TRUE;
	E5:
		UnregisterHotKey(g_TSWindow, IDHK_NEXT);
	E4:
		UnhookWindowsHookEx(g_HKeyHook);
	E3:
		return FALSE;
		}
	}
}


static BOOL
Engine_Inactivate(TSEngine engine)
{
	switch (engine) {
	default:
		assert(FALSE);
		return FALSE;

	case TSEngine_NONE:
		return TRUE;

	case TSEngine_SIMPLE:
		UnregisterHotKey(g_TSWindow, IDHK_PREV);
		UnregisterHotKey(g_TSWindow, IDHK_NEXT);
		return TRUE;

	case TSEngine_HOOK:
		UnregisterHotKey(g_TSWindow, IDHK_PREV);
		UnregisterHotKey(g_TSWindow, IDHK_NEXT);
		return UnhookWindowsHookEx(g_HKeyHook);
	}
}




static LRESULT CALLBACK
TS_WindowProcedure(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp)
{
	switch (msg) {
	default:
		break;


	/* ======== TSM_XXX ======== */

	/* -------- TSM_XXX (public) -------- */

	case TSM_ACTIVATE:
		if (g_Activeness)
			return TRUE;

		g_Activeness = Engine_Activate(g_Engine);
		return g_Activeness;

	case TSM_INACTIVATE:
		if (!g_Activeness)
			return TRUE;

		if (g_Selecting)
			SendMessage(hwnd, TSM_CANCEL, 0, 0);
		g_Activeness = FALSE;
		return Engine_Inactivate(g_Engine);

	case TSM_NEXT:
		DEBUG_PRINTF((T("TSM_NEXT")));
		if (!g_Activeness)
			return FALSE;

		if (!g_Selecting) {
			if (!SendMessage(hwnd, TSM_STARTSEL, 0, 0))
				return FALSE;
		}

		g_TaskList->selected_task_index++;
		while (g_TaskList->count <= g_TaskList->selected_task_index)
			g_TaskList->selected_task_index -= g_TaskList->count;
		SendMessage(hwnd, TSM_REDRAW, 0, 0);
		return TRUE;

	case TSM_PREV:
		DEBUG_PRINTF((T("TSM_PREV")));
		if (!g_Activeness)
			return FALSE;

		if (!g_Selecting) {
			if (!SendMessage(hwnd, TSM_STARTSEL, 0, 0))
				return FALSE;
		}

		g_TaskList->selected_task_index--;
		while (g_TaskList->selected_task_index < 0)
			g_TaskList->selected_task_index += g_TaskList->count;
		SendMessage(hwnd, TSM_REDRAW, 0, 0);
		return TRUE;

	case TSM_SWITCH:
		DEBUG_PRINTF((T("TSM_SWITCH %c"), (g_Selecting ? 'o' : 'x')));
		if ((!g_Activeness) || (!g_Selecting))
			return FALSE;

		(*g_OnSwitch)(TS_FROM_HWND(hwnd));
		SendMessage(hwnd, TSM_ENDSEL, 0, 0);
		return TRUE;

	case TSM_CANCEL:
		DEBUG_PRINTF((T("TSM_CANCEL %c"), (g_Selecting ? 'o' : 'x')));
		if ((!g_Activeness) || (!g_Selecting))
			return FALSE;

		SendMessage(hwnd, TSM_ENDSEL, 0, 0);
		return TRUE;

	/* -------- TSM_XXX (internal use) -------- */

	case TSM_SHOW: ShowWindow(hwnd, SW_SHOW); return TRUE;
	case TSM_HIDE: ShowWindow(hwnd, SW_HIDE); return TRUE;
	case TSM_REDRAW: InvalidateRect(hwnd, NULL, TRUE); return TRUE;

	/* FIXME: Reexamine the order of processes. */
	case TSM_STARTSEL:
		DEBUG_PRINTF((T("TSM_STARTSEL %c"), (g_Selecting?'o':'x')));

		if (g_Selecting)
			return FALSE;

		g_TaskList = EnumerateTasks(g_SieveProcedure);
		if (g_TaskList == NULL) {
			FAILURE(T("TSM_STARTSEL/EnumerateTasks"));
			return FALSE;
		}

		/**
		 * Deny to select tasks when there is no task,
		 * because it causes infinite loop.
		 */
		if (g_TaskList->count < 1) {
			TSTaskList_Del(g_TaskList);
			g_TaskList = NULL;
			return FALSE;
		}

		g_Selecting = TRUE;

		if (SHOW_HEAVY_THINGS) {
			int i;

			for (i = 0; i < g_TaskList->count; i++) {
#if USE_INVESTIGATE
				investigate(g_TaskList->list[i]->window);
#else
				DEBUG_PRINTF((
				  T("%s %2d %s"),
				  ( i == g_TaskList->selected_task_index
				    ? T("(*)") : T("   ") ),
				  i,
				  g_TaskList->list[i]->title
				));
#endif
			}
		}

		g_TaskList->selected_task_index
		  = get_current_task_index( g_TaskList,
		                            get_last_active_window() );
		(*g_OnStartSelecting)(TS_FROM_HWND(hwnd));
		if (g_Engine != TSEngine_NONE) {
			/* [TSEngine_HOOK_PROBLEM] */
			if (!SetForegroundWindow(hwnd))
				FAILURE(T("SetForegroundWindow"));
		}

		/**
		 * Switch automatically when there is only one task
		 * like the default one.
		 */
		if (g_TaskList->count == 1) {
			SendMessage(hwnd, TSM_SWITCH, 0, 0);
			return FALSE;
		}

		/* See [TSEngine_SIMPLE_PROBLEM]. */
		if ((g_Engine == TSEngine_SIMPLE) && !is_pressed_now(VK_MENU))
		{
			DEBUG_PRINTF((T("Missing U-Alt")));
			SendMessage(hwnd, TSM_SWITCH, 0, 0);
			return FALSE;
		}

		SendMessage(hwnd, TSM_SHOW, 0, 0);
		return TRUE;

	case TSM_ENDSEL:
		DEBUG_PRINTF((T("TSM_ENDSEL %c"), (g_Selecting ? 'o' : 'x')));

		if (!g_Selecting)
			return FALSE;

		TSTaskList_Del(g_TaskList);
		g_TaskList = NULL;
		g_Selecting = FALSE;
		SendMessage(hwnd, TSM_HIDE, 0, 0);
		return TRUE;


	/* ======== WM_XXX ======== */

	/* -------- WM_XXX (to check key operations) -------- */

	case WM_HOTKEY:
		switch (wp) {
		default:
			break;

		case IDHK_NEXT:
			SendMessage(hwnd, TSM_NEXT, 0, 0);
			break;

		case IDHK_PREV:
			SendMessage(hwnd, TSM_PREV, 0, 0);
			break;
		}
		break;

	case WM_ACTIVATE:  /* Inactivated -- cancel */
		if (wp == WA_INACTIVE) {
			if ((g_Engine != TSEngine_NONE) && g_Selecting) {
				DEBUG_PRINTF((T("WM_ACTIVATE (WA_INACTIVE)")));
				SendMessage(hwnd, TSM_CANCEL, 0, 0);
			}
		}
		break;

	case WM_KILLFOCUS:  /* Lost focus -- cancel */
		if ((g_Engine != TSEngine_NONE) && g_Selecting) {
			DEBUG_PRINTF((T("WM_KILLFOCUS")));
			SendMessage(hwnd, TSM_CANCEL, 0, 0);
		}
		break;

	case WM_KEYDOWN:  /* A-NonAltNonShiftNonTab -- cancel */
		DEBUG_PRINTF((T("WM_KEYDOWN (%02X)"), wp));
		if ( (g_Engine == TSEngine_SIMPLE)
		     && g_Selecting
		     && (wp != g_HotKey)
		     && (!is_alt_vk(wp)) && (!is_shift_vk(wp)) )
		{
			SendMessage(hwnd, TSM_CANCEL, 0, 0);
		}
		break;

	case WM_SYSKEYDOWN:  /* A-NonAltNonShiftNonTab -- cancel */
		DEBUG_PRINTF((T("WM_SYSKEYDOWN (%02X)"), wp));
		if ( (g_Engine == TSEngine_SIMPLE)
		     && g_Selecting
		     && (wp != g_HotKey)
		     && (!is_alt_vk(wp)) && (!is_shift_vk(wp)) )
		{
			SendMessage(hwnd, TSM_CANCEL, 0, 0);
		}
		break;

	case WM_KEYUP:  /* U-Alt -- switch */
		DEBUG_PRINTF((T("WM_KEYUP (%02X)"), wp));
		if ( (g_Engine == TSEngine_SIMPLE)
		     && g_Selecting
		     && is_alt_vk(wp) )
		{
			SendMessage(hwnd, TSM_SWITCH, 0, 0);
		}
		break;

	case WM_SYSKEYUP: /* U-Alt -- switch */
		DEBUG_PRINTF((T("WM_SYSKEYUP (%02X)"), wp));
		if ( (g_Engine == TSEngine_SIMPLE)
		     && g_Selecting
		     && is_alt_vk(wp) )
		{
			SendMessage(hwnd, TSM_SWITCH, 0, 0);
		}
		break;

	/* -------- WM_XXX (Misc.) -------- */

	case WM_CREATE: {
		BOOL r;

		r = ( SetProp(hwnd, TSKey_OnStartSelecting,&g_OnStartSelecting)
		      && SetProp(hwnd, TSKey_OnPaint, &g_OnPaint)
		      && SetProp(hwnd, TSKey_OnCancel, &g_OnCancel)
		      && SetProp(hwnd, TSKey_OnSwitch, &g_OnSwitch)
		      && SetProp(hwnd, TSKey_SieveProcedure, &g_SieveProcedure)
		      && SetProp(hwnd, TSKey_Window, &g_TSWindow)
		      && SetProp(hwnd, TSKey_Activeness, &g_Activeness)
		      && SetProp(hwnd, TSKey_Selecting, &g_Selecting)
		      && SetProp(hwnd, TSKey_TaskList, &g_TaskList)
		      && SetProp(hwnd, TSKey_HotKey, &g_HotKey)
		      && SetProp(hwnd, TSKey_Engine, &g_Engine)
		      && SetProp(hwnd, TSKey_DebugWindow, &g_DebugWindow) );
		if (!r)
			return 1;
		g_TSWindow = hwnd;
		} break;

	case WM_DESTROY:
		g_TSWindow = NULL;
		if (g_Activeness)
			TS_Inactivate(TS_FROM_HWND(hwnd));
		TS_UseDebugWindow(TS_FROM_HWND(hwnd), FALSE);
		RemoveAllProps(hwnd);
		break;

	case WM_PAINT:
		(*g_OnPaint)(TS_FROM_HWND(hwnd));
		break;
	}

	return DefWindowProc(hwnd, msg, wp, lp);
}




TS_PUBLIC(LRESULT CALLBACK)
TS_DefaultOnStartSelecting(TS* ts)
{
	HWND hwnd = HWND_FROM_TS(ts);
	HDC hdc;
	RECT rect;
	int max_width;
	int max_height;
	int screen_width;
	int screen_height;
	int i;

	hdc = GetDC(hwnd);
	if (hdc == NULL) {
		FAILURE(T("TS_DefaultOnStartSelecting/GetDC"));
		return !0;
	}

	ZeroMemory(&rect, sizeof(rect));
	max_height = 2;
	max_width = 2;
	for (i = 0; i < g_TaskList->count; i++) {
		DrawText( hdc, g_TaskList->list[i]->title, -1, &rect,
			  DT_CALCRECT | DT_NOPREFIX | DT_SINGLELINE );
		if (max_height < rect.bottom)
			max_height = rect.bottom;
		if (max_width < rect.right)
			max_width = rect.right;
	}
	resize( hwnd,
		2 + max_width + 2,
		2 + ((1+max_height+1) * g_TaskList->count) + 2 );

	screen_width = get_screen_width();
	screen_height = get_screen_height();
	GetClientRect(hwnd, &rect);
	move( hwnd,
	      screen_width / 2 - rect.right / 2,
	      screen_height / 2 - rect.bottom / 2 );

	ReleaseDC(hwnd, hdc);

	return 0;
}


TS_PUBLIC(LRESULT CALLBACK)
TS_DefaultOnPaint(TS* ts)
{
	HWND hwnd = HWND_FROM_TS(ts);
	PAINTSTRUCT ps;
	HDC hdc;
	RECT rect;
	int i;
	int height;
	COLORREF normal_text = GetSysColor(COLOR_WINDOWTEXT);
	COLORREF hilite_text = GetSysColor(COLOR_HIGHLIGHTTEXT);

	hdc = BeginPaint(hwnd, &ps);

	GetClientRect(hwnd, &rect);
	FillRect(hdc, &rect, GetSysColorBrush(COLOR_WINDOW));
	FrameRect(hdc, &rect, GetSysColorBrush(COLOR_WINDOWFRAME));
	height = (rect.bottom - 2 - 2) / g_TaskList->count - 1 - 1;
	rect.left += 2;
	rect.top += 2;
	rect.right -= 2;
	rect.bottom -= 2;

	SetTextColor(hdc, normal_text);
	SetBkMode(hdc, TRANSPARENT);
	for (i = 0; i < g_TaskList->count; i++) {
		rect.top += 1;
		if (i == g_TaskList->selected_task_index) {
			RECT r = rect;

			SetTextColor(hdc, hilite_text);
			r.bottom = rect.top + height;
			FillRect(hdc, &r, GetSysColorBrush(COLOR_HIGHLIGHT));
		}
		DrawText( hdc, g_TaskList->list[i]->title, -1, &rect,
			  DT_NOPREFIX | DT_SINGLELINE );
		if (i == g_TaskList->selected_task_index)
			SetTextColor(hdc, normal_text);
		rect.top += height + 1;
	}
	EndPaint(hwnd, &ps);

	return 0;
}


TS_PUBLIC(LRESULT CALLBACK)
TS_DefaultOnSwitch(TS* ts)
{
	HWND target;

	target = g_TaskList->list[g_TaskList->selected_task_index]->window;

	if (IsIconic(target))
		SendMessage(target, WM_SYSCOMMAND, SC_RESTORE, 0);
	SetForegroundWindow(target);

	return 0;
}


TS_PUBLIC(LRESULT CALLBACK)
TS_DefaultOnCancel(TS* ts)
{
	return 0;
}


static HWND
get_root_owner(HWND hwnd)
{
	HWND o;
	HWND p;

	o = hwnd;
	do {
		p = o;
		o = GetWindow(p, GW_OWNER);
	} while (o);

	return p;
}

TS_PUBLIC(BOOL CALLBACK)
TS_DefaultSieveProcedure(HWND hwnd, TSTaskList* tasklist)
{
	HWND root_owner;
	DWORD style;
	DWORD exstyle;
	int i;
	ULONG_PTR extra[1];

	hwnd = GetLastActivePopup(hwnd);

	style = GetWindowLongPtr(hwnd, GWL_STYLE);
	if (!(style & WS_VISIBLE) || (style & WS_DISABLED))
		return TRUE;

	exstyle = GetWindowLongPtr(hwnd, GWL_EXSTYLE);
	if (!( (!(exstyle & WS_EX_TOOLWINDOW))
	       || (exstyle & WS_EX_APPWINDOW) ))
	{
		return TRUE;
	}

	root_owner = get_root_owner(hwnd);

	for (i = 0; i < tasklist->count; i++) {
		if (root_owner == (HWND)(tasklist->list[i]->extra[0])) {
			tasklist->list[i]->window = hwnd;
			GetWindowText( hwnd,
			               tasklist->list[i]->title,
			               NUMBER_OF(tasklist->list[i]->title) );
			tasklist->list[i]->extra[0] = (ULONG_PTR)root_owner;
			return TRUE;
		}
	}

	extra[0] = (ULONG_PTR)root_owner;
	return TSTaskList_Append(tasklist, hwnd, extra, 1);
}


#define IS_PROPER_WINDOW(hwnd, style) \
            ( (!(style & WS_DISABLED)) \
              && ( ((style & WS_POPUP) && (1 <= GetWindowTextLength(hwnd))) \
                   || (style & WS_SYSMENU) ) \
              && (style & WS_VISIBLE) )

TS_PUBLIC(BOOL CALLBACK)
TS_OldSieveProcedure(HWND hwnd, TSTaskList* tasklist)
{
	HWND parent;

	parent = GetParent(hwnd);

	if (parent == NULL) {
		DWORD hwnd_style;
		HWND lapopup;
		DWORD lapopup_style;
		int i;

		hwnd_style = GetWindowLongPtr(hwnd, GWL_STYLE);

		lapopup = GetLastActivePopup(hwnd);
		lapopup_style = GetWindowLongPtr(lapopup, GWL_STYLE);

		if ( (!IS_PROPER_WINDOW(lapopup, lapopup_style))
		     && (!IS_PROPER_WINDOW(hwnd, hwnd_style)) )
		{
			return TRUE;
		}

		for (i = 0; i < tasklist->count; i++) {
			if (lapopup == tasklist->list[i]->window)
				return TRUE;
		}

		return TSTaskList_Append(tasklist, lapopup, NULL, 0);
	}

	return TRUE;
}








/* --------------------------------------------------------- */
/* Public functions */

TS_PUBLIC(TS*)
TS_New(HINSTANCE hinstance)
{
	WNDCLASS wc;
	HWND hwnd;

	if (g_TSWindow != NULL)
		goto E_AlreadyExists;

	ZeroMemory(&wc, sizeof(wc));
	wc.lpfnWndProc = TS_WindowProcedure;
	wc.hInstance = hinstance;
	wc.lpszClassName = TS_CLASS_NAME;
	if (!RegisterClass(&wc))
		goto E_RegisterClass;

	hwnd = CreateWindowEx( WS_EX_TOOLWINDOW | WS_EX_TOPMOST,
	                       TS_CLASS_NAME,
	                       TS_WINDOW_NAME,
	                       WS_POPUP,
	                       0, 0, 0, 0,
	                       NULL,
	                       NULL,
	                       hinstance,
	                       NULL );
	if (hwnd == NULL)
		goto E_CreateWindowEx;

	return TS_FROM_HWND(hwnd);

E_CreateWindowEx:
	UnregisterClass(TS_CLASS_NAME, hinstance);
E_RegisterClass:
E_AlreadyExists:
	return NULL;
}


TS_PUBLIC(void)
TS_Del(TS* self, HINSTANCE hinstance)
{
	DestroyWindow(HWND_FROM_TS(self));
	UnregisterClass(TS_CLASS_NAME, hinstance);
}




TS_PUBLIC(void*)
TS_GetAttr(TS* self, LPCTSTR key)
{
	return GetProp(HWND_FROM_TS(self), key);
}


TS_PUBLIC(BOOL)
TS_SetAttr(TS* self, LPCTSTR key, void* value)
{
	void** pv;
	BOOL activeness;
	BOOL result;

	activeness = g_Activeness;
	if (activeness)
		TS_Inactivate(self);

	pv = (void**)GetProp(HWND_FROM_TS(self), key);
	if (pv == NULL)
		result = FALSE;
	else {
		*pv = value;
		result = TRUE;
	}

	if (activeness) {
		return TS_Activate(self) && result;
	} else {
		return result;
	}
}


TS_PUBLIC(LRESULT)
TS_Control(TS* self, UINT msg)
{
	return SendMessage(HWND_FROM_TS(self), msg, 0, 0);
}




TS_PUBLIC(BOOL)
TS_ChangeEngine(TS* self, TSEngine engine)
{
	BOOL activeness;

	activeness = g_Activeness;
	if (activeness)
		TS_Inactivate(self);

	g_Engine = engine;

	if (activeness) {
		return TS_Activate(self) && TRUE;
	} else {
		return TRUE;
	}
}


TS_PUBLIC(BOOL)
TS_ChangeHotKey(TS* self, UINT hotkey)
{
	BOOL activeness;

	activeness = g_Activeness;
	if (activeness)
		TS_Inactivate(self);

	g_HotKey = hotkey;

	if (activeness) {
		return TS_Activate(self) && TRUE;
	} else {
		return TRUE;
	}
}


TS_PUBLIC(BOOL)
TS_UseDebugWindow(TS* self, BOOL flag)
{
	BOOL activeness;
	BOOL result;

	if (flag == (g_DebugWindow != NULL))
		return TRUE;

	activeness = g_Activeness;
	if (activeness)
		TS_Inactivate(self);

	if (flag) {
		HINSTANCE hinstance;

		hinstance = (HINSTANCE)GetWindowLongPtr( HWND_FROM_TS(self),
		                                         GWLP_HINSTANCE );
		g_DebugWindow = DebugWnd_New(hinstance);
		result = (g_DebugWindow != NULL);
	} else {
		DebugWnd_Del(g_DebugWindow);
		g_DebugWindow = NULL;
		result = TRUE;
	}

	if (activeness) {
		return TS_Activate(self) && result;
	} else {
		return result;
	}
}








TS_PUBLIC(int)
TSTaskList_GetTaskCount(const TSTaskList* self)
{
	return self->count;
}


TS_PUBLIC(int)
TSTaskList_GetSelectedTaskIndex(const TSTaskList* self)
{
	return self->selected_task_index;
}


TS_PUBLIC(HWND)
TSTaskList_GetWindow(const TSTaskList* self, int index)
{
	return self->list[index]->window;
}


TS_PUBLIC(LPCTSTR)
TSTaskList_GetTitle(const TSTaskList* self, int index)
{
	return self->list[index]->title;
}


TS_PUBLIC(ULONG_PTR*)
TSTaskList_GetExtra(const TSTaskList* self, int index)
{
	return self->list[index]->extra;
}




/* __END__ */
