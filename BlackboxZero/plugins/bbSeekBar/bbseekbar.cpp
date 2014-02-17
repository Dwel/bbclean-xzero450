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

#include "bbseekbar.h"

char *plugin_info[] =
{
	"BBSeekbar 0.0.5b",
	"BBSeekbar",
	"0.0.5b",
	"ysuke",
	"2006-08-18",
	"http://zzbb.hp.infoseek.co.jp/",
	"zb2_460@yahoo.co.jp"
};

#define szVersion       plugin_info[0]
#define szAppName       plugin_info[1]
#define szInfoVersion	plugin_info[2]
#define szInfoAuthor	plugin_info[3]
#define szInfoRelDate	plugin_info[4]
#define szInfoLink	plugin_info[5]
#define szInfoEmail	plugin_info[6]

enum
{
	fillbar = 0,
	scrollbar,
	trackknob
};

enum
{
	winamp = 0,
	foobar2000
};

//===========================================================================

bool loadPlayerControl(HINSTANCE hPluginInstance, int playerType)
{
	char path[MAX_LINE_LENGTH];

	GetModuleFileName(hInstance, path, sizeof(path));
	int nLen = strlen(path) - 1;
	while (nLen >0 && path[nLen] != '\\') nLen--;
	path[nLen + 1] = 0;

	if(playerType == 0)
		sprintf(controlPath, "%s%s", path, "winamp.dll");
	else if(playerType == 1)
		sprintf(controlPath, "%s%s", path, "foobar2000.dll");
	else
		return false;

	controlPlugin = LoadLibrary(controlPath);
	if (controlPlugin != NULL) {
		 FgetPlayerData = (PgetPlayerData) GetProcAddress(controlPlugin, "getPlayerData");
		 return true;
        }
	return false;
}

//===========================================================================

DWORD WINAPI TimeProc(LPVOID lpParam){
	while(true){
		// Send out track position and length bro@m...
		play_ms = (*FgetPlayerData)(WM_GETPOSITION, 0);

		value = 0;
		sprintf(tipString, "00:00 / 00:00");
		if(play_ms > 0)
		{
			play_total = (*FgetPlayerData)(WM_GETLENGTH, 0);
			sprintf(tipString, "%02d:%02d / %02d:%02d",
                                play_ms / 60, play_ms % 60, play_total / 60, play_total % 60);
			value = (double)play_ms / (double)play_total;

			char buf[MAX_PATH], buf2[MAX_PATH];
			sprintf(buf, "@BBInterface Control SetAgent ElapsedAndTotal Caption StaticText \"%s\"", tipString);
			SendMessage(hwndBlackbox, BB_BROADCAST, 0, (LPARAM)buf);

			sprintf(buf2, "@BBInterface Control SetAgent ElapsedTime Caption StaticText \"%02d:%02d\"", play_ms / 60, play_ms % 60);
			SendMessage(hwndBlackbox, BB_BROADCAST, 0, (LPARAM)buf2);

			display_update = true;
		}
		else if(display_update == true)
		{
			char buf[MAX_PATH], buf2[MAX_PATH];
			sprintf(buf, "@BBInterface Control SetAgent ElapsedAndTotal Caption StaticText \"%s\"", tipString);
			SendMessage(hwndBlackbox, BB_BROADCAST, 0, (LPARAM)buf);

			sprintf(buf2, "@BBInterface Control SetAgent ElapsedTime Caption StaticText \"%02d:%02d\"", play_ms / 60, play_ms % 60);
			SendMessage(hwndBlackbox, BB_BROADCAST, 0, (LPARAM)buf2);

			display_update = false;
		}
		knob_needsupdate = true;
		InvalidateRect(hwndPlugin, NULL, true);
		// Idle for 500 milliseconds...
		Sleep(500);
	}
	return 0;
}

//===========================================================================

int beginSlitPlugin(HINSTANCE hMainInstance, HWND hBBSlit)
{
	inSlit = true;
	hSlit = hBBSlit;

	// Start the plugin like normal now..
	int res = beginPlugin(hMainInstance);

	// Are we to be in the Slit?
	if (inSlit && hSlit){
		// Yes, so Let's let BBSlit know.
		SendMessage(hSlit, SLIT_ADD, NULL, (LPARAM)hwndPlugin);
	}

	return res;
}

//===========================================================================

int beginPluginEx(HINSTANCE hMainInstance, HWND hBBSlit)
{
	return beginSlitPlugin(hMainInstance, hBBSlit);
}

//===========================================================================

int beginPlugin(HINSTANCE hPluginInstance)
{
	WNDCLASS wc;
	hwndBlackbox = GetBBWnd();
	hInstance = hPluginInstance;

	ZeroMemory(&osvinfo, sizeof(osvinfo));
	osvinfo.dwOSVersionInfoSize = sizeof (osvinfo);
	GetVersionEx(&osvinfo);
	usingWin2kXP =
		osvinfo.dwPlatformId == VER_PLATFORM_WIN32_NT
		&& osvinfo.dwMajorVersion > 4;

	// Register the window class...
	ZeroMemory(&wc,sizeof(wc));
	wc.lpfnWndProc = WndProc;			// our window procedure
	wc.hInstance = hPluginInstance;		// hInstance of .dll
	wc.lpszClassName = szAppName;		// our window class name
	if (!RegisterClass(&wc)) {
		MessageBox(hwndBlackbox, "Error registering window class", szAppName, MB_OK | MB_ICONERROR | MB_TOPMOST);
		return 1;
	}

	// Get plugin and style settings...
	ReadRCSettings();

	if(!loadPlayerControl(hPluginInstance, playerType)){
		MessageBox(hwndBlackbox, "Player control plugin not found", szAppName, MB_OK | MB_ICONERROR | MB_TOPMOST);
		return 1;
	}

	GetStyleSettings();
	dragging = false;
	track_needsupdate = true;
	knob_needsupdate = true;

	ScreenWidth  = GetSystemMetrics(SM_CXVIRTUALSCREEN);
	ScreenHeight = GetSystemMetrics(SM_CYVIRTUALSCREEN);

	// Create the window...
	hwndPlugin = CreateWindowEx(
			usingWin2kXP ? WS_EX_TOOLWINDOW | WS_EX_LAYERED : WS_EX_TOOLWINDOW, // window style
			szAppName, // our window class name
			NULL, // NULL -> does not show up in task manager!
			WS_POPUP | WS_CLIPCHILDREN | WS_CLIPSIBLINGS, // window parameters
			xpos, // x position
			ypos, // y position
			width, // window width
			height, // window height
			NULL, // parent window
			NULL, // no menu
			hPluginInstance, // hInstance of .dll
			NULL);

	if (!hwndPlugin){						   
		MessageBox(0, "Error creating window", szAppName, MB_OK | MB_ICONERROR | MB_TOPMOST);
		// Unregister Blackbox messages...
		SendMessage(hwndBlackbox, BB_UNREGISTERMESSAGE, (WPARAM)hwndPlugin, (LPARAM)msgs);
		return 1;
	}

	// Register to receive Blackbox messages...
	SendMessage(hwndBlackbox, BB_REGISTERMESSAGE, (WPARAM)hwndPlugin, (LPARAM)msgs);
	// Make the window sticky
	MakeSticky(hwndPlugin);
	// Make the window AlwaysOnTop?
	if (alwaysOnTop) SetWindowPos(hwndPlugin, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOACTIVATE | SWP_NOSIZE | SWP_NOMOVE);

	SetWindowModes();

	// Show the window and force it to update...
	ShowWindow(hwndPlugin, SW_SHOW);
	InvalidateRect(hwndPlugin, NULL, true);

  	//====================

	INITCOMMONCONTROLSEX ic;
	ic.dwSize = sizeof(INITCOMMONCONTROLSEX);
	ic.dwICC = ICC_BAR_CLASSES;

        if (InitCommonControlsEx(&ic))
        { // Load "tab" controls, including tooltips.
	        hToolTips = CreateWindowEx(
		        WS_EX_TOPMOST,
		        TOOLTIPS_CLASS, // "tooltips_class32"
		        NULL, //"BBSBTT",
		        WS_POPUP | TTS_ALWAYSTIP | TTS_NOPREFIX,
		        CW_USEDEFAULT,
		        CW_USEDEFAULT,
		        CW_USEDEFAULT,
		        CW_USEDEFAULT,
		        NULL,
		        NULL,
		        hPluginInstance,
		        NULL);

	        SendMessage(hToolTips, TTM_SETMAXTIPWIDTH, 0, 300);

	        //SendMessage(hToolTips, TTM_SETDELAYTIME, TTDT_AUTOMATIC, 200);

	        SendMessage(hToolTips, TTM_SETDELAYTIME, TTDT_AUTOPOP, 4000);
	        SendMessage(hToolTips, TTM_SETDELAYTIME, TTDT_INITIAL, 120);
	        SendMessage(hToolTips, TTM_SETDELAYTIME, TTDT_RESHOW,   60);

                if (NULL != hToolTips)
                      SetAllowTip(allowtip);
                else
                      SetAllowTip(false);
	} 
	else
        SetAllowTip(false);

	// Start the monitoring threads...
	hThread_Time = CreateThread(NULL, 0, TimeProc, NULL, 0, &dwThreadId_Time);

  return 0;
}

//===========================================================================

void endPlugin(HINSTANCE hPluginInstance)
{
	// Stop the monitor...
	unsigned long exitcode;
	GetExitCodeThread(hThread_Time, &exitcode);
	TerminateThread(hThread_Time, exitcode);

	// Write the current plugin settings to the config file...
	WriteRCSettings();
	// Delete the main plugin menu if it exists (PLEASE NOTE that this takes care of submenus as well!)
	if (myMenu){ DelMenu(myMenu); myMenu = NULL;}

	// Make the window unsticky
	RemoveSticky(hwndPlugin);
	// Unregister Blackbox messages...
	SendMessage(hwndBlackbox, BB_UNREGISTERMESSAGE, (WPARAM)hwndPlugin, (LPARAM)msgs);

	if(inSlit && hSlit)
		SendMessage(hSlit, SLIT_REMOVE, NULL, (LPARAM)hwndPlugin);

	ClearToolTips();
  	DestroyWindow(hToolTips);
	// Destroy our window...
	DestroyWindow(hwndPlugin);
	// Unregister window class...
	UnregisterClass(szAppName, hPluginInstance);

	FreeLibrary(controlPlugin);
}

//===========================================================================

LPCSTR pluginInfo(int field)
{
	switch (field)
	{
		default:
		case 0: return szVersion;
		case 1: return szAppName;
		case 2: return szInfoVersion;
		case 3: return szInfoAuthor;
		case 4: return szInfoRelDate;
		case 5: return szInfoLink;
		case 6: return szInfoEmail;
	}
}

//===========================================================================

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int gap = bevelWidth + borderWidth;

	switch (message)
	{
		case WM_PAINT:
		{
			OnPaint(hwnd);
		}
		break;

		// ==========

		case BB_RECONFIGURE:
		{
			if (myMenu){ DelMenu(myMenu); myMenu = NULL;}
			GetStyleSettings();
			InvalidateRect(hwndPlugin, NULL, true);
		}
		break;

		// for bbStylemaker
		case BB_REDRAWGUI:
		{
			GetStyleSettings();
			InvalidateRect(hwndPlugin, NULL, true);
		}
		break;

		case WM_DISPLAYCHANGE:
		{
			if(!inSlit)
			{
				// IntelliMove(tm)... <g>
				// (c) 2003 qwilk
				//should make this a function so it can be used on startup in case resolution changed since
				//the last time blackbox was used.
				int relx, rely;
				int oldscreenwidth = ScreenWidth;
				int oldscreenheight = ScreenHeight;
				ScreenWidth = GetSystemMetrics(SM_CXVIRTUALSCREEN);
				ScreenHeight = GetSystemMetrics(SM_CYVIRTUALSCREEN);
				if (xpos > oldscreenwidth / 2)
				{
					relx = oldscreenwidth - xpos;
					xpos = ScreenWidth - relx;
				}
				if (ypos > oldscreenheight / 2)
				{
					rely = oldscreenheight - ypos;
					ypos = ScreenHeight - rely;
				}
				MoveWindow(hwndPlugin, xpos, ypos, width, height, true);
			}
		}
		break;

		// ==========

		case BB_BROADCAST:
		{
			szTemp = (char*)lParam;

			if (!_stricmp(szTemp, "@BBShowPlugins") &&  pluginToggle && !inSlit)
			{
				// Show window and force update...
				ShowWindow( hwndPlugin, SW_SHOW);
				InvalidateRect( hwndPlugin, NULL, true);
			}
			else if (!_stricmp(szTemp, "@BBHidePlugins") && pluginToggle && !inSlit)
			{
				// Hide window...
				ShowWindow( hwndPlugin, SW_HIDE);
			}

			//===================

			if (strnicmp(szTemp, "@BBSeekbar", 10))
				return 0;
			szTemp += 10;

			if (!_stricmp(szTemp, "About"))
			{
				char tmp_str[MAX_LINE_LENGTH];
				SendMessage(hwndBlackbox, BB_HIDEMENU, 0, 0);
				sprintf(tmp_str, "%s", szVersion);
				MessageBox(hwndBlackbox, tmp_str, szAppName, MB_OK | MB_TOPMOST);
			}

			//===================

			else if (!_strnicmp(szTemp, "StyleType", 9))
			{
				styleType = atoi(szTemp + 10);
				GetStyleSettings();
				InvalidateRect(hwndPlugin, NULL, true);
			}
			else if (!_strnicmp(szTemp, "TrackStyle", 10))
			{
				trackStyleType = atoi(szTemp + 11);
				GetStyleSettings();
				InvalidateRect(hwndPlugin, NULL, true);
			}
			else if (!_strnicmp(szTemp, "KnobStyle", 9))
			{
				knobStyleType = atoi(szTemp + 10);
				GetStyleSettings();
				InvalidateRect(hwndPlugin, NULL, true);
			}

			else if (!_strnicmp(szTemp, "Appearance", 10))
			{
				appearance = atoi(szTemp + 11);
				track_needsupdate = true;
				knob_needsupdate = true;
				InvalidateRect(hwndPlugin, NULL, true);
				ShowMyMenu(false);
			}
			else if (!_strnicmp(szTemp, "PlayerType", 10))
			{
				playerType = atoi(szTemp + 11);

				// Stop the monitor...
				unsigned long exitcode;
				GetExitCodeThread(hThread_Time, &exitcode);
				TerminateThread(hThread_Time, exitcode);

				FreeLibrary(controlPlugin);
				if(!loadPlayerControl(hInstance, playerType)){
					MessageBox(hwndBlackbox, "Player control plugin not found", szAppName, MB_OK | MB_ICONERROR | MB_TOPMOST);
				}

				// Start the monitoring threads...
				hThread_Time = CreateThread(NULL, 0, TimeProc, NULL, 0, &dwThreadId_Time);

				knob_needsupdate = true;
				InvalidateRect(hwndPlugin, NULL, true);
				ShowMyMenu(false);
			}

			//===================

			else if (!_strnicmp(szTemp, "WidthSize", 9))
			{
				newWidth = atoi(szTemp + 10);
				if(ResizeMyWindow(newWidth, height)){
					track_needsupdate = true;
					knob_needsupdate = true;
					InvalidateRect(hwndPlugin, NULL, true);
				}
			}
			else if (!_strnicmp(szTemp, "HeightSize", 10))
			{
				newHeight = atoi(szTemp + 11);
				if(ResizeMyWindow(width, newHeight)){
					track_needsupdate = true;
					knob_needsupdate = true;
					InvalidateRect(hwndPlugin, NULL, true);
				}
			}

			//===================

			else if (!_stricmp(szTemp, "pluginToggle"))
			{
				pluginToggle = (pluginToggle ? false : true);
				SetWindowModes();
			}
			else if (!_stricmp(szTemp, "OnTop"))
			{
				alwaysOnTop = (alwaysOnTop ? false : true);
				if(!inSlit)SetWindowPos( hwndPlugin,
                                                         alwaysOnTop ? HWND_TOPMOST : HWND_NOTOPMOST,
                                                         0, 0, 0, 0,
                                                         SWP_NOACTIVATE | SWP_NOSIZE | SWP_NOMOVE);

				ShowMyMenu(false);
			}

			else if (!_stricmp(szTemp, "InSlit"))
			{
				if(inSlit && hSlit){
					// We are in the slit, so lets unload and get out..
					SendMessage(hSlit, SLIT_REMOVE, NULL, (LPARAM)hwndPlugin);

					// Here you can move to where ever you want ;)
					SetWindowPos(hwndPlugin, NULL, xpos, ypos, 0, 0, SWP_NOZORDER | SWP_NOSIZE);
					inSlit = false;
					SetWindowModes();
				}
				/* Make sure before you try and load into the slit that you have
				* the HWND of the slit ;)
				*/
				else if(hSlit){
					// (Back) into the slit..
					inSlit = true;
					SetWindowModes();
					SendMessage(hSlit, SLIT_ADD, NULL, (LPARAM)hwndPlugin);
				}
			}

			else if (!_stricmp(szTemp, "Transparent"))
			{
				transparency = (transparency ? false : true);
				SetWindowModes();
			}
			else if (!_strnicmp(szTemp, "AlphaValue", 10))
			{
				alpha = atoi(szTemp + 11);
				SetWindowModes();
			}
			else if (!_stricmp(szTemp, "SnapToEdge"))
			{
				snapWindow = (snapWindow ? false : true);
				ShowMyMenu(false);
			}
			else if (!_stricmp(szTemp, "ShowBorder"))
			{
				showBorder = (showBorder ? false : true);
				InvalidateRect(hwndPlugin, NULL, false);
				ShowMyMenu(false);
			}
			else if (!_stricmp(szTemp, "AllowTip"))
			{
				allowtip = (allowtip ? false : true);
				SetAllowTip(allowtip);
				ShowMyMenu(false);
			}

			else if (!_stricmp(szTemp, "Vertical"))
			{
				vertical = (vertical ? false : true);
				if(ResizeMyWindow(height, width)){
					track_needsupdate = true;
					knob_needsupdate = true;
					InvalidateRect(hwndPlugin, NULL, true);
				}
				ShowMyMenu(false);
			}
			else if (!_stricmp(szTemp, "Reversed"))
			{
				reversed = (reversed ? false : true);
				track_needsupdate = true;
				knob_needsupdate = true;
				InvalidateRect(hwndPlugin, NULL, true);
				ShowMyMenu(false);
			}

			//===================

			else if (!_stricmp(szTemp, "EditRC"))
			{
				BBExecute(GetDesktopWindow(), NULL,
				ReadString(extensionsrcPath(), "blackbox.editor:", "notepad.exe"),
                                          rcpath, NULL, SW_SHOWNORMAL, false);
			}
			else if (!_stricmp(szTemp, "ReloadSettings"))
			{
				ReadRCSettings();
				GetStyleSettings();

				if(inSlit && hSlit)
				{
					SetWindowModes();
					SendMessage(hSlit, SLIT_UPDATE, NULL, NULL);
				}
				else if(!inSlit || !hSlit)
					SendMessage(hSlit, SLIT_REMOVE, NULL, (LPARAM)hwndPlugin);

				else inSlit = false;
			}
			else if (!_stricmp(szTemp, "SaveSettings"))
			{
				WriteRCSettings();
			}
		}
		break;

		// ==========

		case WM_WINDOWPOSCHANGING:
		{
			// Is SnapWindowToEdge enabled?
			if (!inSlit && snapWindow)
			{
				// Snap window to screen edges (if the last bool is false it uses the current DesktopArea)
				if(IsWindowVisible(hwnd)) SnapWindowToEdge((WINDOWPOS*)lParam, 10, true);
			}
		}
		break;

		// ==========

		case WM_WINDOWPOSCHANGED:
		{
			if(!inSlit)
			{
				WINDOWPOS* windowpos = (WINDOWPOS*)lParam;
				if (0 == (windowpos->flags & SWP_NOMOVE))
				{
					xpos = windowpos->x;
					ypos = windowpos->y;
				}

				if(ResizeMyWindow(windowpos->cx, windowpos->cy) && 0 == (windowpos->flags & SWP_NOSIZE))
				{
					track_needsupdate = true;
					knob_needsupdate = true;
					InvalidateRect(hwndPlugin, NULL, true);
				}
			}
		}
		break;

		// ==========

		case WM_NCHITTEST:
		{
			if (!inSlit && GetKeyState(VK_MENU) & 0xF0)
			{
				return HTBOTTOMRIGHT;
			}
			else
			if (!inSlit && GetKeyState(VK_CONTROL) & 0xF0)
			{
				return HTCAPTION;
			}
		}
		return HTCLIENT;

		// ==========

		case WM_LBUTTONDOWN:
			SetFocus(hwnd);
			SetCapture(hwnd);

			InvalidateRect(hwndPlugin, NULL, false);
			dragging = true;
			SliderOnValueChange(lParam);
			break;
	
		case WM_MOUSEMOVE:
			if (!dragging) break;
			InvalidateRect(hwndPlugin, NULL, false);
			SliderOnValueChange(lParam);
			break;

		case WM_LBUTTONUP:
			ReleaseCapture();

		case WM_KILLFOCUS: 
			dragging = false;
			break;

		// ==========

		case WM_RBUTTONUP:
			ReleaseCapture();

		case WM_NCRBUTTONUP:
			ShowMyMenu(true);
			break;

		case WM_CLOSE:
			break;

		// bring window into foreground on sizing/moving-start
		case WM_NCLBUTTONDOWN:
			SetWindowPos(hwnd, HWND_TOP, 0,0,0,0, SWP_NOSIZE|SWP_NOMOVE);
			UpdateWindow(hwnd);
			// fall through
 		// ==========
		default:
			return DefWindowProc(hwnd,message,wParam,lParam);
	}
	return 0;
}

//===========================================================================

void ShowMyMenu(bool popup)
{
	if (myMenu){ DelMenu(myMenu); myMenu = NULL;}

	configSubmenu = MakeNamedMenu("Configuration", "BBSB_Config", popup);
	MakeMenuItemInt(configSubmenu, "Width Size", "@BBSeekbarWidthSize", width, 16, ScreenWidth);
	MakeMenuItemInt(configSubmenu, "Height Size", "@BBSeekbarHeightSize", height, 12, ScreenHeight);
	MakeMenuNOP(configSubmenu, NULL);
	if(hSlit) MakeMenuItem(configSubmenu, "In Slit", "@BBSeekbarInSlit", inSlit);
	MakeMenuItem(configSubmenu, "Plugin Toggle", "@BBSeekbarPluginToggle", pluginToggle);
	MakeMenuItem(configSubmenu, "Always on Top", "@BBSeekbarOnTop", alwaysOnTop);
	MakeMenuItem(configSubmenu, "Transparency", "@BBSeekbarTransparent", transparency);
	MakeMenuItemInt(configSubmenu, "Alpha Value", "@BBSeekbarAlphaValue", alpha, 0, 255);
	MakeMenuItem(configSubmenu, "Snap To Edge", "@BBSeekbarSnapToEdge", snapWindow);
	MakeMenuItem(configSubmenu, "Show Border", "@BBSeekbarShowBorder", showBorder);
	MakeMenuItem(configSubmenu, "Allow Tooltip", "@BBSeekbarAllowTip", allowtip);

	playerSubmenu = MakeNamedMenu("Player", "BBSB_Player", popup);
	MakeMenuItem(playerSubmenu, "Winamp",     "@BBSeekbarPlayerType 0", (playerType == 0));
	MakeMenuItem(playerSubmenu, "foobar2000", "@BBSeekbarPlayerType 1", (playerType == 1));

	optionsSubmenu = MakeNamedMenu("Options", "BBSB_Options", popup);
	appearanceSubmenu = MakeNamedMenu("Appearance", "BBSB_Appearance", popup);
	MakeMenuItem(appearanceSubmenu, "Fill Bar",       "@BBSeekbarAppearance 0", (appearance == 0));
	MakeMenuItem(appearanceSubmenu, "Scroll Bar",     "@BBSeekbarAppearance 1", (appearance == 1));
	MakeMenuItem(appearanceSubmenu, "Track and Knob", "@BBSeekbarAppearance 2", (appearance == 2));
	MakeSubmenu(optionsSubmenu, appearanceSubmenu, "Appearance");

	MakeMenuItem(optionsSubmenu, "Vertical", "@BBSeekbarVertical", vertical);
	MakeMenuItem(optionsSubmenu, "Values Reversed", "@BBSeekbarReversed", reversed);

	styleSubmenu = MakeNamedMenu("Style Type", "BBSB_StyleType", popup);
	bgStyleSubmenu = MakeNamedMenu("Background", "BBSB_BGStyle", popup);
	MakeMenuItem(bgStyleSubmenu, "Toolbar",  "@BBSeekbarStyleType 1", (styleType == 1));
	MakeMenuItem(bgStyleSubmenu, "Button",   "@BBSeekbarStyleType 2", (styleType == 2));
	MakeMenuItem(bgStyleSubmenu, "ButtonP",  "@BBSeekbarStyleType 3", (styleType == 3));
	MakeMenuItem(bgStyleSubmenu, "Label",    "@BBSeekbarStyleType 4", (styleType == 4));
	MakeMenuItem(bgStyleSubmenu, "WinLabel", "@BBSeekbarStyleType 5", (styleType == 5));
	MakeMenuItem(bgStyleSubmenu, "Clock",    "@BBSeekbarStyleType 6", (styleType == 6));
	MakeSubmenu(styleSubmenu, bgStyleSubmenu, "Background");

	trackStyleSubmenu = MakeNamedMenu("Track", "BBSB_TrackStyle", popup);
	MakeMenuItem(trackStyleSubmenu, "Toolbar",  "@BBSeekbarTrackStyle 1", (trackStyleType == 1));
	MakeMenuItem(trackStyleSubmenu, "Button",   "@BBSeekbarTrackStyle 2", (trackStyleType == 2));
	MakeMenuItem(trackStyleSubmenu, "ButtonP",  "@BBSeekbarTrackStyle 3", (trackStyleType == 3));
	MakeMenuItem(trackStyleSubmenu, "Label",    "@BBSeekbarTrackStyle 4", (trackStyleType == 4));
	MakeMenuItem(trackStyleSubmenu, "WinLabel", "@BBSeekbarTrackStyle 5", (trackStyleType == 5));
	MakeMenuItem(trackStyleSubmenu, "Clock",    "@BBSeekbarTrackStyle 6", (trackStyleType == 6));
	MakeSubmenu(styleSubmenu, trackStyleSubmenu, "Track");

	knobStyleSubmenu = MakeNamedMenu("Knob", "BBSB_KnobStyle", popup);
	MakeMenuItem(knobStyleSubmenu, "Toolbar",  "@BBSeekbarKnobStyle 1", (knobStyleType == 1));
	MakeMenuItem(knobStyleSubmenu, "Button",   "@BBSeekbarKnobStyle 2", (knobStyleType == 2));
	MakeMenuItem(knobStyleSubmenu, "ButtonP",  "@BBSeekbarKnobStyle 3", (knobStyleType == 3));
	MakeMenuItem(knobStyleSubmenu, "Label",    "@BBSeekbarKnobStyle 4", (knobStyleType == 4));
	MakeMenuItem(knobStyleSubmenu, "WinLabel", "@BBSeekbarKnobStyle 5", (knobStyleType == 5));
	MakeMenuItem(knobStyleSubmenu, "Clock",    "@BBSeekbarKnobStyle 6", (knobStyleType == 6));
	MakeSubmenu(styleSubmenu, knobStyleSubmenu, "Knob");


	settingsSubmenu = MakeNamedMenu("Settings", "BBSB_Settings", popup);
	MakeMenuItem(settingsSubmenu, "Edit Settings",   "@BBSeekbarEditRC", false);
	MakeMenuItem(settingsSubmenu, "Reload Settings", "@BBSeekbarReloadSettings", false);
	MakeMenuItem(settingsSubmenu, "Save Settings",   "@BBSeekbarSaveSettings", false);

	myMenu = MakeNamedMenu("BBSeekbar", "BBSB_Main", popup);
	MakeSubmenu(myMenu, configSubmenu, "Configuration");
	MakeSubmenu(myMenu, playerSubmenu, "Player");
	MakeSubmenu(myMenu, optionsSubmenu, "Options");
	MakeSubmenu(myMenu, styleSubmenu, "Style Type");
	MakeSubmenu(myMenu, settingsSubmenu, "Settings");
  	MakeMenuItem(myMenu, "About", "@BBSeekbarAbout", false);
	ShowMenu(myMenu);
}

//===========================================================================

void GetStyleSettings()
{
	bevelWidth  = *(int*)GetSettingPtr(SN_BEVELWIDTH);
	borderWidth = *(int*)GetSettingPtr(SN_BORDERWIDTH);
	borderColor = *(COLORREF*)GetSettingPtr(SN_BORDERCOLOR);

	myStyleItem = *(StyleItem*)GetSettingPtr(styleType);
	trackStyleItem = *(StyleItem*)GetSettingPtr(trackStyleType);
	knobStyleItem = *(StyleItem*)GetSettingPtr(knobStyleType);

	ShowMyMenu(false);
}

//===========================================================================

void ReadRCSettings()
{
	char temp[MAX_LINE_LENGTH], path[MAX_LINE_LENGTH], defaultpath[MAX_LINE_LENGTH];
	int nLen;

	// First we look for the config file in the same folder as the plugin...
	GetModuleFileName(hInstance, rcpath, sizeof(rcpath));
	nLen = strlen(rcpath) - 1;
	while (nLen >0 && rcpath[nLen] != '\\') nLen--;
	rcpath[nLen + 1] = 0;
	strcpy(temp, rcpath);
	strcpy(path, rcpath);
	strcat(temp, "bbseekbar.rc");
	strcat(path, "bbseekbarrc");
	// ...checking the two possible filenames bbseekbar.rc and bbseekbarrc ...
	if (FileExists(temp)) strcpy(rcpath, temp);
	else if (FileExists(path)) strcpy(rcpath, path);
	// ...if not found, we try the Blackbox directory...
	else
	{
		// ...but first we save the default path (bbseekbar.rc in the same
		// folder as the plugin) just in case we need it later (see below)...
		strcpy(defaultpath, temp);
		GetBlackboxPath(rcpath, sizeof(rcpath));
		strcpy(temp, rcpath);
		strcpy(path, rcpath);
		strcat(temp, "bbseekbar.rc");
		strcat(path, "bbseekbarrc");
		if (FileExists(temp)) strcpy(rcpath, temp);
		else if (FileExists(path)) strcpy(rcpath, path);
		else // If no config file was found, we use the default path and settings, and return
		{
			strcpy(rcpath, defaultpath);
			xpos = ypos = 10;
			width = 160;
			height = 20;
			alpha = 160;
			styleType = 1;
			trackStyleType = 5;
			knobStyleType = 1;
			appearance = 0;
			playerType = 0;
			alwaysOnTop = false;
			transparency = false;
			snapWindow = true;
			pluginToggle = false;
			inSlit = false;
			showBorder = true;
			allowtip = true;
			vertical = false;
			reversed = false;
			WriteRCSettings(); //write a file for editing later
			return;
		}
	}

	// If a config file was found we read the plugin settings from the file...
	xpos = ReadInt(rcpath, "bbseekbar.x:", 10);
	ypos = ReadInt(rcpath, "bbseekbar.y:", 10);
	if (xpos >= GetSystemMetrics(SM_CXVIRTUALSCREEN)) xpos = 10;
	if (ypos >= GetSystemMetrics(SM_CYVIRTUALSCREEN)) ypos = 10;
	width = ReadInt(rcpath, "bbseekbar.width:", 160);
  	height = ReadInt(rcpath, "bbseekbar.height:", 20);
	alpha = ReadInt(rcpath, "bbseekbar.alpha:", 160);
	styleType = ReadInt(rcpath, "bbseekbar.styleType:", 1);
	trackStyleType = ReadInt(rcpath, "bbseekbar.trackStyleType:", 5);
	knobStyleType = ReadInt(rcpath, "bbseekbar.knobStyleType:", 1);
	appearance = ReadInt(rcpath, "bbseekbar.appearance:", 0);
	playerType = ReadInt(rcpath, "bbseekbar.playerType:", 0);
	alwaysOnTop = ReadBool(rcpath, "bbseekbar.alwaysontop:", false);
	transparency = ReadBool(rcpath, "bbseekbar.transparency:", false);
	snapWindow = ReadBool(rcpath, "bbseekbar.snapwindow:", true);
	pluginToggle = ReadBool(rcpath, "bbseekbar.pluginToggle:", false);
	inSlit = ReadBool(rcpath, "bbseekbar.inSlit:", false);
	showBorder = ReadBool(rcpath, "bbseekbar.showBorder:", true);
	allowtip = ReadBool(rcpath, "bbseekbar.allowtooltip:", true);
	vertical = ReadBool(rcpath, "bbseekbar.vertical:", false);
	reversed = ReadBool(rcpath, "bbseekbar.reversed:", false);
}

//===========================================================================

void WriteRCSettings()
{
	WriteInt(rcpath, "bbseekbar.x:", xpos);
	WriteInt(rcpath, "bbseekbar.y:", ypos);
	WriteInt(rcpath, "bbseekbar.width:", width);
	WriteInt(rcpath, "bbseekbar.height:", height);
	WriteInt(rcpath, "bbseekbar.alpha:", alpha);
	WriteInt(rcpath, "bbseekbar.styleType:", styleType);
	WriteInt(rcpath, "bbseekbar.trackStyleType:", trackStyleType);
	WriteInt(rcpath, "bbseekbar.knobStyleType:", knobStyleType);
  	WriteInt(rcpath, "bbseekbar.appearance:", appearance);
  	WriteInt(rcpath, "bbseekbar.playerType:", playerType);
	WriteBool(rcpath, "bbseekbar.alwaysontop:", alwaysOnTop);
	WriteBool(rcpath, "bbseekbar.transparency:", transparency);
	WriteBool(rcpath, "bbseekbar.snapWindow:", snapWindow);
	WriteBool(rcpath, "bbseekbar.pluginToggle:", pluginToggle);
	WriteBool(rcpath, "bbseekbar.inSlit:", inSlit);
	WriteBool(rcpath, "bbseekbar.showBorder:", showBorder);
	WriteBool(rcpath, "bbseekbar.allowtooltip:", allowtip);
	WriteBool(rcpath, "bbseekbar.vertical:", vertical);
	WriteBool(rcpath, "bbseekbar.reversed:", reversed);
}

//===========================================================================

void OnPaint(HWND hwnd)
{
	// Create buffer hdc's, bitmaps etc.
	PAINTSTRUCT ps;  RECT r;

	//get screen buffer
	HDC hdc_scrn = BeginPaint(hwnd, &ps);

	//get window rectangle.
	GetClientRect(hwnd, &r);

	//first get a new 'device context'
	HDC hdc = CreateCompatibleDC(NULL);

	//then create a buffer in memory with the window size
	HBITMAP bufbmp = CreateCompatibleBitmap(hdc_scrn, r.right, r.bottom);

	//select the bitmap into the DC
	HGDIOBJ otherbmp = SelectObject(hdc, bufbmp);


	MakeStyleGradient(hdc, &r, &myStyleItem, showBorder);

	SetToolTip(&r, tipString);

	if (track_needsupdate) SliderUpdateTrack(r);
	if (knob_needsupdate) SliderUpdateKnob(r);

	MakeStyleGradient(hdc, &track, &trackStyleItem, false);
 
	if (knob.right - knob.left > 2 && knob.bottom - knob.top > 2)
		MakeStyleGradient(hdc, &knob, &knobStyleItem, showBorder);

	// Finally, copy from the paint buffer to the window...
	BitBlt(hdc_scrn, 0, 0, width, height, hdc, 0, 0, SRCCOPY);

	// Remember to delete all objects!
	DeleteObject(SelectObject(hdc, otherbmp));
	DeleteDC(hdc);

	EndPaint(hwnd, &ps);
}

//=========================================================================

//so there you just use BBSLWA like normal SLWA
//(c)grischka
BOOL WINAPI BBSetLayeredWindowAttributes(HWND hwnd, COLORREF crKey, BYTE bAlpha, DWORD dwFlags)
{
	static BOOL (WINAPI *pSLWA)(HWND, COLORREF, BYTE, DWORD);
	static int f=0;
	for (;;) {
		if (2==f)   return pSLWA(hwnd, crKey, bAlpha, dwFlags);
		// if it's not there, just do nothing and report success
		if (f)      return TRUE;
		*(FARPROC*)&pSLWA = GetProcAddress(GetModuleHandle("USER32"), "SetLayeredWindowAttributes");
		f = pSLWA ? 2 : 1;
	}
}

//=========================================================================

struct tt
{
	struct tt *next;
	char used_flg;
	char text[256];
	TOOLINFO ti;
} *tt0;

void SetToolTip(RECT *tipRect, char *tipText)
{
	if (NULL==hToolTips) return;

	struct tt **tp, *t; unsigned n=0;
	for (tp=&tt0; NULL!=(t=*tp); tp=&t->next){
		if (0==memcmp(&t->ti.rect, tipRect, sizeof(RECT))){
			t->used_flg = 1;
			if (0!=strcmp(t->ti.lpszText, tipText)){
				strcpy(t->text, tipText);
				SendMessage(hToolTips, TTM_UPDATETIPTEXT, 0, (LPARAM)&t->ti);
			}
			return;
		}
		if (t->ti.uId > n)
			n = t->ti.uId;
	}

	t = (struct tt*)c_alloc(sizeof (*t));
	t->used_flg  = 1;
	t->next = NULL;
	strcpy(t->text, tipText);
	*tp = t;

	memset(&t->ti, 0, sizeof(TOOLINFO));

	t->ti.cbSize   = sizeof(TOOLINFO);
	t->ti.uFlags   = TTF_SUBCLASS;
	t->ti.hwnd     = hwndPlugin;
	t->ti.uId      = n+1;
	//t->ti.hinst    = NULL;
	t->ti.lpszText = t->text;
	t->ti.rect     = *tipRect;

	SendMessage(hToolTips, TTM_ADDTOOL, 0, (LPARAM)&t->ti);
}

void ClearToolTips(void)
{
	struct tt **tp, *t;
	tp=&tt0; while (NULL!=(t=*tp)){
		if (0==t->used_flg){
			SendMessage(hToolTips, TTM_DELTOOL, 0, (LPARAM)&t->ti);
			*tp=t->next;
			m_free(t);
		}else{
			t->used_flg = 0;
			tp=&t->next;
		}
	}
}

void SetAllowTip(bool allowtip)
{
	if (NULL != hToolTips){
		SendMessage(hToolTips, TTM_ACTIVATE, (WPARAM)allowtip, 0);
	}else{
		SendMessage(hToolTips, TTM_ACTIVATE, (WPARAM)false, 0);
	}
}

//===========================================================================

void SetWindowModes(void)
{
	if(usingWin2kXP){
		if(!inSlit){
			// Add the WS_EX_LAYERED atribute to the window.
			SetWindowLong(hwndPlugin, GWL_EXSTYLE , GetWindowLong(hwndPlugin, GWL_EXSTYLE) | WS_EX_LAYERED);
			// Make it transparent...
			BBSetLayeredWindowAttributes(hwndPlugin, NULL, (transparency) ? (unsigned char)alpha : 255, LWA_ALPHA);
		}else{
			// Remove the WS_EX_LAYERED atribute to the window, cos we in di slit!
			SetWindowLong(hwndPlugin, GWL_EXSTYLE, GetWindowLong(hwndPlugin, GWL_EXSTYLE) & ~WS_EX_LAYERED);
		}
	}
	InvalidateRect(hwndPlugin, NULL, false);
	ShowMyMenu(false);
}

//===========================================================================

bool ResizeMyWindow(int newWidth, int newHeight)
{
	if(newWidth != width || newHeight != height){
		width = newWidth;
		height = newHeight;
		if (width <= 16)  width  = 16;
		if (height <= 12) height = 12;
	}
	else
		return false;

	if(inSlit){
		SetWindowPos(hwndPlugin, 0, 0, 0, width, height, SWP_NOACTIVATE | SWP_NOZORDER | SWP_NOMOVE);
		SendMessage(hSlit, SLIT_UPDATE, NULL, NULL);
		return true;
	}else{
		SetWindowPos(hwndPlugin, 0, xpos, ypos, width, height, SWP_NOACTIVATE | SWP_NOZORDER);
		return true;
	}
}

//===========================================================================

//##################################################
//slider_updatetrack
//##################################################

void SliderUpdateTrack(RECT &windowrect)
{
	track.left   = windowrect.left + SLIDER_OUTERBORDERMARGIN;
	track.right  = windowrect.right - SLIDER_OUTERBORDERMARGIN;
	track.bottom = windowrect.bottom - SLIDER_OUTERBORDERMARGIN;
	track.top    = windowrect.top + SLIDER_OUTERBORDERMARGIN;

	if (vertical){
		track_length   = (windowrect.bottom - windowrect.top) - 2 * SLIDER_OUTERBORDERMARGIN;
		knob_maxlength = windowrect.bottom - windowrect.top - 2 * SLIDER_INNERBORDERMARGIN;
	}else{
		track_length   = (windowrect.right - windowrect.left) - 2 * SLIDER_OUTERBORDERMARGIN;
		knob_maxlength = windowrect.right - windowrect.left - 2 * SLIDER_INNERBORDERMARGIN;
	}

	track_clickable = track;
	track_clickable_length = track_length;
	track_countable = track;
	track_countable_length = track_length;

	if (appearance >= scrollbar){
		if (appearance == scrollbar){
			knob_length = track_length / 20;
			if (knob_length < 4) knob_length = 4;
		}else{
			knob_length = 8;
			if (knob_length > track_length) knob_length = 4;         
		}

		int halflength = knob_length / 2;
		if (vertical){
			track_countable.top    += halflength;
			track_countable.bottom -= halflength;
		}else{
			track_countable.left  += halflength;
			track_countable.right -= halflength;
		}
		track_countable_length -= 2 * halflength;

		if (appearance == trackknob){
			//Make the track half it's girth
			int partgirth;
			if (vertical){
				partgirth = (windowrect.right - windowrect.left) / 6;
				if (partgirth > 1){
					track.left  += partgirth;
					track.right -= partgirth;
				}
			}else{
				partgirth = (windowrect.bottom - windowrect.top) / 6;
				if (partgirth > 1){
					track.top    += partgirth;
					track.bottom -= partgirth;
				}
			}
		}
	}
	knob_needsupdate = true;
	track_needsupdate = false;
}

//##################################################
//controltype_slider_updateknob
//##################################################

void SliderUpdateKnob(RECT &windowrect)
{
	double realvalue = value;
	if (reversed) realvalue = 1.0 - realvalue;

	if (appearance == fillbar){
		knob_length = (int) (knob_maxlength * realvalue + 0.49);

		if (vertical){
			knob.left   = windowrect.left + SLIDER_INNERBORDERMARGIN;
			knob.right  = windowrect.right - SLIDER_INNERBORDERMARGIN;
			knob.bottom = windowrect.bottom - SLIDER_INNERBORDERMARGIN;
			knob.top    = knob.bottom - knob_length;
		}else{
			knob.left   = windowrect.left + SLIDER_INNERBORDERMARGIN;
			knob.right  = knob.left + knob_length;
			knob.bottom = windowrect.bottom - SLIDER_INNERBORDERMARGIN;
			knob.top    = windowrect.top + SLIDER_INNERBORDERMARGIN;
		}

	}else if (appearance >= scrollbar){
		int adjusted_track_length = knob_maxlength - knob_length + 2;
		int offset = (int)(realvalue * adjusted_track_length) - 1;

		if (vertical){
			knob.left   = windowrect.left + SLIDER_OUTERBORDERMARGIN;
			knob.right  = windowrect.right - SLIDER_OUTERBORDERMARGIN;
			knob.bottom = windowrect.bottom - SLIDER_INNERBORDERMARGIN - offset;
			knob.top    = knob.bottom - knob_length;
		}else{
			knob.left   = windowrect.left + SLIDER_INNERBORDERMARGIN + offset;
			knob.right  = knob.left + knob_length;
			knob.bottom = windowrect.bottom - SLIDER_OUTERBORDERMARGIN;
			knob.top    = windowrect.top + SLIDER_OUTERBORDERMARGIN;
		}
	}
	knob_needsupdate = false;
}

//##################################################
//controltype_slider_on_value_change
//##################################################

void SliderOnValueChange(LPARAM lParam)
{
	//Chose to ignore it if out of track
	const int d = 20;
	int mouse_x = (short)LOWORD(lParam);
	int mouse_y = (short)HIWORD(lParam);
	if (mouse_x < track_clickable.left - d
		|| mouse_x >= track_clickable.right + d
		|| mouse_y < track_clickable.top - d
		|| mouse_y >= track.bottom + d)
		return;

	//Figure out what value was clicked
	if (vertical)
		value = (double) (-(mouse_y - track_countable.bottom)) / (double) (track_countable_length);
	else
		value = (double) (mouse_x - track_countable.left) / (double) (track_countable_length);

	if (reversed)
		value = 1.0 - value;

	if (value < 0.0) value = 0.0;
	if (value > 1.0) value = 1.0;

	//We'll need to update the knob and offset now
	knob_needsupdate = true;

	(*FgetPlayerData)(WM_JUMPTOTIME, (int)(value * play_total));
}

//===========================================================================

