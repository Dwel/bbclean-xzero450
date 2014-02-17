#include "playback_contol.h"

bool is_playing(PPLAYBACK_CONTROL *p_api)
{
	__asm mov ecx, p_api;
	return (*p_api)->is_playing();
}

bool is_paused(PPLAYBACK_CONTROL *p_api)
{
	__asm mov ecx, p_api;
	return (*p_api)->is_paused();
}

void start(PPLAYBACK_CONTROL *p_api, int p_command)
{
	__asm mov ecx, p_api;
	(*p_api)->start(p_command, false);
}

void stop(PPLAYBACK_CONTROL *p_api)
{
	__asm mov ecx, p_api;
	(*p_api)->stop();
}

void pause(PPLAYBACK_CONTROL *p_api, bool p_state)
{
	__asm mov ecx, p_api;
	(*p_api)->pause(p_state);	
}

void play_pause(PPLAYBACK_CONTROL *p_api)
{
	pause(p_api, !is_paused(p_api));
}

void start_resume(PPLAYBACK_CONTROL *p_api)
{
	if(is_paused(p_api)) pause(p_api, 0);
	else start(p_api, track_command_play);
}

void volume_up(PPLAYBACK_CONTROL *p_api)
{
	__asm mov ecx, p_api;
	(*p_api)->volume_up();
}

void volume_down(PPLAYBACK_CONTROL *p_api)
{
	__asm mov ecx, p_api;
	(*p_api)->volume_down();
}

float get_volume(PPLAYBACK_CONTROL *p_api)
{
	__asm mov ecx, p_api;
	return (*p_api)->get_volume();
}

void set_volume(PPLAYBACK_CONTROL *p_api, int p_value)
{
	__asm mov ecx, p_api;
	(*p_api)->set_volume(p_value);
}

void mute(PPLAYBACK_CONTROL *p_api)
{
	__asm mov ecx, p_api;
	(*p_api)->volume_mute_toggle();
}

bool get_stop_after_current(PPLAYBACK_CONTROL *p_api)
{
	__asm mov ecx, p_api;
	return (*p_api)->get_stop_after_current();
}

void set_stop_after_current(PPLAYBACK_CONTROL *p_api, bool p_state)
{
	__asm mov ecx, p_api;
	(*p_api)->set_stop_after_current(p_state);
}

bool playback_can_seek(PPLAYBACK_CONTROL *p_api)
{
	__asm mov ecx, p_api;
	return (*p_api)->playback_can_seek();
}

void playback_seek(PPLAYBACK_CONTROL *p_api, int p_value)
{
	__asm mov ecx, p_api;
	(*p_api)->playback_seek(p_value);
}

void playback_seek_delta(PPLAYBACK_CONTROL *p_api, int p_value)
{
	__asm mov ecx, p_api;
	(*p_api)->playback_seek_delta(p_value);
}

double playback_get_position(PPLAYBACK_CONTROL *p_api)
{
	__asm mov ecx, p_api;
	return (*p_api)->playback_get_position();
}

double playback_get_length(PPLAYBACK_CONTROL *p_api)
{
	PMETADB_HANDLE *meta_h=0;
	__asm mov ecx, p_api;
	if((*p_api)->get_now_playing(&meta_h)) return meta_get_length(meta_h);
	return P_ERROR;
}
