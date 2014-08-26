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

#include "BBAnalogExMod.h"

char *plugin_info[] =
{
	"BBAnalogExMod 0.0.7",
	"BBAnalogExMod",
	"0.0.7",
	"Mortar|Theo|ysuke|John",
	"2009-01-01",
	"http://bb4win.sourceforge.net/",
	"irc://irc.freenode.net/bb4win",
	"",
	"http://www.lostinthebox.com/viewforum.php?f=50&t=2998"
};

#define szVersion       plugin_info[0]
#define szAppName       plugin_info[1]
#define szInfoVersion	plugin_info[2]
#define szInfoAuthor	plugin_info[3]
#define szInfoRelDate	plugin_info[4]
#define szInfoLink	plugin_info[5]
#define szInfoEmail	plugin_info[6]
#define szInfoUpdateURL	plugin_info[8]

BOOL ( WINAPI *pSetLayeredWindowAttributes ) ( HWND, DWORD, BYTE, DWORD );
BOOL ( WINAPI *pUpdateLayeredWindow ) ( HWND, HDC, POINT*, SIZE*, HDC, POINT*, COLORREF, BLENDFUNCTION*, DWORD );

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
		SendMessage(hSlit, SLIT_ADD, NULL, (LPARAM)hwndBBAnalogExMod);
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
	ZeroMemory(&osvinfo, sizeof(osvinfo));
	osvinfo.dwOSVersionInfoSize = sizeof (osvinfo);
	GetVersionEx(&osvinfo);
	usingWin2kXP =
		osvinfo.dwPlatformId == VER_PLATFORM_WIN32_NT
		&& osvinfo.dwMajorVersion > 4;

	if(usingWin2kXP){
	HMODULE hDll = LoadLibrary("user32");
	pSetLayeredWindowAttributes = (BOOL (WINAPI *)(HWND, DWORD, BYTE, DWORD))
			GetProcAddress(hDll, "SetLayeredWindowAttributes");

	pUpdateLayeredWindow = (BOOL (WINAPI *)(HWND, HDC, POINT*, SIZE*, HDC, POINT*, COLORREF, BLENDFUNCTION*, DWORD))
			GetProcAddress(hDll, "UpdateLayeredWindow");
	FreeLibrary(hDll);
        }

	WNDCLASS wc;
	hwndBlackbox = GetBBWnd();
	hInstance = hPluginInstance;

	// Register the window class...
	ZeroMemory(&wc,sizeof(wc));
	wc.lpfnWndProc = WndProc;		// our window procedure
	wc.hInstance = hPluginInstance;		// hInstance of .dll
	wc.lpszClassName = szAppName;		// our window class name
	if (!RegisterClass(&wc)) 
	{
		MessageBox(hwndBlackbox, "Error registering window class", szAppName, MB_OK | MB_ICONERROR | MB_TOPMOST);
		return 1;
	}

	// Initialize GDI+.
	if(Gdiplus::GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL) != 0)
	{
		UnregisterClass(szAppName, hPluginInstance);
		MessageBox(0, "Error starting GdiPlus.dll", szAppName, MB_OK | MB_ICONERROR | MB_TOPMOST);
		return 1;
	}

	// Get plugin and style settings...
	ReadRCSettings();
	//initialize the plugin before getting style settings
	InitBBAnalog();
	GetStyleSettings();

	// Create the window...
	hwndBBAnalogExMod = CreateWindowEx(
						WS_EX_TOOLWINDOW,								// window style
						szAppName,										// our window class name
						NULL,											// NULL -> does not show up in task manager!
						WS_POPUP | WS_CLIPCHILDREN | WS_CLIPSIBLINGS,	// window parameters
						xpos,											// x position
						ypos,											// y position
						size,											// window width
						size,											// window height
						NULL,											// parent window
						NULL,											// no menu
						hPluginInstance,								// hInstance of .dll
						NULL);
	if (!hwndBBAnalogExMod)
	{
		UnregisterClass(szAppName, hPluginInstance);
		Gdiplus::GdiplusShutdown(gdiplusToken);
		MessageBox(0, "Error creating window", szAppName, MB_OK | MB_ICONERROR | MB_TOPMOST);
		return 1;
	}

	//set static gdi+ object pointers
	WCHAR wTitle[256];
	if (strcmp((char*)imagePath, "") != 0){ 
	mbstowcs(wTitle, imagePath, strlen(imagePath)+1);
	myImage = new Gdiplus::Image(wTitle);
	}

	bitmap = new Gdiplus::Bitmap(1, 1, PixelFormat32bppARGB);
	pen = new Gdiplus::Pen(plusFontColor[0]);
	fontFamily = new Gdiplus::FontFamily(L"Times New Roman");
	format = new Gdiplus::StringFormat;
	myFormat = new Gdiplus::StringFormat;

	//Start the plugin timer
	if(showAlarm <= 0)
	{
		if(showSeconds) 
		{
			if (sweepHands) mySetTimer(2);
			else mySetTimer(0);
		}
		else
		{
			if (sweepHands) mySetTimer(0);
			else mySetTimer(1);
		}
	}

	// Register to receive Blackbox messages...
	SendMessage(hwndBlackbox, BB_REGISTERMESSAGE, (WPARAM)hwndBBAnalogExMod, (LPARAM)msgs);
	// Make the window sticky
	MakeSticky(hwndBBAnalogExMod);

	// Show the window and force it to update...
	setRegion();

	ShowWindow(hwndBBAnalogExMod, SW_SHOW);
	setAttr = true;
	InvalidateRect(hwndBBAnalogExMod, NULL, true);
	return 0;
}

//===========================================================================
//This function is used once in beginPlugin and in @BBAnalogExReloadSettings def. found in WndProc.
//Do not initialize objects here.  Deal with them in beginPlugin and endPlugin

void InitBBAnalog()
{
	//set up center points and dimensions of clock
	//check for even size(0 is starting point from edges for x, y). fix for exact center point.
	//Have to do it this way since width is not a float and any operation would trim it.  darn types.
	if((size % 2) == 0) cntX = cntY = (float)((size / 2) - .5);
	else cntX = cntY = (float)((size - 1) / 2);

	//Init display mode counters
	showDate = 0;
	showAlarm = 0;
	circleCounter = 0;
	oneMoreDraw = false;

	//getCurrentDate();
	getCurrentTime();
	//get screen dimensions
	ScreenWidth = GetSystemMetrics(SM_CXVIRTUALSCREEN);
	ScreenHeight = GetSystemMetrics(SM_CYVIRTUALSCREEN);
}

//===========================================================================

void endPlugin(HINSTANCE hPluginInstance)
{
	if(graphics) delete graphics;
	if(myImage) delete myImage;
	if(bitmap) delete bitmap;
	if(pen) delete pen;
	if(fontFamily) delete fontFamily;
	if(format) delete format;
	if(brush) delete brush;
	if(layoutRect) delete layoutRect;
	if(plusFontColor) delete plusFontColor;
	
	//shutdown the gdi+ engine
	Gdiplus::GdiplusShutdown(gdiplusToken);
	// Release our timer resources
	KillTimer(hwndBBAnalogExMod, IDT_TIMER);
	// Write the current plugin settings to the config file...
	WriteRCSettings();
	// Delete the main plugin menu if it exists (PLEASE NOTE that this takes care of submenus as well!)
	if (myMenu){ DelMenu(myMenu); myMenu = NULL;}
	// Make the window unsticky
	RemoveSticky(hwndBBAnalogExMod);
	// Unregister Blackbox messages...
	SendMessage(hwndBlackbox, BB_UNREGISTERMESSAGE, (WPARAM)hwndBBAnalogExMod, (LPARAM)msgs);
	if(inSlit && hSlit)
		SendMessage(hSlit, SLIT_REMOVE, NULL, (LPARAM)hwndBBAnalogExMod);
	// Destroy our window...
	DestroyWindow(hwndBBAnalogExMod);
	// Unregister window class...
	UnregisterClass(szAppName, hPluginInstance);
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
	switch (message)
	{

		case WM_DROPFILES:
			OnDropFiles((HDROP)wParam);
		break;

		// Window update process...
		case WM_PAINT:
		{
			// Create buffer hdc's, bitmaps etc.
			PAINTSTRUCT ps;  RECT r;
			int gap = bevelWidth + borderWidth;

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


			if(brush) delete brush;
			helpColor = new Gdiplus::Color(GetRValue(numbColor), GetGValue(numbColor), GetBValue(numbColor));

			brush = new Gdiplus::SolidBrush(helpColor[0]);
			delete helpColor;

			if(inSlit || !transBack) MakeStyleGradient(hdc, &r, &myStyleItem, drawBorder);

			if (showDate > 0)
			{
				StyleItem * TB = (StyleItem *)GetSettingPtr(SN_TOOLBAR);
				//Create font and draw some text
				HGDIOBJ otherfont = SelectObject(hdc,
								CreateFont(fontSize,
								0, 0, 0, FW_NORMAL,
								false, false, false,
								DEFAULT_CHARSET,
								OUT_DEFAULT_PRECIS,
								CLIP_DEFAULT_PRECIS,
								DEFAULT_QUALITY,
								DEFAULT_PITCH, TB->Font));

				time(&systemTime);
				localTime = localtime(&systemTime);

				strftime(currentDate, 10, "%a", localTime);
				DrawText(hdc, currentDate, (int)strlen(currentDate), &r, DT_TOP | DT_CENTER | DT_SINGLELINE);
				
				strftime(currentDate, 10, "%#d", localTime);
				DrawText(hdc, currentDate, (int)strlen(currentDate), &r, DT_VCENTER | DT_CENTER | DT_SINGLELINE);

				strftime(currentDate, 10,"%b", localTime);
				DrawText(hdc, currentDate, (int)strlen(currentDate), &r, DT_BOTTOM | DT_CENTER | DT_SINGLELINE);

				//gdi:  delete object when your done
				//delete font object because I'm done with it.
				DeleteObject(SelectObject(hdc, otherfont));

				//Paint to the screen
				BitBlt(hdc_scrn, 0, 0, size, size, hdc, 0, 0, SRCCOPY);

				//Count down 3 seconds so the date can be read.
				showDate--;
				//if we are done showing the date, reset the timer back to where it was.
				if(showDate <= 0)
				{
					if(showAlarm <= 0)
					{
						//reset timer
						if(showSeconds)
						{
							if (sweepHands) mySetTimer(2);
							else mySetTimer(0);
						}
						else
						{
							if (sweepHands) mySetTimer(0);
							else mySetTimer(1);
						}
					}
					showDate = 0; //just to make sure
					getCurrentTime();
					//redraw the window after all objects are deleted
					oneMoreDraw = true;
				}
			}
			else
			{
				//upgraded to gdi+ graphics lib
				graphics = new Gdiplus::Graphics(hdc);
				format->SetAlignment(Gdiplus::StringAlignmentCenter);
				graphics->SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);
				graphics->SetCompositingMode(Gdiplus::CompositingModeSourceOver);
				pen->SetWidth(1);


				//---------------------
				//draw image

				if(myImage) graphics->DrawImage(myImage, 0, 0, size, size);

				//---------------------
				//analog clock is drawnd begin

				helpColor = new Gdiplus::Color(GetRValue(numbColor), GetGValue(numbColor), GetBValue(numbColor));

				pen->SetColor(helpColor[0]);
				delete helpColor;

				if(showCircle) graphics->DrawEllipse(pen,
                                                                     (REAL)cntX - radius,
                                                                     (REAL)cntY - radius,
                                                                     (REAL)(2 * radius),
                                                                     (REAL)(2 * radius));

				//draw pulsing circle for silent alarm.  draw first so it is on bottom.
				if(showAlarm > 0)
				{
					graphics->DrawEllipse(pen, (REAL)(cntX - circleCounter),
										(REAL)(cntY - circleCounter),
										(REAL)(2 * circleCounter),
										(REAL)(2 * circleCounter));

					if(circleCounter == 1 && synchronize && (strcmp(playAlarm, ".none") != 0))
						myPlayWav(playAlarm);
					circleCounter++;
					if(circleCounter >= radius)
					{
						circleCounter = 0;
						showAlarm--;
					}
					if(showAlarm <= 0)
					{
						circleCounter = 0;
						showAlarm = 0;
						//reset timer
						if(showSeconds)
						{
							if (sweepHands) mySetTimer(2);
							else mySetTimer(0);
						}
						else
						{
							if (sweepHands) mySetTimer(0);
							else mySetTimer(1);
						}
						//don't getCurrentTime() so the timer doesn't get reset again
						//getCurrentTime();
						//redraw the window after all objects are deleted
						oneMoreDraw = true;
					}
				}

				//---------------------

				//Draw tics(1-12) on the clock face.  12, 3, 6, 9 are 2x2 pixels, the rest are 1x1 pixel.
				//REAL are floats.  Use a cast to REAL to trim the decimal point from doubles.
				GetClientRect(hwndBBAnalogExMod,&rect);

				rect.left = rect.left + gap;
				rect.top = rect.top + gap;
				rect.bottom = (rect.bottom - gap);
				rect.right = (rect.right - gap);

				layoutRect = new Gdiplus::RectF((float)(rect.left), (float)(rect.top), (float)(rect.right), (float)(rect.bottom));

				for(theta = 0; theta < 2 * PI; theta = theta + (PI / 6))
				{
					if(theta == 0 || theta == PI / 2 || theta == PI || theta == (3 * PI) / 2)
					{
						if(showRomans) //draw roman numerals for the 12, 3, 6, 9 hands
						{
							WCHAR string[4];

							//do some finer adjustments because of different pixel sizes of each string
							//changed the way we adjust the text position - - Theo
							if(theta == 00)			{wcscpy(string, L"VI"); myFormat->SetAlignment(Gdiplus::StringAlignmentCenter); myFormat->SetLineAlignment(Gdiplus::StringAlignmentFar); }
							else if(theta == PI / 2){wcscpy(string, L"XII");myFormat->SetAlignment(Gdiplus::StringAlignmentCenter); myFormat->SetLineAlignment(Gdiplus::StringAlignmentNear); }
							else if(theta == PI)	{wcscpy(string, L"III");myFormat->SetAlignment(Gdiplus::StringAlignmentFar); myFormat->SetLineAlignment(Gdiplus::StringAlignmentCenter); }
							else					{wcscpy(string, L"IX");	myFormat->SetAlignment(Gdiplus::StringAlignmentNear); myFormat->SetLineAlignment(Gdiplus::StringAlignmentCenter); }

							font = new Gdiplus::Font(fontFamily, (float)fontSizeC, Gdiplus::FontStyleBold, Gdiplus::UnitPixel);
							graphics->DrawString(string, (INT)wcslen(string), font, *layoutRect, myFormat, brush);
							delete font;
						}
						else if(showNormal) //draw normal numerals for the 12, 3, 6, 9 hands
						{
							WCHAR string[4];
							if(theta == 00)			{wcscpy(string, L"6"); myFormat->SetAlignment(Gdiplus::StringAlignmentCenter); myFormat->SetLineAlignment(Gdiplus::StringAlignmentFar); }
							else if(theta == PI / 2){wcscpy(string, L"12");myFormat->SetAlignment(Gdiplus::StringAlignmentCenter); myFormat->SetLineAlignment(Gdiplus::StringAlignmentNear); }
							else if(theta == PI)	{wcscpy(string, L"3");	myFormat->SetAlignment(Gdiplus::StringAlignmentFar); myFormat->SetLineAlignment(Gdiplus::StringAlignmentCenter); }
							else					{wcscpy(string, L"9");	myFormat->SetAlignment(Gdiplus::StringAlignmentNear); myFormat->SetLineAlignment(Gdiplus::StringAlignmentCenter); }

							font = new Gdiplus::Font(fontFamily, (float)fontSizeC, Gdiplus::FontStyleBold, Gdiplus::UnitPixel);
							graphics->DrawString(string, (INT)wcslen(string), font, *layoutRect, myFormat, brush);
							delete font;	}
						else if(showLargeTics) //draw large tics
						{
							pen->SetWidth(2);
							graphics->DrawLine(pen,
									(REAL)(cntX + ((radius - 3) * cos(theta))),
									(REAL)(cntY - ((radius - 3) * sin(theta))),
									(REAL)(cntX + ((radius - 1) * cos(theta))),
									(REAL)(cntY - ((radius - 1) * sin(theta))));
						}
					}
					else if(showSmallTics) //draw small tics
					{
						if((showRomans)||(showNormal))
						{
							pen->SetWidth(2);
							graphics->DrawLine(pen,
											(REAL)(cntX + ((radius - 3) * cos(theta))),
											(REAL)(cntY - ((radius - 3) * sin(theta))),
											(REAL)(cntX + ((radius - 1) * cos(theta))),
											(REAL)(cntY - ((radius - 1) * sin(theta))));
						}
						else
						{
							pen->SetWidth(1);
							graphics->DrawLine(pen,
											(REAL)(cntX + ((radius - 2) * cos(theta))),
											(REAL)(cntY - ((radius - 2) * sin(theta))),
											(REAL)(cntX + ((radius - 1) * cos(theta))),
											(REAL)(cntY - ((radius - 1) * sin(theta))));
						}
						
					}

				}


				//---------------------


				pen->SetWidth((float)minuteHandWidth);

				//check for color
				helpColor = new Gdiplus::Color(GetRValue(minuteHandColor), GetGValue(minuteHandColor), GetBValue(minuteHandColor));
				pen->SetColor(helpColor[0]);
				delete helpColor;

				//----
				//Draw the minute hand, always 2 less than radius
				if (sweepHands) 
				{
  					theta = (15 - (currentMinute + ((float)currentSecond/60.0))) * (PI / 30);
				}
				else
				{
  					theta = (15 - currentMinute) * (PI / 30);
				}

				graphics->DrawLine(pen, (REAL)(cntX + (1.5 * cos(theta))),
					(REAL)(cntY - (1.5 * sin(theta))),
					(REAL)(cntX + ((radius * minuteHandLength/100) * cos(theta))),
					(REAL)(cntY - ((radius * minuteHandLength/100) * sin(theta))));
				//----

				//check for color
				pen->SetWidth((float)hourHandWidth);
				helpColor = new Gdiplus::Color(GetRValue(hourHandColor), GetGValue(hourHandColor), GetBValue(hourHandColor));
				pen->SetColor(helpColor[0]);
				delete helpColor;

				//Draw center circle.  Gives a nice pivot looking point. semi-hollow.
				graphics->DrawEllipse(pen, (REAL)(cntX - 1.5), (REAL)(cntY - 1.5), (REAL)3, (REAL)3);

				//----
				//Draw the hour hand 65% of the radius
				theta = (3 - currentHour) * (PI / 6);

				graphics->DrawLine(pen, (REAL)(cntX + (1.5 * cos(theta))),
					(REAL)(cntY - (1.5 * sin(theta))),
					(REAL)(cntX + ((radius * hourHandLength/100) * cos(theta))),
					(REAL)(cntY - ((radius * hourHandLength/100) * sin(theta))));

				//----

				//Draw the second hand, always 1 less than the radius, whose tail is 25% of the radius
				if(showSeconds)
				{
					pen->SetWidth((float)secondHandWidth);

					//check for color
					helpColor = new Gdiplus::Color(GetRValue(secondHandColor), GetGValue(secondHandColor), GetBValue(secondHandColor));
					pen->SetColor(helpColor[0]);
					delete helpColor;

					//draw in a little circle so the seconds hand looks attached
					graphics->DrawEllipse(pen, (REAL)(cntX - secondHandWidth/2),
                                                              (REAL)(cntY - secondHandWidth/2),
                                                              (REAL)secondHandWidth,
                                                              (REAL)secondHandWidth);

					if (sweepHands)
					{
						theta = (15 - (currentSecond + ((float)currentMillisecond/1000.0))) * (PI / 30);
					}
					else
					{
  						theta = (15 - currentSecond) * (PI / 30);
					}
					graphics->DrawLine(pen, (REAL)(cntX - ((radius * .25) * cos(theta))),
					                                                 (REAL)(cntY + ((radius * .25) * sin(theta))),
                                                           (REAL)(cntX + ((radius * secondHandLength/100) * cos(theta))),
                                                           (REAL)(cntY - ((radius * secondHandLength/100) * sin(theta))));
				}
				 
				//------
				//analog end

				if(usingWin2kXP){
				// Set an attribute of the window
				if(setAttr){
					if(!inSlit){
						SetWindowLong(hwnd, GWL_EXSTYLE,
								GetWindowLong(hwnd, GWL_EXSTYLE) & ~WS_EX_LAYERED);

						SetWindowLong(hwnd, GWL_EXSTYLE ,
								GetWindowLong(hwnd, GWL_EXSTYLE) | WS_EX_LAYERED);
					}
					DragAcceptFiles(hwnd, true);
					if(!inSlit) SetWindowPos( hwnd,
								alwaysOnTop ? HWND_TOPMOST : HWND_NOTOPMOST,
								0, 0, 0, 0,
								SWP_NOACTIVATE | SWP_NOSIZE | SWP_NOMOVE);
					setAttr = false;
				}

				// Set the transparency mode
				if(!inSlit && transBack)
				{
					RECT rt;
					GetWindowRect(hwnd, &rt);

					BLENDFUNCTION bf;
					bf.BlendOp = AC_SRC_OVER;
					bf.BlendFlags = 0;
					bf.AlphaFormat = AC_SRC_ALPHA;
					bf.SourceConstantAlpha = alpha;

					POINT ptWnd = {rt.left, rt.top};
					POINT ptSrc = {0, 0};
					SIZE  szWnd = {size, size};

					pUpdateLayeredWindow(hwnd, NULL, NULL, &szWnd,
                                                     hdc, &ptSrc, 0, &bf, ULW_ALPHA);
				}
				else if(!inSlit)
				{
					pSetLayeredWindowAttributes(hwnd, NULL,
							(transparency) ? (unsigned char)alpha : 255, LWA_ALPHA);
				}

				}

				//gdi:  delete objects when your done
				if(graphics){
					delete graphics;
					graphics = NULL;
				}

				//Paint to the screen
				BitBlt(hdc_scrn, 0, 0, size, size, hdc, 0, 0, SRCCOPY);
			}

			// Remember to delete all objects!
			DeleteObject(SelectObject(hdc, otherbmp));
			DeleteDC(hdc);

			EndPaint(hwnd, &ps);

			if(oneMoreDraw)
			{
				oneMoreDraw = false;
				InvalidateRect(hwndBBAnalogExMod, NULL, true);
			}
			return 0;
		}
		break;

		// ==========

		// If Blackbox sends a reconfigure message, load the new style settings and update the window...
		case BB_RECONFIGURE:
		{
			if(myMenu){ DelMenu(myMenu); myMenu = NULL;}
			GetStyleSettings();
			getCurrentTime();
			InvalidateRect(hwndBBAnalogExMod, NULL, true);
		}
		break;

		// ==========

		// Broadcast messages (bro@m -> the bang killah! :D <vbg>)
		case BB_BROADCAST:
		{
			szTemp = (char*)lParam;

			if (!_stricmp(szTemp, "@BBShowPlugins") && !inSlit)
			{
				// Show window and force update...
				ShowWindow( hwndBBAnalogExMod, SW_SHOW);
				getCurrentTime();
				showAlarm = 0;
 
				if(showSeconds)
				{
					if (sweepHands) mySetTimer(2);
					else mySetTimer(0);
				}
				else
				{
					if (sweepHands) mySetTimer(0);
					else mySetTimer(1);
				}
				InvalidateRect( hwndBBAnalogExMod, NULL, true);
			}
			else if (!_stricmp(szTemp, "@BBHidePlugins") && pluginToggle && !inSlit)
			{
				// Hide window...
				ShowWindow( hwndBBAnalogExMod, SW_HIDE);
			}

			//===================

			if (_strnicmp(szTemp, "@BBAnalogExMod", 14))
				return 0;
			szTemp += 14;

			if (!_stricmp(szTemp, "About"))
			{
				char tmp_str[MAX_PATH];
				SendMessage(hwndBlackbox, BB_HIDEMENU, 0, 0);
				sprintf(tmp_str, "%s", szVersion);
				MessageBox(hwndBlackbox, tmp_str, szAppName, MB_OK | MB_TOPMOST);
			}
			else if (!_stricmp(szTemp, "TestTimeAlarm"))
			{
				//only if it is not running.  Always check showAlarm before using mySetTimer
				if(showAlarm <= 0)
				{
					if(synchronize && (StrStrI(timeAlarmWav, ".wav") != NULL))
						strcpy(playAlarm, timeAlarmWav);
					else
					{
						myPlayWav(timeAlarmWav);
						strcpy(playAlarm, ".none");
					}

					showAlarm = timeAlarmFrequency;
					mySetTimer(2);
				}
				ShowMyMenu(false);
			}
			else if (!_stricmp(szTemp, "TestHourAlarm"))
			{
				//only if it is not running.  Always check showAlarm before using mySetTimer
				if(showAlarm <= 0)
				{
					if(synchronize && (StrStrI(hourAlarmWav, ".wav") != NULL))
						strcpy(playAlarm, hourAlarmWav);
					else
					{
						myPlayWav(hourAlarmWav);
						strcpy(playAlarm, ".none");
					}

					if(magicHourFreq)
						showAlarm = (int)currentHour;
					else
						showAlarm = hourAlarmFrequency;
					mySetTimer(2);
				}
				ShowMyMenu(false);
			}
			else if (!_stricmp(szTemp, "TestHalfHourAlarm"))
			{
				//only if it is not running.  Always check showAlarm before using mySetTimer
				if(showAlarm <= 0)
				{
					if(synchronize && (StrStrI(halfHourAlarmWav, ".wav") != NULL))
						strcpy(playAlarm, halfHourAlarmWav);
					else
					{
						myPlayWav(halfHourAlarmWav);
						strcpy(playAlarm, ".none");
					}

					showAlarm = halfHourAlarmFrequency;
					mySetTimer(2);
				}
				ShowMyMenu(false);
			}
			else if (!_stricmp(szTemp, "PluginToggle"))
			{
                                pluginToggle = (pluginToggle ? false : true);
				ShowMyMenu(false);
			}
			else if (!_stricmp(szTemp, "OnTop"))
			{
				alwaysOnTop = (alwaysOnTop ? false : true);
				if(!inSlit)SetWindowPos( hwndBBAnalogExMod,
                                                         alwaysOnTop ? HWND_TOPMOST : HWND_NOTOPMOST,
                                                         0, 0, 0, 0,
                                                         SWP_NOACTIVATE | SWP_NOSIZE | SWP_NOMOVE);

				ShowMyMenu(false);
			}
			else if (!_stricmp(szTemp, "InSlit") && showAlarm == 0)
			{
				if(inSlit && hSlit)
				{
					// We are in the slit, so lets unload and get out..
					SendMessage(hSlit, SLIT_REMOVE, NULL, (LPARAM)hwndBBAnalogExMod);

					// Here you can move to where ever you want ;)
					SetWindowPos(hwndBBAnalogExMod, NULL, xpos, ypos, 0, 0, SWP_NOZORDER | SWP_NOSIZE);
					inSlit = false;
				}
				/* Make sure before you try and load into the slit that you have
				* the HWND of the slit ;)
				*/
				else if(hSlit)
				{
					// (Back) into the slit..
					inSlit = true;
					SetWindowLong(hwndBBAnalogExMod, GWL_EXSTYLE,
                                                      GetWindowLong(hwndBBAnalogExMod, GWL_EXSTYLE) & ~WS_EX_LAYERED);
					SendMessage(hSlit, SLIT_ADD, NULL, (LPARAM)hwndBBAnalogExMod);
				}
				GetStyleSettings();
				//update window
				getCurrentTime();
				setRegion();
				setAttr = true;
				InvalidateRect(hwndBBAnalogExMod, NULL, true);
				ShowMyMenu(false);
			}
			else if (!_stricmp(szTemp, "Transparency"))
			{
				transparency = (transparency ? false : true);
				setAttr = true;
				InvalidateRect(hwndBBAnalogExMod, NULL, true);
				ShowMyMenu(false);
			}
			else if (!_stricmp(szTemp, "TransBack"))
			{
				transBack = (transBack ? false : true);
				setAttr = true;
				InvalidateRect(hwndBBAnalogExMod, NULL, true);
				ShowMyMenu(false);
			}
			else if (!_strnicmp(szTemp, "AlphaValue", 10))
			{
				alpha = atoi(szTemp + 11);
				if (transBack || transparency){
					InvalidateRect(hwndBBAnalogExMod, NULL, true);
                                }
			}
			else if (!_stricmp(szTemp, "SnapToEdge"))
			{
				snapWindow = (snapWindow ? false : true);
				ShowMyMenu(false);
			}
			else if (!_stricmp(szTemp, "LockPosition"))
			{
				lockp = (lockp ? false : true);
				ShowMyMenu(false);
			}
			else if (!_stricmp(szTemp, "Seconds"))
			{
				if(showAlarm <= 0)
				{
					// Set the showSeconds attributes to the clock
					if (showSeconds)
					{
						showSeconds = false;
						if (sweepHands)
						{
							//reset timer to seconds
  							mySetTimer(0);
						}
						else
						{
  							//reset timer to minutes
  							mySetTimer(1);
						}

                                        }
                                        else
                                        {
						showSeconds = true;
						if (sweepHands)
						{
							//reset timer to subseconds
  							mySetTimer(2);
						}
						else
						{
							//reset timer to seconds
							mySetTimer(0);
						}
					}
					getCurrentTime();
					InvalidateRect(hwndBBAnalogExMod, NULL, true);
				}
				ShowMyMenu(false);
			}
			else if (!_stricmp(szTemp, "Sweep"))
			{
				if(showAlarm <= 0)
				{
					// Set the showSeconds attributes to the clock
					if (sweepHands)
					{
						sweepHands = false;
						if (showSeconds)
						{
							//reset timer to seconds
  							mySetTimer(0);
						}
						else
						{
							//reset timer to minutes
							mySetTimer(1);
						}
					}
					else
					{
						sweepHands = true;
						if (showSeconds) //reset timer to subseconds
						{
							mySetTimer(2);
						}
						else
						{
							mySetTimer(0);  //reset timer to seconds
						}
					}
					getCurrentTime();
					InvalidateRect(hwndBBAnalogExMod, NULL, true);
				}
				ShowMyMenu(false);
			}
			else if (!_stricmp(szTemp, "RomanNumerals"))
			{
				// Set the showRomans attributes to the clock
				if (showRomans)
					 showRomans = false;
				else
					{showRomans = true; showNormal = false;}
				getCurrentTime();
				InvalidateRect(hwndBBAnalogExMod, NULL, true);
				ShowMyMenu(false);
			}
			else if (!_stricmp(szTemp, "NormalNumerals"))
			{
				// Set the showNormal attributes to the clock
				if (showNormal)
					 showNormal = false;
				else
					{showNormal = true; showRomans = false;}
				getCurrentTime();
				InvalidateRect(hwndBBAnalogExMod, NULL, true);
				ShowMyMenu(false);
			}
			
			else if (!_stricmp(szTemp, "Circle"))
			{
				showCircle = (showCircle ? false : true);
				getCurrentTime();
				InvalidateRect(hwndBBAnalogExMod, NULL, true);
				ShowMyMenu(false);
			}

			else if (!_stricmp(szTemp, "LargeTics"))
			{
				showLargeTics = (showLargeTics ? false : true);
				getCurrentTime();
				InvalidateRect(hwndBBAnalogExMod, NULL, true);
				ShowMyMenu(false);
			}

			else if (!_stricmp(szTemp, "SmallTics"))
			{
				showSmallTics = (showSmallTics ? false : true);
				getCurrentTime();
				InvalidateRect(hwndBBAnalogExMod, NULL, true);
				ShowMyMenu(false);
			}
			else if (!_stricmp(szTemp, "HourAlarm"))
			{
				hourAlarm = (hourAlarm ? false : true);
				ShowMyMenu(false);
			}
			else if (!_stricmp(szTemp, "HalfHourAlarm"))
			{
				halfHourAlarm = (halfHourAlarm ? false : true);
				ShowMyMenu(false);
			}
			else if (!_stricmp(szTemp, "Synchronize"))
			{
				synchronize = (synchronize ? false : true);
				ShowMyMenu(false);
			}

			else if (!_stricmp(szTemp, "DrawBorder"))
			{
				drawBorder = (drawBorder ? false : true);
				InvalidateRect(hwndBBAnalogExMod, NULL, true);
				ShowMyMenu(false);
			}
			else if (!_strnicmp(szTemp, "StyleType", 9))
			{
				styleType = atoi(szTemp + 10);
				GetStyleSettings();
				InvalidateRect(hwndBBAnalogExMod, NULL, true);
				ShowMyMenu(false);
			}
			else if (!_stricmp(szTemp, "EditRC"))
			{
				char path[MAX_PATH];
				GetBlackboxEditor(path);
				BBExecute(NULL, NULL, path, rcpath, NULL, SW_SHOWNORMAL, false);
			}
			else if (!_stricmp(szTemp, "ReloadSettings"))
			{
				if(showAlarm <= 0)
				{
					//remove from slit before resetting window attributes
					if(inSlit && hSlit)
						SendMessage(hSlit, SLIT_REMOVE, NULL, (LPARAM)hwndBBAnalogExMod);

					//Re-initialize
					ReadRCSettings();
					InitBBAnalog();
					inSlit = wantInSlit;
					GetStyleSettings();
					//reset timer
					if(showSeconds)
					{
						if (sweepHands) mySetTimer(2);
						else mySetTimer(0);
					}
					else
					{
						if (sweepHands) mySetTimer(0);
						else mySetTimer(1);
					}

					if(inSlit && hSlit)
						SendMessage(hSlit, SLIT_ADD, NULL, (LPARAM)hwndBBAnalogExMod);
					else inSlit = false;

					//update window
					getCurrentTime();
					setRegion();
					InvalidateRect(hwndBBAnalogExMod, NULL, true);
				}
			}
			else if (!_stricmp(szTemp, "SaveSettings"))
			{
				WriteRCSettings();
			}

			//===================

			else if (!_strnicmp(szTemp, "Region", 6))
			{
				region = atoi(szTemp + 7);
				setRegion();
				InvalidateRect(hwndBBAnalogExMod, NULL, true);
				ShowMyMenu(false);
			}
			else if (!_strnicmp(szTemp, "Outside", 7))
			{
				outsideCircle = atoi(szTemp + 8);
				if(region != 0) setRegion();
				InvalidateRect(hwndBBAnalogExMod, NULL, true);
			}
			else if (!_strnicmp(szTemp, "Inside", 6))
			{
				insideCircle = atoi(szTemp + 7);
				if(region == 2) setRegion();
				InvalidateRect(hwndBBAnalogExMod, NULL, true);
			}

			//===================

			else if (!_stricmp(szTemp, "HoursHandColor"))
			{ //creating colorchose dialog for the clock parts...
				CHOOSECOLOR cc;
				COLORREF custCol[16];
				ZeroMemory(&cc, sizeof(CHOOSECOLOR));
				cc.lStructSize = sizeof(CHOOSECOLOR);
				cc.hwndOwner = NULL;
				cc.lpCustColors = (LPDWORD) custCol;
				cc.rgbResult = hourHandColor;
				cc.Flags = CC_RGBINIT;

				if (ChooseColor(&cc)) hourHandColor = cc.rgbResult;
				InvalidateRect(hwndBBAnalogExMod, NULL, true);
			}
			else if (!_stricmp(szTemp, "MinutesHandColor"))
			{
				CHOOSECOLOR cc;
				COLORREF custCol[16];
				ZeroMemory(&cc, sizeof(CHOOSECOLOR));
				cc.lStructSize = sizeof(CHOOSECOLOR);
				cc.hwndOwner = NULL;
				cc.lpCustColors = (LPDWORD) custCol;
				cc.rgbResult = minuteHandColor;
				cc.Flags = CC_RGBINIT;
				
				if (ChooseColor(&cc)) minuteHandColor = cc.rgbResult;
				InvalidateRect(hwndBBAnalogExMod, NULL, true);
			}
			else if (!_stricmp(szTemp, "SecondsHandColor"))
			{
				CHOOSECOLOR cc;
				COLORREF custCol[16];
				ZeroMemory(&cc, sizeof(CHOOSECOLOR));
				cc.lStructSize = sizeof(CHOOSECOLOR);
				cc.hwndOwner = NULL;
				cc.lpCustColors = (LPDWORD) custCol;
				cc.rgbResult = secondHandColor;
				cc.Flags = CC_RGBINIT;
				
				if (ChooseColor(&cc)) secondHandColor = cc.rgbResult;
				InvalidateRect(hwndBBAnalogExMod, NULL, true);
			}
			else if (!_stricmp(szTemp, "NumberColor"))
			{
				CHOOSECOLOR cc;
				COLORREF custCol[16];
				ZeroMemory(&cc, sizeof(CHOOSECOLOR));
				cc.lStructSize = sizeof(CHOOSECOLOR);
				cc.hwndOwner = NULL;
				cc.lpCustColors = (LPDWORD) custCol;
				cc.rgbResult = numbColor;
				cc.Flags = CC_RGBINIT;
				
				if (ChooseColor(&cc)) numbColor = cc.rgbResult;
				InvalidateRect(hwndBBAnalogExMod, NULL, true);
			}
			else if (!_stricmp(szTemp, "DFillColor"))
			{
				CHOOSECOLOR cc;
				COLORREF custCol[16];
				ZeroMemory(&cc, sizeof(CHOOSECOLOR));
				cc.lStructSize = sizeof(CHOOSECOLOR);
				cc.hwndOwner = NULL;
				cc.lpCustColors = (LPDWORD) custCol;
				cc.rgbResult = dFillColor;
				cc.Flags = CC_RGBINIT;
				
				if (ChooseColor(&cc)) dFillColor = cc.rgbResult;
				InvalidateRect(hwndBBAnalogExMod, NULL, true);
			}
			else if (!_stricmp(szTemp, "DLineColor"))
			{
				CHOOSECOLOR cc;
				COLORREF custCol[16];
				ZeroMemory(&cc, sizeof(CHOOSECOLOR));
				cc.lStructSize = sizeof(CHOOSECOLOR);
				cc.hwndOwner = NULL;
				cc.lpCustColors = (LPDWORD) custCol;
				cc.rgbResult = dLineColor;
				cc.Flags = CC_RGBINIT;
				
				if (ChooseColor(&cc)) dLineColor = cc.rgbResult;
				InvalidateRect(hwndBBAnalogExMod, NULL, true);
			}
			else if (!_stricmp(szTemp, "SetImage"))
			{
				    OPENFILENAME ofn;
				    memset( &ofn, 0, sizeof(OPENFILENAME) );
				    ofn.lStructSize= sizeof(OPENFILENAME);
				    ofn.lpstrFilter= "PNG(*.png)\0*.png\0BMP(*.bmp)\0*.bmp\0JPG(*.jpg)\0*.jpg\0GIF(*.gif)\0*.gif\0All Files\0*.*\0\0";
				    ofn.lpstrFile= imagePath;
				    ofn.nMaxFile= sizeof(imagePath);
				    ofn.Flags= OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;

				    if( GetOpenFileName(&ofn) == TRUE){
				    	    if(myImage) delete myImage;

				    	    WCHAR wTitle[256];
				    	    mbstowcs(wTitle, imagePath, strlen(imagePath)+1);
				    	    myImage = new Gdiplus::Image(wTitle);
				    	    InvalidateRect(hwndBBAnalogExMod, NULL, true);
				    }
			}
			else if (!_stricmp(szTemp, "RemoveImage"))
			{
				    if(myImage) delete myImage;
				    strcpy(imagePath, "");
				    WCHAR wTitle[256];
				    mbstowcs(wTitle, imagePath, strlen(imagePath)+1);
				    myImage = new Gdiplus::Image(wTitle);
				    InvalidateRect(hwndBBAnalogExMod, NULL, true);
			}
			//---
			else if (!_strnicmp(szTemp, "HourWidthSize", 13))
			{//sizes of the clock parts
				hourHandWidth = atoi(szTemp + 14);
				InvalidateRect(hwndBBAnalogExMod, NULL, true);
			}
			else if (!_strnicmp(szTemp, "MinuteWidthSize", 15))
			{
				minuteHandWidth = atoi(szTemp + 16);
				InvalidateRect(hwndBBAnalogExMod, NULL, true);
			}
			else if (!_strnicmp(szTemp, "SecondWidthSize", 15))
			{
				secondHandWidth = atoi(szTemp + 16);
				InvalidateRect(hwndBBAnalogExMod, NULL, true);
			}

			//---
			else if (!_strnicmp(szTemp, "HourLengthSize", 14))
			{//sizes of the clock parts
				hourHandLength = atoi(szTemp + 15);
				InvalidateRect(hwndBBAnalogExMod, NULL, true);
			}
			else if (!_strnicmp(szTemp, "MinuteLengthSize", 16))
			{
				minuteHandLength = atoi(szTemp + 17);
				InvalidateRect(hwndBBAnalogExMod, NULL, true);
			}
			else if (!_strnicmp(szTemp, "SecondLengthSize", 16))
			{
				secondHandLength = atoi(szTemp + 17);
				InvalidateRect(hwndBBAnalogExMod, NULL, true);
			}
			//---

			else if (!_strnicmp(szTemp, "FontSize", 8))
			{
				fontSizeC = atoi(szTemp + 9);
				InvalidateRect(hwndBBAnalogExMod, NULL, true);
			}
			else if (!_strnicmp(szTemp, "ClockSize", 9))
			{ //changing the clock size
				newSize = atoi(szTemp + 10);

				if((newSize % 2) == 0) cntX = cntY = (float)((newSize / 2) - .5);
				else cntX = cntY = (float)((newSize - 1) / 2);

				if(ResizeMyWindow(newSize)){
					radius = cntX - (float)(((2 * bevelWidth) + borderWidth));
					InvalidateRect(hwndBBAnalogExMod, NULL, true);
                                }
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

		case WM_WINDOWPOSCHANGED:
		{
			if(!inSlit){
				WINDOWPOS* windowpos = (WINDOWPOS*)lParam;
				xpos = windowpos->x;
				ypos = windowpos->y;

				int i = (windowpos->cx > windowpos->cy ? windowpos->cx : windowpos->cy);

				if((i % 2) == 0) cntX = cntY = (float)((i / 2) - .5);
				else cntX = cntY = (float)((i - 1) / 2);

				if(ResizeMyWindow(i)){
					radius = cntX - (float)(((2 * bevelWidth) + borderWidth));
					InvalidateRect(hwndBBAnalogExMod, NULL, true);
                                }
			}
		}
		break;

		// ==========

		case WM_SIZING:
		{
			RECT *rc;
			rc = (RECT *)lParam;
			int width = rc->right - rc->left;
			int height = rc->bottom - rc->top;
			int i = (width > height ? width : height);

			if(((double)width / (double)height) < 1){
				rc->right = rc->left + (int)height;
			}else{
				rc->bottom = rc->top + (int)width;
			}
		}
		break;

		// ==========

		case WM_DISPLAYCHANGE:
		{
			if(!inSlit || !hSlit)
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
				MoveWindow(hwndBBAnalogExMod, xpos, ypos, size, size, true);
			}
		}
		break;

		// ==========

		case WM_NCHITTEST:
		{
			if (GetKeyState(VK_MENU) & 0xF0)
				return HTBOTTOMRIGHT;
			else
				return HTCAPTION;
		}
		break;

		case WM_NCLBUTTONDOWN:
		{
			/* Please do not allow plugins to be moved in the slit.
			 * That's not a request..  Okay, so it is.. :-P
			 * I don't want to hear about people losing their plugins
			 * because they loaded it into the slit and then moved it to
			 * the very edge of the slit window and can't get it back..
			 */
			
			if((!inSlit)&&(!lockp))
				return DefWindowProc(hwnd,message,wParam,lParam);
		}
		break;

		case WM_NCLBUTTONUP: {} break;
		
		// ==========
		//Show the date when the mouse is clicked and the cntrl button is press
		case WM_LBUTTONUP: 
		{
			if (GetAsyncKeyState(VK_CONTROL) & 0x8000)
			{
				if(showAlarm <= 0)
				{
					showDate = 3;
					//set timer to 1 second interval for date
					if(!showSeconds) mySetTimer(2);
					InvalidateRect(hwndBBAnalogExMod, NULL, false);
				}
			}
		}
		break;
		
		// ==========

		case WM_LBUTTONDOWN: {} break;

		// ==========

		// Right mouse button clicked?
		case WM_NCRBUTTONUP:
		{
			ShowMyMenu(true);
		}
		break;

		// ==========

		case WM_NCRBUTTONDOWN: {} break;

		// ==========

		case WM_NCLBUTTONDBLCLK: 
		{
			//open control panel with:  control timedate.cpl,system,0
			BBExecute(GetDesktopWindow(), NULL, "control", "timedate.cpl,system,0", NULL, SW_SHOWNORMAL, false);
		}
		break;

		case WM_NCMBUTTONUP:
		{
			if(showAlarm <= 0)
			{
				showDate = 3;
				//set timer to 1 second interval for date
				if(!showSeconds) mySetTimer(2);
				InvalidateRect(hwndBBAnalogExMod, NULL, false);
			}
		}
		break;
		
		// ==========

		case WM_NCMBUTTONDOWN: {} break;

		// ==========

		case WM_TIMER:
		{
			switch (wParam)
			{
				case IDT_TIMER:
				{
					getCurrentTime();
					//redraw the window
					InvalidateRect(hwndBBAnalogExMod, NULL, false);
				} break;
			}
		}
		break;

		// ==========

		default:
			return DefWindowProc(hwnd,message,wParam,lParam);
	}
	return 0;
}

//===========================================================================

void ShowMyMenu(bool popup)
{
			char temp[MAX_PATH];
			if(myMenu){ DelMenu(myMenu); myMenu = NULL;}

			clockConfigSubmenu = MakeNamedMenu("Clock", "BBAEX_Clock", popup);
			MakeMenuItemInt(clockConfigSubmenu, "Clock Size", "@BBAnalogExModClockSize", size, 20, 300);

			secondSubmenu = MakeNamedMenu("Seconds Hand", "BBAEX_SecondsHand", popup);
			MakeMenuItemInt(secondSubmenu, "Length %", "@BBAnalogExModSecondLengthSize", secondHandLength, 1, 100);
			MakeMenuItemInt(secondSubmenu, "Width",    "@BBAnalogExModSecondWidthSize", secondHandWidth, 1, 10);
			MakeMenuItem(secondSubmenu,    "Color",    "@BBAnalogExModSecondsHandColor", false);
			MakeSubmenu(clockConfigSubmenu, secondSubmenu, "Seconds Hand");

			minuteSubmenu = MakeNamedMenu("Minutes Hand", "BBAEX_MinutesHand", popup);
                	MakeMenuItemInt(minuteSubmenu, "Length %", "@BBAnalogExModMinuteLengthSize", minuteHandLength, 1, 100);
			MakeMenuItemInt(minuteSubmenu, "Width",    "@BBAnalogExModMinuteWidthSize", minuteHandWidth, 1, 10);
			MakeMenuItem(   minuteSubmenu, "Color",    "@BBAnalogExModMinutesHandColor", false);
			MakeSubmenu(clockConfigSubmenu, minuteSubmenu, "Minutes Hand");

			hourSubmenu = MakeNamedMenu("Hours Hand", "BBAEX_HoursHand", popup);
			MakeMenuItemInt(hourSubmenu, "Length %", "@BBAnalogExModHourLengthSize", hourHandLength, 1, 100);
 			MakeMenuItemInt(hourSubmenu, "Width",    "@BBAnalogExModHourWidthSize", hourHandWidth, 1, 10);
			MakeMenuItem(   hourSubmenu, "Color",    "@BBAnalogExModHoursHandColor", false);
			MakeSubmenu(clockConfigSubmenu, hourSubmenu, "Hours Hand");

			textSubmenu = MakeNamedMenu("Text", "BBAEX_Text", popup);
			MakeMenuItem(textSubmenu, "Normal Numerals", "@BBAnalogExModNormalNumerals", showNormal);
			MakeMenuItem(textSubmenu, "Roman Numerals", "@BBAnalogExModRomanNumerals", showRomans);
			MakeMenuItemInt(textSubmenu, "Font Size", "@BBAnalogExModFontSize", fontSizeC, 6, size/3);
			MakeMenuItem(   textSubmenu, "Font Color","@BBAnalogExModNumberColor", false);
			MakeSubmenu(clockConfigSubmenu, textSubmenu, "Text");

			MakeMenuNOP(clockConfigSubmenu, NULL);

			imageSubmenu = MakeNamedMenu("Image", "Image", popup);
			MakeMenuItem(imageSubmenu, "Browse...", "@BBAnalogExModSetImage", false);
			MakeMenuItem(imageSubmenu, "Nothing", "@BBAnalogExModRemoveImage", false);
			MakeSubmenu(clockConfigSubmenu, imageSubmenu, "Image");

			regionSubmenu = MakeNamedMenu("Regions", "BBAEX_Regions", popup);
			MakeMenuItem(regionSubmenu, "Rect", "@BBAnalogExModRegion 0", (region == 0));
			MakeMenuItem(regionSubmenu, "Oval", "@BBAnalogExModRegion 1", (region == 1));
			MakeMenuItem(regionSubmenu, "Hole", "@BBAnalogExModRegion 2", (region == 2));
			MakeMenuNOP(regionSubmenu, NULL);
                	MakeMenuItemInt(regionSubmenu, "Outside", "@BBAnalogExModOutside", outsideCircle, -size/4, size/4);
			MakeMenuItemInt(regionSubmenu, "Inside",  "@BBAnalogExModInside",  insideCircle,  -size/4, size/4);
			MakeSubmenu(clockConfigSubmenu, regionSubmenu, "Regions");

			alarmConfigSubmenu = MakeNamedMenu("Alarm", "BBAEX_Alarm", popup);
			MakeMenuItem(alarmConfigSubmenu, "Set Alarm Time", "@BBAnalogExModEditRC", false);
			MakeMenuItem(alarmConfigSubmenu, "Hour Alarm", "@BBAnalogExModHourAlarm", hourAlarm);
			MakeMenuItem(alarmConfigSubmenu, "Half Hour Alarm", "@BBAnalogExModHalfHourAlarm", halfHourAlarm);
			MakeMenuItem(alarmConfigSubmenu, "Synch Animation\\Wav", "@BBAnalogExModSynchronize", synchronize);
			if(!timeAlarmSet && !hourAlarm && !halfHourAlarm)
			{
				setAlarmsConfigSubmenu = MakeNamedMenu("No Set Alarms", "BBAEX_setAlarm", popup);
				MakeMenuNOP(setAlarmsConfigSubmenu, "No Set Alarms");
				MakeSubmenu(alarmConfigSubmenu, setAlarmsConfigSubmenu, "No Set Alarms");
			}
			else
			{
				setAlarmsConfigSubmenu = MakeNamedMenu("Set Alarms", "BBAEX_setAlarm", popup);
				if(timeAlarmSet)
				{
					sprintf(temp, "%s Alarm Set -> Test", timeAlarm);
					MakeMenuItem(setAlarmsConfigSubmenu, temp, "@BBAnalogExModTestTimeAlarm", false);
				}
				if(hourAlarm)
					MakeMenuItem(setAlarmsConfigSubmenu, "Hour Alarm Set -> Test", "@BBAnalogExModTestHourAlarm", false);
				if(halfHourAlarm)
					MakeMenuItem(setAlarmsConfigSubmenu, "Half Hour Alarm Set -> Test", "@BBAnalogExModTestHalfHourAlarm", false);
				MakeSubmenu(alarmConfigSubmenu, setAlarmsConfigSubmenu, "Set Alarms");
			}
			MakeSubmenu(clockConfigSubmenu, alarmConfigSubmenu, "Alarm");

			optionSubmenu = MakeNamedMenu("Option", "BBAEX_Option", popup);
			MakeMenuItem(optionSubmenu, "Show Seconds Hand", "@BBAnalogExModSeconds", showSeconds);
			MakeMenuItem(optionSubmenu, "Show Outer Circle", "@BBAnalogExModCircle", showCircle);
			MakeMenuItem(optionSubmenu, "Show Large Tics", "@BBAnalogExModLargeTics", showLargeTics);
			MakeMenuItem(optionSubmenu, "Show Small Tics", "@BBAnalogExModSmallTics", showSmallTics);
			MakeMenuItem(optionSubmenu, "Sweeping Hands", "@BBAnalogExModSweep", sweepHands);
			MakeSubmenu(clockConfigSubmenu, optionSubmenu, "Option");

			configSubmenu = MakeNamedMenu("Configuration", "BBAEX_Config", popup);
			if(hSlit) MakeMenuItem(configSubmenu, "In Slit", "@BBAnalogExModInSlit", inSlit);
			MakeMenuItem(configSubmenu, "Plugin Toggle", "@BBAnalogExModPluginToggle", pluginToggle);
			MakeMenuItem(configSubmenu, "Always on top", "@BBAnalogExModOnTop", alwaysOnTop);
			MakeMenuItem(configSubmenu, "Snap To Edge", "@BBAnalogExModSnapToEdge", snapWindow);
			MakeMenuItem(configSubmenu, "Lock Position", "@BBAnalogExModLockPosition", lockp);
			MakeMenuItem(configSubmenu, "Draw Border", "@BBAnalogExModDrawBorder", drawBorder);
			if(!inSlit){
			MakeMenuItem(configSubmenu, "Transparency", "@BBAnalogExModTransparency", transparency);
			MakeMenuItem(configSubmenu, "Trans Back", "@BBAnalogExModTransBack", transBack);
			MakeMenuItemInt(configSubmenu, "Alpha Value", "@BBAnalogExModAlphaValue", alpha, 0, 255);
                        }

			styleSubmenu = MakeNamedMenu("Style Type", "BBAEX_StyleType", popup);
			MakeMenuItem(styleSubmenu, "Toolbar",  "@BBAnalogExModStyleType 1", (styleType == 1));
			MakeMenuItem(styleSubmenu, "Button",   "@BBAnalogExModStyleType 2", (styleType == 2));
			MakeMenuItem(styleSubmenu, "ButtonP",  "@BBAnalogExModStyleType 3", (styleType == 3));
			MakeMenuItem(styleSubmenu, "Label",    "@BBAnalogExModStyleType 4", (styleType == 4));
			MakeMenuItem(styleSubmenu, "WinLabel", "@BBAnalogExModStyleType 5", (styleType == 5));
			MakeMenuItem(styleSubmenu, "Clock",    "@BBAnalogExModStyleType 6", (styleType == 6));

			settingsSubmenu = MakeNamedMenu("Settings", "BBAEX_Settings", popup);
			MakeMenuItem(settingsSubmenu, "Edit Settings", "@BBAnalogExModEditRC", false);
			MakeMenuItem(settingsSubmenu, "Reload Settings", "@BBAnalogExModReloadSettings", false);
			MakeMenuItem(settingsSubmenu, "Save Settings", "@BBAnalogExModSaveSettings", false);

			myMenu = MakeNamedMenu("BBAnalogExMod", "BBAEX_Main", popup);
			MakeSubmenu(myMenu, clockConfigSubmenu, "Clock");
			MakeSubmenu(myMenu, configSubmenu, "Configuration");
			MakeSubmenu(myMenu, styleSubmenu, "Style Type");
			MakeSubmenu(myMenu, settingsSubmenu, "Settings");
			MakeMenuItem(myMenu, "About", "@BBAnalogExModAbout", false);

			ShowMenu(myMenu);
}

//===========================================================================

void OnDropFiles(HDROP h_Drop) {
	DragQueryFile(h_Drop, 0, imagePath, sizeof(imagePath));
	if (strcmp((char*)imagePath, "") != 0){
		if(myImage) delete myImage;

		WCHAR wTitle[256];
		mbstowcs(wTitle, imagePath, strlen(imagePath)+1);
		myImage = new Gdiplus::Image(wTitle);
		InvalidateRect(hwndBBAnalogExMod, NULL, true);
        }
}

//===========================================================================
//Need to clean this function up so that it is more readable, without removing functionality.

void GetStyleSettings()
{
	bool nix = false;
	char style[MAX_PATH];
	char temp[256];
	strcpy(style, stylePath());
	strcpy(temp, ReadString(style, "toolbar.appearance:", (char *)"no"));
	if (strlen(temp) != 2) nix = true;

	myStyleItem = *(StyleItem *)GetSettingPtr(styleType);

	bevelWidth  = nix ? myStyleItem.marginWidth : *(int*)GetSettingPtr(SN_BEVELWIDTH);
	borderWidth = nix ? myStyleItem.borderWidth : *(int*)GetSettingPtr(SN_BORDERWIDTH);
	borderColor = nix ? myStyleItem.borderColor : *(COLORREF*)GetSettingPtr(SN_BORDERCOLOR);


	radius = cntX - (float)(((2 * bevelWidth) + borderWidth));	
	//make a new gdi+ fontColor
	if(plusFontColor) delete plusFontColor;
	plusFontColor = new Gdiplus::Color(GetRValue(fontColor), GetGValue(fontColor), GetBValue(fontColor));

	// ...font settings...
	fontSize = (size / 3);
	//fontColor = ReadColor(stylepath, "toolbar.label.textColor:", "#FFFFFF");
}

//===========================================================================

void ReadRCSettings()
{
	char temp[MAX_PATH], oldstylepath[MAX_PATH];
	int nLen;
	magicHourFreq = false;
		
	// First we look for the config file in the same folder as the plugin...
	GetModuleFileName(hInstance, rcpath, sizeof(rcpath));
	nLen = (int)strlen(rcpath) - 1;
	while (nLen >0 && rcpath[nLen] != '\\') nLen--;
	rcpath[nLen + 1] = 0;
	strcpy(temp, rcpath);
	strcpy(oldstylepath, rcpath);
	strcat(temp, "bbanalogexmod.rc");
	strcat(oldstylepath, "bbanalogexmodrc");
	// ...checking the two possible filenames bbanalog.rc and bbanalogrc ...
	if (FileExists(temp))
		strcpy(rcpath, temp);
	else if (FileExists(oldstylepath))
		strcpy(rcpath, oldstylepath);
	else
		strcpy(rcpath, temp); // default path if no file exists
	// If a config file was found we read the plugin settings from the file...
	//Always checking non-bool values to make sure they are the right format
	xpos = ReadInt(rcpath, "bbanalogexmod.x:", 10);
	ypos = ReadInt(rcpath, "bbanalogexmod.y:", 10);
	if (xpos >= GetSystemMetrics(SM_CXVIRTUALSCREEN)) xpos = 10;
	if (ypos >= GetSystemMetrics(SM_CYVIRTUALSCREEN)) ypos = 10;

	size = ReadInt(rcpath, "bbanalogexmod.size:", 100);
	if(size < 5 || size > 300) size = 100;

	alpha = ReadInt(rcpath, "bbanalogexmod.alpha:", 160);
	if(alpha > 255) alpha = 255;
	inSlit = ReadBool(rcpath, "bbanalogexmod.inSlit:", false);
	alwaysOnTop = ReadBool(rcpath, "bbanalogexmod.alwaysOnTop:", true);
	drawBorder = ReadBool(rcpath, "bbanalogexmod.drawBorder:", true);
	snapWindow = ReadBool(rcpath, "bbanalogexmod.snapWindow:", true);
	transparency = ReadBool(rcpath, "bbanalogexmod.transparency:", false);
	transBack = ReadBool(rcpath, "bbanalogexmod.transBack:", false);
	lockp = ReadBool(rcpath, "bbanalogexmod.lockPosition:", false);
	region = ReadInt(rcpath, "bbanalogexmod.region:", 0);
	fontSizeC = ReadInt(rcpath, "bbanalogexmod.fontSizeC:", 16);
	alwaysOnTop = ReadBool(rcpath, "bbanalogexmod.alwaysontop:", true);
	pluginToggle = ReadBool(rcpath, "bbanalogexmod.pluginToggle:", false);
	showSeconds = ReadBool(rcpath, "bbanalogexmod.showSeconds:", true);
	sweepHands = ReadBool(rcpath, "bbanalogexmod.sweepHands:", false);
	showRomans = ReadBool(rcpath, "bbanalogexmod.romanNumerals:", false);
	showNormal = ReadBool(rcpath, "bbanalogexmod.normalNumerals:", false);
	showCircle = ReadBool(rcpath, "bbanalogexmod.showCircle:", false);
	showLargeTics = ReadBool(rcpath, "bbanalogexmod.showLargeTics:", true);
	showSmallTics = ReadBool(rcpath, "bbanalogexmod.showSmallTics:", true);

	styleType = ReadInt(rcpath, "bbanalogexmod.windowStyle:", 5);
	numbColor = ReadColor(rcpath, "bbanalogexmod.numberColor:", "#FFFFFF");
	secondHandColor = ReadColor(rcpath, "bbanalogexmod.secondHandColor:", "#FFFFFF");
	minuteHandColor = ReadColor(rcpath, "bbanalogexmod.minuteHandColor:", "#FFFFFF");
	hourHandColor   = ReadColor(rcpath, "bbanalogexmod.hourHandColor:", "#FFFFFF");
	secondHandWidth = ReadInt(rcpath, "bbanalogexmod.secondHandWidth:", 1);
	minuteHandWidth = ReadInt(rcpath, "bbanalogexmod.minuteHandWidth:", 2);
	hourHandWidth   = ReadInt(rcpath, "bbanalogexmod.hourHandWidth:", 2);
	secondHandLength = ReadInt(rcpath, "bbanalogexmod.secondHandLength:", 80);
	minuteHandLength = ReadInt(rcpath, "bbanalogexmod.minuteHandLength:", 80);
	hourHandLength   = ReadInt(rcpath, "bbanalogexmod.hourHandLength:", 55);

	outsideCircle = ReadInt(rcpath, "bbanalogexmod.outsideRegion:", 0);
	insideCircle  = ReadInt(rcpath, "bbanalogexmod.insideRegion:", 0);
	strcpy(timeAlarm, ReadString(rcpath, "bbanalogexmod.timeAlarm:", ""));
	readSetAlarmTime();

	hourAlarm = ReadBool(rcpath, "bbanalogexmod.hourAlarm:", true);
	halfHourAlarm = ReadBool(rcpath, "bbanalogexmod.halfHourAlarm:", false);

	timeAlarmFrequency = ReadInt(rcpath, "bbanalogexmod.timeAlarmFreq:", 10);
	if(timeAlarmFrequency < 1 || timeAlarmFrequency > 30) timeAlarmFrequency = 10;

	if(StrStrI(ReadString(rcpath, "bbanalogexmod.hourAlarmFreq:", "10"), "hour") != NULL)
		magicHourFreq = true;
	else
	{
		magicHourFreq = false;
		hourAlarmFrequency = ReadInt(rcpath, "bbanalogexmod.hourAlarmFreq:", 10);
		if(hourAlarmFrequency < 1 || hourAlarmFrequency > 30) hourAlarmFrequency = 10;
	}

	halfHourAlarmFrequency = ReadInt(rcpath, "bbanalogexmod.halfHourAlarmFreq:", 10);
	if(halfHourAlarmFrequency < 1 || halfHourAlarmFrequency > 30) halfHourAlarmFrequency = 10;

	animationFPS = ReadInt(rcpath, "bbanalogexmod.animationFPS:", 10);
	if(animationFPS < 1 || animationFPS > 30) animationFPS = 10;

	synchronize = ReadBool(rcpath, "bbanalogexmod.synchronize:", true);
	strcpy(timeAlarmWav, ReadString(rcpath, "bbanalogexmod.timeAlarmWav:", ".none"));
	strcpy(hourAlarmWav, ReadString(rcpath, "bbanalogexmod.hourAlarmWav:", ".none"));
	strcpy(halfHourAlarmWav, ReadString(rcpath, "bbanalogexmod.halfHourAlarmWav:", ".none"));
	strcpy(imagePath, ReadString(rcpath, "bbanalogexmod.imagePath:", ".none"));
}

//===========================================================================

void WriteRCSettings()
{
	    	WriteInt(rcpath, "bbanalogexmod.x:", xpos);
	    	WriteInt(rcpath, "bbanalogexmod.y:", ypos);
		WriteInt(rcpath, "bbanalogexmod.size:", size);
		WriteInt(rcpath, "bbanalogexmod.alpha:", alpha);
		WriteInt(rcpath, "bbanalogexmod.region:", region);
		WriteBool(rcpath, "bbanalogexmod.drawBorder:", drawBorder);
		WriteInt(rcpath, "bbanalogexmod.fontSizeC:", fontSizeC);
		WriteBool(rcpath, "bbanalogexmod.inSlit:", inSlit);
		WriteBool(rcpath, "bbanalogexmod.alwaysOnTop:", alwaysOnTop);
		WriteBool(rcpath, "bbanalogexmod.snapWindow:", snapWindow);
		WriteBool(rcpath, "bbanalogexmod.transparency:", transparency);
		WriteBool(rcpath, "bbanalogexmod.transBack:", transBack);
		WriteBool(rcpath, "bbanalogexmod.lockPosition:", lockp);
		WriteBool(rcpath, "bbanalogexmod.pluginToggle:", pluginToggle);
		WriteBool(rcpath, "bbanalogexmod.showSeconds:", showSeconds);
		WriteBool(rcpath, "bbanalogexmod.sweepHands:", sweepHands);
		WriteBool(rcpath, "bbanalogexmod.romanNumerals:", showRomans);
		WriteBool(rcpath, "bbanalogexmod.normalNumerals:", showNormal);;
		WriteBool(rcpath, "bbanalogexmod.showCircle:", showCircle);
		WriteBool(rcpath, "bbanalogexmod.showLargeTics:", showLargeTics);
		WriteBool(rcpath, "bbanalogexmod.showSmallTics:", showSmallTics);
		WriteInt(rcpath, "bbanalogexmod.windowStyle:", styleType);
		WriteColor(rcpath, "bbanalogexmod.numberColor:", numbColor);
		WriteColor(rcpath, "bbanalogexmod.secondHandColor:", secondHandColor);
		WriteColor(rcpath, "bbanalogexmod.minuteHandColor:", minuteHandColor);
		WriteColor(rcpath, "bbanalogexmod.hourHandColor:", hourHandColor);
		WriteInt(rcpath, "bbanalogexmod.secondHandWidth:", secondHandWidth);
		WriteInt(rcpath, "bbanalogexmod.minuteHandWidth:", minuteHandWidth);
		WriteInt(rcpath, "bbanalogexmod.hourHandWidth:", hourHandWidth);
		WriteInt(rcpath, "bbanalogexmod.secondHandLength:", secondHandLength);
		WriteInt(rcpath, "bbanalogexmod.minuteHandLength:", minuteHandLength);
		WriteInt(rcpath, "bbanalogexmod.hourHandLength:", hourHandLength);
		WriteString(rcpath, "bbanalogexmod.timeAlarm:", timeAlarm);
		WriteBool(rcpath, "bbanalogexmod.hourAlarm:", hourAlarm);
		WriteBool(rcpath, "bbanalogexmod.halfHourAlarm:", halfHourAlarm);
		WriteInt(rcpath, "bbanalogexmod.timeAlarmFreq:", timeAlarmFrequency);
		WriteInt(rcpath, "bbanalogexmod.hourAlarmFreq:", hourAlarmFrequency);
		WriteInt(rcpath, "bbanalogexmod.halfHourAlarmFreq:", halfHourAlarmFrequency);
		WriteInt(rcpath, "bbanalogexmod.animationFPS:", animationFPS);
		WriteBool(rcpath, "bbanalogexmod.synchronize:", synchronize);
		WriteString(rcpath, "bbanalogexmod.timeAlarmWav:", timeAlarmWav);
		WriteString(rcpath, "bbanalogexmod.hourAlarmWav:", hourAlarmWav);
		WriteString(rcpath, "bbanalogexmod.halfHourAlarmWav:", halfHourAlarmWav);
		WriteString(rcpath, "bbanalogexmod.imagePath:", imagePath);
		WriteInt(rcpath, "bbanalogexmod.outsideRegion:", outsideCircle);
		WriteInt(rcpath, "bbanalogexmod.insideRegion:", insideCircle);
}

//===========================================================================

bool ResizeMyWindow(int new_size)
{
	if(new_size != size)
	{
		size = new_size;
		if (size < 20)  size = 20;
	}
	else
		return false;

	if(inSlit)
	{
		SetWindowPos(hwndBBAnalogExMod, 0, 0, 0, size, size, SWP_NOACTIVATE | SWP_NOZORDER | SWP_NOMOVE);
		SendMessage(hSlit, SLIT_UPDATE, NULL, NULL);
		return true;
	}
	else
	{
		SetWindowPos(hwndBBAnalogExMod, 0, xpos, ypos, size, size, SWP_NOACTIVATE | SWP_NOZORDER);
		return true;
	}
}

//===========================================================================

//get current local time of the users machine
void getCurrentTime()
{
	_timeb Mytimeb;
	time(&systemTime);
	_ftime(&Mytimeb);
	localTime		= localtime(&systemTime);
	currentSecond	= localTime->tm_sec;
	currentMinute	= localTime->tm_min;
	currentHour		= localTime->tm_hour;
	currentMillisecond = Mytimeb.millitm;

	//fix 24 hour time
	if(currentHour > 12) currentHour -= 12;
	if(currentHour == 0) currentHour = 12;

	//timeAlarm takes precedence, then hourAlarm, then halfHourAlarm

	//alarmTime alarm
	if(timeAlarmSet && (showAlarm <= 0))
	{
		//must check if alarm is pm and use 24 hour time -- midnight == 0, noon == 12
		if((meridian && (localTime->tm_hour > 11)) || (!meridian && (localTime->tm_hour < 12)))
		{
			//Set timer and counter for silent alarm, on time alarm.
			if((((currentSecond == 0) || (showSeconds != true)) && ((int)currentMinute == timeAlarmMinute) && (((int)currentHour) == timeAlarmHour)))
			{			
				if(synchronize && (StrStrI(timeAlarmWav, ".wav") != NULL))
					strcpy(playAlarm, timeAlarmWav);
				else
				{
					myPlayWav(timeAlarmWav);
					strcpy(playAlarm, ".none");
				}
				//set timer to alarm -- mode = 2
				mySetTimer(2);
				showAlarm = timeAlarmFrequency;
			}
		}
	}

	//hour alarm
	if(hourAlarm && (showAlarm <= 0))
	{
		//Set timer and counter for silent alarm, every new hour.
		if(((currentSecond == 0) && (currentMinute == 0)) || ((showSeconds != true) && (currentMinute == 0)))
		{			
			if(synchronize && (StrStrI(hourAlarmWav, ".wav") != NULL))
				strcpy(playAlarm, hourAlarmWav);
			else
			{
				myPlayWav(hourAlarmWav);
				strcpy(playAlarm, ".none");
			}
			//set timer to alarm -- mode = 2
			mySetTimer(2);
			if(magicHourFreq)
				showAlarm = (int)currentHour;
			else
				showAlarm = hourAlarmFrequency;
		}
	}

	//half hour alarm
	if(halfHourAlarm && (showAlarm <= 0))
	{
		//Set timer and counter for silent alarm, every new hour.
		if(((currentSecond == 0) && (currentMinute == 30)) || ((showSeconds != true) && (currentMinute == 30)))
		{			
			if(synchronize && (StrStrI(halfHourAlarmWav, ".wav") != NULL))
				strcpy(playAlarm, halfHourAlarmWav);
			else
			{
				myPlayWav(halfHourAlarmWav);
				strcpy(playAlarm, ".none");
			}
			//set timer to alarm -- mode = 2
			mySetTimer(2);
			showAlarm = halfHourAlarmFrequency;
		}
	}

	//for getting fraction of an hour
	currentHour += (currentMinute / 60);
}

//===========================================================================
/*
void getCurrentDate()
{
	time(&systemTime);
	localTime = localtime(&systemTime);
	strftime(currentDate, 128,"%a %#d\n%b", localTime);
}
*/

//===========================================================================
//mode == 0 -> 1 sec, mode == 1 -> 1 min, mode == 2 -> 1/fps sec
//could add a check to make sure SetTimer succeeded and return a boolean for beginPlugin :)

void mySetTimer(int mode)
{
	//1 minute timer
	if(mode == 1)
	{
		//Start the 1 minute plugin timer
		SetTimer(hwndBBAnalogExMod,		// handle to main window 
				IDT_TIMER,			// timer identifier 
				60000,				// 1-minute interval
				(TIMERPROC) NULL);	// no timer callback 
	}
	//alarm mode timer, for faster redraw
	else if(mode == 2)
	{
		//Start the 1/20 second plugin timer
		SetTimer(hwndBBAnalogExMod,			// handle to main window 
				IDT_TIMER,				// timer identifier 
				(UINT)1000/animationFPS,// 1/fps-second interval - default 10 fps
				(TIMERPROC) NULL);		// no timer callback 
	}
	else	//always fall back to the 1 second timer -- mode == 0
	{
		//Start the 1 second plugin timer
		SetTimer(hwndBBAnalogExMod,		// handle to main window 
				IDT_TIMER,			// timer identifier
				1000,				// 1-second interval
				(TIMERPROC) NULL);	// no timer callback 
	}
}

//===========================================================================
//parse the setTime string.  make input string empty if it is wrong and alarmTimeSet = false

void readSetAlarmTime()
{
	char temp[MAX_PATH];
	//bool tempBool = false;
	strcpy(temp, timeAlarm);
	char* pch;

	//get first token
	pch = strtok(temp, ":");
	if(pch != NULL)
	{
		if(strlen(pch) > 2 || strlen(pch) < 1) strcpy(timeAlarm, "");
		else
		{
			if(strlen(pch) == 2) timeAlarmHour = (((int)pch[0] - 48) * 10) + ((int)pch[1] - 48);
			else timeAlarmHour = (int)pch[0] - 48;

			//get second token
			pch = strtok (NULL, "");
			if(pch != NULL)
			{
				if((pch[1] - 48 >= 0) && (pch[1] - 48 <= 9))
				{
					timeAlarmMinute = (((int)pch[0] - 48) * 10) + ((int)pch[1] - 48);
					if(StrStrI(pch , "pm") != NULL)
						meridian = true;
					else if(StrStrI(pch , "am")  != NULL)
						meridian = false;
					else
					{
						meridian =false;
						strcpy(timeAlarm, "");
					}
				}
				else
					strcpy(timeAlarm, "");
			}
			else
				strcpy(timeAlarm, "");
		}
	}
	else
	{
		strcpy(timeAlarm, "");
	}
	
	//make sure they are of a 12 hour format with meridian designation
	if(timeAlarmHour > 12 || timeAlarmHour < 1 || timeAlarmMinute < 0 || timeAlarmMinute > 59 || strcmp(timeAlarm, "") == 0)
	{
		timeAlarmSet = false;
		strcpy(timeAlarm, "");
	}
	else
	{
		timeAlarmSet = true;
	}
}

//===========================================================================
//play wav file internally or execute the unknown file and let windows decide

void myPlayWav(char * alarmFile)
{
	int nLen;
	char temp[MAX_PATH];
	if(StrStrI(alarmFile, ".none") == NULL)
	{
		//First we check if the string points directly to the file
		if(FileExists(alarmFile))
		{
			if (StrStrI(alarmFile, ".wav") != NULL)
				PlaySound(alarmFile, NULL, SND_FILENAME | SND_ASYNC);
			else
				BBExecute(GetDesktopWindow(), NULL, alarmFile, NULL, NULL, SW_SHOWNORMAL, false);
		}
		// Then we look for the alarm file in the same folder as the plugin...
		else
		{
			GetModuleFileName(hInstance, temp, sizeof(temp));
			nLen = (int)strlen(temp) - 1;
			while (nLen >0 && temp[nLen] != '\\') nLen--;
			temp[nLen + 1] = 0;
			strcat(temp, alarmFile);
			if (FileExists(temp))
			{
				if (StrStrI(alarmFile, ".wav") != NULL)
					PlaySound(temp, NULL, SND_FILENAME | SND_ASYNC);
				else
					BBExecute(GetDesktopWindow(), NULL, temp, NULL, NULL, SW_SHOWNORMAL, false);
			}
		}
	}
}

//===========================================================================

void setRegion()   //set regions to the clock
{
	int gap = bevelWidth + borderWidth;

	GetClientRect(hwndBBAnalogExMod,&rect);

	rect.left = rect.left + gap;
	rect.top = rect.top + gap;
	rect.bottom = (rect.bottom - gap);
	rect.right = (rect.right - gap);
	//we wont the oval time

	if (region == 1) {HRGN hRegion = CreateEllipticRgn(rect.left +outsideCircle, rect.top+outsideCircle, rect.right-outsideCircle, rect.bottom-outsideCircle);
	SetWindowRgn(hwndBBAnalogExMod, hRegion, true);
	DeleteObject(hRegion);}
	//or the hole type
	else if(region == 2) {
	HRGN hRegion = CreateEllipticRgn(rect.left +outsideCircle, rect.top+outsideCircle, rect.right-outsideCircle, rect.bottom-outsideCircle);
	HRGN hRegion2 = CreateEllipticRgn(rect.left + rect.right/4+insideCircle, rect.top + rect.bottom/4+insideCircle, rect.right - rect.right/4-insideCircle, rect.bottom - rect.bottom/4-insideCircle);
	CombineRgn(hRegion, hRegion, hRegion2, RGN_XOR);
	SetWindowRgn(hwndBBAnalogExMod, hRegion, true);
	DeleteObject(hRegion);
	DeleteObject(hRegion2);}
	//or nothing
	else SetWindowRgn(hwndBBAnalogExMod, NULL, true);
}

//===========================================================================
