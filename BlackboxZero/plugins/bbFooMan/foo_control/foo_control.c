/*
 ============================================================================

  This program is free software, released under the GNU General Public License
  (GPL version 2 or later).

  This program is distributed in the hope that it will be useful, but
  WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
  or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License
  for more details.

  http://www.fsf.org/licenses/gpl.html

 ============================================================================
*/

#include "foo_control.h"

HWND p_hWnd;
PPLAYLIST_MANAGER plist_api;
PPLAYBACK_CONTROL pback_api;
PTITLEFORMAT_COMPILER ptcomp_api;
PPLAY_CALLBACK_MANAGER pplaycbackman;

#include "callbacks.c"

void init()
{
	WNDCLASS wc;
	plist_api = (PPLAYLIST_MANAGER)create_api(&g_api, &playlist_manager);
	pback_api = (PPLAYBACK_CONTROL)create_api(&g_api, &playback_control);
	ptcomp_api = (PTITLEFORMAT_COMPILER)create_api(&g_api, &titleformat_compiler);
	pplaycbackman = (PPLAY_CALLBACK_MANAGER)create_api(&g_api, &play_callback_manager);

	memset(&wc, 0x00, sizeof(wc));
	wc.lpfnWndProc = WndProc;
	wc.hInstance = g_hIns;
	wc.lpszClassName = FOO_REMOTE_WND_CLASS;
	if(!RegisterClass(&wc)) 
	{
		MessageBox(0, "Error registering window class", FOO_REMOTE_WND_CLASS, MB_OK | MB_ICONERROR | MB_TOPMOST);
		return;
	}
	p_hWnd=CreateWindow(FOO_REMOTE_WND_CLASS, 0, 0, 0, 0, 0, 0, 0, 0, g_hIns, 0);
	if(!p_hWnd)
	{
		MessageBox(0, "Error creating window", FOO_REMOTE_WND_CLASS, MB_OK | MB_ICONERROR | MB_TOPMOST);
		return;
	}

	register_callback(&pplaycbackman, &pplay_callback, flag_on_playback_all, FALSE);
	SetTimer(p_hWnd, CLEAR_CALLBACK_TIMER, 5*60*1000, ClearCallback);

	SendMessage(FindWindow("bbFooman", 0), WM_FOO_ACTIVATE, 0, 0); // used only for bbFooman plugin for blackbox
}

void quit()
{
	KillTimer(p_hWnd, CLEAR_CALLBACK_TIMER);
	unregister_callback(&pplaycbackman, &pplay_callback);
        DestroyWindow(p_hWnd);
	UnregisterClass(FOO_REMOTE_WND_CLASS, g_hIns);
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch(message)
	{	
		case WM_FOO_GET_ACTIVE_PLAYLIST_COUNT:
			if(plist_api) return activeplaylist_get_item_count(&plist_api);
			return P_ERROR;

		case WM_FOO_GET_PLAYLIST_ITEM:
		{
			char title[512];
			wchar_t w_title[512];
			if(!ptcomp_api || !plist_api || !pback_api) return P_ERROR;
			if(wParam == -1 && !is_playing(&pback_api)) return P_ERROR;
			int tracknumb = (wParam==-1)?(int)get_playing_item_location(&plist_api):wParam;
			PTITLEFORMAT_OBJECT *t_obj=0;	// Must be 0!!!
			if(!GetWindowText(p_hWnd, title, 512)) return P_ERROR;

			if(compile(&ptcomp_api, &t_obj, title))
			{
				string_free();
				playlist_item_format_title(&plist_api, get_active_playlist(&plist_api), tracknumb, 0, &str,
								&t_obj, 0, display_level_all);
				MultiByteToWideChar(CP_UTF8, 0, str.data, -1, w_title, 512);
				SetWindowTextW(p_hWnd, w_title);
				service_release((PSERVICE_BASE*)t_obj);
				return 0;
			}
			return P_ERROR;
		}

		case WM_FOO_GET_CURRENT_TRACK:
			if(plist_api) return get_playing_item_location(&plist_api);
			return P_ERROR;

		case WM_FOO_ORDER:
			if(plist_api) {playback_order_set_active(&plist_api, wParam); return 0;}
			return P_ERROR;

		case WM_FOO_OPEN:	return main_open(&g_api);
		case WM_FOO_ADD:	return main_add_files(&g_api);
		case WM_FOO_ADDDIR:	return main_add_directory(&g_api);

		case WM_FOO_PLAY:
			if(pback_api) {start_resume(&pback_api); return 0;}
			return P_ERROR;

		case WM_FOO_STOP:
			if(pback_api) {stop(&pback_api); return 0;}
			return P_ERROR;

		case WM_FOO_PLAY_NEXT:
			if(pback_api) {start(&pback_api, track_command_next); return 0;}
			return P_ERROR;

		case WM_FOO_PLAY_PREV:
			if(pback_api) {start(&pback_api, track_command_prev); return 0;}
			return P_ERROR;

		case WM_FOO_PLAY_RANDOM:
			if(pback_api) {start(&pback_api, track_command_rand); return 0;}
			return P_ERROR;

		case WM_FOO_PAUSE:
			if(pback_api) {pause(&pback_api, true); return 0;}
			return P_ERROR;

		case WM_FOO_PLAY_PAUSE:
			if(pback_api) {play_pause(&pback_api); return 0;}
			return P_ERROR;

		case WM_FOO_VOLUME_UP:
			if(pback_api) {volume_up(&pback_api); return 0;}
			return P_ERROR;

		case WM_FOO_VOLUME_DOWN:
			if(pback_api) {volume_down(&pback_api); return 0;}
			return P_ERROR;

		case WM_FOO_GET_VOLUME:
			if(pback_api) return get_volume(&pback_api);
			return P_ERROR;

		case WM_FOO_SET_VOLUME:
			if(pback_api) {set_volume(&pback_api, wParam); return 0;}
			return P_ERROR;

		case WM_FOO_MUTE:
			if(pback_api) {mute(&pback_api); return 0;}
			return P_ERROR;

		case WM_FOO_IS_PLAYING:
			if(pback_api) return (is_playing(&pback_api) && !is_paused(&pback_api));
			return P_ERROR;

		case WM_FOOL_IS_PAUSED:
			if(pback_api) return is_paused(&pback_api);
			return P_ERROR;

		case WM_FOO_GET_STOP_AFTER_CURRENT:
			if(pback_api) return get_stop_after_current(&pback_api);
			return P_ERROR;

		case WM_FOO_SET_STOP_AFTER_CURRENT:
			if(pback_api) {set_stop_after_current(&pback_api, wParam); return 0;}
			return P_ERROR;

		case WM_FOO_PLAYBACK_CAN_SEEK:
			if(pback_api) return playback_can_seek(&pback_api);
			return P_ERROR;

		case WM_FOO_PLAYBACK_SEEK:
			if(pback_api) {playback_seek(&pback_api, wParam); return 0;}
			return P_ERROR;

		case WM_FOO_PLAYBACK_SEEK_DELTA:
			if(pback_api) {playback_seek_delta(&pback_api, wParam); return 0;}
			return P_ERROR;

		case WM_FOO_GET_POSITION:
			if(pback_api) return playback_get_position(&pback_api);
			return P_ERROR;

		case WM_FOO_GET_LENGTH:
			if(pback_api) return playback_get_length(&pback_api);
			return P_ERROR;

		case WM_FOO_EXIT:	return main_exit(&g_api);
		case WM_FOO_ACTIVATE:	return main_activate(&g_api);
		case WM_FOO_HIDE:	return main_hide(&g_api);

		case WM_FOO_PLAY_TRACK:
			if(plist_api) {playlist_execute_default_action(&plist_api, get_active_playlist(&plist_api), wParam); return 0;}
			return P_ERROR;

		case WM_FOO_REGISTER_CALLBACK:
			if(wParam & PLAYBACK_START)			AddCallback((PFCALLBACK)&callbacks, CSTARTING, (HWND)lParam, 0);
			if(wParam & PLAYBACK_DYNAMIC_INFO) 		AddCallback((PFCALLBACK)&callbacks, CDYNAMIC_INFO, (HWND)lParam, 0);
			if(wParam & PLAYBACK_DYNAMIC_INFO_TRACK)	AddCallback((PFCALLBACK)&callbacks, CDYNAMIC_INFO_TRACK, (HWND)lParam, 0);
			if(wParam & PLAYBACK_TIME)			AddCallback((PFCALLBACK)&callbacks, CTIME, (HWND)lParam, 0);
			if(wParam & PLAYBACK_VOLUME_CHANGE)		AddCallback((PFCALLBACK)&callbacks, CVOLUME_CHANGE, (HWND)lParam, 0);
			if(wParam & PLAYBACK_STOP)			AddCallback((PFCALLBACK)&callbacks, CSTOP, (HWND)lParam, 0);
			if(wParam & PLAYBACK_SEEK)			AddCallback((PFCALLBACK)&callbacks, CSEEK, (HWND)lParam, 0);
			if(wParam & PLAYBACK_PAUSE)			AddCallback((PFCALLBACK)&callbacks, CPAUSE, (HWND)lParam, 0);
			if(wParam & PLAYBACK_NEW_TRACK)
			{
				char caption[512];
				if(GetWindowText(p_hWnd, caption, 512))	AddCallback((PFCALLBACK)&callbacks, CNEW_TRACK, (HWND)lParam, caption);
			}
			
			if(wParam & PLAYBACK_EDITED)
			{
				char caption[512];
				if(GetWindowText(p_hWnd, caption, 512))	AddCallback((PFCALLBACK)&callbacks, CEDITED, (HWND)lParam, caption);
			}
			return 0;

		case WM_FOO_UNREGISTER_CALLBACK:
			if(wParam & PLAYBACK_START)			RemoveCallback((PFCALLBACK)&callbacks, CSTARTING, (HWND)lParam);
			if(wParam & PLAYBACK_NEW_TRACK) 		RemoveCallback((PFCALLBACK)&callbacks, CNEW_TRACK, (HWND)lParam);
			if(wParam & PLAYBACK_STOP)			RemoveCallback((PFCALLBACK)&callbacks, CSTOP, (HWND)lParam);
			if(wParam & PLAYBACK_SEEK)			RemoveCallback((PFCALLBACK)&callbacks, CSEEK, (HWND)lParam);
			if(wParam & PLAYBACK_PAUSE)			RemoveCallback((PFCALLBACK)&callbacks, CPAUSE, (HWND)lParam);
			if(wParam & PLAYBACK_EDITED)			RemoveCallback((PFCALLBACK)&callbacks, CEDITED, (HWND)lParam);
			if(wParam & PLAYBACK_DYNAMIC_INFO) 		RemoveCallback((PFCALLBACK)&callbacks, CDYNAMIC_INFO, (HWND)lParam);
			if(wParam & PLAYBACK_DYNAMIC_INFO_TRACK)	RemoveCallback((PFCALLBACK)&callbacks, CDYNAMIC_INFO_TRACK, (HWND)lParam);
			if(wParam & PLAYBACK_TIME)			RemoveCallback((PFCALLBACK)&callbacks, CTIME, (HWND)lParam);
			if(wParam & PLAYBACK_VOLUME_CHANGE)		RemoveCallback((PFCALLBACK)&callbacks, CVOLUME_CHANGE, (HWND)lParam);
			return 0;

		default:		return DefWindowProc(hWnd, message, wParam, lParam);
	}
}
