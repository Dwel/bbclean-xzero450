#pragma once
#include <BBApi.h>

/* prefix for our broadcast messages */
#define BROAM_PREFIX "@bbCmd."
#define BROAM(key) (BROAM_PREFIX key) /* concatenation */

/* configuration file */
#define RC_FILE TEXT("bbCmd.rc")

/* prefix for items in the configuration file */
#define RC_PREFIX "bbcmd."
#define RC_KEY(key) (RC_PREFIX key ":")

/* prefix for unique menu id's */
#define MENU_ID(key) ("bbCMD_ID" key)

/* ---------------------------------- */
/* plugin info */
const TCHAR szAppName      [] = TEXT("bbCmd");
const TCHAR szInfoVersion  [] = TEXT("0.1");
const TCHAR szInfoAuthor   [] = TEXT("mojmir");
const TCHAR szInfoRelDate  [] = TEXT("2014-10-21");
const TCHAR szInfoLink     [] = TEXT("http://blackbox4windows.com");
const TCHAR szInfoEmail    [] = TEXT("mojmir@somewhere");
const TCHAR szVersion      [] = TEXT("bbCmd 0.1"); /* fallback for pluginInfo() */
const TCHAR szCopyright    [] = TEXT("2014");

#ifdef __cplusplus
extern "C" {
#endif
    DLL_EXPORT int    beginPlugin      (HINSTANCE hPluginInstance);
    DLL_EXPORT int    beginSlitPlugin  (HINSTANCE hPluginInstance, HWND hSlit);
    DLL_EXPORT int    beginPluginEx    (HINSTANCE hPluginInstance, HWND hSlit);
    DLL_EXPORT void   endPlugin        (HINSTANCE hPluginInstance);
    //DLL_EXPORT LPTSTR pluginInfo       (int field);
#ifdef __cplusplus
};
#endif

extern HINSTANCE g_hInstance;
extern HWND g_hSlit;
extern HWND g_BBhwnd;
