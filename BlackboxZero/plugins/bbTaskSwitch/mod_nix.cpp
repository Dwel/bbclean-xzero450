/* BBTaskSwitch module - same appearance as bbkeys on *nix */

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <tchar.h>

#include <BBApiEx.h>

#include "cpp_adaptor.h"
#include "misc.h"
#include "ts.h"




#define MOD_NAME T("mod_nix")




static resize_f* presize;
static move_f* pmove;
static get_screen_rect_f* pget_screen_rect;
static my_strncmp_f* pmy_strncmp;
static tokenize_f* ptokenize;
static Padding_FromTokens_f* pPadding_FromTokens;
static StyleNumber_FromToken_f* pStyleNumber_FromToken;
static BOOL_FromToken_f* pBOOL_FromToken;
static notice_with_last_error_f* pnotice_with_last_error;
static tokenize_check_f* ptokenize_check;
static Cpp_MakeStyleGradient_f* pCpp_MakeStyleGradient;
static Cpp_StyleItem_TextColor_f* pCpp_StyleItem_TextColor;
static Cpp_CreateStyleFont_f* pCpp_CreateStyleFont;

static TS_GetAttr_f* pTS_GetAttr;
static TSTaskList_GetTaskCount_f* pTSTaskList_GetTaskCount;
static TSTaskList_GetSelectedTaskIndex_f* pTSTaskList_GetSelectedTaskIndex;
static TSTaskList_GetTitle_f* pTSTaskList_GetTitle;
static TSTaskList_GetWindow_f* pTSTaskList_GetWindow;


static Padding g_BasePadding = {0, 0, 0, 0};
static int g_BaseStyle = SN_MENUFRAME;
static BOOL g_BaseBorder = FALSE;
static Padding g_TaskPadding = {2, 13, 2, 13};
static int g_TaskStyle = SN_MENUHILITE;
static BOOL g_TaskBorder = FALSE;








DLL_EXPORT BOOL
ModuleInitialize(HINSTANCE hinstance, HMODULE bbts, HMODULE ts)
{
	#define LOAD(L, F) \
	        if ((p##F = (F##_f*)GetProcAddress(L, #F)) != NULL) \
		  ; \
		else \
		  goto E_LOAD

	LOAD(bbts, resize);
	LOAD(bbts, move);
	LOAD(bbts, get_screen_rect);
	LOAD(bbts, my_strncmp);
	LOAD(bbts, tokenize);
	LOAD(bbts, Padding_FromTokens);
	LOAD(bbts, StyleNumber_FromToken);
	LOAD(bbts, BOOL_FromToken);
	LOAD(bbts, notice_with_last_error);
	LOAD(bbts, tokenize_check);
	LOAD(bbts, Cpp_MakeStyleGradient);
	LOAD(bbts, Cpp_StyleItem_TextColor);
	LOAD(bbts, Cpp_CreateStyleFont);

	LOAD(ts, TS_GetAttr);
	LOAD(ts, TSTaskList_GetTaskCount);
	LOAD(ts, TSTaskList_GetSelectedTaskIndex);
	LOAD(ts, TSTaskList_GetWindow);
	LOAD(ts, TSTaskList_GetTitle);

	return TRUE;

E_LOAD:
	return FALSE;
	#undef LOAD
}




#define resize(wnd,w,h) (*presize)((wnd), (w), (h))
#define move(wnd,x,y) (*pmove)((wnd), (x), (y))
#define get_screen_rect(w,r) (*pget_screen_rect)((w), (r))
#define my_strncmp(sz,nsz,n) (*pmy_strncmp)((sz),(nsz),(n))
#define tokenize(s,m,d,n,r) (*ptokenize)((s), (m), (d), (n), (r))
#define Padding_FromTokens(t,n) (*pPadding_FromTokens)((t), (n))
#define StyleNumber_FromToken(t) (*pStyleNumber_FromToken)((t))
#define BOOL_FromToken(t) (*pBOOL_FromToken)((t))
#define notice_with_last_error(t) (*pnotice_with_last_error)(MOD_NAME, (t))
#define tokenize_check(min, max, allow_rest, token_num, rest, broam) \
        (*ptokenize_check)( MOD_NAME, \
                            (min), (max), (allow_rest), \
                            (token_num), (rest), (broam) )
#define Cpp_MakeStyleGradient(d,r,s,b) \
        (*pCpp_MakeStyleGradient)((d),(r),(s),(b))
#define Cpp_StyleItem_TextColor(s) (*pCpp_StyleItem_TextColor)((s))
#define Cpp_CreateStyleFont(s) (*pCpp_CreateStyleFont)((s))

#define TS_GetAttr(ts,k) (*pTS_GetAttr)((ts),(k))
#define TSTaskList_GetTaskCount(l) (*pTSTaskList_GetTaskCount)((l))
#define TSTaskList_GetSelectedTaskIndex(l) \
        (*pTSTaskList_GetSelectedTaskIndex)((l))
#define TSTaskList_GetWindow(l,i) (*pTSTaskList_GetWindow)((l), (i))
#define TSTaskList_GetTitle(l,i) (*pTSTaskList_GetTitle)((l), (i))




DLL_EXPORT BOOL
OnBroam(LPCTSTR broam)
{
	Token token[8];
	int token_num;
	LPCTSTR rest;
	Token cmd;

	/* 0:@BBTaskSwitch, 1:mod_nix, 2:CMD, rest:ARG(S) */
	tokenize(broam, 3, token, &token_num, &rest);
	if (token_num < 3)
		return FALSE;
	if (my_strncmp(T("@BBTaskSwitch"), token[0].string, token[0].length))
		return FALSE;
	if (my_strncmp(T("mod_nix"), token[1].string, token[1].length))
		return FALSE;

	cmd = token[2];
	if (!my_strncmp(T("BasePadding"), cmd.string, cmd.length)) {
		tokenize(rest, 4, token, &token_num, &rest);
		if (!tokenize_check(1, 4, FALSE, token_num, rest, broam))
			return TRUE;
		g_BasePadding = Padding_FromTokens(token, token_num);
	} else if (!my_strncmp(T("BaseStyle"), cmd.string, cmd.length)) {
		tokenize(rest, 1, token, &token_num, &rest);
		if (!tokenize_check(1, 1, FALSE, token_num, rest, broam))
			return TRUE;
		g_BaseStyle = StyleNumber_FromToken(token[0]);
	} else if (!my_strncmp(T("BaseBorder"), cmd.string, cmd.length)) {
		tokenize(rest, 1, token, &token_num, &rest);
		if (!tokenize_check(1, 1, FALSE, token_num, rest, broam))
			return TRUE;
		g_BaseBorder = BOOL_FromToken(token[0]);
	} else if (!my_strncmp(T("TaskPadding"), cmd.string, cmd.length)) {
		tokenize(rest, 4, token, &token_num, &rest);
		if (!tokenize_check(1, 4, FALSE, token_num, rest, broam))
			return TRUE;
		g_TaskPadding = Padding_FromTokens(token, token_num);
	} else if (!my_strncmp(T("TaskStyle"), cmd.string, cmd.length)) {
		tokenize(rest, 1, token, &token_num, &rest);
		if (!tokenize_check(1, 1, FALSE, token_num, rest, broam))
			return TRUE;
		g_TaskStyle = StyleNumber_FromToken(token[0]);
	} else if (!my_strncmp(T("TaskBorder"), cmd.string, cmd.length)) {
		tokenize(rest, 1, token, &token_num, &rest);
		if (!tokenize_check(1, 1, FALSE, token_num, rest, broam))
			return TRUE;
		g_TaskBorder = BOOL_FromToken(token[0]);
	} else {
		return FALSE;
	}

	return TRUE;
}




DLL_EXPORT LRESULT CALLBACK
OnStartSelecting(TS* ts)
{
	HWND hwnd = *(HWND*)(TS_GetAttr(ts, TSKey_Window));
	HDC hdc;
	RECT rect;
	int max_width;
	int max_height;
	int width;
	int height;
	int i;
	HGDIOBJ old_font;
	void* base_style = GetSettingPtr(g_BaseStyle);
	const TSTaskList* TASKLIST
		= *(TSTaskList**)(TS_GetAttr(ts, TSKey_TaskList));
	const int TASKLIST_COUNT = TSTaskList_GetTaskCount(TASKLIST);
	const int SELECTED_TASK_INDEX
		= TSTaskList_GetSelectedTaskIndex(TASKLIST);

	hdc = GetDC(hwnd);
	if (hdc == NULL) {
		notice_with_last_error(T("OnStartSelecting/GetDC"));
		return FALSE;
	}

	old_font = SelectObject(hdc, Cpp_CreateStyleFont(base_style));

	ZeroMemory(&rect, sizeof(rect));
	max_height = 0;
	max_width = 0;
	for (i = 0; i < TASKLIST_COUNT; i++) {
		DrawText( hdc, TSTaskList_GetTitle(TASKLIST, i), -1, &rect,
			  DT_CALCRECT | DT_NOPREFIX | DT_SINGLELINE );

		if (max_height < rect.bottom)
			max_height = rect.bottom;
		if (max_width < rect.right)
			max_width = rect.right;
	}

	DeleteObject(SelectObject(hdc, old_font));
	ReleaseDC(hwnd, hdc);

	width = g_BasePadding.left
	          + (g_TaskPadding.left + max_width + g_TaskPadding.right)
	          + g_BasePadding.right;
	height = g_BasePadding.top
	           + (g_TaskPadding.top + max_height + g_TaskPadding.bottom)
	             * TASKLIST_COUNT
	           + g_BasePadding.bottom;
	resize(hwnd, width, height);

	get_screen_rect( TSTaskList_GetWindow(TASKLIST,SELECTED_TASK_INDEX),
	                 &rect );
	move( hwnd,
	      rect.left + (rect.right - rect.left) / 2 - width / 2,
	      rect.top + (rect.bottom - rect.top) / 2 - height / 2 );
	return TRUE;
}




DLL_EXPORT LRESULT CALLBACK
OnPaint(TS* ts)
{
	HWND hwnd = *(HWND*)(TS_GetAttr(ts, TSKey_Window));
	PAINTSTRUCT ps;
	HDC hdc;
	RECT rect;
	int i;
	int height;
	void* base_style = GetSettingPtr(g_BaseStyle);
	void* task_style = GetSettingPtr(g_TaskStyle);
	COLORREF normal_text = Cpp_StyleItem_TextColor(base_style);
	COLORREF hilite_text = Cpp_StyleItem_TextColor(task_style);
	HGDIOBJ old_font;
	const TSTaskList* TASKLIST
		= *(TSTaskList**)(TS_GetAttr(ts, TSKey_TaskList));
	const int TASKLIST_COUNT = TSTaskList_GetTaskCount(TASKLIST);
	const int SELECTED_TASK_INDEX
		= TSTaskList_GetSelectedTaskIndex(TASKLIST);

	hdc = BeginPaint(hwnd, &ps);

	GetClientRect(hwnd, &rect);
	Cpp_MakeStyleGradient(hdc, &rect, base_style, g_BaseBorder);

	height = rect.bottom - rect.top;
	height -= (g_BasePadding.top + g_BasePadding.bottom);
	height /= TASKLIST_COUNT;
	height -= g_TaskPadding.top + g_TaskPadding.bottom;

	rect.left += g_BasePadding.left + g_TaskPadding.left;
	rect.top += g_BasePadding.top;
	rect.right -= g_BasePadding.right + g_TaskPadding.right;
	rect.bottom -= g_BasePadding.bottom;

	old_font = SelectObject(hdc, Cpp_CreateStyleFont(base_style));
	SetTextColor(hdc, normal_text);
	SetBkMode(hdc, TRANSPARENT);
	for (i = 0; i < TASKLIST_COUNT; i++) {
		if (i == SELECTED_TASK_INDEX) {
			RECT hilite_rect = rect;

			hilite_rect.left -= g_TaskPadding.left;
			hilite_rect.right += g_TaskPadding.right;
			hilite_rect.bottom = g_TaskPadding.top
			                       + hilite_rect.top + height
			                       + g_TaskPadding.bottom;
			Cpp_MakeStyleGradient( hdc, &hilite_rect,
			                       task_style, g_TaskBorder );
			SetTextColor(hdc, hilite_text);
		}

		rect.top += g_TaskPadding.top;
		DrawText( hdc, TSTaskList_GetTitle(TASKLIST, i), -1, &rect,
			  DT_NOPREFIX | DT_SINGLELINE );
		rect.top += height + g_TaskPadding.bottom;

		if (i == SELECTED_TASK_INDEX) {
			SetTextColor(hdc, normal_text);
		}
	}
	DeleteObject(SelectObject(hdc, old_font));

	EndPaint(hwnd, &ps);
	return TRUE;
}




/* __END__ */
