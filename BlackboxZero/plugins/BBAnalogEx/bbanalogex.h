/*         
 ============================================================================
 Blackbox for Windows: Plugin BBAnalogEx 1.0 by Miroslav Petrasko [Theo] 
 ============================================================================
 Copyright � 2001-2004 The Blackbox for Windows Development Team
 http://desktopian.org/bb/ - #bb4win on irc.freenode.net
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
  For additional license information, please read the included license
 ============================================================================
*/



#ifndef __BBANALOGEX_H
#define __BBANALOGEX_H

#ifndef WS_EX_LAYERED
#define WS_EX_LAYERED	0x00080000
#define LWA_COLORKEY	0x00000001
#define LWA_ALPHA		0x00000002
#endif 

#include <windows.h>
#include <shlwapi.h>
//#include <wingdi.h>
#include <math.h>
#include <time.h>
#include "BBApi.h"
#include "MessageBox.h"
#include "AggressiveOptimize.h"
#include <gdiplus.h>

#define IDT_TIMER 1
#define IDT_ALARMTIMER 2
#define IDT_MTIMER 3
/*#define NOTHING 0
#define TOP 1
#define BOTTOM 2
#define LEFT 3
#define RIGHT 4

/*#define VERTICAL 0
#define HORIZONTAL 1
#define JUSTTEXT 2
*/


//gdiplus things
using namespace Gdiplus;
GdiplusStartupInput g_gdiplusStartupInput;
ULONG_PTR g_gdiplusToken;


//OS info storage
DWORD      dwId;
DWORD      dwMajorVer;
DWORD      dwMinorVer;
double	PI = 3.14159265359;

RECT rec;
bool anti=true;
//temp storage
static char szTemp[MAX_LINE_LENGTH];
static char dszTemp[MAX_LINE_LENGTH];

//window instances
HINSTANCE hInstance;
HWND hwndBBAnalogEx, hwndBlackbox;
bool inSlit = false;	//Are we loaded in the slit? (default of no)
HWND hSlit;				//The Window Handle to the Slit (for if we are loaded)

// Blackbox messages we want to "subscribe" to:
// BB_RECONFIGURE -> Sent when changing style and on reconfigure
// BB_BROADCAST -> Broadcast message (bro@m)
int msgs[] = {BB_RECONFIGURE, BB_BROADCAST, 0};

//file path storage
char rcpath[MAX_PATH];
char stylepath[MAX_PATH];
char alarmpath[MAX_PATH];

char alarm[MAX_PATH];
char htime[10];

//char clockformat[256];
//char drawclock[256];

//start up positioning
int ScreenWidth;
int ScreenHeight;

//.rc file settings
bool show = true;
int xpos, ypos;
int width, height;

int smal=10;
int day,month,year;
int second,minute,hour,rhour;
time_t systemTime;
struct tm *localTime;

int swidth,mwidth,hwidth,cwidth;
int slength,mlength,hlength;
//int vl,hl;
//int text_pos;
bool drawCircle;
bool wantInSlit;
bool alwaysOnTop;
bool snapWindow;
bool pluginToggle;
bool transparency;
bool fullTrans;	
bool drawBorder;
//int drawMode;
bool showSeconds = true;
int alpha;
bool noBitmap;
char windowStyle[24];
char bitmapFile[MAX_PATH];
char overBitmapFile[MAX_PATH];
//int ratio;
bool alarms = false;
DWORD drawMode;
int quadrant;
int quad;
int hquad;
int dday;
//----------------------
//small clocks
int scx[10];
int scy[10];
int scr[10];
int scs[10];
COLORREF scch[10];
COLORREF sccm[10];
COLORREF sccs[10];
int scdiff[10];
int sccount;

////////////////////////

//style setting storage
COLORREF backColor, backColor2;
COLORREF backColorTo, backColorTo2;
COLORREF fontColor;
char fontFace[256];
//char mer[3];
int fontSize = 13;
//bool remove_zero = false;

//bool mag = false;

StyleItem *myStyleItem, *myStyleItem2;
int bevelWidth;
int borderWidth;
COLORREF borderColor,helpcolor;
COLORREF scolor,mcolor,hcolor,ccolor;
bool acolor;
//menu items
Menu *myMenu, *windowStyleSubmenu, *configSubmenu, *settingsSubmenu, *otherSubmenu, *bitmapSubmenu, *styleSubmenu, *browseSubmenu;
Menu *generalConfigSubmenu, *handSubmenu, *modeSubmenu, *colorSubmenu, *smallclockSubmenu;

//===========================================================================

//gets OS version
int WINAPI _GetPlatformId(DWORD *pdwId, DWORD *pdwMajorVer, DWORD *pdwMinorVer);
//window process
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
//special function for alpha transparency
//so there you just use BBSLWA like normal SLWA
//(c)grischka
BOOL WINAPI BBSetLayeredWindowAttributes(HWND hwnd, COLORREF crKey, BYTE bAlpha, DWORD dwFlags);

//Plugin functions
void GetStyleSettings();
void ReadRCSettings();
void WriteRCSettings();
void InitBBAnalogEx();
void mySetTimer();
void setStatus();
void createMenu();
void getCurrentDate();
//void drawPart(HDC &hdc, int x, int y, int length, bool dir);
//void drawNumber(HDC &hdc, int number, int x, int y, int vlength, int hlength);
//void drawClock(HDC &hdc, RECT r, int mode);
void executeAlarm();
void createAlarmFile();


//===========================================================================

extern "C"
{
	__declspec(dllexport) int beginPlugin(HINSTANCE hMainInstance);
	__declspec(dllexport) void endPlugin(HINSTANCE hMainInstance);
	__declspec(dllexport) LPCSTR pluginInfo(int field);
	// This is the function BBSlit uses to load your plugin into the Slit
	__declspec(dllexport) int beginSlitPlugin(HINSTANCE hMainInstance, HWND hBBSlit);
}

//===========================================================================

#endif

// the end ....
