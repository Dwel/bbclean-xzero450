/*
 ============================================================================
 Blackbox for Windows: BBPager
 ============================================================================
 Copyright © 2003-2009 nc-17@ratednc-17.com
 Copyright © 2008-2009 The Blackbox for Windows Development Team
 http://www.ratednc-17.com
 http://bb4win.sourceforge.net
 ============================================================================

  Blackbox for Windows is free software, released under the
  GNU General Public License (GPL version 2 or later), with an extension
  that allows linking of proprietary modules under a controlled interface.
  What this means is that plugins etc. are allowed to be released
  under any license the author wishes. Please note, however, that the
  original Blackbox gradient math code used in Blackbox for Windows
  is available under the BSD license.

  http://www.fsf.org/licenses/gpl.html
  http://www.fsf.org/licenses/gpl-faq.html#LinkingOverControlledInterface
  http://www.xfree86.org/3.3.6/COPYRIGHT2.html#5

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
  GNU General Public License for more details.

 ============================================================================
*/

#define _CRT_SECURE_CPP_OVERLOAD_STANDARD_NAMES 1

#ifndef __BBPAGER_H
#define __BBPAGER_H

#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0500
#endif
#define TME_NONCLIENT   0x00000010

#include "BBApi.h"
#include <commctrl.h>
#include <string>
#include <vector>
#include <shellapi.h>

using namespace std;

//===========================================================================

//const long magicDWord = 0x49474541;

extern HINSTANCE hInstance;

// data structures
struct FRAME
{
	int width;
	int height;
	int rows;
	int columns;
	int bevelWidth;
	int borderWidth;
	int hideWidth;

	COLORREF borderColor;
	COLORREF color;
	COLORREF colorTo;

	bool ownStyle;

	StyleItem *style;
	StyleItem Style;
};

struct DESKTOP
{
	int width;
	int height;
	int sizeRatio;

	int fontSize;
	int fontWeight;

	char fontFace[64];

	bool numbers;
	bool windows;
	bool tooltips;

	COLORREF fontColor;
	COLORREF color;
	COLORREF colorTo;

	bool ownStyle;

	StyleItem *style;
	StyleItem Style;
};

struct ACTIVEDESKTOP
{
	char styleType[16];

	//bool useDesktopStyle;

	COLORREF borderColor;
	COLORREF color;
	COLORREF colorTo;

	bool ownStyle;

	StyleItem *style;
	StyleItem Style;
};

struct WINDOW
{
	COLORREF borderColor;
	COLORREF color;
	COLORREF colorTo;

	bool ownStyle;

	StyleItem *style;
	StyleItem Style;
};

struct FOCUSEDWINDOW
{
	char styleType[16];

	//bool useWindowStyle;

	COLORREF borderColor;
	COLORREF color;
	COLORREF colorTo;

	bool ownStyle;

	StyleItem *style;
	StyleItem Style;
};

struct POSITION
{
	int x;
	int y;
	int ox;
	int oy;
	int hx;
	int hy;
	int side;

	bool vertical;
	bool horizontal;
	bool raised;
	int snapWindow;
	bool unix;
	bool autohide;
	bool autohideOld;
	bool hidden;

	char placement[20];
};

extern struct POSITION position;
extern struct FRAME frame;
extern struct DESKTOP desktop;
extern struct ACTIVEDESKTOP activeDesktop;
extern struct WINDOW window;
extern struct FOCUSEDWINDOW focusedWindow;

// Window information
extern int vScreenWidth, vScreenHeight;
extern int vScreenLeft, vScreenTop, vScreenRight, vScreenBottom;
extern int screenWidth, screenHeight;
extern int screenLeft, screenTop, screenRight, screenBottom;
extern double ratioX, ratioY;

typedef struct winStruct
{
	HWND window;
	RECT r;
	BOOL active;
	BOOL sticky;
	int desk;
} winStruct;

extern vector<winStruct> winList;

// flashing tasks
typedef struct flashTask
{
	HWND task;
	bool on;
} flashTask;

extern vector<flashTask> flashList;

extern HWND hToolTips;

extern int winCount;

extern bool winMoving;
extern winStruct moveWin;
extern int mx, yx;
extern HWND hwndBBPager;

extern int leftMargin, topMargin;

extern bool drawBorder;

// File paths
extern char rcpath[MAX_PATH];
extern char bspath[MAX_PATH];
extern char stylepath[MAX_PATH];

extern char editor[MAX_PATH];

// Desktop information
extern int desktops;
extern int currentDesktop;
extern vector<RECT> desktopRect;
extern int desktopChangeButton;
extern int focusButton;
extern int moveButton;

// Transparency
extern bool usingWin2kXP;
extern bool transparency;
extern int transparencyAlpha;

// Slit
extern bool inSlit, useSlit;
extern int xpos, ypos;

// Compatibility
extern struct tasklist *tl;
extern bool is_xoblite;
extern bool usingAltMethod;

//===========================================================================
// function declarations

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK CheckTaskEnumProc(HWND hwnd, LPARAM lParam);
BOOL CALLBACK CheckTaskEnumProc_AltMethod(HWND hwnd, LPARAM lParam);

extern void GetStyleSettings();
extern void ReadRCSettings();
extern void WriteRCSettings();

extern void DrawBBPager(HWND hwnd);
extern void DrawBorder(HDC hdc, RECT rect, COLORREF borderColour, int borderWidth);
extern void DrawActiveDesktop(HDC buf, RECT r, int i);
extern void DrawActiveWindow(HDC buf, RECT r);
extern void DrawInactiveWindow(HDC buf, RECT r);

void GetPos(bool snap);
void SetPos(int place);

bool IsValidWindow(HWND hWnd);
int getDesktop(HWND h);

void UpdatePosition();
void UpdateMonitorInfo();

void ClickMouse();
void TrackMouse();
bool CursorOutside();

void DeskSwitch();

void FocusWindow();
void GrabWindow();
void DropWindow();

void AddFlash(HWND task);
void RemoveFlash(HWND task, bool quick);
bool IsFlashOn(HWND task);

void SetToolTip(RECT *tipRect, char *tipText);
void ClearToolTips(void);

void HidePager();

void DisplayMenu();

void ToggleSlit();

bool AddBBWindow(tasklist* tl);

bool BBPager_SetTaskLocation(HWND hwnd, struct taskinfo *pti, UINT flags);
tasklist* BBPager_GetTaskListPtr(void);

//===========================================================================

#endif

