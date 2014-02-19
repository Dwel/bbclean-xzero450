/* BBTaskSwitch - *box-styled Alt-Tab task-switching window replacement
   Copyright (C) 2004-2005 kana <nicht AT s8 DOT xrea DOT con>

This program is released under the terms of the GNU General Public License
as published by the Free Software Foundation;
either version 2 of the License, or (at your option) any later version.
*/

#include <stdlib.h>

#define WIN32_LEAN_AND_MEAN
#include <BBApiEx.h>
#include <windows.h>

#include <tchar.h>

#include "cpp_adaptor.h"
#include "misc.h"
#include "ts.h"




typedef BOOL ModuleInitialize_f(HINSTANCE, HMODULE, HMODULE);
typedef BOOL BroamHandler(LPCTSTR);




static const TCHAR BBTS_NAME[] = T("BBTaskSwitch");
static const TCHAR BBTS_VERSION[] = T("0.1.1");
static const TCHAR BBTS_AUTHOR[] = T("kana");
static const TCHAR BBTS_RELEASE[] = T("2004-07-28/2005-03-02");
static const TCHAR BBTS_LINK[]
                     = T("http://nicht.s8.xrea.com/2004/07/28/BBTaskSwitch");
static const TCHAR BBTS_EMAIL[] = T("nicht AT s8 DOT xrea DOT con");

static const UINT BBTS_MSGS[] = {BB_BROADCAST, BB_RECONFIGURE};

static const TCHAR BBTS_CLASS_NAME[] = T("BBTaskSwitch");
#define BBTS_WINDOW_NAME NULL

static const TCHAR BBTS_RC_NAME[] = T("BBTaskSwitch.rc");




static HWND g_PluginWindow = NULL;
static HMODULE g_PluginModule = NULL;
static TS* g_TS = NULL;
static HMODULE g_TSModule = NULL;

static TS_New_f* pTS_New;
static TS_Del_f* pTS_Del;
static TS_Control_f* pTS_Control;
static TS_GetAttr_f* pTS_GetAttr;
static TS_SetAttr_f* pTS_SetAttr;
static TS_ChangeEngine_f* pTS_ChangeEngine;
static TS_ChangeHotKey_f* pTS_ChangeHotKey;
static TS_UseDebugWindow_f* pTS_UseDebugWindow;
static TSTaskList_GetSelectedTaskIndex_f* pTSTaskList_GetSelectedTaskIndex;
static TSTaskList_GetWindow_f* pTSTaskList_GetWindow;


static BroamHandler* pBroamHandler = NULL;
static HMODULE g_Module_OnBroam = NULL;

static HMODULE g_Module_OnStartSelecting = NULL;
static HMODULE g_Module_OnPaint = NULL;
static HMODULE g_Module_OnSwitch = NULL;
static HMODULE g_Module_OnCancel = NULL;
static HMODULE g_Module_SieveProcedure = NULL;








/* --------------------------------------------------------- */
/* Misc. functions (weakly-related to BBTaskSwitch) */

#define FAILURE(s) notice_with_last_error(BBTS_NAME, (s))

#define tokenize_check(min,max,allow_rest,token_num,rest,broam) \
        tokenize_check( BBTS_NAME, \
                        (min), (max), (allow_rest), \
                        (token_num), (rest), (broam) )




static BOOL
load_ts_library(void)
{
	TCHAR path[MAX_PATH];
	#define LOAD(F) \
	        if ((p##F = (F##_f*)GetProcAddress(g_TSModule, #F)) != NULL) \
		  ; \
		else \
		  goto E_LOAD

	if (!get_file_path(path, NUMBER_OF(path), T("ts.dll"), g_PluginModule))
		goto E_get_file_path;

	g_TSModule = LoadLibrary(path);
	if (g_TSModule == NULL)
		goto E_LoadLibrary;

	LOAD(TS_New);
	LOAD(TS_Del);
	LOAD(TS_Control);
	LOAD(TS_GetAttr);
	LOAD(TS_SetAttr);
	LOAD(TS_ChangeEngine);
	LOAD(TS_ChangeHotKey);
	LOAD(TS_UseDebugWindow);
	LOAD(TSTaskList_GetSelectedTaskIndex);
	LOAD(TSTaskList_GetWindow);

	return TRUE;

E_LOAD:
	FreeLibrary(g_TSModule);
	g_TSModule = NULL;
E_LoadLibrary:
E_get_file_path:
	return FALSE;
	#undef LOAD
}


static void
unload_ts_library(void)
{
	FreeLibrary(g_TSModule);
	g_TSModule = NULL;
	return;
}




static BOOL
read_settings(void)
{
	TCHAR rc[MAX_PATH];
	FILE* file;
	TCHAR line[256];
	TCHAR* nl;

	if (!get_file_path(rc, NUMBER_OF(rc), BBTS_RC_NAME, g_PluginModule)) {
		notice( BBTS_NAME,
		        T("Failed to load %s (too long path)"), BBTS_RC_NAME );
		return FALSE;
	}

	file = _tfopen(rc, T("rt"));
	if (file == NULL)
		return TRUE;

	while (_fgetts(line, NUMBER_OF(line), file) != NULL) {
		if (line[0] != T('@'))
			continue;

		nl = _tcschr(line, T('\n'));
		if (nl != NULL)
			*nl = T('\0');

		SendMessage(GetBBWnd(), BB_BROADCAST, 0, (LPARAM)line);
	}

	fclose(file);
	return TRUE;
}




static void
load_function(LPCTSTR key, LPCTSTR rest, LPCTSTR broam, HMODULE* phmodule)
{
	Token token[2];
	int token_num;
	TCHAR tmp[MAX_PATH];
	TCHAR path[MAX_PATH];
	TCHAR name[MAX_PATH];
	HMODULE hmodule;
	FARPROC function;
	ModuleInitialize_f* init;

	tokenize(rest, 2, token, &token_num, &rest);
	if (!tokenize_check(2, 2, FALSE, token_num, rest, broam))
		goto E_tokenize_check;

	my_strncpy( tmp, token[0].string,
	            MIN(NUMBER_OF(tmp), token[0].length+1) );
	my_strncpy( name, token[1].string,
	            MIN(NUMBER_OF(name), token[1].length+1) );

	if (!strncmp(tmp, T("<>\\"), 3)) {
		if (!get_file_path(path,NUMBER_OF(path),tmp+3,g_PluginModule))
			goto E_get_file_path;
	} else {
		my_strncpy(path, tmp, NUMBER_OF(path));
	}

	hmodule = LoadLibrary(path);
	if (hmodule == NULL)
		goto E_LoadLibrary;

	function = GetProcAddress(hmodule, name);  /* FIXME: arg 2 - LPCSTR */
	if (function == NULL)
		goto E_GetProcAddress;

	init = (ModuleInitialize_f*)GetProcAddress( hmodule,
	                                            "ModuleInitialize" );
	if (init != NULL) {
		if (!((*init)(hmodule, g_PluginModule, g_TSModule)))
			goto E_ModuleInitialize;
	}

	if (key != NULL) {
		if (!(*pTS_SetAttr)(g_TS, key, (void*)function))
			goto E_TS_SetAttr;
	} else {
		pBroamHandler = (BroamHandler*)function;
	}

	my_FreeLibrary(*phmodule);
	*phmodule = hmodule;
	return;

E_TS_SetAttr:
E_ModuleInitialize:
E_GetProcAddress:
	FreeLibrary(hmodule);
E_LoadLibrary:
E_get_file_path:
	FAILURE(broam);
E_tokenize_check:
	return;
}








/* --------------------------------------------------------- */
/* Misc. functions (strong-related to BBTaskSwitch) */

DLL_EXPORT LRESULT CALLBACK
BBTS_OnSwitch(TS* ts)
{
	const TSTaskList* TASKLIST
		= *(TSTaskList**)((*pTS_GetAttr)(ts, TSKey_TaskList));
	const int i = (*pTSTaskList_GetSelectedTaskIndex)(TASKLIST);

	SendMessage( GetBBWnd(), BB_BRINGTOFRONT, 0,
	             (LPARAM)(*pTSTaskList_GetWindow)(TASKLIST, i) );
	return TRUE;
}




static void
interpret_broam(LPCTSTR broam)
{
	Token token[8];
	int token_num;
	LPCTSTR rest;
	Token cmd;

	if (pBroamHandler != NULL) {
		if ((*pBroamHandler)(broam))
			return;
	}

	/* 0:@BBTaskSwitch, 1:CMD, rest:ARG(S) */
	tokenize(broam, 2, token, &token_num, &rest);
	if (token_num < 2)
		return;
	if (my_strncmp(T("@BBTaskSwitch"), token[0].string, token[0].length))
		return;

	cmd = token[1];
	if (!my_strncmp(T("Activate"), cmd.string, cmd.length)) {
		tokenize(rest, 0, token, &token_num, &rest);
		if (!tokenize_check(0, 0, FALSE, token_num, rest, broam))
			return;

		if (!((*pTS_Control)(g_TS, TSM_ACTIVATE)))
			FAILURE(broam);
	} else if (!my_strncmp(T("Inactivate"), cmd.string, cmd.length)) {
		tokenize(rest, 0, token, &token_num, &rest);
		if (!tokenize_check(0, 0, FALSE, token_num, rest, broam))
			return;

		if (!((*pTS_Control)(g_TS, TSM_INACTIVATE)))
			FAILURE(broam);
	} else if (!my_strncmp(T("Next"), cmd.string, cmd.length)) {
		tokenize(rest, 0, token, &token_num, &rest);
		if (!tokenize_check(0, 0, FALSE, token_num, rest, broam))
			return;
		(*pTS_Control)(g_TS, TSM_NEXT);
	} else if (!my_strncmp(T("Prev"), cmd.string, cmd.length)) {
		tokenize(rest, 0, token, &token_num, &rest);
		if (!tokenize_check(0, 0, FALSE, token_num, rest, broam))
			return;
		(*pTS_Control)(g_TS, TSM_PREV);
	} else if (!my_strncmp(T("Switch"), cmd.string, cmd.length)) {
		tokenize(rest, 0, token, &token_num, &rest);
		if (!tokenize_check(0, 0, FALSE, token_num, rest, broam))
			return;
		(*pTS_Control)(g_TS, TSM_SWITCH);
	} else if (!my_strncmp(T("Cancel"), cmd.string, cmd.length)) {
		tokenize(rest, 0, token, &token_num, &rest);
		if (!tokenize_check(0, 0, FALSE, token_num, rest, broam))
			return;
		(*pTS_Control)(g_TS, TSM_CANCEL);
	} else if (!my_strncmp(T("ChangeEngine"), cmd.string, cmd.length)) {
		TSEngine engine;
		Token t;

		tokenize(rest, 1, token, &token_num, &rest);
		if (!tokenize_check(1, 1, FALSE, token_num, rest, broam))
			return;

		t = token[0];
		if (!my_strncmp(T("None"), t.string, t.length))
			engine = TSEngine_NONE;
		else if (!my_strncmp(T("Simple"), t.string, t.length))
			engine = TSEngine_SIMPLE;
		else if (!my_strncmp(T("Hook"), t.string, t.length))
			engine = TSEngine_HOOK;
		else {
			broam_error( BBTS_NAME, broam,
			             T("Engine must be None, Simple or Hook"));
			return;
		}

		if (!((*pTS_ChangeEngine)(g_TS, engine)))
			FAILURE(broam);
	} else if (!my_strncmp(T("ChangeHotKey"), cmd.string, cmd.length)) {
		TCHAR tmp[16];
		int hotkey;

		tokenize(rest, 1, token, &token_num, &rest);
		if (!tokenize_check(1, 1, FALSE, token_num, rest, broam))
			return;

		my_strncpy( tmp, token[0].string,
		            MIN(NUMBER_OF(tmp), token[0].length+1) );
		hotkey = strtol(tmp, NULL, 0);
		if ((hotkey < 0x00) || (0xFF < hotkey)) {
			broam_error( BBTS_NAME, broam,
			             T("Key code must be in [0x00, 0xFF]") );
			return;
		}

		if (!((*pTS_ChangeHotKey)(g_TS, hotkey)))
			FAILURE(broam);
	} else if (!my_strncmp(T("UseDebugWindow"), cmd.string, cmd.length)) {
		tokenize(rest, 1, token, &token_num, &rest);
		if (!tokenize_check(1, 1, FALSE, token_num, rest, broam))
			return;

		if (!((*pTS_UseDebugWindow)(g_TS, BOOL_FromToken(token[0]))))
			FAILURE(broam);
	} else if (!my_strncmp(T("OnBroam"), cmd.string,cmd.length)) {
		load_function(NULL, rest, broam, &g_Module_OnBroam);
	} else if (!my_strncmp(T("OnStartSelecting"), cmd.string,cmd.length)) {
		load_function( TSKey_OnStartSelecting, rest, broam,
		               &g_Module_OnStartSelecting );
	} else if (!my_strncmp(T("OnPaint"), cmd.string, cmd.length)) {
		load_function(TSKey_OnPaint, rest, broam, &g_Module_OnPaint);
	} else if (!my_strncmp(T("OnCancel"), cmd.string, cmd.length)) {
		load_function(TSKey_OnCancel, rest, broam, &g_Module_OnCancel);
	} else if (!my_strncmp(T("OnSwitch"), cmd.string, cmd.length)) {
		load_function(TSKey_OnSwitch, rest, broam, &g_Module_OnSwitch);
	} else if (!my_strncmp(T("SieveProcedure"), cmd.string, cmd.length)) {
		load_function( TSKey_SieveProcedure, rest, broam,
		               &g_Module_SieveProcedure );
	} else if (!strncmp(T("mod_"), cmd.string, MIN(cmd.length, 4))) {
		/* Ignore */
	} else {
		broam_error(BBTS_NAME, broam, T("Unknown bro@m"));
	}
}




static LRESULT CALLBACK
BBTS_WindowProcedure(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp)
{
	switch (msg) {
	default:
		break;

	case BB_BROADCAST:
		interpret_broam((LPCTSTR)lp);
		break;

	case BB_RECONFIGURE:
		update_screen_rect();
		read_settings();
		break;

	case WM_CREATE:
		SendMessage( GetBBWnd(), BB_REGISTERMESSAGE,
		             (WPARAM)hwnd, (LPARAM)BBTS_MSGS );
		SendMessage(hwnd, BB_RECONFIGURE, 0, 0);
		break;

	case WM_DESTROY:
		SendMessage( GetBBWnd(), BB_UNREGISTERMESSAGE,
		             (WPARAM)hwnd, (LPARAM)BBTS_MSGS );
		break;
	}

	return DefWindowProc(hwnd, msg, wp, lp);
}








/* --------------------------------------------------------- */
/* Public functions */


DLL_EXPORT int
beginPlugin(HINSTANCE hplugininstance)
{
	WNDCLASS wc;

	g_PluginModule = hplugininstance;

	if (!misc_begin()) {
		FAILURE(T("misc_begin"));
		goto E_misc_begin;
	}

	if (!load_ts_library()) {
		FAILURE(T("load_ts_library"));
		goto E_load_ts_library;
	}

	g_TS = (*pTS_New)(g_TSModule);
	if (g_TS == NULL) {
		FAILURE(T("TS_New"));
		goto E_TS_New;
	}
	(*pTS_SetAttr)(g_TS, TSKey_OnSwitch, (void*)BBTS_OnSwitch);

	ZeroMemory(&wc, sizeof(wc));
	wc.lpfnWndProc = BBTS_WindowProcedure;
	wc.hInstance = hplugininstance;
	wc.lpszClassName = BBTS_CLASS_NAME;
	if (!RegisterClass(&wc)) {
		FAILURE(T("RegisterClass"));
		goto E_RegisterClass;
	}

	g_PluginWindow = CreateWindowEx( WS_EX_TOOLWINDOW,
	                                 BBTS_CLASS_NAME,
	                                 BBTS_WINDOW_NAME,
	                                 0,
	                                 0, 0, 0, 0,
	                                 (is_2k_or_later()?HWND_MESSAGE:NULL),
	                                 NULL,
	                                 hplugininstance,
	                                 NULL );
	if (g_PluginWindow == NULL) {
		FAILURE(T("CreateWindowEx"));
		goto E_CreateWindowEx;
	}

	return 0;


E_CreateWindowEx:
	UnregisterClass(BBTS_CLASS_NAME, hplugininstance);
E_RegisterClass:
	(*pTS_Del)(g_TS, g_TSModule);
E_TS_New:
	unload_ts_library();
E_load_ts_library:
	misc_end();
E_misc_begin:
	return !0;
}




DLL_EXPORT void
endPlugin(HINSTANCE hplugininstance)
{
	DestroyWindow(g_PluginWindow);
	UnregisterClass(BBTS_CLASS_NAME, hplugininstance);
	(*pTS_Del)(g_TS, g_TSModule);
	unload_ts_library();
	misc_end();

	my_FreeLibrary(g_Module_SieveProcedure);
	my_FreeLibrary(g_Module_OnCancel);
	my_FreeLibrary(g_Module_OnSwitch);
	my_FreeLibrary(g_Module_OnPaint);
	my_FreeLibrary(g_Module_OnStartSelecting);
	my_FreeLibrary(g_Module_OnBroam);
}




DLL_EXPORT LPCSTR
pluginInfo(int field)
{
	switch (field) {
	default: /* FALLTHRU */
	case PLUGIN_NAME: return BBTS_NAME;
	case PLUGIN_VERSION: return BBTS_VERSION;
	case PLUGIN_AUTHOR: return BBTS_AUTHOR;
	case PLUGIN_RELEASE: return BBTS_RELEASE;
	case PLUGIN_LINK: return BBTS_LINK;
	case PLUGIN_EMAIL: return BBTS_EMAIL;
	}
}




/* __END__ */
