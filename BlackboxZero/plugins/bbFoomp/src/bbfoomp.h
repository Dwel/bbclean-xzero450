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

#include "BBApi.h"
#include <windows.h>
#include <stdlib.h>
#include <time.h>

//===========================================================================
LRESULT CALLBACK WndProc (HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

void UpdateTitle ();
void UpdatePosition ();
void ToggleDockedToSlit ();
void GetWindowName ();
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

