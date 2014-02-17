/* BBPlayer plugin for Winamp */

#include <ctype.h>
#include <stdlib.h>

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <BBApi-C.h>

#include "BBPlayerSDK.h"
#include "for_winamp.h"




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
	return BBPlayer_SUCCESS;    /* nothing to do */
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
	case PLUGIN_NAME:    return "for_winamp";
	case PLUGIN_VERSION: return "0.0.0";
	case PLUGIN_AUTHOR:  return "kana";
	case PLUGIN_RELEASE: return "2004-07-16/2005-01-11";
	case PLUGIN_LINK:    return "";
	case PLUGIN_EMAIL:   return "";
	}
}








static HWND
get_winamp_hwnd(void)
{
	return FindWindow("Winamp v1.x", NULL);
}


static int
is_playing(HWND player)
{
	return SendMessage(player, WM_WA_IPC, 0, IPC_ISPLAYING) == 1;
}


static void
set_playlist_pos(HWND player, int abs_pos)
{
	SendMessage(player, WM_WA_IPC, abs_pos - 1, IPC_SETPLAYLISTPOS);
}




DLL_EXPORT int
BBPlayer_GetMusicInfo( _BBPlayer_PosType pos_type, int pos,
                       _BBPlayer_MusicInfoType info_type,
                       char* buf, int size )
{
	HWND player;
	char* p;
	int playing_time;

	/* BUGS: Not implemented many cases! */
	if (pos_type != BBPlayer_CURRENT || info_type != BBPlayer_DEFAULT)
		return BBPlayer_FAILURE;

	/* Get current track information */
	player = get_winamp_hwnd();
	if (player == NULL) return BBPlayer_FAILURE;

	buf[0] = '\0';
	if (!GetWindowText(player, buf, size)) return BBPlayer_FAILURE;
	buf[size - 1] = '\0';

	/* Remove trailing needless string */
	p = strstr(buf, " - Winamp");
	if (p != NULL) *p = '\0';

	/* Remove leading needless string - playlist number */
	if (isdigit(buf[0]) && (p = strstr(buf, ". ")) != NULL) {
		memmove(buf, p + 2, sizeof(char) * (strlen(p+2) + 1));
	}

	/* Insert playing time into the head on buf */
	playing_time = SendMessage(player, WM_WA_IPC, 0, IPC_GETOUTPUTTIME);
	if (0 <= playing_time) {
		char tmp[80];
		int buf_len;
		int tmp_len;

		SNPRINTF( tmp, NUMBER_OF(tmp), "%02d:%02d - ",
		          (playing_time/1000) / 60,
		          (playing_time/1000) % 60 );

		buf_len = strlen(buf);
		tmp_len = strlen(tmp);

		if (buf_len + tmp_len + 1 <= size) {
			memmove(buf+tmp_len, buf, sizeof(char) * (buf_len+1));
			strncpy(buf, tmp, tmp_len);
		} else if (size <= tmp_len + 1) {
			SNPRINTF(buf, NUMBER_OF(buf), "%s", tmp);
		} else {
			int over_num;

			over_num = (buf_len + tmp_len + 1) - size;
			memmove( buf + tmp_len, buf,
			         sizeof(char) * (buf_len - over_num) );
			strncpy(buf, tmp, tmp_len);
			buf[size - 1] = '\0';
		}
	}

	return BBPlayer_SUCCESS;
}




DLL_EXPORT int
BBPlayer_Control(_BBPlayer_ControlType type, int n)
{
	HWND player;
	WPARAM command;

	player = get_winamp_hwnd();
	if (player == NULL) return BBPlayer_FAILURE;

	switch (type) {
	default:
		return BBPlayer_FAILURE;

	case BBPlayer_PLAY:
		if (0 < n) {
			set_playlist_pos(player, n);
			command = WINAMP_PLAY;
		} else {
			if (!is_playing(player))
				command = WINAMP_PLAY;
			else
				command = WINAMP_PAUSE;
		}
		break;

	case BBPlayer_STOP: command = WINAMP_STOP; break;
	case BBPlayer_PAUSE: command = WINAMP_PAUSE; break;
	case BBPlayer_NEXT: command = WINAMP_NEXTSONG; break;
	case BBPlayer_PREV: command = WINAMP_PREVSONG; break;
	}

	SendMessage(player, WM_COMMAND, command, 0);

	return BBPlayer_SUCCESS;
}




DLL_EXPORT void
BBPlayer_InterpretBroam(const char* broam)
{
	HWND player;

	player = get_winamp_hwnd();
	if (player == NULL)
		return;

	if (!strncmp(broam, "@BBPlayer.for_winamp.WM_COMMAND ", 32)) {
		SendMessage(player, WM_COMMAND, (WPARAM)atol(broam + 32), 0);
	}

	return;
}




/* __END__ */
