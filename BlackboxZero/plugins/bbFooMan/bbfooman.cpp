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

#pragma comment(lib, "blackbox.lib")
#pragma comment(lib, "gdiplus.lib")
#pragma comment(linker, "/EXPORT:beginPlugin")
#pragma comment(linker, "/EXPORT:endPlugin")
#pragma comment(linker, "/EXPORT:pluginInfo")

#include "bbFooman.h"

// Global
HINSTANCE hInstance;
HWND hWndPlugin, hWndBlackbox, rhWnd;
static char rcpath[MAX_PATH];
int curLength;

VOID WINGDIPAPI GdiplusStartupInputProcM(
	DebugEventProc debugEventCallback = NULL,
	BOOL suppressBackgroundThread = FALSE,
	BOOL suppressExternalCodecs = FALSE){};
VOID WINGDIPAPI DebugEventProcM(DWORD level, CHAR *message){};
GdiplusStartupInput GdiStartStruct={1, &DebugEventProcM, 0, 0, &GdiplusStartupInputProcM};
GdiplusStartupOutput GdiOutpustStruct;
HANDLE gdiToken;

#include "bbf_settings.c"

int CDECL beginPlugin(HINSTANCE hPluginInstance)
{
	WNDCLASS wc;
	hWndBlackbox = GetBBWnd();
	hInstance = hPluginInstance;
	ZeroMemory(&wc, sizeof(wc));
	wc.lpfnWndProc = WndProc;
	wc.hInstance = hPluginInstance;
	wc.lpszClassName = szAppName;
	if(!RegisterClass(&wc))
	{
		MessageBox(hWndBlackbox, "Error registering window class", szVersion, MB_OK | MB_ICONERROR | MB_TOPMOST);
		return 1;
	}
	hWndPlugin = CreateWindow(szAppName, 0, 0, 0, 0, 0, 0, 0, 0, hPluginInstance, 0);
	if(!hWndPlugin)
	{						   
		MessageBox(hWndBlackbox, "Error creating window", szVersion, MB_OK | MB_ICONERROR | MB_TOPMOST);
		return 1;
	}
	ReadRCSettings();

	if(setBBIAlbumArt && useBBInterface) GdiplusStartup(&gdiToken, &GdiStartStruct, &GdiOutpustStruct);

	if(useBBInterface)
	{
		SetCallbacks();
		UpdateBBInterface(PLAYBACK_VOLUME_CHANGE, SendMessage(rhWnd, WM_FOO_GET_VOLUME, 0, 0));
		SendMessage(rhWnd, WM_FOO_GET_PLAYLIST_ITEM, -1, 0);
		UpdateBBInterface(PLAYBACK_NEW_TRACK, 0);
	}

	SendMessage(hWndBlackbox, BB_REGISTERMESSAGE, (WPARAM)hWndPlugin, (LPARAM)msgs);
	return 0;
}

void CDECL endPlugin(HINSTANCE hPluginInstance)
{
	WriteRCSettings();
	if(useBBInterface) SendMessage(rhWnd, WM_FOO_UNREGISTER_CALLBACK,
					(setBBITitle||setBBISeek||setBBIAlbumArt ? PLAYBACK_STOP : 0) |
					(setBBISeek ? PLAYBACK_TIME|PLAYBACK_SEEK : 0) |
					(setBBITitle||setBBISeek||setBBIAlbumArt ? PLAYBACK_NEW_TRACK|PLAYBACK_EDITED : 0) |
					(setBBIVolume ? PLAYBACK_VOLUME_CHANGE : 0),
					(LPARAM)hWndPlugin);
        DestroyWindow(hWndPlugin);
        SendMessage(hWndBlackbox, BB_UNREGISTERMESSAGE, (WPARAM)hWndPlugin, (LPARAM)msgs);
        UnregisterClass(szAppName, hPluginInstance);
	if(setBBIAlbumArt && useBBInterface) GdiplusShutdown(gdiToken);
}

LPCSTR CDECL pluginInfo(int field)
{
	switch(field)
	{
		case PLUGIN_NAME:	return szAppName;
		case PLUGIN_VERSION:	return szInfoVersion;
		case PLUGIN_AUTHOR:	return szInfoAuthor;
		case PLUGIN_RELEASE:	return szInfoRelDate;
		case PLUGIN_LINK:	return szInfoLink;
		case PLUGIN_EMAIL:	return szInfoEmail;
		case PLUGIN_BROAMS:	return szBroamList;
		default:		return szVersion;
	}
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch(message)
	{
		case WM_FOO_ACTIVATE:  // Send by foo_control when foobar2k start
			if(useBBInterface)
			{
				SetCallbacks();
				UpdateBBInterface(PLAYBACK_VOLUME_CHANGE, SendMessage(rhWnd, WM_FOO_GET_VOLUME, 0, 0));
				UpdateBBInterface(PLAYBACK_NEW_TRACK, 0);
				curLength+=2;
				UpdateBBInterface(PLAYBACK_SEEK, 0);
			}
		break;

		case WM_FOO_CALLBACK:
			UpdateBBInterface(wParam, lParam);
		break;

		case BB_BROADCAST:	//  bro@ms...
			return SwitchBroams((char*)lParam);
		break;
		
		default:	return DefWindowProc(hWnd,message,wParam,lParam);
	}
	return 0;
}

void SetCallbacks()
{
	char buf[256];
	HWND nWnd = FindWindow(FOO_REMOTE_WND_CLASS, 0);
	if(rhWnd != nWnd)
	{
		rhWnd = nWnd;
		sprintf(buf, "%s%s%s", setBBITitle ? titleFormat : "", setBBIAlbumArt ? ONLY_STRING : "",
				setBBIAlbumArt ? "$cut(%path%, $sub($len(%path%), $len(%filename_ext%)))" : "");
		SetWindowText(rhWnd, buf);
		SendMessage(rhWnd, WM_FOO_REGISTER_CALLBACK,
				(setBBITitle||setBBISeek||setBBIAlbumArt ? PLAYBACK_STOP : 0) |
				(setBBISeek ? PLAYBACK_TIME|PLAYBACK_SEEK : 0) |
				(setBBITitle||setBBISeek||setBBIAlbumArt ? PLAYBACK_NEW_TRACK|PLAYBACK_EDITED : 0) |
				(setBBIVolume ? PLAYBACK_VOLUME_CHANGE : 0),				
				(LPARAM)hWndPlugin);
	}
}

void WINAPI Open(LPVOID lpParameter) {SendMessage(rhWnd, WM_FOO_ACTIVATE, 0, 0); SendMessage(rhWnd, WM_FOO_OPEN, 0, 0);}
void WINAPI Add(LPVOID lpParameter) {SendMessage(rhWnd, WM_FOO_ACTIVATE, 0, 0); SendMessage(rhWnd, WM_FOO_ADD, 0, 0);}
void WINAPI AddDir(LPVOID lpParameter) {SendMessage(rhWnd, WM_FOO_ACTIVATE, 0, 0); SendMessage(rhWnd, WM_FOO_ADDDIR, 0, 0);}


void UpdateBBInterface(int uMsg, int value)
{
	char buf[256], caption[MAX_PATH+128];
	char *rchar;
	BOOL is_playing;
	if(!rhWnd || !useBBInterface) return;
	switch(uMsg)
	{
		case PLAYBACK_NEW_TRACK:
			if(setBBITitle || setBBIAlbumArt)
			{
				is_playing = SendMessage(rhWnd, WM_FOO_IS_PLAYING, 0, 0);
				if(is_playing)
				{
					GetWindowText(rhWnd, caption, MAX_PATH+128);
					rchar = strstr(caption, ONLY_STRING);
					if(rchar) rchar[0] = 0;
				}
			}

			if(setBBIAlbumArt) ShowAlbumArt(is_playing ? rchar+8 : 0);

			if(setBBITitle)
			{
				sprintf(buf, "@BBInterface Control RemoveAgent %s Caption", BBITitleName);
				SendMessage(hWndBlackbox, BB_BROADCAST, 0, (LPARAM)&buf);

				sprintf(buf, "@BBInterface Control SetAgent %s Caption StaticText \"%s\"",
					BBITitleName, is_playing ? caption : "Not playing");
				SendMessage(hWndBlackbox, BB_BROADCAST, 0, (LPARAM)&buf);
			}
			if(setBBISeek)
			{
				curLength = SendMessage(rhWnd, WM_FOO_GET_LENGTH, 0, 0);
				if(curLength!=P_ERROR)
				{
					sprintf(buf, "@BBInterface Control SetControlProperty %s BroadcastValueMaximum %d",
						BBISeekName, curLength);
					SendMessage(hWndBlackbox, BB_BROADCAST, 0, (LPARAM)&buf);
				}
			}
		break;

		case PLAYBACK_STOP:
			if(setBBITitle)
			{
				sprintf(buf, "@BBInterface Control RemoveAgent %s Caption", BBITitleName);
				SendMessage(hWndBlackbox, BB_BROADCAST, 0, (LPARAM)&buf);

				sprintf(buf, "@BBInterface Control SetAgent %s Caption StaticText \"%s\"",
						BBITitleName, "Not playing");
				SendMessage(hWndBlackbox, BB_BROADCAST, 0, (LPARAM)&buf);
			}

			if(setBBIAlbumArt) ShowAlbumArt(0);

			UpdateBBInterface(PLAYBACK_SEEK, 0);
		break;

		case PLAYBACK_SEEK:
		case PLAYBACK_TIME:
			if(setBBISeek)
			{
				if(curLength!=P_ERROR)
				{
					sprintf(buf, "@BBInterface Control SetControlProperty %s Value %f", BBISeekName,
						(float)value/curLength);
					SendMessage(hWndBlackbox, BB_BROADCAST, 0, (LPARAM)&buf);
				}
			}
		break;

		case PLAYBACK_VOLUME_CHANGE:
			if(setBBIVolume)
			{
				int volume = value+100;
				sprintf(buf, "@BBInterface Control SetControlProperty %s Value %f", BBIVolumeName,
					((float)volume-50)/50);
				SendMessage(hWndBlackbox, BB_BROADCAST, 0, (LPARAM)&buf);
			}
		break;
	}
}

int SwitchBroams(char *broam)
{
	char *szTemp = broam;
	if(_strnicmp(szTemp, "@BBFooman.", 10)) return 0;
	szTemp+=10;
	// bbFooman internal commands
	if(!_stricmp(szTemp, "About"))
		MessageBox(hWndBlackbox, "Foobar2k remote control plugin.\n"
					 "Write for using with BBInterface.\n\n"
					 "Powered by RustyNail, 2007\n"
					 "Nail.mailme [at] gmail.com",
					 szVersion, MB_OK | MB_ICONASTERISK | MB_TOPMOST);
	if(!_strnicmp(szTemp, "Seek", 4))
	{
		int pos;
		szTemp+=4;
		pos = atoi(szTemp);
		return SendMessage(rhWnd, WM_FOO_PLAYBACK_SEEK, pos, 0);
	}
	if(!_strnicmp(szTemp, "Volume", 6))
	{
		int vol;
		szTemp+=6;
		vol = (atoi(szTemp)-100)/2;
		return SendMessage(rhWnd, WM_FOO_SET_VOLUME, vol, 0);
	}
	if(!_stricmp(szTemp, "Open"))		return (int)CreateThread(0, 0, (LPTHREAD_START_ROUTINE)Open, 0, 0, 0);
	if(!_stricmp(szTemp, "Add"))		return (int)CreateThread(0, 0, (LPTHREAD_START_ROUTINE)Add, 0, 0, 0);
	if(!_stricmp(szTemp, "AddDir"))	 	return (int)CreateThread(0, 0, (LPTHREAD_START_ROUTINE)AddDir, 0, 0, 0);
	if(!_stricmp(szTemp, "Play"))	 	return SendMessage(rhWnd, WM_FOO_PLAY, 0, 0);
	if(!_stricmp(szTemp, "Stop"))	 	return SendMessage(rhWnd, WM_FOO_STOP, 0, 0);
	if(!_stricmp(szTemp, "Next"))	 	return SendMessage(rhWnd, WM_FOO_PLAY_NEXT, 0, 0);
	if(!_stricmp(szTemp, "Prev"))	 	return SendMessage(rhWnd, WM_FOO_PLAY_PREV, 0, 0);
	if(!_stricmp(szTemp, "Random"))	 	return SendMessage(rhWnd, WM_FOO_PLAY_RANDOM, 0, 0);
	if(!_stricmp(szTemp, "Pause"))	 	return SendMessage(rhWnd, WM_FOO_PAUSE, 0, 0);
	if(!_stricmp(szTemp, "Play_Pause"))	return SendMessage(rhWnd, WM_FOO_PLAY_PAUSE, 0, 0);
	if(!_stricmp(szTemp, "VolUp"))	 	return SendMessage(rhWnd, WM_FOO_VOLUME_UP, 0, 0);
	if(!_stricmp(szTemp, "VolDown"))	return SendMessage(rhWnd, WM_FOO_VOLUME_DOWN, 0, 0);
	if(!_stricmp(szTemp, "Mute"))		return SendMessage(rhWnd, WM_FOO_MUTE, 0, 0);
	if(!_stricmp(szTemp, "FooStart"))	return BBExecute(hWndPlugin, 0, foopath, 0, 0, SW_SHOWNORMAL, 0);
	if(!_stricmp(szTemp, "FooExit"))	return SendMessage(rhWnd, WM_FOO_EXIT, 0, 0);
	if(!_stricmp(szTemp, "FooShow"))	return SendMessage(rhWnd, WM_FOO_ACTIVATE, 0, 0);
	if(!_stricmp(szTemp, "FooHide"))	return SendMessage(rhWnd, WM_FOO_HIDE, 0, 0);
	if(!_stricmp(szTemp, "Ord_Def"))	return SendMessage(rhWnd, WM_FOO_ORDER, 0, 0);
	if(!_stricmp(szTemp, "Ord_RepAll"))	return SendMessage(rhWnd, WM_FOO_ORDER, 1, 0);
	if(!_stricmp(szTemp, "Ord_RepOne"))	return SendMessage(rhWnd, WM_FOO_ORDER, 2, 0);
	if(!_stricmp(szTemp, "Ord_Shuff"))	return SendMessage(rhWnd, WM_FOO_ORDER, 3, 0);
	if(!_stricmp(szTemp, "ShowPlaylist"))	return MakePlaylist();
	if(!_strnicmp(szTemp, "PlayTrack", 9))	return SendMessage(rhWnd, WM_FOO_PLAY_TRACK, atoi(szTemp+10)-1, 0);
	return 0;
}

int MakePlaylist()
{
	char title[128], mcmd[32];
	BOOL is_playing = SendMessage(rhWnd, WM_FOO_IS_PLAYING, 0, 0);
	int nowplay=SendMessage(rhWnd, WM_FOO_GET_CURRENT_TRACK, 0, 0);
	int plcount=SendMessage(rhWnd, WM_FOO_GET_ACTIVE_PLAYLIST_COUNT, 0, 0);
	if(plcount==-1) plcount++;

	if(plcount)
		if(SetWindowText(rhWnd, playlistHeader))
			if(SendMessage(rhWnd, WM_FOO_GET_PLAYLIST_ITEM, is_playing ? nowplay : 0, 0) != P_ERROR)
				GetWindowText(rhWnd, title, 128);

	HANDLE menu = MakeNamedMenu(plcount ? title : "Foobar2k playlist (no items)", "bbFooman", true);
	for(int i=0; i<plcount; i++)
		if(SetWindowText(rhWnd, titleFormat))
			if(SendMessage(rhWnd, WM_FOO_GET_PLAYLIST_ITEM, i, 0) != P_ERROR)
				if(GetWindowText(rhWnd, title, 128))
				{
					sprintf(mcmd, "@bbFooman.PlayTrack %d", i+1);
					MakeMenuItem(menu, title, mcmd, i==nowplay);
				}
	ShowMenu(menu);
	return 0;
}

void ShowAlbumArt(char *szPath)
{
	char buf[MAX_PATH+128], path[MAX_PATH];
	wchar_t wfname[MAX_PATH];
	GpImage *image;
	unsigned int width, height, scale;
	int i=-1, j=-1;
	float wScale, hScale;
	BOOL found=FALSE;

	sprintf(buf, "@BBInterface Control RemoveAgent %s Image", BBIAlbumArtName);
	SendMessage(hWndBlackbox, BB_BROADCAST, 0, (LPARAM)&buf);

	if(szPath)
	{
		while(++i<imnamec && !found)
		{
			while(++j<imtypec && !found)
			{
				sprintf(path, "%s%s.%s", szPath, imname[i], imtype[j]);
				if(FileExists(path)) found=TRUE;
			}
			j=-1;
		}

		MultiByteToWideChar(CP_ACP, 0, found ? path : BBIAANoImage, -1, wfname, MAX_PATH);

		if(found || BBIAANoImage[0])
		{
			GdipLoadImageFromFile(wfname, &image);
			GdipGetImageWidth(image, &width);
			GdipGetImageHeight(image, &height);
			GdipDisposeImage(image);

			hScale = (float)BBIAlbumArtHeight/height;
			wScale = (float)BBIAlbumArtWidth/width;
			scale = hScale>wScale ? lrintf(wScale*100) : lrintf(hScale*100);

			sprintf(buf, "@BBInterface Control SetAgent %s Image Bitmap \"%s\"", BBIAlbumArtName,
					found ? path : BBIAANoImage);
			SendMessage(hWndBlackbox, BB_BROADCAST, 0, (LPARAM)&buf);
		
			sprintf(buf, "@BBInterface Control SetAgentProperty %s Image Scale %d", BBIAlbumArtName, scale);
			SendMessage(hWndBlackbox, BB_BROADCAST, 0, (LPARAM)&buf);
		}
	}
}
