/*

    LeanBar2: The bbLeanBar replacement
    (c) 2008, bbClean Developers

    This project is released under the Gnu Public License (GPL) Version 2.

*/

#include "leanbar.h"

// ----------------------------------
// plugin info

LPCSTR szVersion        = "LeanBar2 0.1 (BBClean SVN)";
LPCSTR szAppName        = "LeanBar2";
LPCSTR szInfoVersion    = "0.1 (BBClean SVN)";
LPCSTR szInfoAuthor     = "Noccy";
LPCSTR szInfoRelDate    = __DATE__;
LPCSTR szInfoLink       = "http://dev.noccy.com";
LPCSTR szInfoEmail      = "lazerfisk@yahoo.com";

extern "C"
{
	DLL_EXPORT int beginPlugin(HINSTANCE hPluginInstance);
	DLL_EXPORT int beginSlitPlugin(HINSTANCE hPluginInstance, HWND hSlit);
	DLL_EXPORT int beginPluginEx(HINSTANCE hPluginInstance, HWND hSlit);
	DLL_EXPORT void endPlugin(HINSTANCE hPluginInstance);
	DLL_EXPORT LPCSTR pluginInfo(int field);
};

LeanBar *LeanBarPlugin;

int beginPlugin(HINSTANCE hInstance)
{
	LeanBarPlugin = new LeanBar(hInstance);
    return 0;
}

int beginSlitPlugin(HINSTANCE hInstance, HWND hSlit)
{
	LeanBarPlugin = new LeanBar(hInstance);
	return 0;
}

int beginPluginEx(HINSTANCE hInstance, HWND slit)
{
	LeanBarPlugin = new LeanBar(hInstance);
	return 0;
}

void endPlugin(HINSTANCE hPluginInstance)
{
    delete LeanBarPlugin;
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

