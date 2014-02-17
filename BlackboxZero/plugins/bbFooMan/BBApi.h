/*
 ============================================================================

  This file is part of the bbLean source code
  Copyright © 2001-2004 grischka and The Blackbox for Windows Development Team

  http://bb4win.sourceforge.net/bblean
  http://sourceforge.net/projects/bb4win

 ============================================================================

  bbLean and bb4win are free software, released under the GNU General
  Public License (GPL version 2 or later), with an extension that allows
  linking of proprietary modules under a controlled interface. This means
  that plugins etc. are allowed to be released under any license the author
  wishes. For details see:

  http://www.fsf.org/licenses/gpl.html
  http://www.fsf.org/licenses/gpl-faq.html#LinkingOverControlledInterface

  This program is distributed in the hope that it will be useful, but
  WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
  or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License
  for more details.

 ============================================================================
*/

#ifndef __BBAPI_H_
#define __BBAPI_H_

//------------------------------------------
// Compiler specifics
//------------------------------------------
/*
#ifdef __GNUC__
  #define _WIN32_IE 0x0500
  #ifndef __BBCORE__
	#define GetBlackboxPath _GetBlackboxPath
  #endif
#endif

#ifdef __BORLANDC__
  #define DLL_EXPORT
#endif

#ifdef __PELLES__
  #define DLL_EXPORT
#endif

#ifndef DLL_EXPORT
  #define DLL_EXPORT __declspec(dllexport)
#endif
*/
#define DLL_EXPORT

//------------------------------------------
// windows include:
//------------------------------------------

#define WIN32_LEAN_AND_MEAN
#define WINVER 0x0500

#include <windows.h>
#include <stdio.h>

//------------------------------------------
// Blackbox:
//------------------------------------------

#define MAX_LINE_LENGTH 1024

// Gradient types + solid
#define B_HORIZONTAL 0
#define B_VERTICAL   1
#define B_DIAGONAL   2
#define B_CROSSDIAGONAL 3
#define B_PIPECROSS  4
#define B_ELLIPTIC   5
#define B_RECTANGLE  6
#define B_PYRAMID    7
#define B_SOLID      8

// Bevels
#define BEVEL_FLAT   0
#define BEVEL_RAISED 1
#define BEVEL_SUNKEN 2

#define BEVEL1 1
#define BEVEL2 2

// Toolbar Placement
#define PLACEMENT_TOP_LEFT 0
#define PLACEMENT_TOP_CENTER 1
#define PLACEMENT_TOP_RIGHT 2
#define PLACEMENT_BOTTOM_LEFT 3
#define PLACEMENT_BOTTOM_CENTER 4
#define PLACEMENT_BOTTOM_RIGHT 5

//===========================================================================
// Blackbox messages - the soft stuff


#define BB_REGISTERMESSAGE      10001
#define BB_UNREGISTERMESSAGE    10002

// -----------------------------------
#define BB_QUIT                 10101
#define BB_RESTART              10102
#define BB_RECONFIGURE          10103
#define BB_SETSTYLE             10104
#define BB_EXITTYPE             10105   // Param3: 0=Shutdown/Reboot/Logoff 1=Quit 2=Restart
	// Exit Types (wParam for BB_EXITTYPE)
	#define B_SHUTDOWN 0
	#define B_QUIT 1
	#define B_RESTART 2

#define BB_TOOLBARUPDATE        10106
#define BB_XOB_RESERVED1        10107

// -----------------------------------
#define BB_EDITFILE             10201   // Param3: 0=CurrentStyle 1=menu.rc, 2=plugins.rc
#define BB_EXECUTE              10202
#define BB_ABOUTSTYLE           10203
#define BB_ABOUTPLUGINS         10204

// -----------------------------------
#define BB_MENU                 10301   // Param3: 0=Main menu, 1=Workspaces menu, 2=Toolbar menu
	// BB_MENU wParam
	#define BB_MENU_ROOT        0
	#define BB_MENU_TASKS       1
	#define BB_MENU_TOOLBAR     2 // not used any longer
	#define BB_MENU_PLUGIN      3 // not used any longer
	#define BB_MENU_ICONS       4
	#define BB_MENU_CONFIG      5
	#define BB_MENU_BYBROAM     6
	#define BB_MENU_CONTEXT     7
	#define BB_MENU_DTCLICK     8
	#define BB_MENU_FOLDER      9
	#define BB_MENU_ALTMENU    10

	// BB_MENU lParam / BB_BROADCAST wParam
	#define BB_FLAG_FROMKBD     1

#define BB_HIDEMENU             10302
#define BB_TOGGLETRAY           10303   // bb4win
#define BB_TOGGLESYSTEMBAR      10303   // xoblite
#define BB_SETTOOLBARLABEL      10304   // Used to set the toolbar label (returns to normal after 2 seconds)
#define BB_TOGGLEPLUGINS        10305
#define BB_SUBMENU              10306
#define BB_TOGGLESLIT           10307   // Added to avoid conflicts with xoblite
#define BB_TOGGLETOOLBAR        10308   // Added to avoid conflicts with xoblite

// -----------------------------------
#define BB_SHUTDOWN             10401   // Param3: 0=Shutdown, 1=Reboot, 2=Logoff, 3=Hibernate, 4=Suspend, 5=LockWorkstation
#define BB_RUN                  10402
#define BB_COMMAND              10403

// -----------------------------------
#define BB_DESKTOPINFO          10501
#define BB_LISTDESKTOPS         10502
#define BB_SWITCHTON            10503
#define BB_BRINGTOFRONT         10504
	// wParam flag: Activate window in current workspace
	#define BBBTF_CURRENT 4

// -----------------------------------
#define BB_WORKSPACE            10505
	// wParam values for BB_WORKSPACE:
	#define BBWS_DESKLEFT           0
	#define BBWS_DESKRIGHT          1
	#define BBWS_ADDDESKTOP         2
	#define BBWS_DELDESKTOP         3
	#define BBWS_SWITCHTON          4
	#define BBWS_GATHERWINDOWS      5
	#define BBWS_MOVEWINDOWLEFT     6
	#define BBWS_MOVEWINDOWRIGHT    7
	#define BBWS_NEXTWINDOW         8
	#define BBWS_PREVWINDOW         9
	#define BBWS_MAKEITSTICKY      10
	#define BBWS_CLEARSTICKYNESS   11
	#define BBWS_TOGGLESTICKYNESS  12
	#define BBWS_EDITNAME          13

//------------------------------------------
#define BB_TASKSUPDATE          10506
	// Task (lParam for BB_TASKSUPDATE)
	#define TASKITEM_ADDED 0        // ^= HSHELL_WINDOWCREATED   wParam: hwnd
	#define TASKITEM_MODIFIED 1     // ^= HSHELL_REDRAW          wParam: hwnd
	#define TASKITEM_ACTIVATED 2    // ^= HSHELL_WINDOWACTIVATED wParam: hwnd
	#define TASKITEM_REMOVED 3      // ^= HSHELL_WINDOWDESTROYED wParam: hwnd
	#define TASKITEM_REFRESH 4      // Workspace changed    wParam: NULL
	#define TASKITEM_FLASHED 5      // ^= HSHELL_REDRAW|0x8000   wParam: hwnd

#define BB_TRAYUPDATE           10507
	// Tray (lParam for BB_TRAYUPDATE)
	#define TRAYICON_ADDED 0
	#define TRAYICON_MODIFIED 1
	#define TRAYICON_REMOVED 2
	#define TRAYICON_REFRESH 3

#define BB_CLEANTRAY            10508

// -----------------------------------
#define BB_ADDTASK              10601  // ShellHook messages, obsolete
#define BB_REMOVETASK           10602  // ...use BB_TASKSUPDATE instead
#define BB_ACTIVATESHELLWINDOW  10603
#define BB_ACTIVETASK           10604
#define BB_MINMAXTASK           10605

#define BB_WINDOWSHADE          10606
#define BB_WINDOWGROWHEIGHT     10607
#define BB_WINDOWGROWWIDTH      10608
#define BB_WINDOWLOWER          10609

#define BB_REDRAW               10610  // ShellHook messages, obsolete
#define BB_MINIMIZE             10611  // obsolete, use BB_WINDOWMINIMIZE

#define BB_WINDOWMINIMIZE       10611
#define BB_WINDOWRAISE          10612
#define BB_WINDOWMAXIMIZE       10613
#define BB_WINDOWRESTORE        10614
#define BB_WINDOWCLOSE          10615

// -----------------------------------
#define BB_BROADCAST            10901   // Broadcast messages (bro@m -> the bang killah! :D <vbg>)

// -----------------------------------
// BBSlit messages (so we don't use them on accident)
#define SLIT_ADD                11001
#define SLIT_REMOVE             11002
#define SLIT_UPDATE             11003

// -----------------------------------
// pluginInfo
#define PLUGIN_NAME         1
#define PLUGIN_VERSION      2
#define PLUGIN_AUTHOR       3
#define PLUGIN_RELEASE      4   // BB4Win
#define PLUGIN_RELEASEDATE  4   // xoblite
#define PLUGIN_LINK         5
#define PLUGIN_EMAIL        6
#define PLUGIN_BROAMS       7   // xoblite
#define PLUGIN_UPDATE_URL   8   // for Kaloth's BBPlugManager

//===========================================================================
// BBLean additions

#define BB_REDRAWGUI            10881   // for bbStylemaker & bbLeanSkin
	// BB_REDRAWGUI wParam bitflags:
	#define BBRG_TOOLBAR (1<<0)
	#define BBRG_MENU    (1<<1)
	#define BBRG_WINDOW  (1<<2)
	#define BBRG_DESK    (1<<3)
	#define BBRG_FOCUS   (1<<4)
	#define BBRG_PRESSED (1<<5)
	#define BBRG_STICKY  (1<<6)
	#define BBRG_FOLDER  (1<<7)

#define BB_AUTORAISE            10884   // sent on Desktop - left-click
#define BB_GETVERSION           10885   // Get BB-Branch and version
	// returns MAKELPARAM(version, branch), where
	// branch is:
	//  0: 0.0.9X
	//  2: bbLean, with version is 0x0113 for bblean 1.13

#define BB_WINKEY               10886   // for bbKeys - left/right winkey on win9x
#define BB_GETSTYLE             10887   // PostMessage(GetBBWnd(), BB_GETSTYLE, 0, (LPARAM)my_hwnd);
#define BB_GETSTYLESTRUCT       10888   // etc.
#define BB_SETSTYLESTRUCT       10889   // etc.

// -----------------------------------
// internal use
#ifdef __BBCORE__
#define BB_CHANGEDMENU          10910   // folder changed
#define BB_DRAGOVERMENU         10911   // dragging over menu
#define BB_DESTROYMENU          10912   // destroy menu
#define BB_CLOSEMENU            10913   // destroy menu
#define BB_CONFIGUPDATE         10914   // update configuration menu
#define BB_MENUCOMMAND          10915   // destroy menu
#endif

// -----------------------------------
// done with BB_XXXXXXX

#define BB_MSGFIRST             10000
#define BB_MSGLAST              13000

//===========================================================================
// extended Style-info for convenience and other purposes (backwards compatible)

typedef struct StyleItem
{
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
	char Font[256-20];

	bool bordered;
	bool reserved1;
	bool reserved2;
	bool reserved3;

	int borderWidth;
	COLORREF borderColor;
	int marginWidth;
	COLORREF bulletColor;
	//bool FontShadow; xoblite extension
} StyleItem;

#define PicColor TextColor

//class Menu;
//class MenuItem;

//===========================================================================
// constants for GetSettingPtr(int index) -> returns:
enum {

	SN_STYLESTRUCT      = 0   , // StyleStruct *

	SN_TOOLBAR          = 1   , // StyleItem *
	SN_TOOLBARBUTTON          , // StyleItem *
	SN_TOOLBARBUTTONP         , // StyleItem *
	SN_TOOLBARLABEL           , // StyleItem *
	SN_TOOLBARWINDOWLABEL     , // StyleItem *
	SN_TOOLBARCLOCK           , // StyleItem *
	SN_MENUTITLE              , // StyleItem *
	SN_MENUFRAME              , // StyleItem *
	SN_MENUHILITE             , // StyleItem *

	SN_MENUBULLET             , // char *
	SN_MENUBULLETPOS            // char *
							  ,
	SN_BORDERWIDTH            , // int *
	SN_BORDERCOLOR            , // COLORREF *
	SN_BEVELWIDTH             , // int *
	SN_FRAMEWIDTH             , // int *
	SN_HANDLEWIDTH            , // int *
	SN_ROOTCOMMAND            , // char *

	SN_MENUALPHA              , // int *
	SN_TOOLBARALPHA           , // int *
	SN_METRICSUNIX            , // bool *
	SN_BULLETUNIX             , // bool *

	SN_WINFOCUS_TITLE         , // StyleItem *
	SN_WINFOCUS_LABEL         , // StyleItem *
	SN_WINFOCUS_HANDLE        , // StyleItem *
	SN_WINFOCUS_GRIP          , // StyleItem *
	SN_WINFOCUS_BUTTON        , // StyleItem *
	SN_WINFOCUS_BUTTONP         // StyleItem *
							  ,
	SN_WINUNFOCUS_TITLE       , // StyleItem *
	SN_WINUNFOCUS_LABEL       , // StyleItem *
	SN_WINUNFOCUS_HANDLE      , // StyleItem *
	SN_WINUNFOCUS_GRIP        , // StyleItem *
	SN_WINUNFOCUS_BUTTON      , // StyleItem *

	SN_WINFOCUS_FRAME_COLOR   , // COLORREF *
	SN_WINUNFOCUS_FRAME_COLOR , // COLORREF *
	SN_MENUFRAME_DISABLECOLOR , // COLORREF *
	SN_LAST
};

//===========================================================================
// StyleItem -> 'valdated' flags

#define VALID_TEXTURE       (1<<0)  // gradient definition
#define VALID_COLORFROM     (1<<1)  // Color
#define VALID_COLORTO       (1<<2)  // ColorTo
#define VALID_TEXTCOLOR     (1<<3)  // TextColor
#define VALID_PICCOLOR      (1<<4)  // PicColor
#define VALID_FONTWEIGHT    (1<<5)  // FontWeight
#define VALID_FONTHEIGHT    (1<<6)  // FontHeight
#define VALID_FONT          (1<<7)  // Font
#define VALID_JUSTIFY       (1<<8)  // Justify

#define VALID_BULLETCOLOR   (1<<9)  // bulletColor
#define VALID_BORDER        (1<<10)
#define VALID_MARGIN        (1<<11)

//===========================================================================
// Exported functions - the hard stuff

#ifdef __BBCORE__
#define API_EXPORT DLL_EXPORT
#else
#define API_EXPORT
#endif

//extern "C" {
	// ------------------------------------
	// Read Settings
	API_EXPORT bool ReadBool(LPCSTR filePointer, LPCSTR string, bool defaultBool);
	API_EXPORT int CDECL ReadInt(LPCSTR filePointer, LPCSTR string, int defaultInt);
	API_EXPORT LPCSTR ReadString(LPCSTR filePointer, LPCSTR string, LPCSTR defaultString);
	API_EXPORT COLORREF ReadColor(LPCSTR filePointer, LPCSTR string, LPCSTR defaultString);

	// Read a rc-value as string. If 'ptr' is specified, it can read a sequence of items with the same name.
	API_EXPORT LPCSTR ReadValue(LPCSTR fp, LPCSTR keyword, LPLONG ptr = NULL);

	// Was the last Value actually read from the rc-file ?
	// Returns: 0 == not found / 1 == found exact value / 2 == found wildcard
	API_EXPORT int CDECL FoundLastValue(void);

	// ------------------------------------
	// Write Settings
	API_EXPORT void CDECL CDECL WriteBool(LPCSTR filePointer, LPCSTR keyword, bool value);
	API_EXPORT void CDECL CDECL WriteInt(LPCSTR filePointer, LPCSTR keyword, int value);
	API_EXPORT void CDECL CDECL WriteString(LPCSTR filePointer, LPCSTR keyword, LPCSTR value);
	API_EXPORT void CDECL CDECL WriteColor(LPCSTR filePointer, LPCSTR keyword, COLORREF value);

	// ------------------------------------
	// Delete Setting (wildcards supported)

	API_EXPORT int CDECL DeleteSetting(LPCSTR fname, LPCSTR keyword);
	// Rename Setting (wildcards supported) e.g. RenameSetting("file.rc", "bbtest.*.setting:", "bbtest.*.renamed:");
	API_EXPORT int CDECL RenameSetting(LPCSTR fname, LPCSTR old_keyword, LPCSTR new_keyword);

	// ------------------------------------
	// Direct access to Settings variables / styleitems / colors
	// See the SN_XXX constants for possible values
	API_EXPORT void CDECL* GetSettingPtr(int index);

	// ------------------------------------
	// File functions
	API_EXPORT bool FileExists(LPCSTR szFileName);
	API_EXPORT FILE *FileOpen(LPCSTR fileName);
	API_EXPORT bool FileClose(FILE *filePointer);
	API_EXPORT bool FileRead(FILE *filePointer, LPSTR readString);
	API_EXPORT bool ReadNextCommand(FILE *filePointer, LPSTR readLine, DWORD lineLength);

	API_EXPORT bool FindConfigFile(LPSTR pszOut, LPCSTR filename, LPCSTR pluginDir);
	API_EXPORT LPCSTR ConfigFileExists(LPCSTR filename, LPCSTR pluginDir);

	// ------------------------------------
	// Make a window visible on all workspaces
	API_EXPORT void CDECL MakeSticky(HWND window);
	API_EXPORT void CDECL RemoveSticky(HWND window);
	API_EXPORT bool CheckSticky(HWND window);

	// ------------------------------------
	// Window utilities

	API_EXPORT int CDECL GetAppByWindow(HWND Window, char*);
	API_EXPORT bool IsAppWindow(HWND hwnd);
	API_EXPORT void CDECL GetMonitorRect(void *point_or_hwnd, RECT *r, int flags);
	#define GETMON_FROM_WINDOW 0
	#define GETMON_FROM_POINT 1
	#define GETMON_WORKAREA 2

	API_EXPORT void CDECL SnapWindowToEdge(WINDOWPOS* windowPosition, LPARAM nDist_or_pContent, UINT Flags);
	// Flags:
	#define SNAP_FULLSCREEN 1  // rather than to workarea
	#define SNAP_NOPLUGINS  2  // dont snap to other plugins
	#define SNAP_CONTENT    4  // the "nDist_or_pContent" parameter points to a SIZE struct
	#define SNAP_NOPARENT   8  // dont snap to parent window edges
	#define SNAP_SIZING    16  // snap, while window is resized (bottom-right only)

	API_EXPORT bool SetTransparency(HWND hwnd, BYTE alpha);

	// ------------------------------------
	// Desktop margins:

	// Add/Remove (with margin=0) a screen margin at the indicated location
	API_EXPORT void CDECL SetDesktopMargin(HWND hwnd, int location, int margin);

	// with hwnd is the plugin's hwnd, location is one of the following:
	enum { BB_DM_TOP, BB_DM_BOTTOM, BB_DM_LEFT, BB_DM_RIGHT, BB_DM_REFRESH = -1 };

	// ------------------------------------
	// Get paths
	API_EXPORT void CDECL GetBlackboxEditor(LPSTR editor);
	API_EXPORT bool WINAPI GetBlackboxPath(LPSTR path, int maxLength);

	API_EXPORT LPCSTR bbrcPath(LPCSTR bbrcFileName = NULL);
	API_EXPORT LPCSTR extensionsrcPath(LPCSTR extensionsrcFileName = NULL);
	API_EXPORT LPCSTR menuPath(LPCSTR menurcFileName = NULL);
	API_EXPORT LPCSTR plugrcPath(LPCSTR pluginrcFileName = NULL);
	API_EXPORT LPCSTR stylePath(LPCSTR styleFileName = NULL);

	// ------------------------------------
	// Get the main window and other info
	API_EXPORT HWND GetBBWnd();
	API_EXPORT LPCSTR GetBBVersion();
	API_EXPORT LPCSTR GetOSInfo();
	API_EXPORT bool GetUnderExplorer();

	// ------------------------------------
	// String tokenizing
	API_EXPORT LPSTR Tokenize(LPCSTR sourceString, LPSTR targetString, LPSTR delimiter);
	API_EXPORT int CDECL BBTokenize (LPCSTR sourceString, LPSTR* targetStrings, DWORD numTokensToParse, LPSTR remainingString);

	// ------------------------------------
	// Shell execute a command
	API_EXPORT BOOL BBExecute(HWND Owner, LPCSTR szOperation, LPCSTR szCommand, LPCSTR szArgs, LPCSTR szDirectory, int nShowCmd, bool noErrorMsgs);

	// ------------------------------------
	// Logging and error messages
	API_EXPORT void CDECL Log(LPCSTR variable, LPCSTR description);
	API_EXPORT int CDECL MBoxErrorFile(LPCSTR szFile);
	API_EXPORT int CDECL MBoxErrorValue(LPCSTR szValue);
	// debugging
	API_EXPORT void CDECL dbg_printf(const char *fmt, ...);

	// ------------------------------------
	// Helpers
	API_EXPORT void CDECL ParseItem(LPCSTR szItem, StyleItem *item);
	API_EXPORT bool IsInString(LPCSTR inputString, LPCSTR searchString);
	API_EXPORT LPSTR StrRemoveEncap(LPSTR string);
	API_EXPORT void CDECL ReplaceEnvVars(LPSTR string);
	API_EXPORT void CDECL ReplaceShellFolders(LPSTR string);

	// ------------------------------------
	// Painting
	API_EXPORT void CDECL MakeGradient(HDC hdc, RECT rect, int gradientType, COLORREF colourFrom, COLORREF colourTo, bool interlaced, int bevelStyle, int bevelPosition, int bevelWidth, COLORREF borderColour, int borderWidth);
	// Draw a Gradient Rectangle from StyleItem, optional using the style border.
	API_EXPORT void CDECL MakeStyleGradient(HDC hDC, RECT* p_rect, StyleItem * m_si, bool withBorder);
	// Create a font handle from styleitem, with parsing and substitution.
	API_EXPORT HFONT CreateStyleFont(StyleItem * si);
	// Draw a Border
	API_EXPORT void CDECL CreateBorder(HDC hdc, RECT* p_rect, COLORREF borderColour, int borderWidth);

	// ------------------------------------

	// ------------------------------------
	// Plugin - Menus

	// MakeNamedMenu for automatic updating menues. Use instead of Makemenu.
	// See the SDK for examples.

	API_EXPORT HANDLE MakeNamedMenu(LPCSTR HeaderText, LPCSTR Id, bool forceshow);

	// inserts an item to execute a command or to set a boolean value
	API_EXPORT HANDLE MakeMenuItem(HANDLE *PluginMenu, LPCSTR Title, LPCSTR Cmd, bool ShowIndicator);
/*
	// inserts an inactive item, optionally with text. "Title" may be NULL.
	API_EXPORT MenuItem *MakeMenuNOP(Menu *PluginMenu, LPCSTR Title);

	// inserts an item to adjust a numeric value (implemented in bblean for now)
	API_EXPORT MenuItem *MakeMenuItemInt(Menu *PluginMenu, LPCSTR Title, LPCSTR Cmd, int val, int minval, int maxval);

	// inserts an item to edit a string value (not yet implemented)
	API_EXPORT MenuItem *MakeMenuItemString(Menu *PluginMenu, LPCSTR Title, LPCSTR Cmd, LPCSTR init_string);

	// inserts an item, which opens a submenu
	API_EXPORT MenuItem *MakeSubmenu(Menu *ParentMenu, Menu *ChildMenu, LPCSTR Title);

	// shows the menu*/
	API_EXPORT void CDECL ShowMenu(HANDLE *PluginMenu);
/*
	// ------------------------------------
	// obsolete with MakeNamedMenu:
	API_EXPORT Menu *MakeMenu(LPCSTR HeaderText);
	API_EXPORT void CDECL DelMenu(Menu *PluginMenu);

	// ------------------------------------
	// experimental
	void RegisterMenu(Menu *PluginMenu, int id_menu);
	//MenuItem* MakePathmenu(Menu *ParentMenu, LPCSTR Title, LPCSTR path, int id);
	//void MakeContextmenu(MenuItem *ParentItem, Menu *ChildMenu, LPCSTR Title);

	// ------------------------------------

	// ------------------------------------
	// Tray icon access
*/
	struct systemTray
	{
		HWND    hWnd;
		UINT    uID;
		UINT    uCallbackMessage;
		HICON   hIcon;
		char    szTip[256];
	};

	API_EXPORT int CDECL GetTraySize();
//	API_EXPORT systemTray* GetTrayIcon(int pointer);

	// ------------------------------------

	// ------------------------------------
	// Task items access

	struct tasklist // current 0.90 compatibility outlay
	{
		struct tasklist* next;
		HWND    hwnd;
		HICON   icon;
		int     wkspc;
		char    caption[248];
		bool    active;
		bool    flashing;

		bool    _former_hidden;
		bool    _former_reserved;
		HICON   _former_icon_big;
	};

	// get the size
	API_EXPORT int CDECL GetTaskListSize();

	// get task's HWND by index
	API_EXPORT HWND GetTask(int index);
	//renamed: API_EXPORT HWND GetTaskWindow(int index);
	#define GetTaskWindow GetTask

	// get the index of the currently active task
	API_EXPORT int CDECL GetActiveTask();

	// Retrieve a pointer to the internal TaskList.
	API_EXPORT struct tasklist *GetTaskListPtr();

	struct taskinfo
	{
		RECT r;
		int desk;
		bool active;
		bool iconic;

		bool set_position;
		bool set_desk;
		bool switch_to;
	};

	// get workspace and original reactangle for window
	API_EXPORT int CDECL GetTaskInfo(HWND, struct taskinfo *tlp);

	// set workspace and position for window (i.e. move task-windows over workspaces)
	API_EXPORT int CDECL SetTaskInfo(HWND, struct taskinfo *tlp);

	// ------------------------------------
	// experimental
	typedef BOOL (*TASKENUMPROC)(struct tasklist *, LPARAM);
	API_EXPORT void CDECL EnumTasks (TASKENUMPROC lpEnumFunc, LPARAM lParam);

	typedef BOOL (*TRAYENUMPROC)(struct systemTray *, LPARAM);
	API_EXPORT void CDECL EnumTray (TRAYENUMPROC lpEnumFunc, LPARAM lParam);

	// ------------------------------------
	// Get the workspace number for a task
	API_EXPORT  int GetTaskWorkspace(HWND hwnd);

	// For backwards compatibility only, depreciated!
	API_EXPORT void CDECL SetTaskWorkspace(HWND hwnd, int workspace);

	// ------------------------------------

	// ------------------------------------
	// Desktop Information:
	typedef struct string_node { struct string_node *next; char str[1]; } string_node;

	typedef struct DesktopInfo
	{
		char name[32];  // name of the desktop
		bool isCurrent; // if it's the current desktop
		int number;     // desktop number
		int ScreensX;   // total number of screens
		string_node *deskNames; // list of all names
	} DesktopInfo;

	// Get the current Workspace number and name
	API_EXPORT void CDECL GetDesktopInfo(DesktopInfo *deskInfo);

	// Also, BB sends a BB_DESKTOPINFO message on workspace changes
	// with lParam pointing to a DesktopInfo structure of the new Desktop.

	typedef BOOL (*DESKENUMPROC)(struct DesktopInfo *, LPARAM);
	API_EXPORT void CDECL EnumDesks (DESKENUMPROC lpEnumFunc, LPARAM lParam);

	// ------------------------------------

	// ------------------------------------
	// Get position and other info about the toolbar

	typedef struct
	{
		// rc - settings
		int     placement;      // 0 - 5 (top-left/center/right, bottom-left/center/right)
		int     widthPercent;   // 0 - 100
		bool    onTop;
		bool    autoHide;
		bool    pluginToggle;
		bool    disabled;       // completely

		bool    transparency;
		BYTE    alphaValue;     // 0 - 255 (255 = transparency off)

		// current status
		bool    autohidden;     // currently by autoHide
		bool    hidden;         // currently by pluginToggle
		int     xpos;
		int     ypos;
		int     width;
		int     height;
		HWND    hwnd;

		// bbsb info
		HWND    bbsb_hwnd;
		bool    bbsb_linkedToToolbar;
		bool    bbsb_reverseTasks;
		bool    bbsb_currentOnly;
	 } ToolbarInfo;

	// retrieve a pointer to core's static ToolbarInfo
	API_EXPORT ToolbarInfo *GetToolbarInfo(void);

	// ------------------------------------
	// plugin interface

	DLL_EXPORT int CDECL beginSlitPlugin(HINSTANCE, HWND hSlit);
	DLL_EXPORT int CDECL beginPluginEx(HINSTANCE, HWND hSlit);
	DLL_EXPORT int CDECL beginPlugin(HINSTANCE);
	DLL_EXPORT void CDECL endPlugin(HINSTANCE);
	DLL_EXPORT LPCSTR CDECL pluginInfo(int field);
//};

//===========================================================================
#endif /* __BBAPI_H_ */
