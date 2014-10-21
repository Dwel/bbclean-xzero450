#include "plugin.h"
#include "BBCmd.h"
#include <BBApi.h>
#include <stdlib.h>

BBCmd * g_bbCmd = 0;

/* ---------------------------------- */
/* Global variables */

HINSTANCE g_hInstance;
HWND g_hSlit;
HWND g_BBhwnd;

/* ------------------------------------------------------------------ */
LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    return g_bbCmd->WndProc(hwnd, message, wParam, lParam);
}

/* ------------------------------------------------------------------ */
/* The startup interface */
/* slit interface */
int beginPluginEx (HINSTANCE hPluginInstance, HWND hSlit)
{
    if (g_BBhwnd)
    {
        MessageBox(g_BBhwnd, TEXT("Do not load me twice!"), szVersion, MB_OK | MB_ICONERROR | MB_TOPMOST);
        return 1; /* 1 = failure */
    }

    // grab some global information
    g_BBhwnd = GetBBWnd();
    g_hInstance = hPluginInstance;
    g_hSlit = hSlit;

    //register the window class
    WNDCLASS wc;
    memset(&wc, 0, sizeof(wc));
    wc.lpfnWndProc  = WndProc;      /* window procedure */
    wc.hInstance    = g_hInstance;  /* hInstance of .dll */
    wc.lpszClassName = szAppName;    /* window class name */
    wc.hCursor      = LoadCursor(NULL, IDC_ARROW);
    wc.style        = CS_DBLCLKS;
    if (!RegisterClass(&wc))
    {
        MessageBox(g_BBhwnd, TEXT("Error registering window class"), szVersion, MB_OK | MB_ICONERROR | MB_TOPMOST);
        return 1; /* 1 = failure */
    }

    // and init bbCmd
    g_bbCmd = new BBCmd;
    return g_bbCmd->Init();
}

/* no-slit interface */
int beginPlugin (HINSTANCE hPluginInstance)
{
    return beginPluginEx(hPluginInstance, NULL);
}

/* ------------------------------------------------------------------ */
/* on unload... */
void endPlugin (HINSTANCE hPluginInstance)
{
    g_bbCmd->Done();
    delete g_bbCmd;
    g_bbCmd = 0;

    /* Unregister window class... */
    UnregisterClass(szAppName, hPluginInstance);
}

/* ------------------------------------------------------------------ */
/* pluginInfo is used by Blackbox for Windows to fetch information
   about a particular plugin. */
//LPTSTR pluginInfo (int index)
LPCSTR pluginInfo(int index)
{
    switch (index)
    {
        case PLUGIN_NAME:       return szAppName;       /* Plugin name */
        case PLUGIN_VERSION:    return szInfoVersion;   /* Plugin version */
        case PLUGIN_AUTHOR:     return szInfoAuthor;    /* Author */
        case PLUGIN_RELEASE:    return szInfoRelDate;   /* Release date, preferably in yyyy-mm-dd format */
        case PLUGIN_LINK:       return szInfoLink;      /* Link to author's website */
        case PLUGIN_EMAIL:      return szInfoEmail;     /* Author's email */
        default:                return szVersion;       /* Fallback: Plugin name + version, e.g. "MyPlugin 1.0" */
    }
}


