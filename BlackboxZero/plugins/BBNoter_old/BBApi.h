/*
 ============================================================================
 This file is part of the Blackbox for Windows source code
 Copyright � 2001-2003 The Blackbox for Windows Development Team
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

  For additional license information, please read the included license.html

 ============================================================================
*/

#ifndef __BBAPI_H_
#define __BBAPI_H_

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdio.h>

#define MAX_LINE_LENGTH 4096

//------------------------------------------

#ifdef __BORLANDC__
	#include "BB_BCC55.H"
#endif

#ifndef DLL_EXPORT
	#define DLL_EXPORT __declspec(dllexport)
#endif

#ifndef WS_EX_LAYERED
	#define WS_EX_LAYERED   0x00080000
	#define LWA_ALPHA       0x00000002
#endif

//------------------------------------------
// bimage constants

// Gradient types + solid
#define B_HORIZONTAL 0
#define B_VERTICAL 1
#define B_DIAGONAL 2
#define B_CROSSDIAGONAL 3
#define B_PIPECROSS 4
#define B_ELLIPTIC 5
#define B_RECTANGLE 6
#define B_PYRAMID 7
#define B_SOLID 8

// Bevels
#define BEVEL_FLAT 0
#define BEVEL_RAISED 1
#define BEVEL_SUNKEN 2

#define BEVEL1 1
#define BEVEL2 2

//------------------------------------------
// Tray (wParam for BB_TRAYUPDATE)
#define TRAYICON_ADDED 0
#define TRAYICON_MODIFIED 1
#define TRAYICON_REMOVED 2
#define TRAYICON_REFRESH 3

// Task (wParam for BB_TASKSUPDATE)
#define TASKITEM_ADDED 0
#define TASKITEM_MODIFIED 1
#define TASKITEM_ACTIVATED 2
#define TASKITEM_REMOVED 3
#define TASKITEM_REFRESH 4
#define TASKITEM_FLASHED 5

// Toolbar Placement
#define TOOLBAR_TOP_LEFT 0
#define TOOLBAR_TOP_CENTER 1
#define TOOLBAR_TOP_RIGHT 2
#define TOOLBAR_BOTTOM_LEFT 3
#define TOOLBAR_BOTTOM_CENTER 4
#define TOOLBAR_BOTTOM_RIGHT 5

// Exit Types (wParam for BB_EXITTYPE)
#define B_SHUTDOWN 0
#define B_QUIT 1
#define B_RESTART 2

//===========================================================================
// Blackbox messages

#define BB_MSGFIRST             10000
#define BB_MSGLAST              13000

#define BB_REGISTERMESSAGE      10001
#define BB_UNREGISTERMESSAGE    10002

#define BB_QUIT                 10101
#define BB_RESTART              10102
#define BB_RECONFIGURE          10103
#define BB_SETSTYLE             10104
#define BB_EXITTYPE             10105   // Param3: 0=Shutdown/Reboot/Logoff 1=Quit 2=Restart
#define BB_TOOLBARUPDATE        10106

#define BB_EDITFILE             10201   // Param3: 0=CurrentStyle 1=menu.rc, 2=plugins.rc
#define BB_EXECUTE              10202
#define BB_ABOUTSTYLE           10203
#define BB_ABOUTPLUGINS         10204

#define BB_MENU                 10301   // Param3: 0=Main menu, 1=Workspaces menu, 2=Toolbar menu
#define BB_HIDEMENU             10302
#define BB_TOGGLETRAY           10303   // bb4win
#define BB_TOGGLESYSTEMBAR      10303   // xoblite
#define BB_SETTOOLBARLABEL      10304   // Used to set the toolbar label (returns to normal after 2 seconds)
#define BB_TOGGLEPLUGINS        10305
#define BB_SUBMENU              10306
#define BB_TOGGLESLIT           10307   // Added to avoid conflicts with xoblite
#define BB_TOGGLETOOLBAR        10308   // Added to avoid conflicts with xoblite

#define BB_SHUTDOWN             10401   // Param3: 0=Shutdown, 1=Reboot, 2=Logoff, 3=Hibernate, 4=Suspend, 5=LockWorkstation
#define BB_RUN                  10402
#define BB_COMMAND              10403

#define BB_DESKTOPINFO          10501
#define BB_LISTDESKTOPS         10502
#define BB_SWITCHTON            10503
#define BB_BRINGTOFRONT         10504
#define BB_WORKSPACE            10505   // Param3: 0=DeskLeft, 1=DeskRight, 2=AddDesktop, 3=DelDesktop, 4=DeskSwitchToN,
										//         5=GatherWindows 6=MoveWindowLeft, 7=MoveWindowRight
#define BB_TASKSUPDATE          10506
#define BB_TRAYUPDATE           10507
#define BB_CLEANTRAY            10508
#define BB_CLEANTASKS           10509

#define BB_ADDTASK              10601   // Hook messages...
#define BB_REMOVETASK           10602
#define BB_ACTIVATESHELLWINDOW  10603
#define BB_ACTIVETASK           10604
#define BB_MINMAXTASK           10605
#define BB_WINDOWSHADE          10606
#define BB_WINDOWGROWHEIGHT     10607
#define BB_WINDOWGROWWIDTH      10608
#define BB_WINDOWLOWER          10609
#define BB_REDRAW               10610
#define BB_MINIMIZE             10611   // obsolete
#define BB_WINDOWMINIMIZE       10611
#define BB_WINDOWRAISE          10612
#define BB_WINDOWMAXIMIZE       10613
#define BB_WINDOWRESTORE        10614
#define BB_WINDOWCLOSE          10615

#define BB_BROADCAST            10901   // Broadcast messages (bro@m -> the bang killah! :D <vbg>)

// BBSlit messages (so we don't use them on accident)
#define SLIT_ADD                11001
#define SLIT_REMOVE             11002
#define SLIT_UPDATE             11003

// pluginInfo
#define PLUGIN_NAME         1
#define PLUGIN_VERSION      2
#define PLUGIN_AUTHOR       3
#define PLUGIN_RELEASE      4   // BB4Win
#define PLUGIN_RELEASEDATE  4   // xoblite
#define PLUGIN_LINK         5
#define PLUGIN_EMAIL        6
#define PLUGIN_BROAMS       7   // xoblite

//===========================================================================
// Unique Blackbox DWORD
// (same as in LiteStep to make apps like Winamp sticky)
const long magicDWord = 0x49474541; // obsolete

//===========================================================================
// extended Styleitem, backwards compatible

class StyleItem
{
public:
	int bevelstyle;
	int bevelposition;
	int type;
	bool parentRelative;
	bool interlaced;
	COLORREF Color;
	COLORREF ColorTo;
	COLORREF TextColor;
	int FontHeight;
	int FontWeight;
	int Justify;
	int validated;
	char Font[256];
};

#define PicColor TextColor

class Menu;
class MenuItem;

//===========================================================================
// constants for GetSettingPtr(int index) // returns:
enum {
	 SN_TOOLBAR = 1             // StyleItem *
	,SN_TOOLBARBUTTON           // StyleItem *
	,SN_TOOLBARBUTTONP          // StyleItem *
	,SN_TOOLBARLABEL            // StyleItem *
	,SN_TOOLBARWINDOWLABEL      // StyleItem *
	,SN_TOOLBARCLOCK            // StyleItem *
	,SN_MENUTITLE               // StyleItem *
	,SN_MENUFRAME               // StyleItem *
	,SN_MENUHILITE              // StyleItem *

	,SN_MENUBULLET              // char *
	,SN_MENUBULLETPOS           // char *

	,SN_BORDERWIDTH             // int *
	,SN_BORDERCOLOR             // int *
	,SN_BEVELWIDTH              // int *
	,SN_FRAMEWIDTH              // int *
	,SN_HANDLEWIDTH             // int *

	,SN_ROOTCOMMAND             // char *

	,SN_MENUAPLHA               // int *
	,SN_TOOLBARALPHA            // int *

	,SN_METRICSUNIX             // bool *
	,SN_BULLETUNIX              // bool *

/* not implemented
	,SN_WINFOCUS_BUTTON         // StyleItem *
	,SN_WINFOCUS_BUTTONP        // StyleItem *
	,SN_WINFOCUS_GRIP           // StyleItem *
	,SN_WINFOCUS_HANDLE         // StyleItem *
	,SN_WINFOCUS_LABEL          // StyleItem *
	,SN_WINFOCUS_TITLE          // StyleItem *
	,SN_WINFOCUS_FRAME          // StyleItem *

	,SN_WINUNFOCUS_BUTTON       // StyleItem *
	,SN_WINUNFOCUS_GRIP         // StyleItem *
	,SN_WINUNFOCUS_HANDLE       // StyleItem *
	,SN_WINUNFOCUS_LABEL        // StyleItem *
	,SN_WINUNFOCUS_TITLE        // StyleItem *
	,SN_WINUNFOCUS_FRAME        // StyleItem *
	,SN_WINDOW                  // StyleItem *
*/
};

//===========================================================================
// Exported functions

extern "C"
{
	// ------------------------------------
	// Read Settings
	DLL_EXPORT bool ReadBool(LPCSTR filePointer, LPCSTR string, bool defaultBool);
	DLL_EXPORT int ReadInt(LPCSTR filePointer, LPCSTR string, int defaultInt);
	DLL_EXPORT LPCSTR ReadString(LPCSTR filePointer, LPCSTR string, LPCSTR defaultString);
	DLL_EXPORT COLORREF ReadColor(LPCSTR filePointer, LPCSTR string, LPCSTR defaultString);
	// Read a rc-value as string. If 'ptr' is specified, it can read a sequence of items with the same name.
	DLL_EXPORT LPCSTR ReadValue(LPCSTR fp, LPCSTR keyword, LPLONG ptr=NULL);
	// Was the last Value actually read from the rc-file ?
	DLL_EXPORT bool FoundLastValue(void);

	// ------------------------------------
	// Write Settings
	DLL_EXPORT void WriteBool(LPCSTR filePointer, LPCSTR keyword, bool value);
	DLL_EXPORT void WriteInt(LPCSTR filePointer, LPCSTR keyword, int value);
	DLL_EXPORT void WriteString(LPCSTR filePointer, LPCSTR keyword, LPCSTR value);
	DLL_EXPORT void WriteColor(LPCSTR filePointer, LPCSTR keyword, COLORREF value);

	// ------------------------------------
	// Direct access to Settings variables / styleitems / colors
	// See the SN_XXX constants for possible values
	DLL_EXPORT void* GetSettingPtr(int index);

	// ------------------------------------
	// File functions
	DLL_EXPORT bool FileExists(LPCSTR szFileName);
	DLL_EXPORT FILE *FileOpen(LPCSTR fileName);
	DLL_EXPORT bool FileClose(FILE *filePointer);
	DLL_EXPORT bool FileRead(FILE *filePointer, LPSTR readString);
	DLL_EXPORT bool ReadNextCommand(FILE *filePointer, LPSTR readLine, DWORD lineLength);

	// ------------------------------------
	// Make a window visible on all workspaces
	DLL_EXPORT void MakeSticky(HWND window);
	DLL_EXPORT void RemoveSticky(HWND window);
	DLL_EXPORT bool CheckSticky(HWND window);

	// ------------------------------------
	// Is BB running under explorer ?
	DLL_EXPORT bool GetUnderExplorer();

	// ------------------------------------
	// Tray icon access
	typedef struct
	{
		HWND    hWnd;
		UINT    uID;
		UINT    uCallbackMessage;
		HICON   hIcon;
		char    szTip[256];
	} systemTray;

	DLL_EXPORT int GetTraySize();
	DLL_EXPORT systemTray* GetTrayIcon(int pointer);

	// ------------------------------------
	// Task items access
	DLL_EXPORT int GetTaskListSize();
	DLL_EXPORT void SetTaskWorkspace(HWND hwnd, int workspace);
	DLL_EXPORT int GetTaskWorkspace(HWND hwnd);
	DLL_EXPORT HWND GetTask(int pointer);
	DLL_EXPORT int GetActiveTask();
	// Retrieve a pointer to the internal TaskList.
	DLL_EXPORT struct tasklist* GetTaskListPtr(void);
	// with ...
	struct tasklist
	{
		struct tasklist *next;
		HWND  hwnd;
		HICON icon;
		int   wkspc;
		char  caption[256];
	};

	// ------------------------------------
	// Window utilities
	DLL_EXPORT int GetAppByWindow(HWND Window, char*);
	DLL_EXPORT bool IsAppWindow(HWND hwnd);
	DLL_EXPORT void SnapWindowToEdge(WINDOWPOS* windowPosition, int snapDistance, bool useScreenSize);
	DLL_EXPORT bool SetTransparency(HWND hwnd, BYTE alpha);

	// ------------------------------------
	// Get paths
	DLL_EXPORT bool WINAPI GetBlackboxPath(LPSTR path, int maxLength);
	DLL_EXPORT LPCSTR bbrcPath(LPCSTR bbrcFileName=NULL);
	DLL_EXPORT LPCSTR extensionsrcPath(LPCSTR extensionsrcFileName=NULL);
	DLL_EXPORT LPCSTR menuPath(LPCSTR menurcFileName=NULL);
	DLL_EXPORT LPCSTR plugrcPath(LPCSTR pluginrcFileName=NULL);
	DLL_EXPORT LPCSTR stylePath(LPCSTR styleFileName=NULL);
	DLL_EXPORT void GetBlackboxEditor(LPSTR editor);

	// ------------------------------------
	// Get the main window and other info
	DLL_EXPORT HWND GetBBWnd();
	DLL_EXPORT LPCSTR GetBBVersion();
	DLL_EXPORT LPCSTR GetOSInfo();

	// ------------------------------------
	// String helpers
	DLL_EXPORT LPSTR Tokenize(LPCSTR sourceString, LPSTR targetString, LPSTR delimiter);
	DLL_EXPORT int BBTokenize (LPCSTR sourceString, LPSTR* targetStrings, DWORD numTokensToParse, LPSTR remainingString);
	DLL_EXPORT LPSTR StrRemoveEncap(LPSTR string);
	DLL_EXPORT bool IsInString(LPCSTR inputString, LPCSTR searchString);
	DLL_EXPORT void ReplaceEnvVars(LPSTR string);
	DLL_EXPORT void ParseItem(LPCSTR szItem, StyleItem *item);

	// ------------------------------------
	// Shell execute a command
	DLL_EXPORT HINSTANCE BBExecute(HWND Owner, LPCSTR szOperation, LPCSTR szCommand, LPCSTR szArgs, LPCSTR szDirectory, int nShowCmd, bool noErrorMsgs);

	// ------------------------------------
	// MakeNamedMenu for automatic updating menues. Use instead of Makemenu.
	// See BBTray.cpp or the SDK for examples.
	DLL_EXPORT Menu *MakeNamedMenu(LPCSTR HeaderText, LPCSTR Id, bool forceshow);

	// inactive item
	DLL_EXPORT MenuItem *MakeMenuNOP(Menu *PluginMenu, LPCSTR Title);

	// command item
	DLL_EXPORT MenuItem *MakeMenuItem(Menu *PluginMenu, LPCSTR Title, LPCSTR Cmd, bool ShowIndicator);

	// item to adjust a numeric value (implemented in bblean only for now)
	DLL_EXPORT MenuItem *MakeMenuItemInt(Menu *PluginMenu, LPCSTR Title, LPCSTR Cmd, int val, int minval, int maxval);

	// item to edit a string value (not yet implemented)
	DLL_EXPORT MenuItem *MakeMenuItemString(Menu *PluginMenu, LPCSTR Title, LPCSTR Cmd, LPCSTR init_string);

	// item to opens a submenu
	DLL_EXPORT MenuItem *MakeSubmenu(Menu *ParentMenu, Menu *ChildMenu, LPCSTR Title);

	// show the menu
	DLL_EXPORT void ShowMenu(Menu *PluginMenu);

	// obsolete with MakeNamedMenu:
	DLL_EXPORT void DelMenu(Menu *PluginMenu);
	// obsolete with MakeNamedMenu:
	DLL_EXPORT Menu *MakeMenu(LPCSTR HeaderText);

	// ------------------------------------
	// Logging and error messages
	DLL_EXPORT void Log(LPCSTR variable, LPCSTR description);
	DLL_EXPORT int MBoxErrorFile(LPCSTR szFile);
	DLL_EXPORT int MBoxErrorValue(LPCSTR szValue);

	// ------------------------------------
	// Painting
	DLL_EXPORT void MakeGradient(HDC hdc, RECT rect, int gradientType, COLORREF colourFrom, COLORREF colourTo, bool interlaced, int bevelStyle, int bevelPosition, int bevelWidth, COLORREF borderColour, int borderWidth);
	// Draw a Gradient Rectangle from StyleItem, optional using the style border.
	DLL_EXPORT void MakeStyleGradient(HDC hDC, RECT* p_rect, StyleItem * m_si, bool withBorder);
	// Create a font handle from styleitem, with parsing and substitution.
	DLL_EXPORT HFONT CreateStyleFont(StyleItem * si);
	// Draw a Border
	DLL_EXPORT void CreateBorder(HDC hdc, RECT* p_rect, int borderColour, int borderWidth);

	// ------------------------------------
	// Get position and other info about the toolbar
	typedef struct
	{
		int     width;
		int     height;
		int     position;
		bool    onTop;
		bool    autohide;
		HWND    hwnd;
	} ToolbarInfo;

	DLL_EXPORT void GetToolbarInfo(ToolbarInfo *tbInfo);

	// ------------------------------------
	// Desktop margins:
	// Add / Remove (with margin=0) a screen margin at the indicated location
	DLL_EXPORT RECT* SetDesktopMargin(HWND hwnd, int location, int margin);
	// with hwnd is the plugin's hwnd, location is one of the following:

	enum { BB_DM_TOP, BB_DM_BOTTOM, BB_DM_LEFT, BB_DM_RIGHT, BB_DM_REFRESH = -1 };

	// ------------------------------------
	// Get the current Workspace number and name
	struct strlist { struct strlist *next; char str[1]; };
	class DesktopInfo
	{
	public:
		char name[32];  // name of the desktop
		bool isCurrent; // if it's the current desktop
		int number;     // desktop number
		int ScreensX;   // total number of screens
		strlist *deskNames; // list of all names
	};

	DLL_EXPORT void GetDesktopInfo(DesktopInfo *deskInfo);
	// Also, BB sends a BB_DESKTOPINFO on workspace changes with lParam
	// pointing to a DesktopInfo structure of the new Desktop.

	// ------------------------------------
};

//===========================================================================
#endif /* __BBAPI_H_ */
