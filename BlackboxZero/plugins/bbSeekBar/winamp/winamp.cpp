//BBSeekbar component for Winamp

#include <windows.h>

#define EXPORT extern "C" __declspec(dllexport)

typedef enum {
	WM_GETPOSITION,
	WM_GETLENGTH,
	WM_JUMPTOTIME
} playerDataType;

#define IPC_GETOUTPUTTIME 105
#define IPC_JUMPTOTIME 106


EXPORT int getPlayerData(playerDataType type, int value){

	HWND hwndWinamp = FindWindow("Winamp v1.x",NULL);
	if (hwndWinamp == NULL) return 0;

	switch (type) {
	case WM_GETPOSITION:
		return SendMessage(hwndWinamp, WM_USER, 0, IPC_GETOUTPUTTIME) / 1000;
	case WM_GETLENGTH:
		return SendMessage(hwndWinamp, WM_USER, 1, IPC_GETOUTPUTTIME);
	case WM_JUMPTOTIME:
		return SendMessage(hwndWinamp, WM_USER, value * 1000, IPC_JUMPTOTIME);
	}
	return 0;
}
