// foo_seek_remote.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"
#include "foobar2000.h"
#include "foo_seek_remote.h"

BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved )
{
    switch ( ul_reason_for_call )
	{
		case DLL_PROCESS_ATTACH:
		case DLL_THREAD_ATTACH:
		case DLL_THREAD_DETACH:
		case DLL_PROCESS_DETACH:
			break;
    }

    return TRUE ;
}

// remote control window
HWND g_hwnd = NULL ;

// set of subscribed windows
THwndSet g_set_sub ;

// remote window procedure
LRESULT CALLBACK foo_SeekRemoteWndProc( HWND hWnd, UINT uMessage, WPARAM wParam, LPARAM lParam )
{
	static_api_ptr_t<playback_control> pbc;
	if ( hWnd == g_hwnd )
	{
		switch ( uMessage )
		{
		case WM_FOO_GETPOSITION:
			{
				double len = pbc->playback_get_position();
				return (LRESULT) len;
			}
			break;
		case WM_FOO_GETLENGTH:
			{
				double len = pbc->playback_get_length();
				return (LRESULT) len;
			}
			break;
		case WM_FOO_JUMPTOTIME:
			pbc->playback_seek(wParam);
			break;
		}
	}
	return DefWindowProc( hWnd, uMessage, wParam, lParam ) ;
}

// handles init and quit of plug-in
class initquit_ui : public initquit
{
	virtual void on_init()
	{
		WNDCLASS wcls;

		wcls.style             = 0 ;
		wcls.cbClsExtra        = 0 ;
		wcls.cbWndExtra        = 16 ;
		wcls.hbrBackground     = ( HBRUSH )NULL ;
		wcls.hIcon             = 0 ;
		wcls.hCursor           = NULL ;
		wcls.lpszMenuName      = _T( "" ) ;
		wcls.lpfnWndProc = foo_SeekRemoteWndProc ;
		wcls.hInstance = core_api::get_my_instance() ;
		wcls.lpszClassName = FOO_SEEK_REMOTE_WND_CLASS ;

		RegisterClass( &wcls ) ;

		g_hwnd = CreateWindow( FOO_SEEK_REMOTE_WND_CLASS, FOO_SEEK_REMOTE_WND_NAME,
								WS_POPUP, 0, 0, 10, 10, NULL, NULL,
								core_api::get_my_instance(), NULL ) ;
	}

	virtual void on_quit()
	{
/*		if ( current_track != NULL )
		{
			current_track->handle_release() ;
			current_track = NULL ;
		}*/

		//DistributeSingle( FOO_ACTION_QUIT ) ;

		g_set_sub.clear() ;

		if ( g_hwnd )
		{
			DestroyWindow( g_hwnd ) ;
			g_hwnd = NULL ;
		}
	}
};

// quit callback
static service_factory_single_t<initquit_ui> foo_init_quit ;

// plug-in version
DECLARE_COMPONENT_VERSION("Seek Remote Control", "0.0.2", 0)
