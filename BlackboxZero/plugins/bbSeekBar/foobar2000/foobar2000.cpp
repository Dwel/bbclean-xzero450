//BBSeekbar component for foobar2000

#include <windows.h>

#define EXPORT extern "C" __declspec(dllexport)

typedef enum {
	WM_GETPOSITION,
	WM_GETLENGTH,
	WM_JUMPTOTIME
} playerDataType;

const TCHAR FOO_SEEK_REMOTE_WND_CLASS[] = TEXT( "FOO_SEEK_REMOTE_CONTROL" );

#define WM_FOO_GETPOSITION	(WM_USER + 1)
#define WM_FOO_GETLENGTH	(WM_USER + 2)
#define WM_FOO_JUMPTOTIME	(WM_USER + 3)


EXPORT int getPlayerData(playerDataType type, int value){

	HWND hwndFoo2k = FindWindow( FOO_SEEK_REMOTE_WND_CLASS, NULL);
	if (hwndFoo2k == NULL) return 0;

	switch (type) {
	case WM_GETPOSITION:
		return SendMessage(hwndFoo2k, WM_FOO_GETPOSITION, 0, 0);
	case WM_GETLENGTH:
		return SendMessage(hwndFoo2k, WM_FOO_GETLENGTH, 0, 0);
	case WM_JUMPTOTIME:
		return SendMessage(hwndFoo2k, WM_FOO_JUMPTOTIME, value, 0);
	}
	return 0;
}
