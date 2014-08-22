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
#include "Settings.h"
#include "bbPager.h"

extern struct POSITION position;
extern int xpos, ypos;
extern struct DESKTOP desktop;
extern struct FRAME frame;
extern bool usingAltMethod;
extern bool usingWin2kXP;
extern HWND hwndBBPager;
extern bool is_xoblite;
extern struct ACTIVEDESKTOP activeDesktop;
extern struct WINDOW window;
extern struct FOCUSEDWINDOW focusedWindow;
//===========================================================================

char rcpath[MAX_PATH];
char editor[MAX_PATH];

//struct POSITION position;
int desktopChangeButton;
int focusButton;
int moveButton;

// Window information
int screenWidth, screenHeight, screenLeft, screenTop, screenRight, screenBottom;
int vScreenWidth, vScreenHeight, vScreenLeft, vScreenTop, vScreenRight, vScreenBottom;
double ratioX, ratioY;
int leftMargin, topMargin;
bool drawBorder;

// Transparency
bool transparency;
int transparencyAlpha;

bool useSlit;

//===========================================================================

char bspath[MAX_PATH];
char * getbspath () { return bspath; }
char stylepath[MAX_PATH];

//===========================================================================
#define IS_SLASH(c) ((c) == '\\' || (c) == '/')

int File_Exists(const char* szFileName)
{
    DWORD a = GetFileAttributes(szFileName);
    return (DWORD)-1 != a && 0 == (a & FILE_ATTRIBUTE_DIRECTORY);
}

int locate_file(HINSTANCE hInstance, char *path, const char *fname, const char *ext)
{
	GetModuleFileName(hInstance, path, MAX_PATH);
	char *file_name_start = strrchr(path, '\\');
	if (file_name_start) ++file_name_start;
	else file_name_start = strchr(path, 0);
	sprintf(file_name_start, "%s.%s", fname, ext);
	return File_Exists(path);
}

const char *file_basename(const char *path)
{
    int nLen = strlen(path);
    while (nLen && !IS_SLASH(path[nLen-1])) nLen--;
    return path + nLen;
}

int imin(int a, int b)
{
    return a<b?a:b;
}

void ReadRCSettings()
{
	char temp[16];

	// If a config file was not found we use the defaults...
		
	// Position of BBPager window
	int n, n1, n2;
	strcpy(temp, ReadString(rcpath, "bbPager.position:", ""));
	n = sscanf(temp, "+%d-%d", &n1, &n2);

	if (n == 2)
	{
		position.unix = true;
		xpos = position.x = position.ox = n1;
		ypos = position.y = position.oy = n2;
	}
	else 
	{
		position.unix = false;
		xpos = position.x = position.ox = ReadInt(rcpath, "bbPager.position.x:", 895);
		ypos = position.y = position.oy = ReadInt(rcpath, "bbPager.position.y:", 10);
	}

	position.raised = ReadBool(rcpath, "bbPager.raised:", true);
	position.snapWindow = ReadInt(rcpath, "bbPager.snapWindow:", 20);

	// BBPager metrics
	desktop.width = ReadInt(rcpath, "bbPager.desktop.width:", 40);
	desktop.height  = ReadInt(rcpath, "bbPager.desktop.height:", 30);

	UpdateMonitorInfo();

	desktop.sizeRatio = ReadInt(rcpath, "bbPager.sizeRatio:", 20);
	desktop.width = vScreenWidth / desktop.sizeRatio;
	desktop.height = vScreenHeight / desktop.sizeRatio;

	// get mouse button for desktop changing, etc., 1 = LMB, 2 = Middle, 3 = RMB
	desktopChangeButton = ReadInt(rcpath, "bbPager.desktopChangeButton:", 2);

	focusButton = ReadInt(rcpath, "bbPager.windowFocusButton:", 1);

	moveButton = ReadInt(rcpath, "bbPager.windowMoveButton:", 3);
 
	// default BB editor
	GetBlackboxEditor(editor);
	if (strlen(editor) < 2)
		strcpy(editor, "notepad.exe");

	//get vertical or horizontal alignment setting
	strcpy(temp, ReadString(rcpath, "bbPager.alignment:", "horizontal"));
	if (!_stricmp(temp, "vertical")) 
	{
		position.horizontal = false;
		position.vertical = true;
	}
	else 
	{
		position.horizontal = true;
		position.vertical = false;
	}

	// row and column number
	frame.columns = ReadInt(rcpath, "bbPager.columns:", 1);
	frame.rows = ReadInt(rcpath, "bbPager.rows:", 1);

	if (frame.rows < 1) frame.rows = 1;
	if (frame.columns < 1) frame.columns = 1;

	//numbers on desktop enable
	desktop.numbers = ReadBool(rcpath, "bbPager.desktopNumbers:", false);

	//windows on desktop enable
	desktop.windows = ReadBool(rcpath, "bbPager.desktopWindows:", true);

	desktop.tooltips = ReadBool(rcpath, "bbPager.windowToolTips:", false);

	// Autohide enable
	position.autohide = ReadBool(rcpath, "bbPager.autoHide:", false);

	topMargin = ReadInt(extensionsrcPath(), "blackbox.desktop.marginTop:", 0);
	leftMargin = ReadInt(extensionsrcPath(), "blackbox.desktop.marginLeft:", 0);
	usingAltMethod = ReadBool(extensionsrcPath(), "blackbox.workspaces.altMethod:", false);

	// Transparency
	transparency = ReadBool(rcpath, "bbPager.transparency:", false);
	transparencyAlpha = ReadInt(rcpath, "bbPager.transparency.alpha:", 192);

	useSlit = ReadBool(rcpath, "bbPager.useSlit:", false);

	if (!locate_file(hInstance, (char*)rcpath, "bbPager", "rc"))
		WriteRCSettings();
}

//===========================================================================

void WriteRCSettings()
{
	static char szTemp[128];
	static char temp[32];
	DWORD retLength = 0;

	// Write plugin settings to config file, using path found in ReadRCSettings()...
	HANDLE file = CreateFile(rcpath, GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (file)
	{
		// BBPager position
		if (position.unix) 
		{
			sprintf(szTemp, "bbPager.position: +%d-%d\r\n", position.ox, position.oy);
			WriteFile(file, szTemp, strlen(szTemp), &retLength, NULL);
		}
		else 
		{
			sprintf(szTemp, "bbPager.position.x: %d\r\n", position.ox);
			WriteFile(file, szTemp, strlen(szTemp), &retLength, NULL);
			sprintf(szTemp, "bbPager.position.y: %d\r\n", position.oy);
			WriteFile(file, szTemp, strlen(szTemp), &retLength, NULL);
		}

		// desktop size
		sprintf(szTemp, "bbPager.desktop.width: %d\r\n", desktop.width);
		WriteFile(file, szTemp, strlen(szTemp), &retLength, NULL);
		sprintf(szTemp, "bbPager.desktop.height: %d\r\n", desktop.height);
		WriteFile(file, szTemp, strlen(szTemp), &retLength, NULL);
		sprintf(szTemp, "bbPager.desktop.sizeRatio: %d\r\n", desktop.sizeRatio);
		WriteFile(file, szTemp, strlen(szTemp), &retLength, NULL);

		// alignment
		(position.vertical) ? strcpy(temp, "vertical") : strcpy(temp, "horizontal");
		sprintf(szTemp, "bbPager.alignment: %s\r\n", temp);
 		WriteFile(file, szTemp, strlen(szTemp), &retLength, NULL);

		// Write column/row values
		sprintf(szTemp, "bbPager.columns: %d\r\n", frame.columns);
		WriteFile(file, szTemp, strlen(szTemp), &retLength, NULL);

		sprintf(szTemp, "bbPager.rows: %d\r\n", frame.rows);
		WriteFile(file, szTemp, strlen(szTemp), &retLength, NULL);

		// desktop change mouse button, etc.
		sprintf(szTemp, "bbPager.desktopChangeButton: %d\r\n", desktopChangeButton);
		WriteFile(file, szTemp, strlen(szTemp), &retLength, NULL);

		sprintf(szTemp, "bbPager.windowMoveButton: %d\r\n", moveButton);
		WriteFile(file, szTemp, strlen(szTemp), &retLength, NULL);

		sprintf(szTemp, "bbPager.windowFocusButton: %d\r\n", focusButton);
		WriteFile(file, szTemp, strlen(szTemp), &retLength, NULL);

		// Snap window to edge of screen
		sprintf(szTemp, "bbPager.snapWindow: %d\r\n", position.snapWindow);
 		WriteFile(file, szTemp, strlen(szTemp), &retLength, NULL);

		// Always on top
		(position.raised) ? strcpy(temp, "true") : strcpy(temp, "false");
		sprintf(szTemp, "bbPager.raised: %s\r\n", temp);
 		WriteFile(file, szTemp, strlen(szTemp), &retLength, NULL);

		// Autohide
		(position.autohide) ? strcpy(temp, "true") : strcpy(temp, "false");
		sprintf(szTemp, "bbPager.autoHide: %s\r\n", temp);
 		WriteFile(file, szTemp, strlen(szTemp), &retLength, NULL);

		// Transparency
		if (usingWin2kXP)
		{
			(transparency) ? strcpy(temp, "true") : strcpy(temp, "false");
			sprintf(szTemp, "bbPager.transparency: %s\r\n", temp);
 			WriteFile(file, szTemp, strlen(szTemp), &retLength, NULL);

			sprintf(szTemp, "bbPager.transparency.alpha: %d\r\n", transparencyAlpha);
			WriteFile(file, szTemp, strlen(szTemp), &retLength, NULL);
		}

		// Numbers on Desktops
		(desktop.numbers) ? strcpy(temp, "true") : strcpy(temp, "false");
		sprintf(szTemp, "bbPager.desktopNumbers: %s\r\n", temp);
 		WriteFile(file, szTemp, strlen(szTemp), &retLength, NULL);

		// Windows on Desktops
		(desktop.windows) ? strcpy(temp, "true") : strcpy(temp, "false");
		sprintf(szTemp, "bbPager.desktopWindows: %s\r\n", temp);
 		WriteFile(file, szTemp, strlen(szTemp), &retLength, NULL);

		(usingAltMethod) ? strcpy(temp, "true") : strcpy(temp, "false");
		sprintf(szTemp, "bbPager.desktopAltMethod: %s\r\n", temp);
 		WriteFile(file, szTemp, strlen(szTemp), &retLength, NULL);

		(desktop.tooltips) ? strcpy(temp, "true") : strcpy(temp, "false");
		sprintf(szTemp, "bbPager.windowToolTips: %s\r\n", temp);
 		WriteFile(file, szTemp, strlen(szTemp), &retLength, NULL);

		// Are we in the slit?
		(useSlit) ? strcpy(temp, "true") : strcpy(temp, "false");
		sprintf(szTemp, "bbPager.useSlit: %s\r\n", temp);
 		WriteFile(file, szTemp, strlen(szTemp), &retLength, NULL);
 	}
	CloseHandle(file);
}

//===========================================================================

void UpdateMonitorInfo()
{
	// multimonitor
	vScreenLeft = GetSystemMetrics(SM_XVIRTUALSCREEN);
	vScreenTop = GetSystemMetrics(SM_YVIRTUALSCREEN);

	vScreenWidth = GetSystemMetrics(SM_CXVIRTUALSCREEN);
	vScreenHeight = GetSystemMetrics(SM_CYVIRTUALSCREEN);

	vScreenRight = vScreenLeft + vScreenWidth;
	vScreenBottom = vScreenTop + vScreenHeight;

	ratioX = vScreenWidth / desktop.width;
	ratioY = vScreenHeight / desktop.height;

	
	int xScreen, yScreen;
	xScreen = GetSystemMetrics(SM_CXSCREEN);
	yScreen = GetSystemMetrics(SM_CYSCREEN);

	if (vScreenWidth > xScreen || vScreenHeight > yScreen)
	{	// multimon
		// current monitor
		HMONITOR hMon = MonitorFromWindow(hwndBBPager, MONITOR_DEFAULTTONEAREST);
		MONITORINFO mInfo;
		mInfo.cbSize = sizeof(MONITORINFO);
		GetMonitorInfo(hMon, &mInfo);

		screenLeft = mInfo.rcMonitor.left;
		screenRight = mInfo.rcMonitor.right;
		screenTop = mInfo.rcMonitor.top;
		screenBottom = mInfo.rcMonitor.bottom;
		screenWidth = screenRight - screenLeft;
		screenHeight = screenBottom - screenTop;
	}
	else	// single mon (or treat as such)
	{
		vScreenTop = vScreenLeft = 0;

		screenLeft = 0;
		screenRight = screenWidth;
		screenTop = 0;
		screenBottom = screenHeight;
		screenWidth = vScreenWidth = xScreen;
		screenHeight = vScreenHeight = yScreen;

		ratioX = screenWidth / desktop.width;
		ratioY = screenHeight / desktop.height;
	}
}

//===========================================================================

void GetStyleSettings()
{
	char tempstring[32];
	char colorAsString[32] = "#000000";

	// Get the path to the current style file from Blackbox...
	strcpy(stylepath, stylePath());
	// Get the path to the plugin.bb file
	locate_file(hInstance, (char*)bspath, "bbPager", "bb");

	bool nix = false;
	if (!is_xoblite)
		nix = 0 == (int)GetSettingPtr(SN_NEWMETRICS);
	else 
	{
		strcpy(tempstring, ReadString(stylepath, "toolbar.appearance:", "no"));
		if (strlen(tempstring) != 2) nix = true;
	}

//===========================================================
// bbpager.frame: -> this is for the BBPager frame/background

	frame.ownStyle = false;
	frame.Style = *(StyleItem *)GetSettingPtr(SN_TOOLBAR);

	strcpy(tempstring, ReadString(bspath, "bbPager.frame:", ReadString(stylepath, "bbPager.frame:", "no")));
	if (strlen(tempstring) != 2)
 	{
		frame.ownStyle = true;
 		if (frame.style) delete frame.style;
 		frame.style = new StyleItem;
		ParseItem(tempstring, frame.style);
		frame.color = ReadColor(bspath, "bbPager.frame.color:", ReadString(stylepath, "bbPager.frame.color:", colorAsString));
		frame.colorTo = ReadColor(bspath, "bbPager.frame.colorTo:", ReadString(stylepath, "bbPager.frame.colorTo:", colorAsString));
 	}

//===========================================================
// bbpager.frame.borderColor: -> this is the colour for the border around BBPager

	frame.borderColor = ReadColor(bspath, "bbPager.frame.borderColor:", ReadString(stylepath, "bbPager.frame.borderColor:", ReadString(stylepath, "toolbarLabel.TextColor:", ReadString(stylepath, "borderColor:", colorAsString))));

//===========================================================
// bbpager.desktop: -> this is for the normal desktops

	desktop.ownStyle = false;

	desktop.Style = *(StyleItem *)GetSettingPtr(SN_TOOLBARBUTTON);
	if (desktop.Style.parentRelative)
		desktop.Style = *(StyleItem *)GetSettingPtr(SN_TOOLBARLABEL);
 
	strcpy(tempstring, ReadString(bspath, "bbPager.desktop:", ReadString(stylepath, "bbPager.desktop:", "no")));
	if (strlen(tempstring) != 2)
 	{
		desktop.ownStyle = true;
		if (desktop.style) delete desktop.style;
		desktop.style = new StyleItem;
		ParseItem(tempstring, desktop.style);
		desktop.color = ReadColor(bspath, "bbPager.desktop.color:", ReadString(stylepath, "bbPager.desktop.color:", colorAsString));
		desktop.colorTo = ReadColor(bspath, "bbPager.desktop.colorTo:", ReadString(stylepath, "bbPager.desktop.colorTo:", colorAsString));
 	}

	desktop.Style.bevelstyle = BEVEL_RAISED; 
	desktop.Style.bevelposition = BEVEL2; 

//===========================================================
// bbpager.desktop.focusStyle: -> specifies how to draw active desktop - none|border|border2|border3|texture

	strcpy(activeDesktop.styleType, ReadString(bspath, "bbPager.desktop.focusStyle:", ReadString(stylepath, "bbPager.desktop.focusStyle:", "border")));

//===========================================================
// bbpager.desktop.focus: -> style definition used for current workspace

	activeDesktop.ownStyle = false;

	activeDesktop.Style = *(StyleItem *)GetSettingPtr(SN_TOOLBARBUTTONP);
	if (activeDesktop.Style.parentRelative)
		activeDesktop.Style = *(StyleItem *)GetSettingPtr(SN_TOOLBARWINDOWLABEL);

	strcpy(tempstring, ReadString(bspath, "bbPager.desktop.focus:", ReadString(stylepath, "bbPager.desktop.focus:", "no")));
	if (strlen(tempstring) != 2)
 	{
		activeDesktop.ownStyle = true;
		if (activeDesktop.style) delete activeDesktop.style;
		activeDesktop.style = new StyleItem;
		ParseItem(tempstring, activeDesktop.style);
		activeDesktop.color = ReadColor(bspath, "bbPager.desktop.focus.color:", ReadString(stylepath, "bbPager.desktop.focus.color:", colorAsString));
		activeDesktop.colorTo = ReadColor(bspath, "bbPager.desktop.focus.colorTo:", ReadString(stylepath, "bbPager.desktop.focus.colorTo:", colorAsString));
 	}

	activeDesktop.Style.bevelstyle = BEVEL_SUNKEN; 

//===========================================================
// bbpager.active.desktop.borderColor:

	activeDesktop.borderColor = ReadColor(bspath, "bbPager.active.desktop.borderColor:", ReadString(stylepath, "bbPager.active.desktop.borderColor:", ReadString(stylepath, "borderColor:", colorAsString)));
	if (activeDesktop.borderColor == 0x000000)
		activeDesktop.borderColor = activeDesktop.Style.TextColor;

//===========================================================
// frame.bevelWidth: spacing between desktops and edges of BBPager
	
	frame.bevelWidth = frame.ownStyle ? ReadInt(bspath, "bbPager.bevelwidth:", *(int *)GetSettingPtr(SN_BEVELWIDTH)) : (nix ? frame.Style.marginWidth : *(int *)GetSettingPtr(SN_BEVELWIDTH));

//===========================================================
// frame.borderWidth: width of border around BBPager

	frame.borderWidth = frame.ownStyle ? ReadInt(bspath, "bbPager.frame.borderWidth:", *(int *)GetSettingPtr(SN_BORDERWIDTH)) : (nix ? frame.Style.borderWidth : *(int *)GetSettingPtr(SN_BORDERWIDTH));

//===========================================================
// frame.hideWidth: amount of pager seen in the hidden state

	frame.hideWidth = imin(frame.bevelWidth + frame.borderWidth, 3);

//===========================================================
// bbpager.window: -> this is for the normal windows

	window.Style = *(StyleItem *)GetSettingPtr(SN_MENUFRAME);

	window.ownStyle = false;
	if (window.style) delete window.style;
	window.style = new StyleItem;

	strcpy(tempstring, ReadString(bspath, "bbPager.window:", "no"));	
	if (strlen(tempstring) == 2) // plugin.window NOT in BB
	{
		strcpy(tempstring, ReadString(stylepath, "bbPager.window:", "no"));	
		if (strlen(tempstring) != 2) // plugin.window IS in STYLE
			window.ownStyle = true;
	}
	else // plugin.window IS in BB
		window.ownStyle = true;

	bool labelPR = false;
	strcpy(tempstring, ReadString(bspath, "window.label.unfocus:", ReadString(stylepath, "window.label.unfocus.appearance:", "parentrelative")));
	if (strlen(tempstring) == 14)
		labelPR = true;

	if (window.ownStyle)
	{
		if (labelPR)
		{
			strcpy(tempstring, ReadString(bspath, "bbPager.window:", ReadString(stylepath, "bbPager.window:", ReadString(bspath, "window.title.unfocus:", ReadString(bspath, "window.title.unfocus.appearance:", "Sunken Bevel2 Gradient Vertical")))));
			ParseItem(tempstring, window.style);

			window.color = ReadColor(bspath, "bbPager.window.color:", ReadString(stylepath, "bbPager.window.color:", ReadString(stylepath, "window.title.unfocus.color:", ReadString(stylepath, "window.title.unfocus.color1:", ReadString(stylepath, "window.title.unfocus.backgroundColor:", colorAsString)))));
			if (window.style->type == B_SOLID)
				window.colorTo = window.color;
			else
				window.colorTo = ReadColor(bspath, "bbPager.window.colorTo:", ReadString(stylepath, "bbPager.window.colorTo:", ReadString(stylepath, "window.title.unfocus.colorTo:", ReadString(stylepath, "window.title.unfocus.color2:", colorAsString))));
		}
		else
		{
			window.color = ReadColor(bspath, "bbPager.window.color:", ReadString(stylepath, "bbPager.window.color:", ReadString(stylepath, "window.label.unfocus.color:", ReadString(stylepath, "window.label.unfocus.color1:", ReadString(stylepath, "window.label.unfocus.backgroundColor:", colorAsString)))));
			if (window.style->type == B_SOLID)
				window.colorTo = window.color;
			else
				window.colorTo = ReadColor(bspath, "bbPager.window.colorTo:", ReadString(stylepath, "bbPager.window.colorTo:", ReadString(stylepath, "window.label.unfocus.colorTo:", ReadString(stylepath, "window.label.unfocus.color2:", colorAsString))));
		}
	}

	window.Style.bevelstyle = BEVEL_SUNKEN; 

//===========================================================
// bbpager.inactive.window.borderColor:

	window.borderColor = ReadColor(bspath, "bbPager.inactive.window.borderColor:", ReadString(stylepath, "bbPager.inactive.window.borderColor:", ReadString(stylepath, "window.label.unfocus.TextColor:", colorAsString)));
	if (window.borderColor == 0x000000)
		window.borderColor = activeDesktop.Style.Color;

//===========================================================
// bbpager.window.focusStyle: -> specifies how to draw active window - none|border|border2|border3|texture

	strcpy(focusedWindow.styleType, ReadString(bspath, "bbPager.window.focusStyle:", ReadString(stylepath, "bbPager.window.focusStyle:", "texture")));

//===========================================================
// bbpager.window.focus: -> style definition used for active window

	focusedWindow.Style = *(StyleItem *)GetSettingPtr(SN_MENUHILITE);

	focusedWindow.ownStyle = false;
	if (focusedWindow.style) delete focusedWindow.style;
	focusedWindow.style = new StyleItem;

	strcpy(tempstring, ReadString(bspath, "bbPager.window.focus:", "no"));	
	if (strlen(tempstring) == 2) // plugin.active.desktop NOT in BB
	{
		strcpy(tempstring, ReadString(stylepath, "bbPager.window.focus:", "no"));	
		if (strlen(tempstring) != 2) // plugin.window.focus IS in STYLE
			focusedWindow.ownStyle = true;
	}
	else // plugin.active.desktop IS in BB
		focusedWindow.ownStyle = true;

	if (focusedWindow.ownStyle)
	{
		if (labelPR)
		{
			strcpy(tempstring, ReadString(bspath, "bbPager.focus.window:", ReadString(stylepath, "bbPager.focus.window:", ReadString(bspath, "window.title.focus:", ReadString(bspath, "window.title.focus.appearance:", "Raised Bevel2 Gradient Vertical")))));
			ParseItem(tempstring, focusedWindow.style);

			focusedWindow.color = ReadColor(bspath, "bbPager.window.focus.color:", ReadString(stylepath, "bbPager.window.focus.color:", ReadString(stylepath, "window.title.focus.color:", ReadString(stylepath, "window.title.focus.color1:", ReadString(stylepath, "window.title.focus.backgroundColor:", colorAsString)))));
			if (focusedWindow.style->type == B_SOLID)
				focusedWindow.colorTo = focusedWindow.color;
			else
				focusedWindow.colorTo = ReadColor(bspath, "bbPager.window.focus.colorTo:", ReadString(stylepath, "bbPager.window.focus.colorTo:", ReadString(stylepath, "window.title.focus.colorTo:", ReadString(stylepath, "window.title.focus.color2:", colorAsString))));
		}
		else
		{
			strcpy(tempstring, ReadString(bspath, "bbPager.focus.window:", ReadString(stylepath, "bbPager.focus.window:", ReadString(bspath, "window.label.focus:", ReadString(bspath, "window.label.focus.appearance:", "Raised Bevel2 Gradient Vertical")))));
			ParseItem(tempstring, focusedWindow.style);

			focusedWindow.color = ReadColor(bspath, "bbPager.window.focus.color:", ReadString(stylepath, "bbPager.window.focus.color:", ReadString(stylepath, "window.label.focus.color:", ReadString(stylepath, "window.label.focus.color1:", ReadString(stylepath, "window.label.focus.backgroundColor:", colorAsString)))));
			if (focusedWindow.style->type == B_SOLID)
				focusedWindow.colorTo = focusedWindow.color;
			else
				focusedWindow.colorTo = ReadColor(bspath, "bbPager.window.focus.colorTo:", ReadString(stylepath, "bbPager.window.focus.colorTo:", ReadString(stylepath, "window.label.focus.colorTo:", ReadString(stylepath, "windowlabel.focus.color2:", colorAsString))));
		}
	}

	focusedWindow.Style.bevelstyle = BEVEL_RAISED; 
	focusedWindow.Style.bevelposition = BEVEL2; 

//===========================================================
// bbpager.active.window.borderColor:

	focusedWindow.borderColor = ReadColor(bspath, "bbPager.active.window.borderColor:", ReadString(stylepath, "bbPager.active.window.borderColor:", ReadString(stylepath, "window.label.focus.TextColor:", colorAsString)));
	if (focusedWindow.borderColor == 0x000000)
		focusedWindow.borderColor = frame.Style.Color;

}

//===========================================================================
