/*
 ============================================================================
 bbFoomp // The foobar2000 plugin for Blackbox for Windows!
 Copyright © 2004 freeb0rn@yahoo.com

 Credits and thanks:
 qwilk, without his help and code I would have never started!
 azathoth, nc-17, tres`ni and other channel regulars, coders or not,
 thanks for bearing with my (often stupid) questions and helping me out!
 ============================================================================
*/
#pragma once
#ifndef _WIN32_WINNT
# define _WIN32_WINNT 0x0500
#endif

#define WM_XBUTTONDOWN 0x020B
#define WM_XBUTTONUP 0x020C
#define WM_NCXBUTTONDOWN 0x00AB
#define WM_NCXBUTTONUP 0x00AC
#define WM_NCXBUTTONDBLCLK 0x00AD
#define XBUTTON1 0x0001
#define XBUTTON2 0x0002

#include "BBApi.h"
#include <windows.h>
#include <stdlib.h>
#include <time.h>

//===========================================================================

#define PLUGIN_NAME		1
#define PLUGIN_VERSION	2
#define PLUGIN_AUTHOR	3
#define PLUGIN_RELEASE	4
#define PLUGIN_LINK		5
#define PLUGIN_EMAIL	6

const short int NUM_BUTTONS = 9;

enum ButtonType
{
	REWIND_BUTTON,
	PLAY_BUTTON,
	PAUSE_BUTTON,
	STOP_BUTTON,
	FORWARD_BUTTON,
	PLAYLIST_BUTTON,
	OPEN_BUTTON,
	UPARROW_BUTTON,
	DOWNARROW_BUTTON
};

struct FoompButton
{
	RECT hitrect;
	int x, y;
	ButtonType type;
	char cmdarg[256];
	char altcmdarg[256];

	void draw (HDC buf);
	int width ();
	int height ();
	bool pressed;
	bool clicked (int mouseX, int mouseY);

private:
	void drawShape (HDC buf, int Penx, int Peny);
};


//===========================================================================


//===========================================================================
LRESULT CALLBACK WndProc (HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

void UpdateTitle ();
void UpdatePosition ();
void ToggleDockedToSlit ();
void GetStyleSettings ();
void GetWindowName ();
void ReadRCSettings ();
void WriteDefaultRCSettings ();
void Transparency ();
void ClickMouse (int mouseX, int mouseY);
void TrackMouse ();
void ChangeRectStyle (int style);
void refresh ();
void DispModeControls (RECT r, HDC buf);
void DispModeTitle (RECT r, HDC buf, HDC src, HDC hdc, HBITMAP bufbmp, HBITMAP srcbmp, HBITMAP oldbuf, PAINTSTRUCT ps);
void CalculateButtonPositions (RECT r);
COLORREF GetShadowColor (StyleItem & style);


//===========================================================================
extern "C"
{
	__declspec(dllexport) int beginPlugin(HINSTANCE hMainInstance);
	__declspec(dllexport) int beginPluginEx(HINSTANCE hMainInstance, HWND hBBSlit);
	__declspec(dllexport) int beginSlitPlugin(HINSTANCE hMainInstance, HWND hwndBBSlit);
	__declspec(dllexport) void endPlugin(HINSTANCE hMainInstance);
	__declspec(dllexport) LPCSTR pluginInfo(int field);

}

