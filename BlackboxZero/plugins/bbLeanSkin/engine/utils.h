#pragma once
#include <BBApi.h>
#include <WinUser.h>
#include "hookinfo.h"

struct WinInfo;

void get_workarea (HWND hwnd, RECT * w, RECT * s);
void get_rect (HWND hwnd, RECT *rp);
HWND GetRootWindow (HWND hwnd);
void send_log (HWND, const char *msg);
void window_set_pos (HWND hwnd, RECT rc);
int get_shade_height (HWND hwnd);
void ShadeWindow (HWND hwnd);
void ToggleShadeWindow (HWND hwnd);
bool get_rolled (WinInfo *WI);
void post_redraw (HWND hwnd);
HWND GetRootWindow (HWND hwnd);
int get_module (HWND hwnd, char * buffer, int buffsize);
char * sprint_window (char *buffer, size_t max_ln, HWND hwnd, const char *msg);

//===========================================================================
// Function: SnapWindowToEdge
// Purpose:Snaps a given windowpos at a specified distance
// In: WINDOWPOS* = WINDOWPOS recieved from WM_WINDOWPOSCHANGING
// In: int = distance to snap to
// In: bool = use screensize of workspace
// Out: void = none
//===========================================================================
void SnapWindowToEdge (WinInfo * WI, WINDOWPOS * pwPos, int nDist);

inline WinInfo * get_WinInfo (HWND hwnd)
{
	return static_cast<WinInfo *>(GetProp(hwnd, BBLEANSKIN_INFOPROP));
}
inline void set_WinInfo (HWND hwnd, WinInfo * wi)
{
	SetProp(hwnd, BBLEANSKIN_INFOPROP, wi);
}
inline void del_WinInfo (HWND hwnd)
{
	RemoveProp(hwnd, BBLEANSKIN_INFOPROP);
}



