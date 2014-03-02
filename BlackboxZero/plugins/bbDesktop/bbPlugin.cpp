/*
	bbDesktop -- Plugin version of bbDesktopIcons

	Adds desktop icons to your blackbox distribution. Point it to a folder and
	it will render whatever it finds there onto your desktop. Icon positions
	will be saved and remembered.

	(c) 2007, Noccy <dev.noccy.com>

	Distributed under GPL2+
*/

#define __PLUGIN

#include "BBApi.h"
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>

#include "DrawIcons.h"

// ----------------------------------
// plugin info

LPCSTR szVersion        = "bbDesktop 0.1 (BBClean SVN)";
LPCSTR szAppName        = "bbDesktop";
LPCSTR szInfoVersion    = "0.1 (BBClean SVN)";
LPCSTR szInfoAuthor     = "Noccy";
LPCSTR szInfoRelDate    = __DATE__;
LPCSTR szInfoLink       = "http://dev.noccy.com";
LPCSTR szInfoEmail      = "lazerfisk@yahoo.com";

// ----------------------------------
// The About MessageBox

void about_box(void)
{
	char szTemp[1000];
	sprintf(szTemp,
		"%s - Adds icons to your desktop!"
		"\n"
		"\n© 2004 %s"
		"\n%s"
		, szVersion, szInfoEmail, szInfoLink
		);
	MessageBox(NULL, szTemp, "About", MB_OK|MB_TOPMOST);
}

// ----------------------------------
// Interface declaration

extern "C"
{
	DLL_EXPORT int beginPlugin(HINSTANCE hPluginInstance);
	DLL_EXPORT int beginSlitPlugin(HINSTANCE hPluginInstance, HWND hSlit);
	DLL_EXPORT int beginPluginEx(HINSTANCE hPluginInstance, HWND hSlit);
	DLL_EXPORT void endPlugin(HINSTANCE hPluginInstance);
	DLL_EXPORT LPCSTR pluginInfo(int field);
};

// ----------------------------------
// Global vars

HINSTANCE hInstance;

// receives the path to "bbSDK.rc"
char rcpath[MAX_PATH];




LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

// case insensitive string compare, up to lenght of second string
int my_substr_icmp(const char *a, const char *b)
{
	return _memicmp(a, b, strlen(b));
}


int beginSlitPlugin(HINSTANCE hInstance, HWND hSlit)
{
	return beginPlugin(hInstance);
}

int beginPluginEx(HINSTANCE hInstance, HWND slit)
{
  return beginPlugin(hInstance);
}

int beginPlugin(HINSTANCE hInstance)
{
    Main(hInstance);
    return 0;
}

void endPlugin(HINSTANCE hPluginInstance)
{
    CleanUp();
}

//===========================================================================
// pluginInfo is used by Blackbox for Windows to fetch information about
// a particular plugin.

LPCSTR pluginInfo(int field)
{
	switch (field)
	{
		case PLUGIN_NAME:           return szAppName;       // Plugin name
		case PLUGIN_VERSION:        return szInfoVersion;   // Plugin version
		case PLUGIN_AUTHOR:         return szInfoAuthor;    // Author
		case PLUGIN_RELEASE:        return szInfoRelDate;   // Release date, preferably in yyyy-mm-dd format
		case PLUGIN_LINK:           return szInfoLink;      // Link to author's website
		case PLUGIN_EMAIL:          return szInfoEmail;     // Author's email
		default:                    return szVersion;       // Fallback: Plugin name + version, e.g. "MyPlugin 1.0"
	}
}

