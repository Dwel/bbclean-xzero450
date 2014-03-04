/*
 ============================================================================
 BBmuse - Winamp track name displaying thing...
 ============================================================================
 Created by MADant aka Kaloth - http://www.geocities.com/madant8
 ============================================================================

  Blackbox for Windows is free software and so is this plugin
  released under the GNU General Public License (GPL version 2 or later).
  
  You may mess with it as you wish! (But keep it open source please :))

 ============================================================================
*/

#ifdef __GNUC__
#define _WIN32_IE 0x0500
#define GetBlackboxPath _GetBlackboxPath
#define DLL_EXPORT //__declspec(dllexport)
#define LWA_ALPHA 2
#endif

#ifdef __BORLANDC__
#define DLL_EXPORT
#endif

#include "BBApi.h"
#include <stdlib.h>
#include <windows.h>
#include <mmsystem.h>
#include <stdio.h>
#include <time.h>
#include <malloc.h>


#pragma comment (lib, "msimg32.lib")

bool (__stdcall *pSetLayeredWindowAttributes)(HWND, COLORREF, BYTE, DWORD);

extern "C"
{
	DLL_EXPORT int beginSlitPlugin(HINSTANCE hMainInstance, HWND hBBSlit);
	DLL_EXPORT int beginPluginEx(HINSTANCE hMainInstance, HWND hBBSlit);
	DLL_EXPORT int beginPlugin(HINSTANCE hMainInstance);
	DLL_EXPORT void endPlugin(HINSTANCE hMainInstance);
	DLL_EXPORT LPCSTR pluginInfo(int field);
}

LPSTR szAppName         = "BBmuse";          // The name of our window class, etc.
LPSTR szVersion         = "BBmuse v0.97b";   // Used in MessageBox titlebars
LPSTR szInfoVersion     = "v0.97b";
LPSTR szInfoAuthor      = "Kaloth";
LPSTR szInfoRelDate     = "29-07-2006";
LPSTR szInfoLink        = "http://www.geocities.com/madant8";
LPSTR szInfoEmail       = "mulletwarriorextreem@hotmail.com";
char updateURL[MAX_PATH];

#define PLUGIN_UPDATE_URL 8

#define STREQ(str1, str2) (strcmp((str1), (str2)) == 0)

#define M_BOL 0
#define M_INT 1
#define M_STR 2

#define TICK_PAUSE 25

#define DISP_ERRORS 0
#define DISP_TIME 1
#define DISP_MESSAGE 2

#define MAX_ACTIONS 5

typedef struct ACTION_TAG
{
	char message[1024];
	HANDLE thread;
	DWORD threadId;
	long startTime;
} ACTION, *PACTION;

//===========================================================================
HINSTANCE hInstance;
HWND hwndPlugin, hwndBlackbox, hwndWinamp;
OSVERSIONINFO  osvinfo;

char rcpath[MAX_PATH];
char playlistpath[MAX_PATH];
char title_string[MAX_PATH];
char title_string_old[MAX_PATH];
char theFont[MAX_PATH];
int theFontHeight;
int theFontWeight;
int title_string_len, title_string_height;
char timestrval[MAX_PATH];

int xpos, ypos;
int width, height;

bool alwaysOnTop;
bool snapWindow;
bool hidden;
bool pluginToggle;        
bool transparencyEnabled;
bool transback;
bool startInRecord;
bool showBorder;
bool scroll_style = false;
bool usestylefont = true;
int transparencyAlpha;
int display_state;
char default_message[MAX_PATH];

HDC buf, clip;
HBITMAP bufbmp, clipbmp;
HGDIOBJ otherbmp, otherotherbmp;

bool usingWin2kXP;

StyleItem myStyleItem;

int bevelWidth;
int borderWidth;
int scopeWidth;
COLORREF borderColor;
COLORREF TRANSCOLOUR = RGB(255,0,230);

//blend stuff...
BLENDFUNCTION blend;
bool motionblur;

int tick_count;

void GetStyleSettings();
void ReadRCSettings();
void WriteRCSettings();

HFONT hFont;

// Slit stuff...
bool inSlit;
HWND hSlit;

// Thread stuff...
DWORD dwThreadId, dwThreadId_Time;
HANDLE hThread, hThread_Time;
HWINEVENTHOOK eventhook;

// Action stuff...
ACTION aActions[MAX_ACTIONS];

// Ticker stuff...
int tickpos = 0, tickmax, tickmin, tickspeed = 2;
bool tickdirec = true;
bool scrolling;
bool flash = false;
bool showTrackNumber;
int playback = -1;
int tick_end_pause = 0;
int prev_data = 0;

// BBinterface extension stuff...
bool bbInterfaceEnabled = true;
bool catchFeedback = false;

// ----- Winamp Comunication Stuff ----- \\

#define WINAMP_VERSION 0
#define WINAMP_START_PLAYBACK 100
#define WINAMP_CLEAR_PLAYLIST 101
#define WINAMP_PLAY_SELECTED_TRACK 102
#define WINAMP_DIR_Cdownlaods 103
#define WINAMP_PLAYBACK_STATUS 104
#define WINAMP_TRACK_INFO 105
#define WINAMP_TRACK_SEEK 106
#define WINAMP_SAVE_PLAYLIST 120
#define WINAMP_SET_TRACK 121
#define WINAMP_SET_VOLUME 122
#define WINAMP_SET_PANNING 123
#define WINAMP_PLAYLIST_LENGTH 124
#define WINAMP_TRACK 125
#define WINAMP_TRACK_INFO_2 126
#define WINAMP_EQ_INFO 127
#define WINAMP_AUTOLOAD_OR_SETEQ 128
#define WINAMP_BOOKMARK_FILE 129
#define WINAMP_RESTART_WINAMP 135

#define WINAMP_PLAYBACK_PLAYING 1
#define WINAMP_PLAYBACK_PAUSED 3

int getWinampData(int id, UINT type){
	if(hwndWinamp == NULL)return -1;
	
	return SendMessage(hwndWinamp, type, 0, id);
}

int getWinampData(int data, int id, UINT type){
	if(hwndWinamp == NULL)return -1;
	
	return SendMessage(hwndWinamp, type, data, id);
}

void sendBroam(char *str){
	char buf[MAX_PATH];
	strcpy(buf, str);
	SendMessage(hwndBlackbox, BB_BROADCAST, 0, (LPARAM)buf);
}

// Relays data from winamp in the form of a bro@m...
void sendWinampDataBroam(int id, int data, char *broam_name, UINT type){
	char buf[MAX_PATH];
	int res = getWinampData(data, id, type);
	
	sprintf(buf, "%s %d", broam_name, res);
	SendMessage(hwndBlackbox, BB_BROADCAST, 0, (LPARAM)buf);
}

void getStringDimensions(char *str, int & v0, int & v1)
{	
	RECT r = {0,0,0,0};
	HDC hdc = CreateCompatibleDC(NULL);
	HGDIOBJ otherfont = SelectObject(hdc, hFont);
	DrawText(hdc, str, -1, &r, DT_LEFT|DT_CALCRECT);

	v0 = r.right;
	v1 = r.bottom;

	SelectObject(hdc, otherfont);
	DeleteDC(hdc);
}

char * getErrorString(bool play_stopped, char * buf, size_t max_buf_sz)
{
	switch(display_state){
		case DISP_ERRORS:{
			if(play_stopped && hwndWinamp != NULL)return "Playback Stopped";
			return "Couldn't Find Winamp";
		}
		case DISP_TIME:{
			const time_t t = time(0);
			const tm *tml = localtime(&t);
			strftime(buf, max_buf_sz, timestrval, tml);
			return buf;
		}
		case DISP_MESSAGE:{
			return default_message;
		}
		default:{
			return "BBmuse";
		}
	}
}

/*
	START

	Thanks to EasyWinampControl by Yann Hamon and Malnilion for
	pointing me in the direction of this...
	
	The following is partialy modified from the original source.
*/
// For example, circularstrstr("winamp", "pwi", 3) would return 5 (the index of p)
int circularstrstr(char* a, char* b, int nb)
{
  int equal = 1;
  int length;
  int pos=-1;
  int i, j;

  length = strlen(a);

  for (i=0; i<length && pos == -1; ++i) {
    equal = 1;
    for (j=0; j<nb;j++) {
      if (a[(i+j)%length] != b[j])
        equal = 0;
    }
    if (equal == 1)
      pos = i;
  }

  return pos;
}
/*
	END
*/

char *readSongName(char * buff, size_t const buff_sz){
	if(hwndWinamp == NULL){
		return getErrorString(false, buff, buff_sz);
	}
	
	int pos, length;
	GetWindowText(hwndWinamp, buff, buff_sz);
	length = strlen(buff);
	
	/*
		START
	
		Thanks to EasyWinampControl by Yann Hamon and Malnilion for
		pointing me in the direction of this...
		
		The following is partialy modified from the original source.
	*/
	if ((pos = circularstrstr(buff, "- Winamp ***", 12)) != -1) {
		// The option "scroll song title in taskbar" is on...
		
		char * buffer2 = static_cast<char *>(alloca(buff_sz));
		int i, j=0;
	
		for (i=(pos+12)%length; i!=pos; i=(i+1)%length)
			buffer2[j++] = buff[i];
		
		buffer2[j] = '\0';
		strcpy(buff, buffer2);
	}
	/*
		END
	*/
	
	// Get rid of the annoying " - Winamp" tag...
	char *ptr;
	ptr = strstr(buff, " - Winamp");
	if(ptr != NULL){
		while(*ptr != '\0'){
			*ptr++ = '\0';
		}
	}
	
	// Toggle track number...
	if(!showTrackNumber){
		ptr = strstr(buff, ".");
		if(ptr == NULL){
			ptr = buff;
		}else{
			ptr += 2;
		}
	}else{
		ptr = buff;
	}
	
	return ptr;
}

// Finds the winamp window...
int findWinamp(){
	// Look for the winamp window...
	hwndWinamp = FindWindow("Winamp v1.x",NULL);
	
	// Not ideal, but is the most reliable way to do this :)
	if(hwndWinamp == NULL)sendBroam("@BBmuse_fromWinamp_Closed");
	
	return hwndWinamp == NULL;
}

// Change the sting to be displayed by bbmuse...
void setTitleString(const char *str){
	strcpy(title_string, str);
	
	if(!STREQ(title_string, title_string_old)){
		getStringDimensions(title_string, title_string_len, title_string_height);
		tickmax = 0;
		tickmin = (width - ((bevelWidth + borderWidth)*2)) - title_string_len;
		strcpy(title_string_old, title_string);
		//tickpos = 0;
		tick_end_pause = 0;
	}
	
	InvalidateRect(hwndPlugin, NULL, FALSE);
}

// Asks winamp for information, find winamp if not found already...
void getWinampInfo(){
	findWinamp();
	
	playback = getWinampData(WINAMP_PLAYBACK_STATUS, WM_USER);

	size_t const buff_sz = 1024;
	char buff[buff_sz];
	
	if(playback == WINAMP_PLAYBACK_PAUSED){
		tickspeed = 0;
		motionblur = true;
		setTitleString(readSongName(buff, buff_sz));
		sendBroam("@BBmuse_fromWinamp_Paused");
	}else if(playback == WINAMP_PLAYBACK_PLAYING){
		flash = true;
		tickspeed = 1;
		motionblur = false;
		setTitleString(readSongName(buff, buff_sz));
		sendBroam("@BBmuse_fromWinamp_Playing");
	}else{
		flash = true;
		tickspeed = 1;
		motionblur = false;
		char buff[1024];
		setTitleString(getErrorString(true, buff, 1024));
		sendBroam("@BBmuse_fromWinamp_Stopped");
	}
}

VOID CALLBACK WinEventProc(	HWINEVENTHOOK hWinEventHook, DWORD event, HWND hwnd, 
				LONG idObject, LONG idChild, DWORD dwEventThread, DWORD dwmsEventTime){			
	if(hwnd != NULL){
		TCHAR buffer[MAX_PATH];
		RealGetWindowClass(hwnd, buffer, sizeof(buffer));
		if(STREQ(buffer, "Winamp v1.x")){
			switch(event){
				case EVENT_OBJECT_CREATE:{
					if(hwndWinamp == NULL){
						getWinampInfo();
						if(hwndWinamp != NULL) sendBroam("@BBmuse_fromWinamp_Opened");
					}
					break;}
				case EVENT_OBJECT_DESTROY:{
					if(hwnd == hwndWinamp){
						getWinampInfo();
						unsigned long exitcode;
						// Kill all active actions...
						for(int i = 0; i < MAX_ACTIONS; i++){
							PACTION pa = &aActions[i];
							if(pa->thread != NULL){
								GetExitCodeThread(pa->thread, &exitcode);
								TerminateThread(pa->thread, exitcode);
								pa->thread = NULL;
							}
						}
						sendBroam("@BBmuse_fromWinamp_Closed");
					}
					break;}
				case EVENT_OBJECT_NAMECHANGE:{
					if(hwnd == hwndWinamp){
						getWinampInfo();
						sendBroam("@BBmuse_fromWinamp_TitleChange");
					}
					break;}
			}
		}
	}
}

void hook(){
	eventhook = SetWinEventHook(
		EVENT_OBJECT_CREATE,
		EVENT_OBJECT_NAMECHANGE,
		GetModuleHandle(NULL),
		(WINEVENTPROC) WinEventProc,
		0, // get stuff from all processes.
		0, // all threads.
		WINEVENT_OUTOFCONTEXT | WINEVENT_SKIPOWNTHREAD); // Don't look at ourself because we'll get an infinite loop ;p
}

void unhook(){
	UnhookWinEvent(eventhook);
}

// Function to retrieve time, and send out data relays...
DWORD WINAPI TimeProc(LPVOID lpParam){
	while(true){
		// Display time string...
		if(display_state == DISP_TIME) getWinampInfo();
		
		// Send out track position and length bro@m...
		int play_ms = getWinampData(0, WINAMP_TRACK_INFO, WM_USER) / 1000;
		float f = 0;
		if(play_ms > 0){
			int play_total = getWinampData(1, WINAMP_TRACK_INFO, WM_USER);
			f = (float)play_ms/(float)play_total;
		}
		prev_data = (int)(f * 100);
		char buf[MAX_PATH];
		
		if(bbInterfaceEnabled && !catchFeedback){
			// BBInterface slider...
			catchFeedback = true;
			
			sprintf(buf, "@BBInterface Control SetControlProperty winampslider Value %f", f);
			SendMessage(hwndBlackbox, BB_BROADCAST, 0, (LPARAM)buf);
			
			// BBinterface label...
			sprintf(buf, "@BBInterface Control SetAgent winamplabel Caption StaticText \"%s\"", title_string);
			SendMessage(hwndBlackbox, BB_BROADCAST, 0, (LPARAM)buf);
		}
		
		// Idle for 3 seconds...
		Sleep(3000);
	}
	return 0;
}


// 2 types of scrolling....

// End to End shunting style...
void EndToEnd(){
	if(tickdirec){
		if(tick_end_pause == 0)tickpos += tickspeed;
		if(tickpos > tickmax){
			if(tick_end_pause >= TICK_PAUSE){
				tickdirec = !tickdirec;
				tick_end_pause = 0;
			}else{
				tick_end_pause++;
			}
		}
	}else{
		if(tick_end_pause == 0)tickpos -= tickspeed;
		if(tickpos < tickmin){
			if(tick_end_pause >= TICK_PAUSE){
				tickdirec = !tickdirec;
				tick_end_pause = 0;
			}else{
				tick_end_pause++;
			}
		}
	}
}

// Streaming Ticker style...
void TickerStream(){
	if(tickpos < -(title_string_len + 20)){
		tickpos = width;
	}else{
		tickpos -= tickspeed;
	}
}

// Draws the window on a timer...
DWORD WINAPI DrawProc(LPVOID lpParam){
	int flashcount = 0;
	while(true){
		if((title_string_len > (width - ((bevelWidth + borderWidth)*2))) && scrolling){
			if(scroll_style){
				EndToEnd();
			}else{
				TickerStream();
			}
			InvalidateRect(hwndPlugin, NULL, FALSE);
		}
		if(playback == WINAMP_PLAYBACK_PAUSED){
			flashcount++;
			if(flashcount >= TICK_PAUSE){
				flash = !flash;
				flashcount = 0;
			}
			InvalidateRect(hwndPlugin, NULL, FALSE);
		}
		Sleep(50);
	}
	return 0;
}

//===========================================================================
// like the name says...

void set_window_modes(void)
{
	if (hidden && pluginToggle)
	{
		ShowWindow(hwndPlugin, SW_HIDE);
	}
	else
	{
		if(usingWin2kXP){
			if(!inSlit && pSetLayeredWindowAttributes != NULL){
				// Add the WS_EX_LAYERED atribute to the window.
				SetWindowLong(hwndPlugin, GWL_EXSTYLE , GetWindowLong(hwndPlugin, GWL_EXSTYLE) | WS_EX_LAYERED);
				// Make it transparent...
				pSetLayeredWindowAttributes(	hwndPlugin,
								TRANSCOLOUR,
								(transparencyEnabled) ? (unsigned char)transparencyAlpha : 255,
								LWA_COLORKEY | LWA_ALPHA);
			}else{
				// Remove the WS_EX_LAYERED atribute to the window, cos we in di slit!
				SetWindowLong(hwndPlugin, GWL_EXSTYLE, GetWindowLong(hwndPlugin, GWL_EXSTYLE) & ~WS_EX_LAYERED);
			}
		}else{
			SetTransparency(
				hwndPlugin,
				transparencyEnabled && !inSlit ? (unsigned char)transparencyAlpha : 255
				);
		}
		
		SetWindowPos(
			hwndPlugin,
			alwaysOnTop ? HWND_TOPMOST : HWND_NOTOPMOST,
			xpos, ypos, width, height, SWP_NOACTIVATE|SWP_SHOWWINDOW
			);
		
		InvalidateRect(hwndPlugin, NULL, false);
	}
}

int beginSlitPlugin(HINSTANCE hMainInstance, HWND hBBSlit)
{
	inSlit = true;
	hSlit = hBBSlit;
	
	// Start the plugin like normal now..
	int res = beginPlugin(hMainInstance);
	
	// Are we to be in the Slit?
	if (inSlit && hSlit){
		// Yes, so Let's let BBSlit know.
		SendMessage(hSlit, SLIT_ADD, NULL, (LPARAM)hwndPlugin);
	}
	
	return res;
}

int beginPluginEx(HINSTANCE hMainInstance, HWND hBBSlit)
{
	return beginSlitPlugin(hMainInstance, hBBSlit);
}

//===========================================================================
int beginPlugin(HINSTANCE hPluginInstance)
{
	HMODULE usrdll = LoadLibrary("user32.dll");
	if(usrdll != NULL)*(FARPROC*)&pSetLayeredWindowAttributes = GetProcAddress(usrdll, "SetLayeredWindowAttributes");
	
	blend.BlendOp = AC_SRC_OVER;
	blend.BlendFlags = 0;
	blend.SourceConstantAlpha = 30;
	blend.AlphaFormat = NULL;
	
	tick_count = GetTickCount();
	
	for(int i = 0; i < MAX_ACTIONS; i++){
		aActions[i].thread = NULL;
	}
	
	LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
	WNDCLASS wc;
	
	ZeroMemory(&osvinfo, sizeof(osvinfo));
	osvinfo.dwOSVersionInfoSize = sizeof (osvinfo);
	GetVersionEx(&osvinfo);
	usingWin2kXP =
		osvinfo.dwPlatformId == VER_PLATFORM_WIN32_NT
		&& osvinfo.dwMajorVersion > 4;

	hwndBlackbox = GetBBWnd();
	hInstance = hPluginInstance;

	// Register the window class...
	ZeroMemory(&wc,sizeof(wc));
	wc.lpfnWndProc = WndProc;           // our window procedure
	wc.hInstance = hPluginInstance;     // hInstance of .dll
	wc.lpszClassName = szAppName;       // our window class name
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.style = CS_DBLCLKS;

	if (!RegisterClass(&wc)) 
	{
		MessageBox(hwndBlackbox,
			"Error registering window class", szVersion,
				MB_OK | MB_ICONERROR | MB_TOPMOST);
		return 1;
	}

	// Get plugin and style settings...
	ReadRCSettings();
	GetStyleSettings();

	// Create the window...
	if (!CreateWindowEx(
		usingWin2kXP ? WS_EX_TOOLWINDOW | WS_EX_LAYERED : WS_EX_TOOLWINDOW, // window style
		szAppName,                      // our window class name
		NULL,                           // NULL -> does not show up in task manager!                
		WS_POPUP
		| WS_CLIPCHILDREN
		| WS_CLIPSIBLINGS
		| WS_VISIBLE,                   // window parameters
		xpos,                           // x position
		ypos,                           // y position
		width,                          // window width
		height,                         // window height
		NULL,                           // parent window
		NULL,                           // no menu
		hPluginInstance,                // hInstance of .dll
		NULL))
	{                          
		MessageBox(0,
			"Error creating window", szVersion,
				MB_OK | MB_ICONERROR | MB_TOPMOST);
		return 1;
	}
	
	// Start the monitoring threads...
	hThread_Time = CreateThread(NULL, 0, TimeProc, NULL, 0, &dwThreadId_Time);
	hook();
	hThread = CreateThread(NULL, 0, DrawProc, NULL, 0, &dwThreadId);
	
	getWinampInfo();
	
	return(0);
}
                   
//===========================================================================

void endPlugin(HINSTANCE hPluginInstance)
{	
	// Stop the monitor...
	unhook();
	unsigned long exitcode;
	GetExitCodeThread(hThread_Time, &exitcode);
	TerminateThread(hThread_Time, exitcode);
	GetExitCodeThread(hThread, &exitcode);
	TerminateThread(hThread, exitcode);
	
	// Kill all active actions...
	for(int i = 0; i < MAX_ACTIONS; i++){
		PACTION pa = &aActions[i];
		if(pa->thread != NULL){
			GetExitCodeThread(pa->thread, &exitcode);
			TerminateThread(pa->thread, exitcode);
			pa->thread = NULL;
		}
	}
	
	// Jump outa da slit!
	if(inSlit && hSlit){
		// We are in the slit, so lets unload and get out..
		SendMessage(hSlit, SLIT_REMOVE, NULL, (LPARAM)hwndPlugin);
		
		// Here you can move to where ever you want ;)
		SetWindowPos(hwndPlugin, NULL, xpos, ypos, 0, 0, SWP_NOZORDER | SWP_NOSIZE);
		inSlit = false;
	}
	
	// Destroy the window...
	DestroyWindow(hwndPlugin);

	// Unregister window class...
	UnregisterClass(szAppName, hPluginInstance);

	if (hFont){
		DeleteObject(hFont), hFont = NULL;
	}
}

//===========================================================================

LPCSTR pluginInfo(int field)
{
	// pluginInfo is used by Blackbox for Windows to fetch information about
	// a particular plugin. At the moment this information is simply displayed
	// in an "About loaded plugins" MessageBox, but later on this will be
	// expanded into a more advanced plugin handling system. Stay tuned! :)
	switch (field)
	{
		case PLUGIN_NAME:
			return szAppName;       // Plugin name
		case PLUGIN_VERSION:
			return szInfoVersion;   // Plugin version
		case PLUGIN_AUTHOR:
			return szInfoAuthor;    // Author
		case PLUGIN_RELEASE:
			return szInfoRelDate;   // Release date, preferably in yyyy-mm-dd format
		case PLUGIN_LINK:
			return szInfoLink;      // Link to author's website
		case PLUGIN_EMAIL:
			return szInfoEmail;     // Author's email
		case PLUGIN_UPDATE_URL:
			return updateURL;	// Dev's online update meta file
		case PLUGIN_BROAMS:
		{
			return "@BBmuse SetCaption"
			       "@BBmuse SetCaptionNow"
			       "@BBmuse GetWinampInfo";
		}
		default:
			return szVersion;       // Fallback: Plugin name + version, e.g. "MyPlugin 1.0"
	}
}

//===========================================================================

void ShowMyMenu(bool popup)
{
	Menu *pMenu, *pSub;
	Menu *scrollstyle, *idle;
	// We create the main plugin menu.
	// Each menu or submenu has an unique ID-string, which identifies it
	// for later updates.

	pMenu = MakeNamedMenu("BBmuse", "BBmuse_IDMain", popup);

	// --------------------------------------------------------------
	// ... and the configuration options menu, note the "internal" bro@ms!...
	pSub = MakeNamedMenu("Configuration", "BBmuse_IDConfig", popup);

	MakeMenuItem(pSub, "Always On Top", "@BBmuseInternal AlwaysOnTop", alwaysOnTop);
	MakeMenuItem(pSub, "Docked to Slit", "@BBmuseInternal SlitToggle", inSlit);
	
	MakeMenuItem(pSub, "Snap Window To Edge", "@BBmuseInternal SnapWindowToEdge", snapWindow);
	MakeMenuItem(pSub, "Toggle With Plugins", "@BBmuseInternal ToggleWithPlugins", pluginToggle);
	
	MakeMenuNOP(pSub, NULL);
	
	MakeMenuItem(pSub, "BBInterface Extensions", "@BBmuseInternal ToggleBBInterface", bbInterfaceEnabled);
	
	MakeMenuNOP(pSub, NULL);
	
	MakeMenuItem(pSub, "Transparency Enabled", "@BBmuseInternal TransparencyEnabled", transparencyEnabled);
	if(usingWin2kXP)MakeMenuItem(pSub, "Transparent Back Enabled", "@BBmuseInternal TransbackEnabled", transback);
	// insert an item to adjust an integer value. Implemented in bblean for now
	MakeMenuItemInt(pSub, "Alpha Value", "@BBmuseInternal TransparencyAlpha", transparencyAlpha, 0, 255);
	
	MakeMenuNOP(pSub, NULL);
	
	scrollstyle = MakeNamedMenu("Scroll Style", "BBmuse_ScrollStyle", popup);
		MakeMenuItem(scrollstyle, "Stream", "@BBmuseInternal ToggleScrollStyle", !scroll_style);
		MakeMenuItem(scrollstyle, "End to End", "@BBmuseInternal ToggleScrollStyle", scroll_style);
	MakeSubmenu(pSub, scrollstyle, "Scroll Style");
	MakeMenuItem(pSub, "Scroll Song Title", "@BBmuseInternal ToggleScrolling", scrolling);
	
	MakeMenuNOP(pSub, NULL);
	
	MakeMenuItem(pSub, "Show Track Number", "@BBmuseInternal ToggleTrackNumber", showTrackNumber);
	MakeMenuItem(pSub, "Show Border", "@BBmuseInternal ToggleBorder", showBorder);
	
	MakeMenuNOP(pSub, NULL);
	
	idle = MakeNamedMenu("Idle Message", "BBmuse_Idle Message", popup);
		MakeMenuItem(idle, "Errors", "@BBmuseInternal ToggleIdleState 0", display_state == 0);
		MakeMenuItem(idle, "Time", "@BBmuseInternal ToggleIdleState 1", display_state == 1);
		MakeMenuItem(idle, "Message", "@BBmuseInternal ToggleIdleState 2", display_state == 2);
	MakeSubmenu(pSub, idle, "Idle Message");
	
	// ...and add it to the main plugin menu
	MakeSubmenu(pMenu, pSub, "Configuration");
	
	// --------------------------------------------------------------
	
	// Blank space...
	MakeMenuNOP(pMenu, NULL);
	// Edit the RC-File, using the default Blackbox Editor
	MakeMenuItem(pMenu, "Edit BBmuse.rc", "@BBmuseInternal Edit", false);
	// ...an "About" box is always nice, even though we now have the regular plugin info function...
	MakeMenuItem(pMenu, "About BBmuse", "@BBmuseInternal About", false);

	// And finally, we show the menu...
	ShowMenu(pMenu);
}

//===========================================================================

void set_setting(bool *flag, const char *toolbar_message, const char *rc_string)
{
	// send a display sting to the toolbar
	char msg[80];
	sprintf(msg, "BBmusePlugin -> %s %sabled!",
		toolbar_message, *flag ? "en" : "dis");
	SendMessage(hwndBlackbox, BB_SETTOOLBARLABEL, 0, (LPARAM)msg);

	// write the new setting to the rc - file
	WriteBool(rcpath, rc_string, *flag);

	// apply new settings
	set_window_modes();

	// and update the menu checkmarks
	ShowMyMenu(false);
}

void toggle_setting(bool *flag, const char *toolbar_message, const char *rc_string)
{
	// toggle the setting
	*flag = false == *flag;

	// send a display sting to the toolbar
	char msg[80];
	sprintf(msg, "BBmusePlugin -> %s %sabled!",
		toolbar_message, *flag ? "en" : "dis");
	SendMessage(hwndBlackbox, BB_SETTOOLBARLABEL, 0, (LPARAM)msg);

	// write the new setting to the rc - file
	WriteBool(rcpath, rc_string, *flag);

	// apply new settings
	set_window_modes();

	// and update the menu checkmarks
	ShowMyMenu(false);
}

//===========================================================================

//===========================================================================

void MakeBuffer(){
	PAINTSTRUCT ps; RECT r;
	HDC hdc = BeginPaint(hwndPlugin, &ps);
	
	//get window rectangle
	GetClientRect(hwndPlugin, &r);
	
	//first get a new 'device context'
	buf = CreateCompatibleDC(NULL);
	
	//then create a buffer in memory with the window size
	bufbmp = CreateCompatibleBitmap(hdc, r.right, r.bottom);
	
	//make another buffer for drawing the text onto...
	int padding = ((bevelWidth + borderWidth)*2);
	clip = CreateCompatibleDC(NULL);
	clipbmp = CreateCompatibleBitmap(hdc, r.right - padding, r.bottom - padding);
	
	//select it into the DC and store, whatever there was before.
	otherbmp = SelectObject(buf, bufbmp);
	otherotherbmp = SelectObject(clip, clipbmp);
	
	EndPaint(hwndPlugin, &ps);	
}

void resizeWindow(){
	DeleteObject(SelectObject(buf, otherbmp));
	DeleteObject(SelectObject(clip, otherotherbmp));
	DeleteDC(buf);
	DeleteDC(clip);
	MakeBuffer();
	
	tickmin = (width - ((bevelWidth + borderWidth)*2)) - title_string_len;
	tickpos = 0;
	tick_end_pause = 0;
	
	InvalidateRect(hwndPlugin, NULL, FALSE);
}

/** 
 * I Intruduced this function so that functions
 * called on winamp that don't return instantly
 * don't cause BB to hang.
 */
DWORD WINAPI WinampControlProc(LPVOID lParam){
	PACTION pa = &aActions[(int)lParam];
	
	if(hwndWinamp == NULL){
		pa->thread = NULL;
		return 0;
	}
	
	int id = 0, data = 0; char *ptr;
	char buf[MAX_PATH];
	char type[MAX_PATH];
	
	strcpy(buf, pa->message);
	
	//SendMessage(hwndBlackbox, BB_SETTOOLBARLABEL, 0, (LPARAM)lParam);
	
	ptr = strtok(buf, " ");
	if(ptr == NULL){
		pa->thread = NULL;
		//SendMessage(hwndBlackbox, BB_SETTOOLBARLABEL, 0, (LPARAM)"Fail 1");
		return 0;
	}
	
	ptr = strtok(NULL, " ");
	if(ptr == NULL){
		pa->thread = NULL;
		//SendMessage(hwndBlackbox, BB_SETTOOLBARLABEL, 0, (LPARAM)"Fail 2");
		return 0;
	}
	strcpy(type, ptr);
	
	ptr = strtok(NULL, " ");
	if(ptr == NULL){
		pa->thread = NULL;
		//SendMessage(hwndBlackbox, BB_SETTOOLBARLABEL, 0, (LPARAM)"Fail 3");
		return 0;
	}
	id = atoi(ptr);
	
	ptr = strtok(NULL, " ");
	if(ptr != NULL)data = atoi(ptr);
	
	if(STREQ(type, "WM_USER")){
		sprintf(buf, "@BBmuse_fromWinamp_Result %d", id);
		sendWinampDataBroam(id, data, buf, WM_USER);
	}else if(STREQ(type, "BBI_SPECIAL")){
		if(id == 106){
			int playback = getWinampData(WINAMP_PLAYBACK_STATUS, WM_USER);
	
			if(playback == WINAMP_PLAYBACK_PLAYING){
				if(catchFeedback){
					pa->thread = NULL;
					catchFeedback = false;
					return 0;
				}
				
				prev_data = data;
				int play_total = getWinampData(1, WINAMP_TRACK_INFO, WM_USER);
				data = (int)(((float)data/(float)100) * play_total) * 1000;
			}else{
				pa->thread = NULL;
				return 0;
			}
		}
		
		sprintf(buf, "@BBmuse_fromWinamp_Result %d", id);
		sendWinampDataBroam(id, data, buf, WM_USER);
	}else{
		sprintf(buf, "@BBmuse_fromWinamp_Result %d", id);
		sendWinampDataBroam(data, id, buf, WM_COMMAND);
		
		// This is to fix a bug when closing winamp with this method.
		if(id == 40001)sendBroam("@BBmuse_fromWinamp_Closed");
	}
	
	pa->thread = NULL;
	return 0;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	MINMAXINFO *minmax;
	char szTemp[MAX_PATH];
	RECT *r;

	// Blackbox messages we want to "subscribe" to:
	// BB_RECONFIGURE -> Sent when changing style and on reconfigure
	// BB_BROADCAST -> Broadcast message (bro@m)
	static int msgs[] = { BB_RECONFIGURE, BB_BROADCAST, 0};

	switch (message)
	{
		// ==========
		case WM_CREATE:
			// set the global variable
			hwndPlugin = hwnd;
			
			// Register to receive Blackbox messages...
			SendMessage(hwndBlackbox, BB_REGISTERMESSAGE, (WPARAM)hwnd, (LPARAM)msgs);
			
			// make the window visible on all worspaces
			MakeSticky(hwnd);
			
			MakeBuffer();
			
			set_window_modes();
			
			break;

		// ==========
		case WM_DESTROY:
			//remove us from the sticky-windows-list
			RemoveSticky(hwnd);
			
			//restore the first previous whatever to the dc,
			//get in exchange back our bitmap, and delete it.
			DeleteObject(SelectObject(buf, otherbmp));
			DeleteObject(SelectObject(clip, otherotherbmp));

			//delete the memory - 'device context'
			DeleteDC(buf);
			DeleteDC(clip);

			// Unregister Blackbox messages...
			SendMessage(hwndBlackbox, BB_UNREGISTERMESSAGE, (WPARAM)hwnd, (LPARAM)msgs);
			break;

		// ==========
		// If Blackbox sends a reconfigure message, load the new style settings and
		// update the window...

		case BB_RECONFIGURE:
			ReadRCSettings();
			GetStyleSettings();
			set_window_modes();
			break;

		// ==========
		// Window redraw processing...
		case WM_PAINT:
		{
			PAINTSTRUCT ps; RECT r;
			
			//get the screen-buffer
			HDC hdc = BeginPaint(hwnd, &ps);
			
			//get window rectangle
			GetClientRect(hwnd, &r);
			
			// use new api
			// Paint background according to the current style on the bitmap,
			// using the default border from style (true)
			
			if(!transback || inSlit){
				// Draw gradient background.
				MakeStyleGradient(buf, &r, &myStyleItem, showBorder);
			}else{
				// Draw the transparent pink bib.
				HBRUSH hbbrush = CreateSolidBrush(TRANSCOLOUR);
				FillRect(buf, &r, hbbrush);
				DeleteObject(hbbrush);
			}
			
			BitBlt(
				clip,        // handle to destination DC
				ps.rcPaint.left,
				ps.rcPaint.top,
				ps.rcPaint.right  - ps.rcPaint.left - ((bevelWidth + borderWidth)*2),
				ps.rcPaint.bottom - ps.rcPaint.top - ((bevelWidth + borderWidth)*2),
				buf,         // handle to source DC
				ps.rcPaint.left + (bevelWidth + borderWidth),
				ps.rcPaint.top + (bevelWidth + borderWidth),
				SRCCOPY
				);
			
			//select it into the DC and store, whatever there was before
			HGDIOBJ otherfont = SelectObject(clip, hFont);
			
			//draw transparently on our background-gradient
			SetBkMode(clip, TRANSPARENT);
			
			//draw the song title...
			//... with some color
			SetTextColor(clip, myStyleItem.TextColor);
			//... at the right place
			
			RECT rc; rc.left = 0; rc.top = 0;
			rc.right = ps.rcPaint.right  - ps.rcPaint.left  - ((bevelWidth + borderWidth)*2);
			rc.bottom = ps.rcPaint.bottom - ps.rcPaint.top - ((bevelWidth + borderWidth)*2);
			
			rc.top += (ps.rcPaint.bottom - ps.rcPaint.top - ((bevelWidth + borderWidth)*2) - title_string_height) >> 1;
			
			//...some text
			if(playback != WINAMP_PLAYBACK_PAUSED){
				if((title_string_len > (width - ((bevelWidth + borderWidth)*2))) && scrolling){
					rc.left += tickpos;
					DrawText(clip, title_string, -1, &rc, DT_LEFT);
				}else{
					DrawText(clip, title_string, -1, &rc, DT_CENTER);
				}
			}else{
				if(flash){
					/*if((title_string_len > (width - ((bevelWidth + borderWidth)*2))) && scrolling){
						rc.left += tickpos;
						DrawText(clip, title_string, -1, &rc, DT_LEFT);
					}else{*/
						DrawText(clip, title_string, -1, &rc, DT_CENTER);
					//}
				}
			}
			
			//restore the second previous whatever to the dc,
			
			SelectObject(clip, otherfont);
			
			BitBlt(
				buf,        // handle to destination DC
				ps.rcPaint.left + (bevelWidth + borderWidth),
				ps.rcPaint.top + (bevelWidth + borderWidth),
				ps.rcPaint.right  - ps.rcPaint.left - ((bevelWidth + borderWidth)*2),
				ps.rcPaint.bottom - ps.rcPaint.top - ((bevelWidth + borderWidth)*2),
				clip,         // handle to source DC
				ps.rcPaint.left,
				ps.rcPaint.top,
				SRCCOPY
				);

			
			//now copy the drawing from the paint buffer to the window...
			if(motionblur && !transback){
				AlphaBlend(	hdc,
						ps.rcPaint.left,
						ps.rcPaint.top,
						ps.rcPaint.right  - ps.rcPaint.left,
						ps.rcPaint.bottom - ps.rcPaint.top,
						buf,
						ps.rcPaint.left,
						ps.rcPaint.top,
						ps.rcPaint.right  - ps.rcPaint.left,
						ps.rcPaint.bottom - ps.rcPaint.top,
						blend
						);
			}else{
				BitBlt(		hdc,
						ps.rcPaint.left,
						ps.rcPaint.top,
						ps.rcPaint.right  - ps.rcPaint.left,
						ps.rcPaint.bottom - ps.rcPaint.top,
						buf,
						ps.rcPaint.left,
						ps.rcPaint.top,
						SRCCOPY
						);
			}

			//done
			EndPaint(hwnd, &ps);
			break;
		}

		// ==========
		// Broadcast messages (bro@m -> the bang killah! :D <vbg>)
		case BB_BROADCAST:
		{
			const char *msg_string = (LPCSTR)lParam;
			
			if (!_stricmp(msg_string, "@BBmuse GetWinampInfo"))
			{
				// @BBShowPlugins: Show window (global bro@m)
				getWinampInfo();
				break;
			}
			
			if (strstr(msg_string, "@BBmuse SetCaptionNow ") != NULL)
			{
				const char *ptr;
				ptr = strstr(msg_string, "@BBmuse SetCaptionNow ");
				ptr += strlen("@BBmuse SetCaptionNow ");
				setTitleString(ptr);
				break;
			}
			
			if (strstr(msg_string, "@BBmuse SetCaption ") != NULL && hwndWinamp == NULL)
			{
				const char *ptr;
				ptr = strstr(msg_string, "@BBmuse SetCaption ");
				ptr += strlen("@BBmuse SetCaption ");
				setTitleString(ptr);
				break;
			}
			
			// First we check for the two "global" bro@ms,
			if (!_stricmp(msg_string, "@BBShowPlugins"))
			{
				// @BBShowPlugins: Show window (global bro@m)
				if(!inSlit){
					hidden = false;
					set_window_modes();
				}
				break;
			}
			if (!_stricmp(msg_string, "@BBHidePlugins"))
			{
				// @BBHidePlugins: Hide window (global bro@m)
				if(!inSlit){
					hidden = true;
					set_window_modes();
				}
				break;
			}
			
			// handles sending of bro@ms to winamp;
			// sends out bro@m containing the result.
			if (strstr(msg_string, "@BBmuse_toWinamp"))
			{
				
				if(hwndWinamp != NULL){
					bool found = false;
					while(!found){
						for(int i = 0; i < MAX_ACTIONS; i++){
							PACTION pa = &aActions[i];
							if(pa->thread == NULL){
								strcpy(pa->message, msg_string);
								pa->thread = CreateThread(NULL, 0, WinampControlProc, (void *)i, 0, &pa->threadId);
								pa->startTime = timeGetTime();
								found = true;
								break;
							}
						}
						if(!found){
							// Find the oldest thread and kill it...
							PACTION oldest = &aActions[0];
							for(int i = 1; i < MAX_ACTIONS; i++){
								PACTION pa = &aActions[i];
								if(pa->thread == NULL){
									if(pa->startTime < oldest->startTime) oldest = pa;
								}
							}
							
							unsigned long exitcode;
							GetExitCodeThread(oldest->thread, &exitcode);
							TerminateThread(oldest->thread, exitcode);
							oldest->thread = NULL;
						}
						// Let other stuff happen for a bit...
						Sleep(10);
					}
				}
				break;
			}

			// ==========
			// ...then we check for the plugin bro@ms that are available to
			// the end-user (ie. listed in the documentation! <g>)...
			

			// ==========
			// Finally, we check for our "internal" bro@m, which is used for
			// all plugin internal commands, for example the menu commands...

			static const char Example_Broam_ID[] = "@BBmuseInternal";

			if (!_strnicmp(msg_string, Example_Broam_ID, sizeof Example_Broam_ID - 1))
			{
				msg_string += sizeof Example_Broam_ID - 1;
				while (*msg_string == ' ') msg_string ++;
				
				if (!_stricmp(msg_string, "SlitToggle"))
				{
					if(inSlit && hSlit)
					{
						// We are in the slit, so lets unload and get out..
						SendMessage(hSlit, SLIT_REMOVE, NULL, (LPARAM)hwndPlugin);

						// Here you can move to where ever you want ;)
						SetWindowPos(hwndPlugin, NULL, xpos, ypos, 0, 0, SWP_NOZORDER | SWP_NOSIZE);
						inSlit = false;
						set_window_modes();
					}
					/* Make sure before you try and load into the slit that you have
					 * the HWND of the slit ;)
					 */
					else if(hSlit)
					{
						// (Back) into the slit..
						inSlit = true;
						set_window_modes();
						SendMessage(hSlit, SLIT_ADD, NULL, (LPARAM)hwndPlugin);
					}
					
					set_setting(&inSlit,        // the * to the bool to toggle
						msg_string,                     // the message to display on toolbar
						"BBmuse.plugin.inSlit:"   // the rc-setting to write
						);
				}
				
				if (!_stricmp(msg_string, "ToggleBBInterface"))
				{
					// make use of our frienly ghost from above
					toggle_setting(&bbInterfaceEnabled,        // the * to the bool to toggle
						msg_string,                     // the message to display on toolbar
						"BBmuse.plugin.bbInterfaceExtensions:"   // the rc-setting to write
						);
					break;
				}
				
				if (strstr(msg_string, "ToggleIdleState") != NULL){
					const char *temp = strchr(msg_string, ' ');
					display_state = atoi(temp);
					WriteInt(rcpath, "BBmuse.plugin.idle.state:", display_state);
					getWinampInfo();
					WriteString(rcpath, "BBmuse.plugin.idle.TimeStr:", timestrval);
					InvalidateRect(hwndPlugin, NULL, FALSE);
					break;
				}
				
				if (!_stricmp(msg_string, "ToggleBorder"))
				{
					// make use of our frienly ghost from above
					toggle_setting(&showBorder,        // the * to the bool to toggle
						msg_string,                     // the message to display on toolbar
						"BBmuse.plugin.showBorder:"   // the rc-setting to write
						);
					break;
				}
				if (!_stricmp(msg_string, "ToggleScrolling"))
				{
					// make use of our frienly ghost from above
					toggle_setting(&scrolling,        // the * to the bool to toggle
						msg_string,                     // the message to display on toolbar
						"BBmuse.plugin.scrolling:"   // the rc-setting to write
						);
					break;
				}
				if (!_stricmp(msg_string, "AlwaysOnTop"))
				{
					// make use of our frienly ghost from above
					toggle_setting(&alwaysOnTop,        // the * to the bool to toggle
						msg_string,                     // the message to display on toolbar
						"BBmuse.plugin.alwaysontop:"   // the rc-setting to write
						);
					break;
				}
				if (!_stricmp(msg_string, "SnapWindowToEdge"))
				{
					toggle_setting(&snapWindow,
						msg_string,
						"BBmuse.plugin.snapwindow:"
						);
					break;
				}
				if (!_stricmp(msg_string, "TransparencyEnabled"))
				{
					toggle_setting(&transparencyEnabled,
						msg_string,
						"BBmuse.plugin.transparency.enabled:"
						);
					break;
				}
				if (!_stricmp(msg_string, "TransbackEnabled"))
				{
					toggle_setting(&transback,
						msg_string,
						"BBmuse.plugin.transback.enabled:"
						);
					break;
				}

				{
					static char trans_alpha_id[] = "TransparencyAlpha ";
					
					// If the original command was
					//      "@ExampleInternal TransparencyAlpha"

					// the integer item sends a message like this:
					//      "@ExampleInternal TransparencyAlpha 45"

					if (!_memicmp(msg_string, trans_alpha_id, sizeof(trans_alpha_id)-1))
					{
						transparencyAlpha = atoi(msg_string + sizeof(trans_alpha_id)-1);
						set_window_modes();
						WriteInt(rcpath, "BBmuse.plugin.transparency.alpha:", transparencyAlpha);
						break;
					}
				}

				if (!_stricmp(msg_string, "ToggleWithPlugins"))
				{
					toggle_setting(&pluginToggle,
						msg_string,
						"BBmuse.plugin.togglewithplugins:"
						);
					break;
				}
				
				if (!_stricmp(msg_string, "ToggleTrackNumber"))
				{
					toggle_setting(&showTrackNumber,
						msg_string,
						"BBmuse.plugin.showTrackNumber:"
						);
					getWinampInfo();
					break;
				}
				
				if (!_stricmp(msg_string, "ToggleScrollStyle"))
				{
					scroll_style = !scroll_style;
					WriteString(rcpath, "BBmuse.plugin.scroll_style:", scroll_style?"EndToEnd":"Stream");
					getWinampInfo();
					break;
				}
				
				if (!_stricmp(msg_string, "Edit"))
				{
					GetBlackboxEditor(szTemp);
					BBExecute(NULL, NULL, szTemp, rcpath, NULL, SW_SHOWNORMAL, false);
					break;
				}
				if (!_stricmp(msg_string, "About"))
				{
					sprintf(szTemp,
						"%s\n\n"
						"Author : Kaloth\n"
						"Web: http://www.geocities.com/madant8\n"
						"Email: mulletwarriorextreem@hotmail.com",
						szVersion
						);
					MessageBox(
						NULL, szTemp, "About this plugin...",
						MB_OK | MB_ICONINFORMATION);
					break;
				}
			}
			break;
		}
		// ==========

		case WM_NCRBUTTONUP:
			ShowMyMenu(true);
			break;
			
		case WM_RBUTTONUP:
			ShowMyMenu(true);
			break;

		// ==========
		// The window is moving
		case WM_WINDOWPOSCHANGING:
			// Is SnapWindowToEdge enabled? - and the plugin not hidden?
			if (!inSlit && snapWindow && IsWindowVisible(hwnd))
			{
				// ...then snap window to screen edges, if the distance is
				// less than 10 pixel (if the last bool is false it uses the
				// current DesktopArea)
				SnapWindowToEdge((WINDOWPOS*)lParam, 10, true);
			}
			break;
			
		// ==========
		// Save window position if it changed...
		case WM_WINDOWPOSCHANGED:
		{
			if(!inSlit){
				WINDOWPOS* wp = (WINDOWPOS*)lParam;
				xpos = wp->x;
				ypos = wp->y;
				if (wp->cx < 16) wp->cx = 16;
				if (wp->cy < 16) wp->cy = 16;
				width = wp->cx; height = wp->cy;
				WriteInt(rcpath, "BBmuse.plugin.x:", xpos);
				WriteInt(rcpath, "BBmuse.plugin.y:", ypos);
				WriteInt(rcpath, "BBmuse.plugin.width:", width);
				WriteInt(rcpath, "BBmuse.plugin.height:", height);
			}
			break;
		}

		// ==========
		// Allow window to move...
		case WM_NCHITTEST:
			if(!inSlit){
				return HTCAPTION;
			}
			return HTCLIENT;

		// ==========
		// Prevent closing with alt-f4
		case WM_CLOSE:
			break;
		
		case WM_SIZE:
			{int w = LOWORD(lParam);
			int h = HIWORD(lParam);
			if (w < 5) w = 5;
			if (h < 5) h = 5;
			width = w; height = h;
			WriteInt(rcpath, "BBmuse.plugin.width:", width);
			WriteInt(rcpath, "BBmuse.plugin.height:", height);}
			resizeWindow();
			break;
		
		case WM_SIZING:
			r = (RECT *)lParam;
			width = r->right - r->left;
			height = r->bottom - r->top;
			resizeWindow();
			break;
		
		case WM_NCLBUTTONDOWN:
			// initialize moving, when alt-key is held down
			if (GetAsyncKeyState(VK_MENU) & 0x8000){
				PostMessage(hwnd, WM_SYSCOMMAND, 0xf008, 0); // bottom right corner - resize
			}else{
				PostMessage(hwnd, WM_SYSCOMMAND, 0xf012, 0); // move
			}
			break;
		
		//Set minimum window size...
		case WM_GETMINMAXINFO:
			minmax = (MINMAXINFO *)lParam;
			minmax->ptMinTrackSize.x = 16;
			minmax->ptMinTrackSize.y = 16;
			break;
		
		// ==========
		// Let windows do everything else
		default:
			return DefWindowProc(hwnd,message,wParam,lParam);
	}
	// return zero, except from the hit-test message
	return 0;
}

//===========================================================================

void ReadRCSettings()
{
	char defaultpath[MAX_PATH]; int nLen, i;
	// First we look for the config file in the same folder as the plugin...
	GetModuleFileName(hInstance, rcpath, sizeof(rcpath));
	for (i=0;;i++)
	{
		nLen = strlen(rcpath); while (nLen && rcpath[nLen-1] != '\\') nLen--;
		// ...check the two possible filenames 'examplerc' and 'example.rc'...
		strcpy(rcpath+nLen, "BBmuserc");  if (FileExists(rcpath)) break;
		strcpy(rcpath+nLen, "BBmuse.rc"); if (FileExists(rcpath)) break;
		if (0 == i)
		{   //...if not found aside the plugin, we keep this as the default path...
			strcpy(defaultpath, rcpath);
			//...and try the Blackbox directory.
			GetBlackboxPath(rcpath, sizeof(rcpath));
			continue;
		}
		//if again no config file was found, we use the default path...
		strcpy(rcpath, defaultpath);
		break; // ...and quit the loop
	}
	// If a config file was found we read the plugin settings from it...
	// ...if not, the ReadXXX functions give us just the defaults.
	xpos = ReadInt(rcpath, "BBmuse.plugin.x:", 10);
	ypos = ReadInt(rcpath, "BBmuse.plugin.y:", 10);
	bbInterfaceEnabled  = ReadBool(rcpath, "BBmuse.plugin.bbInterfaceExtensions:", true);
	inSlit	   	    = ReadBool(rcpath, "BBmuse.plugin.inSlit:", false);
	transparencyEnabled = ReadBool(rcpath, "BBmuse.plugin.transparency.enabled:", false);
	transback	    = ReadBool(rcpath, "BBmuse.plugin.transback.enabled:", false);
	transparencyAlpha   = ReadInt(rcpath,  "BBmuse.plugin.transparency.alpha:", 60);
	alwaysOnTop         = ReadBool(rcpath, "BBmuse.plugin.alwaysontop:", true);
	snapWindow          = ReadBool(rcpath, "BBmuse.plugin.snapwindow:", true);
	pluginToggle        = ReadBool(rcpath, "BBmuse.plugin.togglewithplugins:", true);
	strcpy(updateURL, ReadString(rcpath,"BBmuse.plugin.update.url:", ""));

	// Animation Settings...
	width 		    = ReadInt(rcpath, "BBmuse.plugin.width:", 100);
	height		    = ReadInt(rcpath, "BBmuse.plugin.height:", 17);
	if(width < 5) width = 5;
	if(height < 5) height = 5;
	
	scrolling	    = ReadBool(rcpath, "BBmuse.plugin.scrolling:", false);
	showTrackNumber     = ReadBool(rcpath, "BBmuse.plugin.showTrackNumber:", true);
	showBorder	    = ReadBool(rcpath, "BBmuse.plugin.showBorder:", true);
	scroll_style	    = !STREQ("Stream", ReadString(rcpath, "BBmuse.plugin.scroll_style:", "Stream"));
	usestylefont 	    = !ReadBool(rcpath, "BBmuse.plugin.useCustomFont:", false);
	strcpy(theFont, ReadString(rcpath,"BBmuse.plugin.font:", "snap"));
	theFontHeight 	    = ReadInt(rcpath, "BBmuse.plugin.font.height:", 12);
	theFontWeight 	    = ReadInt(rcpath, "BBmuse.plugin.font.weight:", 1);
	display_state 	    = ReadInt(rcpath, "BBmuse.plugin.idle.state:", 0);
	strcpy(default_message, ReadString(rcpath,"BBmuse.plugin.idle.message:", "BBmuse"));
	strcpy(timestrval, ReadString(rcpath,"BBmuse.plugin.idle.TimeStr:","%a %d %b - %I:%M %p"));
}

//===========================================================================
void GetStyleSettings()
{

	if (hFont)
		DeleteObject(hFont), hFont = NULL;
	
	// -------------------------------------------------------------------
	// the direct way:
	// ---------------------

	myStyleItem = *(StyleItem*)GetSettingPtr(SN_TOOLBAR);

	bevelWidth  = *(int*)GetSettingPtr(SN_BEVELWIDTH);
	borderWidth = *(int*)GetSettingPtr(SN_BORDERWIDTH);
	borderColor = *(COLORREF*)GetSettingPtr(SN_BORDERCOLOR);
	
	// Read Custom style mods...
	
	const char *stylepath = stylePath();

	if(usestylefont){
		hFont = CreateStyleFont(&myStyleItem);
	}else{
		hFont = CreateFont(
			theFontHeight, 0, 0, 0, theFontWeight,
			false, false, false,
			DEFAULT_CHARSET,
			OUT_DEFAULT_PRECIS,
			CLIP_DEFAULT_PRECIS,
			DEFAULT_QUALITY,
			DEFAULT_PITCH,
			theFont
			);
	}

	/*// -------------------------------------------------------------------
	// the individual way:
	// -------------------------

	// Get the path to the current style file from Blackbox...
	const char *stylepath = stylePath();
	const char *temp;

	// Get the applicable color settings from the current style...
	myStyleItem.Color = ReadColor(stylepath, Color, "#000000");
	myStyleItem.ColorTo = ReadColor(stylepath, ColorTo, "#000000");
	myStyleItem.TextColor = ReadColor(stylepath, TextColor, "#FFFFFF");
	
	// ...gradient type, bevel etc. (using a StyleItem)...
	temp = ReadString(stylepath, "toolbar.label:", "Flat Gradient Vertical");
	ParseItem(temp, &myStyleItem);

	// ...font settings...
	strcpy(myStyleItem.Font, ReadString(stylepath, "toolbar.font:", "tahoma"));
	myStyleItem.FontHeight = ReadInt(stylepath, "toolbar.fontHeight:", 12);

	temp = ReadString(stylepath, "toolbar.fontHeight:", "normal");
	myStyleItem.FontWeight = stricmp(temp, "bold") ? FW_BOLD : FW_NORMAL;

	// ...and some additional parameters
	bevelWidth  = ReadInt(stylepath, "bevelWidth:",  1);
	borderWidth = ReadInt(stylepath, "borderWidth:", 1);
	borderColor = ReadColor(stylepath, BorderColor, "#000000");

	//make the font
	hFont = CreateFont(
		myStyleItem.FontHeight, 0, 0, 0, myStyleItem.FontWeight,
		false, false, false,
		DEFAULT_CHARSET,
		OUT_DEFAULT_PRECIS,
		CLIP_DEFAULT_PRECIS,
		DEFAULT_QUALITY,
		DEFAULT_PITCH,
		myStyleItem.Font
		);*/
}

//===========================================================================
