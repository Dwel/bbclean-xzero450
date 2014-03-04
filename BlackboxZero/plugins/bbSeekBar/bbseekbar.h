/*
 ============================================================================

  This program is free software, released under the GNU General Public License
  (GPL version 2 or later). See for details:

  http://www.fsf.org/licenses/gpl.html

  This program is distributed in the hope that it will be useful, but
  WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
  or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License
  for more details.

 ============================================================================
*/

#ifdef _MSC_VER
  #ifdef BBOPT_STACKDUMP
    #define TRY if (1)
    #define EXCEPT if(0)
  #else
    #define TRY _try
    #define EXCEPT _except(1)
  #endif
  #define stricmp _stricmp
  #define strnicmp _strnicmp
  #define memicmp _memicmp
  #define strlwr _strlwr
  #define strupr _strupr
#else
  #undef BBOPT_STACKDUMP
#endif

#ifndef __BBSEEKBAR_H
#define __BBSEEKBAR_H

#ifndef magicDWord
#define magicDWord		0x49474541
#endif

#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0501
#endif

#include <windows.h>
#include <shlwapi.h>
#include <commctrl.h>
#include <tchar.h>
#include "BBApi.h"

#ifndef SLIT_ADD
#define SLIT_ADD 11001
#endif

#ifndef SLIT_REMOVE
#define SLIT_REMOVE 11002
#endif

#ifndef SLIT_UPDATE
#define SLIT_UPDATE 11003
#endif

#ifndef WS_EX_LAYERED
#define WS_EX_LAYERED	0x00080000
#define LWA_COLORKEY	0x00000001
#define LWA_ALPHA	0x00000002
#endif

#define SLIDER_OUTERBORDERMARGIN 3
#define SLIDER_INNERBORDERMARGIN 4
#define SLIDER_MINIMUMWIDTH 2

//===========================================================================

HINSTANCE hInstance;
HWND hwndPlugin, hwndBlackbox, hwndFoobar2k;

bool inSlit = false;
HWND hSlit = NULL;

int msgs[] = {BB_RECONFIGURE, BB_REDRAWGUI, BB_BROADCAST, 0};

char rcpath[MAX_PATH];
char stylepath[MAX_PATH];

int xpos, ypos;
int width, height;
int alpha;
int fontHeight;
int styleType;
bool alwaysOnTop;
bool snapWindow;
bool transparency;
bool pluginToggle;
bool showBorder;
bool allowtip;

bool usingWin2kXP;
OSVERSIONINFO  osvinfo;

void ShowMyMenu(bool popup);
void SetWindowModes(void);
void OnPaint(HWND hwnd);
StyleItem myStyleItem, trackStyleItem, knobStyleItem;
int bevelWidth;
int borderWidth;
COLORREF borderColor;

Menu *myMenu, *configSubmenu, *settingsSubmenu,  *styleSubmenu, *bgStyleSubmenu,
*trackStyleSubmenu, *knobStyleSubmenu, *appearanceSubmenu, *optionsSubmenu, *playerSubmenu;

char tipString[MAX_LINE_LENGTH];
char *szTemp;

int ScreenWidth;
int ScreenHeight;

int newWidth;
int newHeight;

//===========================================================================

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
BOOL WINAPI BBSetLayeredWindowAttributes(HWND hwnd, COLORREF crKey, BYTE bAlpha, DWORD dwFlags);

void GetStyleSettings();
void ReadRCSettings();
void WriteRCSettings();

void ShowMyMenu();

//===========================================================================

bool hidden;

#define m_alloc(n) malloc(n)
#define c_alloc(n) calloc(1,n)
#define m_free(v) free(v)

HWND hToolTips;
void SetToolTip(RECT *tipRect, char *tipText);
void ClearToolTips(void);
void SetAllowTip(bool);

bool ResizeMyWindow(int newWidth, int newHeight);

bool vertical;
bool reversed;

int appearance;
int trackStyleType;
int knobStyleType;

bool dragging;
bool display_update;

void SliderUpdateTrack(RECT &windowrect);
void SliderUpdateKnob(RECT &windowrect);
void SliderOnValueChange(LPARAM lParam);

bool track_needsupdate;
RECT track;
RECT track_clickable;
RECT track_countable;
int track_length;
int track_clickable_length;
int track_countable_length;

bool knob_needsupdate;
RECT knob;
int knob_offset;
int knob_length;
int knob_maxlength;

double value;

DWORD dwThreadId_Time;
HANDLE hThread_Time;

int play_ms, play_total;

int playerType;
char controlPath[MAX_LINE_LENGTH];
HINSTANCE controlPlugin;


typedef enum {
	WM_GETPOSITION,
	WM_GETLENGTH,
	WM_JUMPTOTIME
} playerDataType;

typedef int (*PgetPlayerData)(playerDataType, int);

PgetPlayerData FgetPlayerData;

//===========================================================================

extern "C"
{
	__declspec(dllexport) LPCSTR pluginInfo(int field);
	__declspec(dllexport) void endPlugin(HINSTANCE hMainInstance);
	__declspec(dllexport) int beginPlugin(HINSTANCE hMainInstance);
	__declspec(dllexport) int beginSlitPlugin(HINSTANCE hMainInstance, HWND hBBSlit);
	__declspec(dllexport) int beginPluginEx(HINSTANCE hMainInstance, HWND hBBSlit);
}

//===========================================================================

#endif
