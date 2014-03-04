/* BBPlayer plugin for foobar2000 */

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <shellapi.h>

#include <BBApi.h>

#include "BBPlayerSDK.h"


#define NUMBER_OF(a) (sizeof(a) / sizeof((a)[0]))




static struct {
	char foobar2000_exe[MAX_PATH];
	char class_name[256];
} G;

#define FOOBAR2000_CLASS_DEFAULT_UI "{DA7CD0DE-1602-45e6-89A1-C2CA151E008E}"
#define FOOBAR2000_CLASS_COLUMNS_UI "{E7076D1C-A7BF-4f39-B771-BCBE88F2A2A8}"








DLL_EXPORT int
BBPlayer_PluginBegin( HINSTANCE bbplayer_instance,
                      HINSTANCE bbplayer_plugin_instance )
{
	char tmp[MAX_PATH];
	char rc[MAX_PATH + 2];  /* extra 2 for `rc' */
	char* p;
	const char* s;

	GetModuleFileName(bbplayer_instance, tmp, NUMBER_OF(tmp));
	tmp[NUMBER_OF(tmp) - 1] = '\0';
	p = strrchr(tmp, '.');
	if (p != NULL) p[1] = '\0';
	strcpy(rc, tmp);
	strcat(rc, "rc");

	s = ReadString(rc, "bbplayer.for_foobar2000.exe:", "");
	strncpy(G.foobar2000_exe, s, NUMBER_OF(G.foobar2000_exe));
	G.foobar2000_exe[NUMBER_OF(G.foobar2000_exe) - 1] = '\0';

	if (!strcmp(G.foobar2000_exe, "")) {
		MessageBox(
		  NULL,
		  "Please write the following line in `BBPlayer.rc':\n"
		    "\n"
		    "bbplayer.for_foobar2000.exe: <path to foobar2000.exe>",
		  "BBPlayer - for_foobar2000",
		  MB_OK | MB_ICONERROR | MB_SETFOREGROUND
		    | MB_TASKMODAL | MB_TOPMOST
		);
		return BBPlayer_FAILURE;
	}

	s = ReadString( rc, "bbplayer.for_foobar2000.class:",
	                FOOBAR2000_CLASS_DEFAULT_UI );
	strncpy(G.class_name, s, NUMBER_OF(G.class_name));
	G.class_name[NUMBER_OF(G.class_name) - 1] = '\0';

	return BBPlayer_SUCCESS;
}




DLL_EXPORT void
BBPlayer_PluginEnd( HINSTANCE bbplayer_instance,
                    HINSTANCE bbplayer_plugin_instance )
{
	return;    /* nothing to do */
}




DLL_EXPORT const char*
BBPlayer_PluginInfo(int field)
{
	switch (field) {
	default:             return "";
	case PLUGIN_NAME:    return "for_foobar2000";
	case PLUGIN_VERSION: return "0.0.1";
	case PLUGIN_AUTHOR:  return "kana";
	case PLUGIN_RELEASE: return "2005-01-11/19";
	case PLUGIN_LINK:    return "";
	case PLUGIN_EMAIL:   return "";
	}
}








static HWND
get_foobar2000_hwnd(void)
{
	return FindWindow(G.class_name, NULL);
}


static void
execute_foobar2000(const char* arg)
{
	ShellExecute(NULL, "open", G.foobar2000_exe, arg, NULL, SW_SHOWNORMAL);
}




DLL_EXPORT int
BBPlayer_GetMusicInfo( _BBPlayer_PosType pos_type, int pos,
                       _BBPlayer_MusicInfoType info_type,
                       char* buf, int size )
{
	HWND foobar2000;

	/* BUGS: Not implemented many cases! */
	if (pos_type != BBPlayer_CURRENT || info_type != BBPlayer_DEFAULT)
		return BBPlayer_FAILURE;

	foobar2000 = get_foobar2000_hwnd();
	if (foobar2000 == NULL) return BBPlayer_FAILURE;

	buf[0] = '\0';
	GetWindowText(foobar2000, buf, size);
	buf[size - 1] = '\0';

	return BBPlayer_SUCCESS;
}




DLL_EXPORT int
BBPlayer_Control(_BBPlayer_ControlType type, int n)
{
	HWND foobar2000;
	const char* arg;

	foobar2000 = get_foobar2000_hwnd();

	switch (type) {
	default:
		return BBPlayer_FAILURE;

	case BBPlayer_PLAY: arg = "/playpause"; break;
	case BBPlayer_STOP: arg = "/stop"; break;
	case BBPlayer_PAUSE: arg = "/pause"; break;
	case BBPlayer_NEXT: arg = "/next"; break;
	case BBPlayer_PREV: arg = "/prev"; break;
	}

	execute_foobar2000(arg);

	return BBPlayer_SUCCESS;
}




DLL_EXPORT void
BBPlayer_InterpretBroam(const char* broam)
{
	if (!strncmp(broam, "@BBPlayer.for_foobar2000.exe ", 29))
		execute_foobar2000(broam + 29);
	return;
}




/* __END__ */
