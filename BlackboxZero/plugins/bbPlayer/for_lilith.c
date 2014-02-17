/* BBPlayer plugin for sound player Lilith */

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <shellapi.h>

#include <BBApi-C.h>

#include "BBPlayerSDK.h"




static struct {
	char dde_client[MAX_PATH];
} G;




#define NUMBER_OF(a) (sizeof(a) / sizeof((a)[0]))


static void
SNPRINTF(char* buf, int size, const char* format, ...)
{
	va_list va;

	va_start(va, format);
	vsnprintf(buf, size, format, va);
	va_end(va);

	buf[size - 1] = '\0';
}








DLL_EXPORT int
BBPlayer_PluginBegin( HINSTANCE bbplayer_instance,
                      HINSTANCE bbplayer_plugin_instance )
{
	char rc[MAX_PATH];
	char tmp[MAX_PATH];
	char* p;
	const char* dde_client;
	const char ERROR_MESSAGE[] = "\
Please add the following line in `BBPlayer.rc':\n\
\n\
bbplayer.for_lilith.dde_client: <Path to DDEClient.exe>\n\
\n\
`DDEClient.exe' is included Lilith's archive.\
";

	GetModuleFileName(bbplayer_instance, tmp, NUMBER_OF(tmp));
	tmp[NUMBER_OF(tmp) - 1] = '\0';
	p = strrchr(tmp, '.');
	if (p != NULL) p[1] = '\0';
	SNPRINTF(rc, NUMBER_OF(rc), "%s%s", tmp, "rc");

	dde_client = ReadString(rc, "bbplayer.for_lilith.dde_client:", "");
	SNPRINTF(G.dde_client, NUMBER_OF(G.dde_client), "%s", dde_client);

	if (!strcmp(G.dde_client, "")) {
		MessageBox( NULL, ERROR_MESSAGE, "BBPlayer - for_lilith",
		            MB_OK | MB_ICONERROR | MB_SETFOREGROUND
		              | MB_TASKMODAL | MB_TOPMOST );
		return BBPlayer_FAILURE;
	}

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
	case PLUGIN_NAME:    return "for_lilith";
	case PLUGIN_VERSION: return "0.0.0";
	case PLUGIN_AUTHOR:  return "kana";
	case PLUGIN_RELEASE: return "2004-07-12/2005-01-11";
	case PLUGIN_LINK:    return "";
	case PLUGIN_EMAIL:   return "";
	}
}








DLL_EXPORT int
BBPlayer_GetMusicInfo( _BBPlayer_PosType pos_type, int pos,
                       _BBPlayer_MusicInfoType info_type,
                       char* buf, int size )
{
	HWND lilith;

	/* BUGS: Not implemented many cases! */
	if (pos_type != BBPlayer_CURRENT || info_type != BBPlayer_DEFAULT)
		return BBPlayer_FAILURE;

	lilith = FindWindow("Lilith", NULL);
	if (lilith == NULL) return BBPlayer_FAILURE;

	buf[0] = '\0';
	GetWindowText(lilith, buf, size);
	buf[size - 1] = '\0';

	return BBPlayer_SUCCESS;
}




static void
execute_dde_client(const char* arg)
{
	ShellExecute(NULL, "open", G.dde_client, arg, NULL, SW_SHOWNORMAL);
}




DLL_EXPORT int
BBPlayer_Control(_BBPlayer_ControlType type, int n)
{
	const char* arg;
	char buf[80];

	switch (type) {
	default:
		return BBPlayer_FAILURE;

	case BBPlayer_PLAY:
		if (0 < n) {
			SNPRINTF( buf, NUMBER_OF(buf),
			          "/pos %d /newplay", n - 1 );
			arg = buf;
		} else {
			arg = "/command 40002";
		}
		break;

	case BBPlayer_STOP: arg = "/stop"; break;
	case BBPlayer_PAUSE: arg = "/pause"; break;
	case BBPlayer_NEXT: arg = "/next"; break;
	case BBPlayer_PREV: arg = "/back"; break;
	}

	execute_dde_client(arg);

	return BBPlayer_SUCCESS;
}




DLL_EXPORT void
BBPlayer_InterpretBroam(const char* broam)
{
	if (!strncmp(broam, "@BBPlayer.for_lilith.dde ", 25)) {
		execute_dde_client(broam + 25);
	}

	return;
}




/* __END__ */
