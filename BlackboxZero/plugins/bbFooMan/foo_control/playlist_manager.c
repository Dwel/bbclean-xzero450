#include "playlist_manager.h"

int get_active_playlist(PPLAYLIST_MANAGER *p_api)
{
	__asm mov ecx, p_api;
	return (*p_api)->get_active_playlist();
}

int activeplaylist_get_item_count(PPLAYLIST_MANAGER *p_api)
{
	__asm mov ecx, p_api;
	int playlist=(*p_api)->get_active_playlist();
	if(playlist!=INFINITE)
	{
		__asm mov ecx, p_api;
		return (*p_api)->playlist_get_item_count(playlist);
	}
	else return P_ERROR;
}

long long get_playing_item_location(PPLAYLIST_MANAGER *p_api)
{
	int playlist, index;
	__asm mov ecx, p_api;
	(*p_api)->get_playing_item_location(&playlist, &index);
	return index+playlist*0x100000000;	//ret: eax - current track; edx - current playlist
}

void playlist_item_format_title(PPLAYLIST_MANAGER *p_api, int p_playlist, int p_item, DWORD *p_hook,
				PSTRING p_out, PTITLEFORMAT_OBJECT **p_script	, DWORD *p_filter, int p_playback_info_level)
{
	__asm mov ecx, p_api;
	(*p_api)->playlist_item_format_title(p_playlist, p_item, (DWORD*)0, p_out, p_script,
						0, p_playback_info_level);
}

void playback_order_set_active(PPLAYLIST_MANAGER *p_api, int p_index)
{
	__asm mov ecx, p_api;
	(*p_api)->playback_order_set_active(p_index);
}

void playlist_execute_default_action(PPLAYLIST_MANAGER *p_api, int p_playlist, int p_index)
{
	__asm mov ecx, p_api;
	(*p_api)->playlist_execute_default_action(p_playlist, p_index);
}
