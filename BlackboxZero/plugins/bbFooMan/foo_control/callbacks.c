#include "callbacks.h"

static FCALLBACK callbacks[10];

PLAY_CALLBACK play_callback=
{
	on_playback_starting,
	on_playback_new_track,
	on_playback_stop,
	on_playback_seek,
	on_playback_pause,
	on_playback_edited,
	on_playback_dynamic_info,
	on_playback_dynamic_info_track,
	on_playback_time,
	on_volume_change,
};
PPLAY_CALLBACK pplay_callback=&play_callback;

int AddCallback(FCALLBACK callback[], int back_index, HWND newhWnd, char *str)
{
	char *data=0;
	if(callback[back_index].count>=MAX_CALLBACK || !newhWnd) return P_ERROR;
	
	if(str)
	{
		data = calloc(strlen(str)+1, 1);
		strcpy(data, str);
	}

	callbacks[back_index].wcallback[callback[back_index].count].hwnd = newhWnd;
	callbacks[back_index].wcallback[callback[back_index].count].strmem = data;
	return ++callback[back_index].count;
}

int RemoveCallback(FCALLBACK callback[], int back_index, HWND delhWnd)
{
	int hwndi=0;
	bool found=false;

	if(!callback[back_index].count) return P_ERROR;
	while(hwndi < callback[back_index].count)
		if(callback[back_index].wcallback[hwndi++].hwnd == delhWnd) {found=true; break;}
	if(!found) return P_ERROR;
	callback[back_index].count--;
	hwndi--;
	if(callback[back_index].wcallback[hwndi].strmem) free(callback[back_index].wcallback[hwndi].strmem);
	for(int i=hwndi; i<callback[back_index].count; i++)
	{
		callback[back_index].wcallback[i].hwnd = callback[back_index].wcallback[i+1].hwnd;
		callback[back_index].wcallback[i].strmem = callback[back_index].wcallback[i+1].strmem;
	}
	callback[back_index].wcallback[callback[back_index].count].hwnd = 0;
	callback[back_index].wcallback[callback[back_index].count].strmem = 0;
	return callback[back_index].count;
}

VOID CALLBACK ClearCallback(HWND hWnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime)
{
	for(int i=0; i<10; i++)
		for(int j=0; j<callbacks[i].count; j++)
			if(!GetWindowLong(callbacks[i].wcallback[j].hwnd, GWL_STYLE))
				RemoveCallback((PFCALLBACK)&callbacks, i, callbacks[i].wcallback[j].hwnd);
}

void on_playback_starting(int t_track_command, bool p_paused)
{
	for(int i=0; i<callbacks[CSTARTING].count; i++)
		SendMessage(callbacks[CSTARTING].wcallback[i].hwnd, WM_FOO_CALLBACK, PLAYBACK_START, t_track_command);
}

void on_playback_new_track(PMETADB_HANDLE *p_track)
{
	wchar_t w_title[512];
	PTITLEFORMAT_OBJECT *t_obj=0;	// Must be 0!!!
	if(!ptcomp_api || !p_track) {service_release((PSERVICE_BASE*)p_track); return;}
	
	for(int i=0; i<callbacks[CNEW_TRACK].count; i++)
	{
		if(compile(&ptcomp_api, &t_obj, callbacks[CNEW_TRACK].wcallback[i].strmem))
		{
			string_free();
			format_title(p_track, 0, &str, &t_obj, 0);
			MultiByteToWideChar(CP_UTF8, 0, str.data, -1, w_title, 512);
			SetWindowTextW(p_hWnd, w_title);
			service_release((PSERVICE_BASE*)t_obj);
		}
		SendMessage(callbacks[CNEW_TRACK].wcallback[i].hwnd, WM_FOO_CALLBACK, PLAYBACK_NEW_TRACK, 0);
	}
	service_release((PSERVICE_BASE*)p_track);
}

void on_playback_stop(int t_stop_reason)
{
	for(int i=0; i<callbacks[CSTOP].count; i++)
		SendMessage(callbacks[CSTOP].wcallback[i].hwnd, WM_FOO_CALLBACK, PLAYBACK_STOP, t_stop_reason);
}

void on_playback_seek(double p_time)
{
	for(int i=0; i<callbacks[CSEEK].count; i++)
		SendMessage(callbacks[CSEEK].wcallback[i].hwnd, WM_FOO_CALLBACK, PLAYBACK_SEEK, p_time);
}

void on_playback_pause(bool p_state)
{
	for(int i=0; i<callbacks[CPAUSE].count; i++)
		SendMessage(callbacks[CPAUSE].wcallback[i].hwnd, WM_FOO_CALLBACK, PLAYBACK_PAUSE, p_state);
}

void on_playback_edited(PMETADB_HANDLE *p_track)
{
	wchar_t w_title[512];
	PTITLEFORMAT_OBJECT *t_obj=0;	// Must be 0!!!
	if(!ptcomp_api || !p_track) {service_release((PSERVICE_BASE*)p_track); return;}
	
	for(int i=0; i<callbacks[CNEW_TRACK].count; i++)
	{
		if(compile(&ptcomp_api, &t_obj, callbacks[CNEW_TRACK].wcallback[i].strmem))
		{
			string_free();
			format_title(p_track, 0, &str, &t_obj, 0);
			MultiByteToWideChar(CP_UTF8, 0, str.data, -1, w_title, 512);
			SetWindowTextW(p_hWnd, w_title);
			service_release((PSERVICE_BASE*)t_obj);
		}
		SendMessage(callbacks[CNEW_TRACK].wcallback[i].hwnd, WM_FOO_CALLBACK, PLAYBACK_EDITED, 0);
	}
	service_release((PSERVICE_BASE*)p_track);
}

void on_playback_dynamic_info(PFILE_INFO *p_info)
{
	for(int i=0; i<callbacks[CDYNAMIC_INFO].count; i++)
		SendMessage(callbacks[CDYNAMIC_INFO].wcallback[i].hwnd, WM_FOO_CALLBACK, PLAYBACK_DYNAMIC_INFO, 0);
}

void on_playback_dynamic_info_track(PFILE_INFO *p_info)
{
	for(int i=0; i<callbacks[CDYNAMIC_INFO_TRACK].count; i++)
		SendMessage(callbacks[CDYNAMIC_INFO_TRACK].wcallback[i].hwnd, WM_FOO_CALLBACK, PLAYBACK_DYNAMIC_INFO_TRACK, 0);
}

void on_playback_time(double p_time)
{
	for(int i=0; i<callbacks[CTIME].count; i++)
		SendMessage(callbacks[CTIME].wcallback[i].hwnd, WM_FOO_CALLBACK, PLAYBACK_TIME, p_time);
}

void on_volume_change(float p_new_val)
{
	for(int i=0; i<callbacks[CVOLUME_CHANGE].count; i++)
		SendMessage(callbacks[CVOLUME_CHANGE].wcallback[i].hwnd, WM_FOO_CALLBACK, PLAYBACK_VOLUME_CHANGE, p_new_val);
}
