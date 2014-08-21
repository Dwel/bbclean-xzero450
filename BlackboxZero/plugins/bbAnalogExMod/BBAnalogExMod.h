/*
 ============================================================================

  This program is free software, released under the GNU General Public License
  (GPL version 2 or later).

  This program is distributed in the hope that it will be useful, but
  WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
  or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License
  for more details.

  http://www.fsf.org/licenses/gpl.html

 ============================================================================
*/
#pragma once

/*#ifdef _MSC_VER
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
#endif*/

//#ifndef _WIN32_WINNT
//# define _WIN32_WINNT 0x0501
//#endif

#ifndef magicDWord
#   define magicDWord		0x49474541
#endif

//#ifndef ULONG_PTR
//#define ULONG_PTR DWORD
//#endif

//#ifndef WS_EX_LAYERED
//#define WS_EX_LAYERED	0x00080000
//#define LWA_COLORKEY	0x00000001
//#define LWA_ALPHA		0x00000002
//#endif // ndef WS_EX_LAYERED
//#pragma warning(disable : 4996)

#include <windows.h>
#include <math.h>
#include <time.h>
#include <sys/timeb.h>
#include <GdiPlus.h>
#include <Shlwapi.h>
#include "BBApi.h"

#define IDT_TIMER 1
//typedef unsigned __int64 ULONG_PTR;
//typedef DWORD ULONG_PTR;
typedef float REAL;
//typedef unsigned __int64* ULONG_PTR;

//===========================================================================

//gdi+ structs
Gdiplus::GdiplusStartupInput	gdiplusStartupInput;
ULONG_PTR   gdiplusToken;

//OS info storage
bool usingWin2kXP;
OSVERSIONINFO  osvinfo;

//for circle calculations
double	PI = 3.14159265359;
float	radius;
double	theta;
float	cntX;
float	cntY;

//the rest of these are all gdi+ objects
Gdiplus::Graphics		*graphics;
Gdiplus::Image		 	*myImage;
Gdiplus::Bitmap		 	*bitmap;
Gdiplus::Pen			*pen;
Gdiplus::FontFamily		*fontFamily;
Gdiplus::Font			*font;
Gdiplus::StringFormat		*format;
Gdiplus::StringFormat		*myFormat;
Gdiplus::Color			*plusFontColor;
Gdiplus::Color			*helpColor;
Gdiplus::Brush			*hBrush;
Gdiplus::SolidBrush		*brush;
Gdiplus::RectF			*layoutRect;
Gdiplus::ImageAttributes	*attr;
Gdiplus::RectF	                *imgrect;

RECT rect;

//temp storage
char *szTemp;

//window instances
HINSTANCE hInstance;
HWND hwndBBAnalogExMod, hwndBlackbox;
bool inSlit = false;	//Are we loaded in the slit? (default of no)
HWND hSlit;				//The Window Handle to the Slit (for if we are loaded)

bool LayerdInit();

// Blackbox messages we want to "subscribe" to:
// BB_RECONFIGURE -> Sent when changing style and on reconfigure
// BB_BROADCAST -> Broadcast message (bro@m)
int msgs[] = {BB_RECONFIGURE, BB_BROADCAST, 0};

//file path storage
char rcpath[MAX_PATH];
char stylepath[MAX_PATH];

//start up positioning
int ScreenWidth;
int ScreenHeight;


//form toggle
bool rtoggleRect;
bool rtoggleOval;
bool rtoggleHole;
int region;
int outsideCircle;
int insideCircle;

//border drawing
bool drawBorder;

//font setting
int fontSizeC;

//.rc file settings
int xpos, ypos;
int size;

int newSize;
bool ResizeMyWindow(int new_size);

//clock settings
bool wantInSlit;
bool alwaysOnTop;
bool snapWindow;
bool pluginToggle;
bool transparency;
bool transBack;
bool lockp;		//lock position

int alpha;

bool showSeconds;
bool showCircle;
bool showRomans;
bool showNormal;
bool showLargeTics;
bool showSmallTics;
bool sweepHands;

int styleType;

char timeAlarm[24];
bool hourAlarm;
bool halfHourAlarm;
int timeAlarmFrequency;
int hourAlarmFrequency;
int halfHourAlarmFrequency;
int animationFPS;
bool synchronize;			//play wav once only if not sync'ed
//bool aubibleAlarm;		//don't need, just do .none for no wav file
char timeAlarmWav [MAX_PATH];
char hourAlarmWav [MAX_PATH];
char halfHourAlarmWav [MAX_PATH];

//time stuff
time_t systemTime;
static char currentDate[10];
struct tm *localTime;
double currentSecond, currentMinute, currentHour, currentMillisecond;

//for different drawing modes
int showDate;
int showAlarm;
char playAlarm[MAX_PATH];


//digital clock variables
int digiL; int digiH;
bool digiFill;


int circleCounter;
bool oneMoreDraw;

bool timeAlarmSet;
//"AM" stands for the Latin phrase Ante Meridian--which means "before noon"--and
//"PM" stands for Post Meridian: "after noon."
bool meridian;
bool magicHourFreq;
int timeAlarmHour;
int timeAlarmMinute;

//style setting storage
COLORREF backColor, backColor2;
COLORREF backColorTo, backColorTo2;
char fontFace[236];
int fontSize;
COLORREF fontColor;
COLORREF secondHandColor;
COLORREF minuteHandColor;
COLORREF hourHandColor;
COLORREF numbColor;
COLORREF dFillColor;
COLORREF dLineColor;


//hands width
int secondHandWidth;
int minuteHandWidth;
int hourHandWidth;

//hands length
int secondHandLength;
int minuteHandLength;
int hourHandLength;

StyleItem myStyleItem, *T;
int bevelWidth;
int borderWidth;
COLORREF borderColor;

//menu items
Menu *myMenu, *clockConfigSubmenu, *hourSubmenu, *minuteSubmenu, *secondSubmenu, *textSubmenu, *regionSubmenu,
*configSubmenu, *styleSubmenu, *alarmConfigSubmenu, *setAlarmsConfigSubmenu, *settingsSubmenu, *imageSubmenu, *optionSubmenu;

void ShowMyMenu(bool popup);
//===========================================================================

//window process
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

//Plugin functions
void GetStyleSettings ();
void ReadRCSettings ();
void WriteRCSettings ();
void InitBBAnalog ();
void getCurrentTime ();
//void getCurrentDate();
void mySetTimer (int mode);
void readSetAlarmTime ();
void myPlayWav (char * alarmFile);
void setRegion ();   //sets the regions
void drawDigiPart (int x, int y, bool dir);   //draws one part of the number in the digi clock
void drawNumber (int number, int x, int y);	//draws the number in the digi clock
void setStatus ();
void SetWindowModes ();

char imagePath[MAX_PATH];

void OnDropFiles (HDROP);
HDROP hDrop;

bool setAttr;

//===========================================================================

extern "C"
{
	__declspec(dllexport) int beginPlugin(HINSTANCE hMainInstance);
	__declspec(dllexport) void endPlugin(HINSTANCE hMainInstance);
	__declspec(dllexport) LPCSTR pluginInfo(int field);
	// This is the function BBSlit uses to load your plugin into the Slit
	__declspec(dllexport) int beginSlitPlugin(HINSTANCE hMainInstance, HWND hBBSlit);
}

