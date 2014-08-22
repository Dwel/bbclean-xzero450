/*
 ============================================================================
 Blackbox for Windows: BBStyle plugin
 ============================================================================
 Copyright © 2002-2009 nc-17@ratednc-17.com
 Copyright © 2007-2009 The Blackbox for Windows Development Team
 http://www.ratednc-17.com
 http://bb4win.sourceforge.net
 ============================================================================

  Blackbox for Windows is free software, released under the
  GNU General Public License (GPL version 2 or later), with an extension
  that allows linking of proprietary modules under a controlled interface.
  What this means is that plugins etc. are allowed to be released
  under any license the author wishes. Please note, however, that the
  original Blackbox gradient math code used in Blackbox for Windows
  is available under the BSD license.

  http://www.fsf.org/licenses/gpl.html
  http://www.fsf.org/licenses/gpl-faq.html#LinkingOverControlledInterface
  http://www.xfree86.org/3.3.6/COPYRIGHT2.html#5

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
  GNU General Public License for more details.

  For additional license information, please read the included license.html

 ============================================================================

  In the case of bbStyle, "TheDevTeam"
  includes (in alphabetical order): grischka, unkamunka.

 --------------------------------------------------------------------------------*/
#include "bbStyle.h"

// ----------------------------------
//Some global variables containing our plugin info
const char szVersion []    = "bbStyle 4.0";  
const char szAppName []    = "bbStyle"; // The name of our window class
const char szInfoVersion [] = "4.0";
const char szInfoAuthor []  = "NC-17|TheDevTeam";
const char szInfoRelDate [] = "2009-06-01";
const char szInfoLink []    = "http://bb4win.sourceforge.net/";
const char szInfoEmail []   = "irc://irc.freenode.net/bb4win";
const char szInfoUpdateURL []= "http://www.lostinthebox.com/viewforum.php?t=2941";

const char szCopyright []  = "2001-2009 nc-17@ratednc-17.com";
const char broam_prefix [] = "@bbStyle.";

//====================
// ----------------------------------
// Interface declaration

extern "C"
{
	// The startup interface
	// beginPlugin does not call endPlugin if beginPlugin fails, or returns 1.
	DLL_EXPORT int beginPlugin(HINSTANCE hPluginInstance);
	DLL_EXPORT int beginPluginEx(HINSTANCE hPluginInstance, HWND hSlit)
	{
		// ---------------------------------------------------
		// grab some global information

		BBhwnd          = GetBBWnd();
		hInstance       = hPluginInstance;
		hSlit_present   = hSlit;

		// Always do window/plugin creation stuff before setting window attributes.
		// Don't forget to "UnregisterClass(szAppName, hInstance)" or any other succesfully
		// loaded resources on a bail out (return 1).
		// i.e. below code --  "if (!hwndPlugin)".

		WNDCLASSEX wcl;
		// ---------------------------------------------------
		// register the window class
		ZeroMemory(&wcl, sizeof wcl);

		wcl.cbSize			= sizeof(WNDCLASSEX);
		wcl.style            = CS_DBLCLKS;
		wcl.lpfnWndProc      = WndProc;      // window procedure
		wcl.hInstance        = hInstance;    // hInstance of .dll
		wcl.hCursor          = LoadCursor(NULL, IDC_ARROW);
		wcl.lpszClassName    = szAppName;    // window class name

		if (!RegisterClassEx(&wcl))
		{
			MessageBox(BBhwnd,
				"Error registering window class", szVersion,
				MB_OK | MB_ICONERROR | MB_TOPMOST);
			return 1;
		}

		// ---------------------------------------------------
		// Zero out variables, read configuration and style

		ZeroMemory(&style_info,sizeof(style_info));
		ZeroMemory(&position,sizeof(position));
		ZeroMemory(&plugin, sizeof plugin);
		plugin.shown = true;

		getRCSettings();
		getStyleSettings();

		// ---------------------------------------------------
		// create the window
		plugin.hwnd = CreateWindowEx(
			WS_EX_TOOLWINDOW,   // window style
			szAppName,          // window class name
			NULL,               // window caption: NULL -> does not show up in task manager!
			WS_POPUP | WS_CLIPCHILDREN | WS_CLIPSIBLINGS, // window style
			position.X,            // x position
			position.Y,            // y position
			plugin.width,           // window width
			plugin.height,          // window height
			NULL,               // parent window
			NULL,               // no window menu
			hInstance,          // hInstance of .dll
			NULL                // creation data
			);

		if (!plugin.hwnd)
		{
			MessageBox(0, "Error creating window", szVersion, MB_OK | MB_ICONERROR | MB_TOPMOST);
			UnregisterClass(szAppName, hInstance);
			//unload any other previously loaded resources here.
			//--->
			return 1;
		}

		// ---------------------------------------------------
		//Wooh, done with all the initialization safety checks.
		//You may continue as normal.  ;)

		// Order matters here
		const char *bbv = GetBBVersion();
		is_xoblite = 0 == (_memicmp(bbv, "bb", 2) + strlen(bbv) - 3);
		
		// Transparency is only supported under Windows 2000/XP...
		plugin.usingWin2kPlus = GetOSVersion() >= 50;

		// ---------------------------------------------------
		// set window location and properties
		setPosition();
		set_window_modes();
		// Show the window and force it to update...
		if (plugin.shown)
			ShowWindow(plugin.hwnd, SW_SHOWNA);

		// changeOnStart?
		if (plugin.changeOnStart)
			setStyle();

		// check the clock status - use uneven # of seconds
		if (plugin.timerOn)
			SetTimer(plugin.hwnd, 1, plugin.changeTime*59000, (TIMERPROC)NULL);
		
		return 0;
	}
	// on unload...
	// only end plugin stuff that would have been done if the the begin did not fail
	DLL_EXPORT void endPlugin(HINSTANCE hPluginInstance)
	{
		// Get out of the Slit, in case we are...
		if (plugin.hSlit) SendMessage(plugin.hSlit, SLIT_REMOVE, 0, (LPARAM)plugin.hwnd);

		// Destroy the window...
		DestroyWindow(plugin.hwnd);

		// clean up HBITMAP object
		if (plugin.bufbmp) DeleteObject(plugin.bufbmp);

		// clean up HFONT object
		if (plugin.hFont) DeleteObject(plugin.hFont);

		// Unregister window class...
		UnregisterClass(szAppName, hPluginInstance);
	}
	// pluginInfo is used by Blackbox for Windows to fetch information about
	// a particular plugin. At the moment this information is simply displayed
	// in an "About loaded plugins" MessageBox, but later on this will be
	// expanded into a more advanced plugin handling system. Stay tuned! :)
	DLL_EXPORT LPCSTR pluginInfo(int field)
	{
		LPCSTR infostr[9] =
		{
			szVersion       , // Fallback: Plugin name + version, e.g. "MyPlugin 1.0"
			szAppName       , // Plugin name
			szInfoVersion   , // Plugin version
			szInfoAuthor    , // Author
			szInfoRelDate   , // Release date, preferably in yyyy-mm-dd format
			szInfoLink      , // Link to author's website
			szInfoEmail     , // Author's email
			(LPCSTR)("@bbStyle.about"  //Display message box about the plugin	
			"@bbStyle.useSlit"  //Toggle whether plugin is in slit
			"@bbStyle.changeOnStart"  //Toggle whether a random style is set on startup
			"@bbStyle.stylePath"  //Set the default root Style path of your choice
			"@bbStyle.showStyleInfo"  //Toggle whether whether stayle names are displayed
			"@bbStyle.random"  //Select a random style and apply it
			"@bbStyle.togglePlugins"  //Standard BB bro@m to show|hide all plugins
			"@bbStyle.timerOn"  //Toggle the use of the timer for changing styles
			"@bbStyle.onTop"  //Toggle window always on top
			"@bbStyle.loadDocs"  //Load the documentation
			"@bbStyle.editRC") , // Broams
			szInfoUpdateURL  // Link to update page

			// ==========
		};
		return (field >= 0 && field < 9) ? infostr[field] : infostr[0];
	}
};

//===========================================================================
void about_box(void)
{
		char buffer[MAX_PATH];
		sprintf(buffer,
			"%s - A style changer for Blackbox for Windows"
			"\n"
			"\n© %s"
			"\n"
			"\n%s"
			"\n%s"
			, szVersion, szCopyright, szInfoLink, szInfoEmail
			);
		message_box(MB_OK, buffer);
}

//===========================================================================

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	static int msgs[] = { BB_RECONFIGURE, BB_BROADCAST, 0};
	char path[MAX_PATH];

	switch (message)
	{
		case WM_CREATE:
		{
			// Register to receive these messages
			SendMessage(BBhwnd, BB_REGISTERMESSAGE, (WPARAM)hwnd, (LPARAM)msgs);
			// Make the window appear on all workspaces
			MakeSticky(hwnd);
		}
		break;

		case WM_DESTROY:
		{
			RemoveSticky(hwnd);
			// Unregister Blackbox messages...
			SendMessage(BBhwnd, BB_UNREGISTERMESSAGE, (WPARAM)hwnd, (LPARAM)msgs);
			styleList.clear();
		}
		break;

		// ----------------------------------------------------------
		// Window update process...
		// Painting with a cached double-buffer.

		case WM_PAINT:
		{
			drawPlugin();
		}
		break;

		//====================
		// Timer message:

		case WM_TIMER:
			{
				//StyleChanger
				if(wParam==1)
					setStyle();
			}
			break;

		// ----------------------------------------------------------
		// Manually moving/sizing has been started

		case WM_ENTERSIZEMOVE:
			plugin.is_moving = true;
			break;

		case WM_EXITSIZEMOVE:
			if (plugin.is_moving)
			{
				if (plugin.hSlit) // already we are
				{
					SendMessage(plugin.hSlit, SLIT_UPDATE, 0, (LPARAM)hwnd);
				}
				else
				{
					// record new location
					WriteInt(rcpath, "bbStyle.X:", position.X);
					WriteInt(rcpath, "bbStyle.Y:", position.Y);
					WriteString(rcpath, "bbStyle.placement:", (char*)"User");
				}
			}
			plugin.is_moving =  false;
			break;

		// ---------------------------------------------------
		// Make sure plugin stays on screen.

		case WM_WINDOWPOSCHANGING:
			if (plugin.is_moving)
				if(IsWindowVisible(hwnd)) SnapWindowToEdge((WINDOWPOS*)lParam, position.snap, true);
			break;

		// ----------------------------------------------------------
		// start moving according to keys held down

		case WM_LBUTTONDOWN:
			UpdateWindow(hwnd);
			if (GetAsyncKeyState(VK_CONTROL) & 0x8000)
			{
				// start moving, when control-key is held down
				PostMessage(hwnd, WM_SYSCOMMAND, 0xf012, 0);
				break;
			}
			if (GetAsyncKeyState(VK_SHIFT) & 0x8000)
			{
				// toggle styleInfo display
				eval_menu_cmd(M_BOL, &plugin.showStyleInfo, "showStyleInfo");
				SendMessage(BBhwnd, BB_RECONFIGURE, 0, 0);
				break;
			}
			// otherwise change styles
			// required for pressed effect
			leftButtonDown = true;
			invalidate_window();
			// do not reset until ButtonUp
			break;

		case WM_LBUTTONUP:
			{
			if (GetAsyncKeyState(VK_CONTROL) & 0x8000)
				break;
				
			if (GetAsyncKeyState(VK_SHIFT) & 0x8000)
				break;
				
			leftButtonDown = false;
			invalidate_window();
			setStyle();
			}
			break;

		case WM_MBUTTONUP:
			{
			// show current Styles menu - xoblite has its own default
			if (!is_xoblite)
			{
				char p[MAX_PATH];
				sprintf(p, "@BBCore.ShowMenu %s  >> @BBCore.style %%1", plugin.stylePath);
				SendMessage(BBhwnd, BB_BROADCAST, 0, (LPARAM)p);
			}
			}
			break;
			
		// ----------------------------------------------------------
		// Show the user menu

		case WM_RBUTTONUP:
			if (GetAsyncKeyState(VK_SHIFT) & 0x8000)
			{
				// enter|leave slit, when shift-key is held down
				eval_menu_cmd(M_BOL, &plugin.useSlit, "useSlit");
				break;
			}
			else
				displayMenu(true);
			break;

		// ----------------------------------------------------------

		case WM_DISPLAYCHANGE:  
		{ 
			if (NULL == plugin.hSlit)
				setPosition();
		}
		break; 

		case WM_KEYUP:
		{
			if (wParam == VK_NUMPAD7) 
				SendMessage(plugin.hwnd, BB_BROADCAST, 0, (LPARAM)"@bbStyle.TopLeft");
			else if (wParam == VK_NUMPAD8) 
				SendMessage(plugin.hwnd, BB_BROADCAST, 0, (LPARAM)"@bbStyle.TopCenter");
			else if (wParam == VK_NUMPAD9) 
				SendMessage(plugin.hwnd, BB_BROADCAST, 0, (LPARAM)"@bbStyle.TopRight");
			else if (wParam == VK_NUMPAD4) 
				SendMessage(plugin.hwnd, BB_BROADCAST, 0, (LPARAM)"@bbStyle.CenterLeft");
			else if (wParam == VK_NUMPAD6) 
				SendMessage(plugin.hwnd, BB_BROADCAST, 0, (LPARAM)"@bbStyle.CenterRight");
			else if (wParam == VK_NUMPAD1) 
				SendMessage(plugin.hwnd, BB_BROADCAST, 0, (LPARAM)"@bbStyle.BottomLeft");
			else if (wParam == VK_NUMPAD2) 
				SendMessage(plugin.hwnd, BB_BROADCAST, 0, (LPARAM)"@bbStyle.BottomCenter");
			else if (wParam == VK_NUMPAD3) 
				SendMessage(plugin.hwnd, BB_BROADCAST, 0, (LPARAM)"@bbStyle.BottomRight");
			else if ((wParam == VK_BACK) && !is_xoblite)
				SendMessage(plugin.hwnd, BB_BROADCAST, 0, (LPARAM)"@bbStyle.stylePath");
			else if (wParam == VK_SUBTRACT)
				SendMessage(plugin.hwnd, BB_BROADCAST, 0, (LPARAM)"@bbStyle.editRC");
			else if (wParam == VK_ADD)
				SendMessage(plugin.hwnd, BB_BROADCAST, 0, (LPARAM)"@bbStyle.onTop");
			else if (wParam == VK_ESCAPE)	
				SendMessage(plugin.hwnd, BB_BROADCAST, 0, (LPARAM)"@bbStyle.timerOn");
			
			invalidate_window();			
		}
		break;

		// ----------------------------------------------------------
		// prevent the user from closing the plugin with alt-F4

		case WM_CLOSE:
			break;

		// ----------------------------------------------------------
		// If Blackbox sends a reconfigure message, load the new style settings and update the window...

		case BB_RECONFIGURE:
			getStyleSettings();
			set_window_modes();
			break;

		// ----------------------------------------------------------
		// Blackbox sends Bro@ms to all windows...

		case BB_BROADCAST:
		{
			const char *msg_string = (LPCSTR)lParam;

			// check general broams
			if (!stricmp(msg_string, "@BBShowPlugins"))
			{
				if (!plugin.shown)
				{
					plugin.shown = true;
					ShowWindow(hwnd, SW_SHOWNA);
				}
				break;
			}

			if (!stricmp(msg_string, "@BBHidePlugins"))
			{
				if (plugin.toggle && NULL == plugin.hSlit)
				{
					plugin.shown = false;
					ShowWindow(hwnd, SW_HIDE);
				}
				break;
			}

			// Our broadcast message prefix:
			const int broam_prefix_len = sizeof broam_prefix - 1; // minus terminating \0

			// check broams sent from our own menu
			if (!memicmp(msg_string, broam_prefix, broam_prefix_len))
			{
				msg_string += broam_prefix_len;
				// note - styleList is refreshed on every style change
				if (!stricmp(msg_string, "random"))
					{
					setStyle();
					break;
					}
				if (!stricmp(msg_string, "timerOn"))
				{
					eval_menu_cmd(M_BOL, &plugin.timerOn, msg_string);
					if (plugin.timerOn)
					// Force randomisation by uneven # of seconds
						SetTimer(plugin.hwnd, 1, plugin.changeTime*59000, (TIMERPROC)NULL);
					else
						KillTimer(plugin.hwnd, 1);
					break;
				}
				if (!stricmp(msg_string, "editRC"))
				{
					GetBlackboxEditor(path);
					BBExecute(NULL, NULL, path, rcpath, NULL, SW_SHOWNORMAL, false);
					break;
				}
				if (!stricmp(msg_string, "stylePath"))
				{
					if (false == select_folder(plugin.hwnd, plugin.stylePath, path, false))
						break;
					WriteString(rcpath, "bbStyle.stylePath:", path);
					getRCSettings();
					setStyle();
					break;
				}
				if (!_stricmp(msg_string, "loadDocs"))
				{
					locate_file(hInstance, path, szAppName, "html");
					BBExecute(BBhwnd, "open", path, NULL, NULL, SW_SHOWNORMAL, false);
				}
				if (!stricmp(msg_string, "about"))
				{
					about_box();
					break;
				}
				// set out the standard broams values in the broamprops table below
				const broamprop *bp = check_item(msg_string, always_broams);
				if (bp)
				{
					eval_menu_cmd(bp->mode, bp->val, msg_string);
					break;
				}
				// check bro@ms that won't work in the Slit
				if (!plugin.useSlit)
				{
					const broamprop *bps = check_item(msg_string, nonSlit_broams);
					if (bps)
					{
						eval_menu_cmd(bps->mode, bps->val, msg_string);
						break;
					}
					// check position bro@ms
					const broamprop *bpp = check_item(msg_string, position_broams);
					if (bpp)
					{
						strcpy(position.placement, bpp->key);
						setPosition();
						WriteString(rcpath, "bbStyle.placement:", bpp->key);
						set_window_modes();
					break;
					}
				}
				char szTemp[MAX_PATH];
				GetBlackboxEditor(szTemp);
				if (!stricmp(msg_string, "edit3dcRC"))
				{
					get_path(path, sizeof(path), "plugins\\bbColor_3dc2\\bbColor_3dc2.rc");
					if (File_Exists(path))
					{
						BBExecute(NULL, NULL, szTemp, path, NULL, SW_SHOWNORMAL, false);
						break;
					}
					else
						get_path(path, sizeof(path), "plugins\\bbColor3dc\\bbColor3dc.rc");
					if (File_Exists(path))
						BBExecute(NULL, NULL, szTemp, path, NULL, SW_SHOWNORMAL, false);
					break;
				}
				if (!stricmp(msg_string, "load3dcDocs"))
				{
					get_path(path, sizeof(path), "plugins\\bbColor_3dc2\\bbColor_3dc2.html");
					if (File_Exists(path))
						BBExecute(BBhwnd, "open", path, NULL, NULL, SW_SHOWNORMAL, false);
					break;
				}
			}
			break;
		}

		// ----------------------------------------------------------
		// let windows handle any other message
		default:
			return DefWindowProc(hwnd,message,wParam,lParam);
	}
	return 0;
}

//===========================================================================

void drawPlugin()
{
		// Create buffer hdc's, bitmaps etc.
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(plugin.hwnd, &ps);

		// create a DC for the buffer
		HDC buf = CreateCompatibleDC(hdc);
		HGDIOBJ otherbmp;

		if (NULL == plugin.bufbmp) // No bitmap yet?
		{
			// Generate it and paint everything
			plugin.bufbmp = CreateCompatibleBitmap(hdc, plugin.width, plugin.height);

			// Select it into the DC, storing the previous default.
			otherbmp = SelectObject(buf, plugin.bufbmp);

			// Setup the rectangle
			RECT r; r.left = r.top = 0; r.right = plugin.width; r.bottom = plugin.height;

			// and draw the frame
			MakeStyleGradient(buf, &r, &style_info.Frame, style_info.Frame.bordered);

			// Set the font, storing the default..
			HGDIOBJ otherfont = SelectObject(buf, plugin.hFont);
			SetBkMode(buf, TRANSPARENT);

			// adjust the rectangle for a margin
			r.left  += style_info.frameWidth;
			r.top   += style_info.frameWidth;
			r.right   -= style_info.frameWidth;
			r.bottom  -= style_info.frameWidth;

			if (leftButtonDown)
			{
				MakeStyleGradient(buf, &r, &style_info.Pressed, style_info.Pressed.bordered);
				SetTextColor(buf, style_info.Pressed.TextColor);
			}
			else
			{
				MakeStyleGradient(buf, &r, &style_info.Button, style_info.Button.bordered);
				SetTextColor(buf, style_info.Button.TextColor);
			}


			if (!plugin.showStyleInfo)
			{
				strcpy(plugin.windowText, "S");
				BBDrawText(buf, plugin.windowText, -1, &r, DT_CENTER|DT_VCENTER|DT_SINGLELINE|DT_WORD_ELLIPSIS, leftButtonDown ? &style_info.Pressed : &style_info.Button);
			}
			else
			{
				//vertical centering not in API for multiple lines
				getTitleText(true);
				BBDrawText(buf, plugin.windowText, -1, &r, DT_CENTER|DT_WORDBREAK, leftButtonDown ? &style_info.Pressed : &style_info.Button);
			}

			// Put back the previous default font.
			SelectObject(buf, otherfont);
		}
		else
		{
			// Otherwise it has been painted previously,
			// so just select it into the DC
			otherbmp = SelectObject(buf, plugin.bufbmp);
		}

		// ... and copy the buffer on the screen:
		BitBlt(hdc,
			ps.rcPaint.left,
			ps.rcPaint.top,
			ps.rcPaint.right  - ps.rcPaint.left,
			ps.rcPaint.bottom - ps.rcPaint.top,
			buf,
			ps.rcPaint.left,
			ps.rcPaint.top,
			SRCCOPY
			);

		// Put back the previous default bitmap
		SelectObject(buf, otherbmp);

		//remember to delete all objects
		DeleteDC(buf);

		// takes care of hdc
		EndPaint(plugin.hwnd, &ps);
}

//===========================================================================

void getRCSettings(void)
{
	// If a config file was found we read the plugin settings from it...
	// ...if not, the ReadXXX functions give us just the defaults.
	locate_file(hInstance, (char*)rcpath, szAppName, "rc");

	position.X     = ReadInt(rcpath, "bbStyle.X:", 775);
	position.Y     = ReadInt(rcpath, "bbStyle.Y:", 10);
	position.fullScreen = ReadBool(rcpath, "bbStyle.fullScreen:", false);
	position.snap       = ReadInt(rcpath, "bbStyle.snap:", 20);
	plugin.shown	= ReadBool(rcpath, "bbStyle.shown:", true);
	strcpy(position.placement, ReadString(rcpath, "bbStyle.placement:", "TopCenter"));

	plugin.alpha      = plugin.usingWin2kPlus ? ReadInt(rcpath,  "bbStyle.alpha:", *(int *)GetSettingPtr(SN_MENUALPHA)) : 8;
	plugin.alpha      = eightScale_up(plugin.alpha);
	plugin.onTop      = ReadBool(rcpath, "bbStyle.onTop:", true);
	plugin.toggle     = ReadBool(rcpath, "bbStyle.togglePlugins:", true);
	plugin.useSlit    = ReadBool(rcpath, "bbStyle.useSlit:", false);

	plugin.changeTime	= ReadInt(rcpath, "bbStyle.changeTime:", 5);
	plugin.showStyleInfo = ReadBool(rcpath, "bbStyle.showStyleInfo:", true);
	plugin.timerOn		= ReadBool(rcpath, "bbStyle.timerOn:", true);
	plugin.changeOnStart = ReadBool(rcpath, "bbStyle.changeOnStart:", false);
	plugin.count	= ReadInt(rcpath, "bbStyle.count:", 0);
	plugin.chance	= ReadBool(rcpath, "bbStyle.chance:", true);

	strcpy(plugin.stylePath, ReadString(rcpath, "bbStyle.stylePath:", "no"));
	if (strlen(ReadString(rcpath, "bbStyle.stylePath:", "no")) == 2) 
		strcpy(plugin.stylePath, set_stylePath());
	strcpy(listPath, plugin.stylePath);
	
	//find a new style - list refreshed on every styleChange
	initList(listPath, true);
}

void get3dcSettings(void)
{
	char dcPath[MAX_PATH];
	dcFile = File_Exists(get_path(dcPath, sizeof(dcPath), "plugins\\bbColor_3dc2\\bbColor_3dc2.rc")) ? true : File_Exists(get_path(dcPath, sizeof(dcPath), "plugins\\bbColor3dc\\bbColor3dc.rc"));

	dc_info.Menus		 = ReadBool(dcPath, "bbColor3dc.SetMenus:", false);
	dc_info.Selected     = ReadBool(dcPath, "bbColor3dc.SetSelectedItem:", false);
	dc_info.Tooltips     = ReadBool(dcPath, "bbColor3dc.SetTooltip:", false);
	dc_info.Highlights   = ReadBool(dcPath, "bbColor3dc.SetMenuSelect:", false);
	dc_info.Titles		 = ReadBool(dcPath, "bbColor3dc.SetTitles:", false);
}

//===========================================================================

void getStyleSettings(void)
{
	bool nix = false;
	if (!is_xoblite)
		nix = 0 == (int)GetSettingPtr(35); //SN_NEWMETRICS aka SN_ISSTYLE070
	else 
	{
		char style[MAX_PATH]; char temp[64]; 
		strcpy(style, stylePath());
		strcpy(temp, ReadString(style, "toolbar.appearance:", (char *)"no"));
		if (strlen(temp) != 2) nix = true;
	}

	style_info.Frame = *(StyleItem *)GetSettingPtr(SN_TOOLBAR);
	if (false == (style_info.Frame.validated & VALID_TEXTCOLOR))
		style_info.Frame.TextColor = ((StyleItem *)GetSettingPtr(SN_TOOLBARLABEL))->TextColor;
	style_info.Button = *(StyleItem *)GetSettingPtr(SN_TOOLBARBUTTON);
	if (style_info.Button.parentRelative)
		style_info.Button = *(StyleItem *)GetSettingPtr(SN_TOOLBAR);
	if (false == (style_info.Button.validated))
		style_info.Button.TextColor = ((StyleItem *)GetSettingPtr(SN_TOOLBARLABEL))->TextColor;
	style_info.Pressed = *(StyleItem *)GetSettingPtr(SN_TOOLBARBUTTONP);
	if (style_info.Pressed.parentRelative)
		style_info.Pressed = *(StyleItem *)GetSettingPtr(SN_TOOLBAR);
	if (false == (style_info.Pressed.validated))
		style_info.Pressed.TextColor = ((StyleItem *)GetSettingPtr(SN_TOOLBARLABEL))->TextColor;

	style_info.frameWidth = nix ? (style_info.Frame.marginWidth +style_info.Frame.borderWidth) : (*(int*)GetSettingPtr(SN_BEVELWIDTH) + *(int*)GetSettingPtr(SN_BORDERWIDTH));
	plugin.hideWidth = imin(style_info.frameWidth, 3);
	
	if (plugin.hFont) DeleteObject(plugin.hFont);
	plugin.hFont = CreateStyleFont((StyleItem *)GetSettingPtr(SN_TOOLBAR));	

	if (plugin.showStyleInfo)
	{
		plugin.height = ((int)(5.25 * style_info.Frame.FontHeight));
		if (style_info.Frame.FontHeight >8)
			plugin.height = (int)(plugin.height/2);
		plugin.width = int((((int)getTitleText(false) * .6) + 4) * style_info.Frame.FontHeight * .6) ;
		/*		plugin.width = (150 *//*+ temp + (2 * style_info.Frame.borderWidth) + (2 * style_info.Button.marginWidth));
		if (plugin.width > 170) plugin.width = 170;*/
	}
	else
	{
		plugin.width = plugin.height = ((style_info.Frame.FontHeight) +  (5 * style_info.Frame.borderWidth) + (5 * style_info.Button.marginWidth))/**/;
		if (plugin.width > 40) plugin.width = 40;
	}
}

//===========================================================================

void displayMenu(bool popup)
{
	Menu *pMenu, *pSub, *pStyle, *p3dc, *pPlace;
	bool MonitorCount = GetSystemMetrics(SM_CMONITORS) > 1;

	// Create the main menu, with a title and an unique IDString
	pMenu = MakeNamedMenu("bbStyle_3dc", "bbStyle_IDMain", popup);

	if (!hSlit_present) hSlit_present = FindWindow("BBSlit", "");

	// Insert first Item
	if (NULL == plugin.hSlit)
	{
		// Create a submenu, also with title and unique IDString
		pSub = MakeNamedMenu("Window", "bbStyle_IDConfig", popup);
			// these are only available if outside the slit
			MakeMenuItem(pSub,      "Always On Top",        "@bbStyle.onTop", plugin.onTop);
			MakeMenuItemInt(pSub,      "Snap To Edges",        "@bbStyle.snap", position.snap, 0, 50);
			MakeMenuNOP(pSub, NULL);
			MakeMenuItem(pSub,      "Toggle With Plugins",  "@bbStyle.togglePlugins", plugin.toggle);
			if (plugin.usingWin2kPlus)
			MakeMenuItemInt(pSub,   "Alpha Value",          "@bbStyle.alpha", eightScale_down(plugin.alpha), 0, 8);
		// Insert the submenu into the main menu
		MakeSubmenu(pMenu, pSub, "Window");

	// Position Submenu
		pPlace = MakeNamedMenu("Placement", "bbStyle_IDPlace", popup);
		MakeMenuItem(pPlace, "Top Left", "@bbStyle.TopLeft", (position.side == 7));
		MakeMenuItem(pPlace, "Top Center", "@bbStyle.TopCenter", (position.side == 8));
		MakeMenuItem(pPlace, "Top Right", "@bbStyle.TopRight", (position.side == 9));
		MakeMenuNOP(pPlace, NULL);
		MakeMenuItem(pPlace, "Center Left", "@bbStyle.CenterLeft", (position.side == 4));
		MakeMenuItem(pPlace, "Center Right", "@bbStyle.CenterRight", (position.side == 6));
		MakeMenuNOP(pPlace, NULL);
		MakeMenuItem(pPlace, "Bottom Left", "@bbStyle.BottomLeft", (position.side == 1));
		MakeMenuItem(pPlace, "Bottom Center", "@bbStyle.BottomCenter", (position.side == 2));
		MakeMenuItem(pPlace, "Bottom Right", "@bbStyle.BottomRight", (position.side == 3));
	if (MonitorCount)
	{
		MakeMenuNOP(pPlace, NULL);
		MakeMenuItem(pPlace, "Full Screen", "@bbStyle.fullScreen", position.fullScreen);
	}
		MakeSubmenu(pMenu, pPlace, "Placement");		

		MakeMenuNOP(pMenu, NULL);
	}

	// bbStyle Submenu
	pStyle = MakeNamedMenu("Edit bbStyle Values", "bbStyle_IDStyle", popup);
		MakeMenuItem(pStyle, "Visible", "@bbStyle.shown", plugin.shown);
		MakeMenuItem(pStyle, "Display Style Info", "@bbStyle.showStyleInfo", plugin.showStyleInfo);
		MakeMenuItem(pStyle, "Timer Style Change", "@bbStyle.timerOn", plugin.timerOn);
		MakeMenuItem(pStyle, "Startup Style Change", "@bbStyle.changeOnStart", plugin.changeOnStart);		

		MakeMenuNOP(pStyle, NULL);

		if (hSlit_present)
			MakeMenuItem(pStyle,  "Use Slit",     "@bbStyle.useSlit", plugin.useSlit);
		MakeMenuItemInt(pStyle, "Timer Interval", "@bbStyle.changeTime", plugin.changeTime, 1, 600);
		/*if (!is_xoblite)*/
			MakeMenuItem(pStyle, "Random Change", "@bbStyle.chance", plugin.chance);		
		MakeMenuNOP(pStyle, NULL);
		if (!is_xoblite)
			MakeMenuItem(pStyle, "Style Path", "@bbStyle.stylePath", false);
		else
			MakeMenuItemString(pStyle, "Style Path", "@bbStyle.stylePath", plugin.stylePath);

		MakeMenuItem(pStyle, "Open bbStyle Settings File", "@bbStyle.editRC", false);
	MakeSubmenu(pMenu, pStyle, "Edit bbStyle Values");

	// bbColor3dc Submenu
	if (FindWindow("bbColor_3dc2", NULL) || FindWindow("bbColor-3dc2", NULL) || FindWindow("bbColor3dc", NULL))
	{
		p3dc = MakeNamedMenu("Edit bb3dc Values", "bbStyle_IDBroams", popup);
		get3dcSettings();
		if (dcFile)
		{
			MakeMenuItem(p3dc, "3dc Menus && Desktop", "@bbColor3dc.SetMenus", dc_info.Menus);
			MakeMenuItem(p3dc, "3dc Selected Item", "@bbColor3dc.SetSelectedItem", dc_info.Selected);
			MakeMenuItem(p3dc, "3dc Tooltips", "@bbColor3dc.SetTooltip", dc_info.Tooltips);
			MakeMenuItem(p3dc, "3dc Highlights", "@bbColor3dc.SetMenuSelect", dc_info.Highlights);
			MakeMenuItem(p3dc, "3dc Titles", "@bbColor3dc.SetTitles", dc_info.Titles);
			MakeMenuNOP(p3dc, NULL);
			MakeMenuItem(p3dc, "Append 3dc to Style", "@bbColor3dc.Append", false);
		}
		MakeMenuItem(p3dc, "Clear 3dc", "@bbColor3dc.Clear", false);
		MakeMenuItem(p3dc, "Keep 3dc", "@bbColor3dc.Keep", false);
		if (dcFile)
		{
			MakeMenuNOP(p3dc, NULL);
			MakeMenuItem(p3dc, "Open 3dc Settings File", "@bbStyle.edit3dcRC", false);
			MakeMenuItem(p3dc, "bb3dc Documentation", "@bbStyle.load3dcDocs", false);
		}
		MakeSubmenu(pMenu, p3dc, "Edit bb3dc Values");
	}

	// ----------------------------------
	// add an empty line
	MakeMenuNOP(pMenu, NULL);

	// add an entry for access to the documentation
	MakeMenuItem(pMenu, "Documentation", "@bbStyle.LoadDocs", false);

	// and an about box
	MakeMenuItem(pMenu, "About", "@bbStyle.About", false);

	// ----------------------------------
	// Finally, show the menu...
	ShowMenu(pMenu);
}

//===========================================================================
// helpers to handle commands  from the menu

// fill in standard bro@ms values here
const struct broamprop always_broams[] = {
    {"useSlit"			,M_BOL	,&plugin.useSlit		},
    {"changeOnStart"	,M_BOL 	,&plugin.changeOnStart     },
    {"changeTime"		,M_INT 	,&plugin.changeTime     },
    {"timerOn"			,M_BOL 	,&plugin.timerOn     },
    {"showStyleInfo" 	,M_BOL	,&plugin.showStyleInfo	},
    {"shown"			,M_BOL	,&plugin.shown			},
    {"chance"		 	,M_BOL	,&plugin.chance			},
    {NULL          ,0	,0         }
    };

// this is for bro@ms that won't work in the Slit
const struct broamprop nonSlit_broams[] = {
    {"snap"				,M_INT	,&position.snap		},
    {"fullScreen"		,M_BOL	,&position.fullScreen	},
    {"onTop"			,M_BOL	,&plugin.onTop			},
    {"alpha"			,M_INT	,&plugin.alpha			},
    {"togglePlugins"	,M_BOL	,&plugin.toggle			},
    {NULL          ,0	,0         }
    };

const struct broamprop position_broams[] = {
    {"TopLeft"			,7	,0		},
    {"TopCenter"		,8	,0		},
    {"TopRight"			,9	,0		},
    {"CenterLeft"		,4	,0		},
    {"CenterRight"		,6	,0		},
    {"BottomLeft"		,1	,0     },
    {"BottomCenter"		,2	,0     },
    {"BottomRight"		,3	,0     },
    {NULL			,0	,0     }
    };

const struct broamprop *check_item(const char *key, const broamprop *prop)
{
    const broamprop *p = prop;
    do if (strstr(key, p->key)) return p; while ((++p)->key);
    return NULL;
}

void eval_menu_cmd(int mode, void *pValue, const char *msg_string)
{
	// Our rc_key prefix:
	const int rc_prefix_len = sizeof szAppName;

	char rc_string[80];

	// scan for a second argument after a space, like in "AlphaValue 200"
	const char *p = strchr(msg_string, ' ');
	INT_PTR msg_len = p ? p++ - msg_string : (int)strlen(msg_string);

	// Build the full rc_key. i.e. "<plugin>.<subkey>:"
	strcpy(rc_string, szAppName);
	strcpy(rc_string + (rc_prefix_len - 1), "."); // adjust for terminating \0
	memcpy(rc_string + rc_prefix_len, msg_string, msg_len);
	strcpy(rc_string + rc_prefix_len + msg_len, ":");

	switch (mode)
	{
		case M_BOL: // --- check for pre-chosen boolean value ---
			if (NULL != p)
			{
				strcpy((char*)pValue, p);

				// write the new setting to the rc - file
				WriteString(rcpath, rc_string, (char*)pValue);
			}
			else // --- toggle boolean variable -----------------
			{
				*(bool*)pValue = false == *(bool*)pValue;

			// write the new setting to the rc - file
			WriteBool(rcpath, rc_string, *(bool*)pValue);
			}
			break;

		case M_INT: // --- set integer variable -------------------
			if (p)
			{
				*(int*)pValue = atoi(p);

				// write the new setting to the rc - file
				WriteInt(rcpath, rc_string, *(int*)pValue);
			}
			break;

		case M_STR: // --- set string variable -------------------
			if (p)
			{
				strcpy((char*)pValue, p);

				// write the new setting to the rc - file
				WriteString(rcpath, rc_string, (char*)pValue);
			}
			break;
	}

	// apply new settings
	set_window_modes();
}

//===========================================================================
// this invalidates the window, and resets the bitmap at the same time.

void invalidate_window(void)
{
	if (plugin.bufbmp)
	{
		// delete the bitmap, so it will be drawn again
		// next time with WM_PAINT
		DeleteObject(plugin.bufbmp);
		plugin.bufbmp = NULL;
	}
	// notify the os that the window needs painting
	InvalidateRect(plugin.hwnd, NULL, FALSE);
}

//===========================================================================
// Update position and size, as well as onTop, transparency and inSlit states.

void set_window_modes(void)
{
	HWND hwnd = plugin.hwnd;

	// update the menu checkmarks
	displayMenu(false);

	// and load latest settings
	getRCSettings();
	//dbg_printf("Xpos", ReadString(rcpath, "bbStyle.X:", "no"));

	if (!hSlit_present) hSlit_present = FindWindow("BBSlit", "");
	if (plugin.useSlit && hSlit_present)
	{
		// if in slit, dont move...
		SetWindowPos(hwnd, NULL,
			0, 0, plugin.width, plugin.height,
			SWP_NOACTIVATE|SWP_NOSENDCHANGING|SWP_NOZORDER|SWP_NOMOVE
			);

		if (plugin.hSlit) // already we are
		{
			SendMessage(plugin.hSlit, SLIT_UPDATE, 0, (LPARAM)hwnd);
		}
		else // enter it
		{
			SetTransparency(hwnd, 255);
			plugin.hSlit = hSlit_present;
			SendMessage(plugin.hSlit, SLIT_ADD, 0, (LPARAM)hwnd);
		}
	}
	else
	{
		if (plugin.hSlit) // leave it
		{
			SendMessage(plugin.hSlit, SLIT_REMOVE, 0, (LPARAM)hwnd);
			plugin.hSlit = NULL;
		}

		if (!plugin.shown)
		{
			ShowWindow(plugin.hwnd, SW_HIDE);
			return;
		}
		else
		{
			// is the window AlwaysOnTop?
			HWND hwnd_after = plugin.onTop ? HWND_TOPMOST : HWND_NOTOPMOST;
			UINT flags = SWP_NOACTIVATE|SWP_NOSENDCHANGING;
		/*getMonitorInfo();
		updatePosition();*/

			SetWindowPos(hwnd, hwnd_after, position.X, position.Y, plugin.width, plugin.height, flags);
			SetTransparency(hwnd, (BYTE)plugin.alpha);
		}
	}

	// window needs drawing
	invalidate_window();
}

//*****************************************************************************
void setPosition()
{
	int ScreenWidth = position.fullScreen ? GetSystemMetrics(SM_CXVIRTUALSCREEN) : GetSystemMetrics(SM_CXSCREEN);
    int ScreenHeight = position.fullScreen ? GetSystemMetrics(SM_CYVIRTUALSCREEN) : GetSystemMetrics(SM_CYSCREEN);


	if (!_stricmp(position.placement, "BottomCenter"))
	{
		position.X = (ScreenWidth - plugin.width) / 2;
		position.Y = ScreenHeight - plugin.height;
	}
	else if (!stricmp(position.placement, "TopLeft"))
	{
		position.X = 0;
		position.Y = 0;
	}
	else if (!_stricmp(position.placement, "BottomLeft"))
	{
		position.X = 0;
		position.Y = ScreenHeight - plugin.height;
	}
	else if (!_stricmp(position.placement, "TopRight"))
	{
		position.X = ScreenWidth - plugin.width;
		position.Y = 0;
	}
	else if (!_stricmp(position.placement, "BottomRight"))
	{
		position.X = ScreenWidth - plugin.width;
		position.Y = ScreenHeight - plugin.height;
	}
	else if(!_stricmp(position.placement, "CenterLeft"))
	{
		position.Y = (ScreenHeight - plugin.height) / 2;
		position.X = 0;
	}
	else if(!_stricmp(position.placement, "CenterRight"))
	{
		position.Y = (ScreenHeight - plugin.height) / 2;
		position.X = ScreenWidth - plugin.width;
	}
	else if(!stricmp(position.placement, "TopCenter"))
	{
		position.X = ReadInt(rcpath, "bbStyle.X:", (ScreenWidth - plugin.width) / 2 );
		position.Y = ReadInt(rcpath, "bbStyle.Y:", 0);
	}
	else 
	{
		position.X = ReadInt(rcpath, "bbStyle.X:", 895);
		position.Y = ReadInt(rcpath, "bbStyle.Y:", 10);
		strcpy(position.placement, "User");
	}
	
	WriteInt(rcpath, "bbStyle.X:", position.X);
	WriteInt(rcpath, "bbStyle.Y:", position.Y);
	WriteString(rcpath, "bbStyle.placement:", position.placement);
	set_window_modes();
}

//===========================================================================
// helper

char getTitleText(bool Text)
{
	char a1[MAX_PATH], a2[MAX_PATH];
	strcpy(stylepath, stylePath());
	strcpy(a1, ReadString(stylepath, "style.name:", (char *)""));
	strcpy(a2, ReadString(stylepath, "style.author:", (char *)""));
	if (strlen(a1) <2)
	{
		//use the filename if  "style.name:" is empty
		WIN32_FIND_DATA FindFileData;
		HANDLE hFind;
		hFind = FindFirstFile(stylepath, &FindFileData);
		strcpy(a1, FindFileData.cFileName);
	}

	if (strlen(a2) <2) strcpy(a2, "[unsigned]");
	//remove e-mail|website address - if in style.author
	//e-mail address
	if(!trim_address(a2, '@',  ' '))
		//website address
		trim_address(a2, ':',  ' ');
	//else
	//lyrae et al.
	trim_address(a2, '|',  ' ');

	//remove file extension - if in style.name
	*(char*)file_extension(a1) = 0;

	if (!Text)
		if (strlen(a1) > (strlen(a2) + 3))
			return (char)strlen(a1);
		else
			//padding to deal with font proportionality
			return (char)(strlen(a2) + 8);
	else
		return sprintf(plugin.windowText,"%s\nby %s", a1, a2);
}

char *set_stylePath()
{
	//xoblite style path
	if (is_xoblite && (strlen(ReadString(extensionsrcPath(), "xoblite.stylesFolder:", (char *)"no")) > 2))
	{
		strcpy(plugin.stylePath, ReadString(extensionsrcPath(), "xoblite.stylesFolder:", (char *)"no"));
	if (plugin.stylePath[strlen(plugin.stylePath)-1] != '\\')
		strcat(plugin.stylePath, "\\");
	}
	else
		get_path(plugin.stylePath, sizeof(plugin.stylePath), "styles\\");

	WriteString(rcpath, "bbStyle.stylePath:", plugin.stylePath);
	return (char *)plugin.stylePath;
}

void initList(char *path, bool init)
{
	WIN32_FIND_DATA FindFileData;
	HANDLE hFind;
	char buf[MAX_PATH];

	if (init)
	{
		styleList.clear();

		if (listPath[strlen(listPath)-1] != '*')
		{
			if (listPath[strlen(listPath)-1] == '\\')
				strcat(listPath, "*");
			else
				strcat(listPath, "\\*");
		}
	}

	hFind = FindFirstFile(path, &FindFileData);

	while (FindNextFile(hFind, &FindFileData) != 0)
	{
		// Skip hidden files... and 3dc stuff
		if ( strcmp(FindFileData.cFileName, ".") == 0 || strcmp(FindFileData.cFileName, "..") == 0
			|| (FindFileData.dwFileAttributes & FILE_ATTRIBUTE_HIDDEN)
			|| IsInString(FindFileData.cFileName, ".3dc")
			// also shortcuts (.lnk)
			|| IsInString(FindFileData.cFileName, ".lnk")  )
			continue;

		// skip files outside the typical size range
		if (((FindFileData.nFileSizeLow > 12288) && !(FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
			|| ((FindFileData.nFileSizeLow < 3072) && !(FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)))
			continue;/**/

		// if directory, recurse...
		if (FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			strcpy(buf, path);
			buf[strlen(buf)-1] = '\0';
			strcat(buf, FindFileData.cFileName);
			strcat(buf, "\\*");
			initList(buf, false);
		}
		else
		{
			strcpy(buf, path);
			buf[strlen(buf)-1] = '\0';
			strcat(buf, FindFileData.cFileName);
			styleList.push_back((std::string)buf);
		}
	}

	FindClose(hFind);
	
	if (styleList.size() < 1)
	{
		// reset styles path to default - as none found
		WriteString(rcpath, "bbStyle.stylePath:", (char *)"");
		set_stylePath();
	}
}

//===========================================================================

void setStyle(void)
{
	if (styleList.size() < 1)	// dont try to set a style if list empty
		return;

	int j;

	if (plugin.chance)
	{
		// seed randomness
		srand( (unsigned)time( NULL ) );

		//generate more randomness
		int i = (int)(10 * rand() / (RAND_MAX + 1.0));
		for (j = 0; j < i; j++) {
			rand();
		}

		//get style list number
		j = (int)(styleList.size() * rand() / (RAND_MAX + 1.0));
	}
	else
	{
		j = plugin.count >= (styleList.size() - 1) ? 0 : plugin.count + 1;
		WriteInt(rcpath, "bbStyle.count:", j);
	}

	// get new style's path
	strcpy(plugin.styleToSet, styleList[j].c_str());

	if (strlen(ReadString(plugin.styleToSet, "menu.plugin.appearance:", (char *)"")) || strlen(ReadString(plugin.styleToSet, "menu.frame:", (char *)"")) || strlen(ReadString(plugin.styleToSet, "menu.items.bg:", (char *)"")))
	{
		// Set new style...
		SendMessage(BBhwnd, BB_SETSTYLE, 0, (LPARAM)plugin.styleToSet);

		if (plugin.timerOn)
			// Force randomisation by uneven # of seconds
			SetTimer(plugin.hwnd, 1, plugin.changeTime*59000, (TIMERPROC)NULL);
	}
}

//===========================================================================
