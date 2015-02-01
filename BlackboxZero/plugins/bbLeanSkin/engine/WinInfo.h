#pragma once
#include <wtypes.h>
#include "hookinfo.h"

int const NUMOFGDIOBJS = 12+1; // 12 bitmaps, 1 font
#define hFont gdiobjs[12].hObj

struct SizeInfo
{
    int width, height;
    int HiddenTop;
    int HiddenBottom;
    int HiddenSide;
    int BottomAdjust;
    int BottomHeight;
    RECT rcClient;
};

struct GdiInfo
{
    int cx, cy;
    HGDIOBJ hObj;
};

struct button_set
{
    int set, pos;
};

struct WinInfo
{
    HMODULE hModule;
    HWND hwnd;
    LRESULT (WINAPI *pCallWindowProc) (WNDPROC,HWND,UINT,WPARAM,LPARAM);
    WNDPROC wpOrigWindowProc;
    LONG_PTR style, exstyle;

    SizeInfo S;
    GdiInfo gdiobjs[NUMOFGDIOBJS];
    HDC buf;

    bool is_unicode;
    bool apply_skin;

    bool in_set_region;
    bool dont_paint;
    bool sync_paint;

    bool is_active;
    bool is_active_app;
    bool is_zoomed;
    bool is_iconic;
    bool is_moving;

    bool is_rolled;
    bool is_ontop;
    bool is_sticky;
    bool has_sticky;

    bool is_onbg;
    bool has_onbg;

    bool dblclk_timer_set;

    char capture_button;
    char button_down;
    char button_count;
    button_set button_set[BUTTON_COUNT];
};

enum button_types {
    btn_None      = 0,

    btn_Close     = 1,
    btn_Max       = 2,
    btn_Min       = 3,
    btn_Rollup    = 4,
    btn_OnTop     = 5,
    btn_Sticky    = 6,
    btn_Icon      = 7,
	btn_OnBG      = 8,

    btn_Lower     ,
    btn_TMin      ,

    btn_VMax      ,
    btn_HMax      ,

    btn_Caption   ,
    btn_Nowhere   ,

    btn_Topleft   ,
    btn_Topright  ,
    btn_Top       ,

    btn_SysMenu
};
    
button_types const btn_Last = btn_HMax;

