/*
	bbDesktop -- Plugin version of bbDesktopIcons

	Adds desktop icons to your blackbox distribution. Point it to a folder and
	it will render whatever it finds there onto your desktop. Icon positions
	will be saved and remembered.

	(c) 2007, Noccy <dev.noccy.com>

	Distributed under GPL2+
*/

#include <windows.h>
#include <string>
#include <vector>
#include <iostream>

#include "Utils.h"
#include "convert.hpp"

enum eTextEffect {
	EF_NONE = 0,
	EF_SHADOW,
	EF_OUTLINE
};

struct SettingsType {
	int MaxIconLabelWidth;
	int GridSize;
	bool SnapToGrid;
	std::string FontName;
	int FontSize;
	COLORREF ForeColor;
	COLORREF BackColor;
	eTextEffect Effect;
} Settings;

// debugging (checkout "DBGVIEW" from "http://www.sysinternals.com/")
void dbg_printf(const char *fmt, ...)
{
	char buffer[4096]; va_list arg;
	va_start(arg, fmt);
	vsprintf (buffer, fmt, arg);
	OutputDebugString(buffer);
}

#define BB_RECONFIGURE          10103


#include "configfile.h"
#include "stringutils.h"
#include "DesktopIcon.hpp"
#include "DrawIcons.h"

DesktopIcon* myIcon[256];

HINSTANCE hInst;
WNDPROC OldWndProc;
LRESULT CALLBACK DesktopWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);






//
//  Returns the HWND of the blackbox desktop window. This is where we'll paint
//  our icons, and it's also the window we need to hook to be able to move the
//  icons.
//
HWND GetBlackboxWindow()
{

    HWND bbhwnd;
    bbhwnd = FindWindow("DesktopBackgroundClass", NULL);
    if (!bbhwnd) bbhwnd = FindWindow("BBLeanDesktop", NULL);
    if (!bbhwnd) bbhwnd = GetDesktopWindow();
    return(bbhwnd);

}

//
//  Since I'm lazy, this function returns the Device Context of the Blackbox
//  desktop ;)
//
HDC GetBlackboxDC()
{

    return(GetDC(GetBlackboxWindow()));

}

//
//  Main function, should be replaced by the proper plugin code eventually
//
int Main(HINSTANCE hInstance)
{

    // Save the hInstance
    hInst = hInstance;

    Settings.MaxIconLabelWidth = 60;
    Settings.Effect = EF_SHADOW;

    int n;
    for (n=0;n<256;n++) myIcon[n] = NULL;

    ReadConfiguration();
    PaintIcons(GetBlackboxDC());

    // Hook the desktop
#if defined _WIN64
    OldWndProc = (WNDPROC)SetWindowLongPtr(GetBlackboxWindow(), GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(DesktopWndProc));
#else
	OldWndProc = (WNDPROC)SetWindowLong(GetBlackboxWindow(), GWL_WNDPROC, (LONG)DesktopWndProc);
#endif
    return(0);
}

//
// Read the configuration from the icons.rc file
//
bool ReadConfiguration()
{
    std::string root; std::string key; std::string title; std::string command;
    std::string params; std::string workdir; std::string iconfile;
    int iconx; int icony;

    // Grab the path of the plugin and append the filename of the .rc file
    char dllpath[MAX_PATH];
    std::string rcpath;
    std::string iconpath;
    GetModuleFileName(hInst, dllpath, MAX_PATH);
    rcpath = strpath(ConvertToString(dllpath)) + "\\bbdesktop.rc";
    iconpath = strpath(ConvertToString(dllpath)) + "\\icons.rc";

    //dbg_printf("Attempting to read config from %s...",rcpath.c_str());

    ConfigFile config(rcpath);
    Settings.MaxIconLabelWidth = config.GetInt("bbDesktop.maxIconLabelWidth",60);
    std::string texteffect = config.GetString("bbDesktop.textEffect","shadow");
    dbg_printf("Effect = %s",texteffect.c_str());
    if (texteffect == "outline") {
        Settings.Effect = EF_OUTLINE;
    } else if (texteffect == "shadow") {
        Settings.Effect = EF_SHADOW;
    } else {
        Settings.Effect = EF_NONE;
    }
    Settings.ForeColor = 0xE0E0E0; // GetSysColor(COLOR_CAPTIONTEXT)
    Settings.BackColor = 0x202020; // GetSysColor(COLOR_3DDKSHADOW)

    // Access the config file
    ConfigFile rcfile(iconpath);
    int icons = rcfile.GetInt("bbdesktop.iconcount",0);

    // Now, enumerate the icons from the file
    for (int n = 1; n <= icons; n++)
    {
        root = "bbdesktop.icon" + ConvertToString(n) + ".";
        key = root + "title";
        dbg_printf("Trying to read key %s...", key.c_str());
        title = rcfile.GetString(key,"No title");
        dbg_printf("Returning from read...");
        command = rcfile.GetString(root + "command","");
        params = rcfile.GetString(root + "parameters","");
        workdir = rcfile.GetString(root + "workdir",".");
        iconfile = rcfile.GetString(root + "icon",command);
        iconx = rcfile.GetInt(root + "x",10);
        icony = rcfile.GetInt(root + "y",10);
        // Create a new icon here; icons in file are 1-based while we use 0-based lists here
        myIcon[n-1] = new DesktopIcon(title, command, params, workdir, iconfile, iconx, icony);
    }

    return(true);
}

bool CleanUp()
{
    // Release subclassing
#if defined _WIN64
    SetWindowLongPtr(GetBlackboxWindow(), GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(OldWndProc));
#else
    SetWindowLong(GetBlackboxWindow(), GWL_WNDPROC, (LONG)OldWndProc);
#endif
    // Send a InvalidateRect message to the desktop
    InvalidateRect(GetBlackboxWindow(), NULL, true);
    // Delete all current icons
    for (int n=0;n<256;n++) if (myIcon[n] != NULL) delete myIcon[n];
    return(true);
}

//
// This is a ugly implementation of reconfigure.
//
void Reconfigure()
{

    // Wipe the icons...
    for (int n=0;n<256;n++) if (myIcon[n] != NULL) delete myIcon[n];
    // ...and reread the configuraiton
    ReadConfiguration();

    // Finally invalidate the desktop so our icons get redrawn.
    InvalidateRect(GetBlackboxWindow(), NULL, true);

}

//
// Perform hittesting on *all* the icons.
//
bool IconHitTest(long X, long Y)
{

    //dbg_printf("Invoking IconHitTest...");

	for (int n=0;n<256;n++) {

        //dbg_printf("Testing icon %d...",n);
		if (myIcon[n] != NULL)
		{
		    if (myIcon[n]->IsMouseIn((int)X,(int)Y))
            {
                dbg_printf("Calling Invoke for %d...",n);
                myIcon[n]->Invoke();
                //dbg_printf("Returned from invoke");
                return true;
            }
		}

	}
	return false;

}

void PaintIcons(HDC dc)
{

    for (int n=0;n<256;n++) if (myIcon[n] != NULL) myIcon[n]->Paint(dc);

}

//
// Here is our hooked WndProc for the desktop. This is where we do the actual
// chomping of messages that Windows sends to the desktop.
//
LRESULT CALLBACK DesktopWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{

    static bool isPainting;
    PAINTSTRUCT  ps;
    HDC          pshdc;
	long MouseX;
	long MouseY;


    switch (msg) {
        case WM_PAINT:
        if (!(isPainting)) {
            isPainting = true;
			pshdc = BeginPaint(hwnd, &ps);
			// do the painting; start by drawing the actual desktop into the canvas so
			// we can paint our icons on top. Then draw the icons.
			PaintDesktop(pshdc);
            PaintIcons(pshdc);
			EndPaint(hwnd, &ps);
            isPainting = false;
        }
        return CallWindowProc(OldWndProc, hwnd, msg, wParam, lParam);
        break;
    case WM_LBUTTONDBLCLK:
		MouseX = LOWORD(lParam);
		MouseY = HIWORD(lParam);
        dbg_printf("DoubleClick at %d:%d",MouseX,MouseY);
        if (!(IconHitTest(MouseX,MouseY))) {
			return CallWindowProc(OldWndProc, hwnd, msg, wParam, lParam);
		}
		return 0;
        break;
    case BB_RECONFIGURE:
        Reconfigure();
        return CallWindowProc(OldWndProc, hwnd, msg, wParam, lParam);
        break;
    case WM_DESTROY:
        CleanUp();
         // dont add break here so that it will continue to default processing
    default:
        return CallWindowProc(OldWndProc, hwnd, msg, wParam, lParam);
    }

}
