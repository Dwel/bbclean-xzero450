/*
 ============================================================================

	BBIME.CPP - Input Language Selector for bbLean 1.16 (bbClean)

 ============================================================================

	This file is part of the bbLean source code.

	Copyright © 2007 noccy
	http://dev.noccy.com/bbclean

	bbClean is free software, released under the GNU General Public License
	(GPL version 2 or later).

	http://www.fsf.org/licenses/gpl.html

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
	GNU General Public License for more details.

 ============================================================================
*/


#include "BBApi.h"
#include <windows.h>
//#include "m_alloc.h"
//#include "Tinylist.cpp"

//
// Function templates
//
void ErrorStrn(LPTSTR lpszFunction, DWORD dw);

//
// Here we declare the values that the plugin will give to blackbox via the
// pluginInfo function.
//
const char szVersion     [] = "bbIME 0.1 (BBClean SVN)";
const char szAppName     [] = "bbIME";
const char szInfoVersion [] = "0.1 (BBClean SVN)";
const char szInfoAuthor  [] = "noccy";
const char szInfoRelDate [] = __DATE__;
const char szInfoLink    [] = "http://dev.noccy.com/bblean";
const char szInfoEmail   [] = "noccy@angelicparticles.net";

//
// The classname of our message window. We create a new windowclass so that
// we can assign the address of our windowproc in order to intercept the
// WM_CREATE and WM_DESTROY messages.
//
const char szWindowClass [] = "bbimemessagewindow";

//
// These are the exported functions. These three is the bare minimum of what
// a normal blackbox plugin must support.
//
extern "C"
{
	DLL_EXPORT int beginPlugin(HINSTANCE hMainInstance);
	DLL_EXPORT void endPlugin(HINSTANCE hMainInstance);
	DLL_EXPORT LPCSTR pluginInfo(int field);
}

//
// pluginInfo function, called by blackbox to retrieve info on the plugin
//
LPCSTR pluginInfo(int field)
{
	switch (field)
	{
		default:
		case 0: return szVersion;
		case 1: return szAppName;
		case 2: return szInfoVersion;
		case 3: return szInfoAuthor;
		case 4: return szInfoRelDate;
		case 5: return szInfoLink;
		case 6: return szInfoEmail;
	}
}

//
// Some variables we need.
//
HWND hPluginWnd;
HWND BBhwnd;
HINSTANCE m_hMainInstance;
bool usingNT;


//
// Our windowproc. We will catch WM_CREATE in order to begin listening to
// blackbox messages, and the WM_DESTROY in order to stop doing it. The
// BB_BROADCAST message is used to send broams to plugins.
//
LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	static int msgs[] = {BB_RECONFIGURE, BB_BROADCAST, 0};

	switch (message)
	{
		case WM_CREATE:
			dbg_printf("Registering messages");
			SendMessage(BBhwnd, BB_REGISTERMESSAGE, (WPARAM)hwnd, (LPARAM)msgs);
			break;

		case WM_DESTROY:
			dbg_printf("Unregistering messages");
			SendMessage(BBhwnd, BB_UNREGISTERMESSAGE, (WPARAM)hwnd, (LPARAM)msgs);
			break;

		case BB_RECONFIGURE:
			break;

		// A broam! Check if the first 7 chars are "@bbIME".
		case BB_BROADCAST:
			if (0 == _memicmp((LPCSTR)lParam, "@bbIME.", 7))
			{
				const char *bbmsg = (LPCSTR)lParam + 7;
				if (_stricmp(bbmsg, "NextLanguage") == 0) {
					dbg_printf("Switching to next language!");
					HWND hWnd=GetForegroundWindow();
					if (hWnd)
						PostMessage(hWnd,WM_INPUTLANGCHANGEREQUEST,0,(LPARAM)HKL_NEXT);
				}
				if (_stricmp(bbmsg, "PreviousLanguage") == 0) {
					HWND hWnd=GetForegroundWindow();
					if (hWnd)
						PostMessage(hWnd,WM_INPUTLANGCHANGEREQUEST,0,(LPARAM)HKL_PREV);
					break;
				}
			}
			break;
		
		// Call the default window proc. Forgetting this may be painful since
		// it will cause CreateWindowEx to fail without any error message.
		default:
			return DefWindowProc(hwnd, message, wParam, lParam);
	}
	return 0;
}

//
// beginPlugin is called by blackbox to start the plugin. 
//
int beginPlugin(HINSTANCE hPluginInstance)
{
	DWORD dw;
	
	// If BBhwnd is already set, the plugin has already been loaded.
	if (BBhwnd)
	{
		MessageBox(NULL, "Dont load me twice!", szAppName, MB_OK|MB_TOPMOST|MB_SETFOREGROUND);
		return 1;
	}

	// Grab the HWND of the blackbox window. 
	BBhwnd = GetBBWnd();

	// Match the version to make sure it's bbLean
	if (0 != _memicmp(GetBBVersion(), "bbLean", 6))
	{
		MessageBox(NULL, "This plugin requires bbLean.", szVersion, MB_OK|MB_TOPMOST|MB_SETFOREGROUND);
		return 1;
	}

	// Now we store the main instance of the plugin as passed to the function.
	m_hMainInstance = hPluginInstance;

	// Stub to detect if we're running under a NT based system (2K/XP)
	OSVERSIONINFO osInfo;
	ZeroMemory(&osInfo, sizeof(osInfo));
	osInfo.dwOSVersionInfoSize = sizeof(osInfo);
	GetVersionEx(&osInfo);
	usingNT = osInfo.dwPlatformId == VER_PLATFORM_WIN32_NT;

	// First we declare our windowclass
	WNDCLASS wc; ZeroMemory(&wc, sizeof(wc));
	wc.lpfnWndProc = WndProc;            // our window procedure
	wc.hInstance = m_hMainInstance;
	wc.lpszClassName = szWindowClass;          // our window class name

	// And then we try to register it. If it fail, bail.
	if (!RegisterClass(&wc)) {
		dw = GetLastError(); 
		ErrorStrn("RegisterClass",dw);
		return 1;
	}

	// Time to create our window
	hPluginWnd = CreateWindowEx(
		WS_EX_TOOLWINDOW,       // exstyles
		szWindowClass,          // our window class name
		NULL,                   // use description for a window title
		WS_POPUP,               // styles
		0, 0,                   // position
		0, 0,                   // width & height of window
		NULL,                   // parent window
		NULL,                   // no menu
		m_hMainInstance,        // hInstance of DLL
		NULL                    //
		);

	// If we get a NULL here, the CreateWindowEx call failed. 
	if (hPluginWnd == NULL) 
	{
		// So we grab the error code and pass it on to the ErrorStrn function.
		dw = GetLastError(); 
		ErrorStrn("CreateWindowEx",dw);
		
		// Then we unregister our windowclass.
		UnregisterClass(szWindowClass, m_hMainInstance);
		return 1;
	}

	return 0;
}

//
// endPlugin is called on unload. Here we destroy the window and unregister
// the windowclass.
//
void endPlugin(HINSTANCE hPluginInstance)
{
	DestroyWindow(hPluginWnd);
	UnregisterClass(szWindowClass, m_hMainInstance);
}



//===========================================================================

//
// ErrorStrn returns a formatted error message. 
//
void ErrorStrn(LPTSTR lpszFunction, DWORD dw) 
{ 
    LPVOID lpMsgBuf;
    LPVOID lpDisplayBuf;

    FormatMessage(
        FORMAT_MESSAGE_ALLOCATE_BUFFER | 
        FORMAT_MESSAGE_FROM_SYSTEM |
        FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL,
        dw,
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        (LPTSTR) &lpMsgBuf,
        0, NULL );

    lpDisplayBuf = (LPVOID)LocalAlloc(LMEM_ZEROINIT, 
        (lstrlen((LPCTSTR)lpMsgBuf)+lstrlen((LPCTSTR)lpszFunction)+40)*sizeof(TCHAR)); 
    dbg_printf("%s failed with error %d: %s", 
        lpszFunction, dw, lpMsgBuf); 

    LocalFree(lpMsgBuf);
    LocalFree(lpDisplayBuf);
}
