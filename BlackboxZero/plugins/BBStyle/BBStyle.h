/*
 ============================================================================
 Blackbox for Windows: bbStyle plugin
 ============================================================================
 Copyright © 2003-2009 nc-17@ratednc-17.com
 Copyright © 2007-2009 The Blackbox for Windows Development Team
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

  For additional license information, please read the included bbStyle.html

 ============================================================================
*/

#ifndef __BBSTYLE_H
#define __BBSTYLE_H

#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0500
#endif

#include "BBApi.h"
#include "../bbPlugin/moreutils.cpp"
#include <string>
#include <time.h>
#include <vector>

#ifndef VALID_TEXTCOLOR
#define VALID_TEXTCOLOR     (1<<3)  // TextColor
#endif

using namespace std;

//===========================================================================
/* compiler specifics */

// non-Visual Studio
#ifndef _MSC_VER
// Positioning
#define TME_NONCLIENT   0x00000010
#endif

// Visual Studio
#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
//pre processor commands to link the needed libraries.  Nice little list ;).
//You may comment out these lines and add them to your project settings.
//#pragma comment(lib, "Kernel32.lib")
#pragma comment(lib, "user32.lib")
#pragma comment(lib, "gdi32.lib")
#pragma comment(lib, "../bbMien/Blackbox.lib")
#pragma comment(lib, "../bbUtils/bbUtils.lib")
#endif

// ----------------------------------
// Global vars
HINSTANCE hInstance;
HWND BBhwnd;
HWND hSlit_present;

// Compatibility
bool is_xoblite;
bool dcFile;

int vScreenWidth, vScreenHeight, vScreenLeft, vScreenTop;
int vScreenRight, vScreenBottom;
int screenWidth, screenHeight, screenLeft, screenTop;
int screenRight, screenBottom;

bool leftButtonDown = false;

// receives the path to "bbStyle.rc"
char rcpath[MAX_PATH];
char stylepath[MAX_PATH];
char listPath[MAX_PATH];

typedef vector<string> StringVector;
StringVector styleList;

// ----------------------------------
// Style info

struct STYLE_INFO
{
	StyleItem Frame;
	StyleItem Button;
	StyleItem Pressed;

	int frameWidth;
	
};

// ----------------------------------
// Position
struct POSITION
{
	int X;
	int Y;
	int side;
	int snap;

	bool shown;
	bool fullScreen;
	char placement[16];
	HMONITOR hMon;
	RECT mon_rect;
	
};

// ----------------------------------
// 3dc info
struct DC_INFO
{
	bool Menus;
	bool Selected;
	bool Tooltips;
	bool Highlights;
	bool Titles;
	
};

// ----------------------------------
// Plugin window properties
struct PLUGIN_PROPERTIES
{
	// settings
	int width, height, hideWidth;

	bool useSlit;
	bool onTop;
	bool snapWindow;
	bool toggle;
	bool shown;

	// transparency
	bool usingWin2kPlus;
	int  alpha;

	// our plugin window
	HWND hwnd;

	// current state variables
	bool is_moving;

	// the Slit window, if we are in it.
	HWND hSlit;

	// GDI objects
	HBITMAP bufbmp;
	HFONT hFont;

	// the text
	char windowText[MAX_PATH];

	// Styling stuff
	bool changeOnStart;
	bool timerOn;
	bool showStyleInfo;
	int  changeTime;
	bool chance;
	unsigned short count;

	char styleToSet[MAX_PATH];
	char localPath[MAX_PATH];
	char stylePath[MAX_PATH];
	
};

struct STYLE_INFO style_info;
struct POSITION position;
struct PLUGIN_PROPERTIES plugin;
struct DC_INFO dc_info;

// ----------------------------------
// some function prototypes
void drawPlugin();
void getStyleSettings(void);
void getRCSettings(void);
void displayMenu(bool popup);
void invalidate_window(void);
void set_window_modes(void);
void setPosition();
void updateMonitorInfo();
void setStyle(void);
char getTitleText(bool Text);
char *set_stylePath();
void initList(char *path, bool init);

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

// ----------------------------------
// helpers to handle commands  from the menu

struct broamprop { const char *key; int mode; void *val; };
const struct broamprop *check_item(const char *key, const broamprop *prop);
void eval_menu_cmd(int mode, void *pValue, const char *sub_message);
enum eval_menu_cmd_modes
{
	M_BOL = 1,
	M_INT = 2,
	M_STR = 3,
};
// forward declarations
extern const struct broamprop always_broams[];
extern const struct broamprop nonSlit_broams[];
extern const struct broamprop position_broams[];

#endif
