/*
 ============================================================================
 Blackbox for Windows: BBPager
 ============================================================================
 Copyright © 2003-2009 nc-17@ratednc-17.com
 Copyright © 2008-2009 The Blackbox for Windows Development Team
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

 ============================================================================
*/
#include "bbPager.h"
#include "Drawing.h"
#include <vector>

//using namespace std;

//===========================================================================

// Desktop information
int currentDesktop;
//RECT desktopRect[64];
std::vector<RECT> desktopRect;

extern struct FRAME frame;
extern struct DESKTOP desktop;
extern struct POSITION position;
extern struct ACTIVEDESKTOP activeDesktop;
extern int desktops;
extern int winCount;
extern std::vector<winStruct> winList;
extern bool usingAltMethod;
extern bool is_xoblite;
extern bool winMoving;
extern winStruct moveWin;
extern HWND hwndBBPager;
extern struct FOCUSEDWINDOW focusedWindow;
extern struct WINDOW window;
#if 0
extern int currentDesktop;
extern vector<RECT> desktopRect;
extern int desktopChangeButton;
extern int focusButton;
extern int moveButton;
#endif

char desktopNumber[4];

int col, row, currentCol, currentRow;

//===========================================================================

void DrawBBPager(HWND hwnd)
{
	// Create buffer hdc's, bitmaps etc.
	PAINTSTRUCT ps;
	HDC hdc = BeginPaint(hwnd, &ps);
	HDC buf = CreateCompatibleDC(NULL);
	HBITMAP bufbmp = CreateCompatibleBitmap(hdc, frame.width, frame.height);
	HGDIOBJ oldbmp = SelectObject(buf, bufbmp);
	RECT r;
	char toolText[256];

	GetClientRect(hwnd, &r);

	// Paint background and border according to the current style...
	MakeStyleGradient(buf, &r, frame.ownStyle ? frame.style : &frame.Style, true);

	HFONT font = CreateStyleFont((StyleItem *)GetSettingPtr(SN_TOOLBARLABEL));
	HGDIOBJ oldfont = SelectObject(buf, font);
	SetBkMode(buf, TRANSPARENT);
	SetTextColor(buf, desktop.fontColor);
	UINT flags = DT_CENTER|DT_VCENTER|DT_SINGLELINE|DT_WORD_ELLIPSIS|DT_NOPREFIX;

	desktopRect.clear();

	// Paint desktops :D
	if (position.horizontal) 
	{
		// Do loop to draw desktops other than current selected desktop
		int i = 0;

		do 
		{
			col = i / frame.rows;
			row = i % frame.rows + 1;

			if (currentDesktop == i) 
			{
				currentCol = col;
				currentRow = row;
			}
			else 
			{
				r.left = frame.borderWidth + frame.bevelWidth + ((col) * (desktop.width + frame.bevelWidth));
				r.right = r.left + desktop.width;
				r.top = frame.borderWidth + frame.bevelWidth + ((row - 1) * (desktop.height + frame.bevelWidth));
				r.bottom = r.top + desktop.height;

				//desktopRect[i] = r; // set RECT item for this desktop
				//desktopRect.insert(desktopRect.begin() + i - 1, r);
				desktopRect.push_back(r);

				if (desktop.ownStyle)
					CreateBorder(buf, &r, activeDesktop.borderColor, 1);
				else
				{
					r.left  += 1;
					r.top   += 1;
					r.right   -= 1;
					r.bottom  -= 1;
					MakeStyleGradient(buf, &r, &activeDesktop.Style, false);
				}
				if (desktop.numbers) 
				{
					sprintf(desktopNumber, "%d", (i + 1));
					DrawText(buf, desktopNumber, -1, &r, flags);
				}
			}
			i++;
		}
		while (i < desktops);

		// Do this now so bordered desktop is drawn last
		i = currentDesktop;

		r.left = frame.borderWidth + frame.bevelWidth + ((currentCol) * (desktop.width + frame.bevelWidth));
		r.right = r.left + desktop.width;
		r.top = frame.borderWidth + frame.bevelWidth + ((currentRow - 1) * (desktop.height + frame.bevelWidth));
		r.bottom = r.top + desktop.height;

		//desktopRect[i] = r; // set RECT item for this desktop

		DrawActiveDesktop(buf, r, i);

		if (desktop.numbers) 
		{
			sprintf(desktopNumber, "%d", (i + 1));
			DrawText(buf, desktopNumber, -1, &r, flags);
		}
	}
	else if (position.vertical) 
	{
		// Do loop to draw desktops other than current selected desktop
		int i = 0;

		do 
		{					
			row = i / frame.columns;
			col = i % frame.columns + 1;

			if (currentDesktop == i) 
			{
				currentCol = col;
				currentRow = row;
			}
			else 
			{
				r.left = frame.borderWidth + frame.bevelWidth + ((col - 1) * (desktop.width + frame.bevelWidth));
				r.right = r.left + desktop.width;
				r.top = frame.borderWidth + frame.bevelWidth + ((row) * (desktop.height + frame.bevelWidth));
				r.bottom = r.top + desktop.height;

				//desktopRect[i] = r; // set RECT item for this desktop
				//desktopRect.insert(desktopRect.begin() + i - 1, r);
				desktopRect.push_back(r);

				MakeStyleGradient(buf, &r, desktop.ownStyle ? desktop.style : &desktop.Style, false);
				if (desktop.ownStyle)
					CreateBorder(buf, &r, activeDesktop.borderColor, 1);
				else
				{
					r.left  += 1;
					r.top   += 1;
					r.right   -= 1;
					r.bottom  -= 1;
					MakeStyleGradient(buf, &r, &activeDesktop.Style, false);
				}
				if (desktop.numbers) 
				{
					sprintf(desktopNumber, "%d", (i + 1));
					DrawText(buf, desktopNumber, -1, &r, flags);
				}
			}
			i++;
		}
		while (i < desktops);

		// Do this now so bordered desktop is drawn last
		i = currentDesktop;

		r.left = frame.borderWidth + frame.bevelWidth + ((currentCol - 1) * (desktop.width + frame.bevelWidth));
		r.right = r.left + desktop.width;
		r.top = frame.borderWidth + frame.bevelWidth + ((currentRow) * (desktop.height + frame.bevelWidth));
		r.bottom = r.top + desktop.height;

		//desktopRect[i] = r; // set RECT item for this desktop

		DrawActiveDesktop(buf, r, i);

		if (desktop.numbers) 
		{
			sprintf(desktopNumber, "%d", (i + 1));
			SetTextColor(buf, activeDesktop.borderColor);
			DrawText(buf, desktopNumber, -1, &r, DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_WORD_ELLIPSIS | DT_NOPREFIX);
			//DrawText(buf, desktopNumber, strlen(desktopNumber), &r, DT_CALCRECT|DT_NOPREFIX);
			//SetTextColor(buf, desktop.fontColor);
		}
	}

	//DeleteObject(font);
	DeleteObject(SelectObject(buf, oldfont));

	// Draw windows on workspaces if wanted
	if (desktop.windows)
	{
		winCount = 0; // Reset number of windows to 0 on each paint to be counted by...
		winList.clear();

		// ... this function which passes HWNDs to CheckTaskEnumProc callback procedure
		if (!is_xoblite && usingAltMethod) 
			EnumWindows(CheckTaskEnumProc_AltMethod, 0);
		else
			EnumWindows(CheckTaskEnumProc, 0); 

		//struct tasklist *tlist;
		/*tl = GetTaskListPtr();
		while (tl)
		{
			AddBBWindow(tl);
			tl = tl->next;
		}*/

		// Only paint windows if there are any!
		if (winCount > 0)
		{
			// Start at end of list (bottom of zorder)
			for (int i = (winCount - 1);i > -1;i--)
			{
				RECT win = winList[i].r;
				RECT desk = desktopRect[winList[i].desk];

				if (win.right - win.left <= 1 && win.bottom - win.top <= 1)
					continue;
				
				// This is done so that windows only show within the applicable desktop RECT
				if (win.top < desk.top) 
					win.top = desk.top; // + 1;

				if (win.right > desk.right) 
					win.right = desk.right; // - 1;

				if (win.bottom > desk.bottom) 
					win.bottom = desk.bottom; // - 1;

				if (win.left < desk.left) 
					win.left = desk.left; // + 1;

				if (winList[i].sticky)
				{
					RECT sWin;
					RECT sDesk;
					win.bottom = win.bottom - desk.top;
					win.top = win.top - desk.top;
					win.left = win.left - desk.left;
					win.right = win.right - desk.left;

					for (int j = 0; j < desktops; j++)
					{
						sDesk = desktopRect[j];
						sWin.bottom = sDesk.top + win.bottom;
						sWin.top = sDesk.top + win.top;
						sWin.left = sDesk.left + win.left;
						sWin.right = sDesk.left + win.right;

						if (winList[i].active) // draw active window style
						{
							DrawActiveWindow(buf, sWin);
							RemoveFlash(winList[i].window, true);
						}
						else if (IsFlashOn(winList[i].window))
						{
							DrawActiveWindow(buf, sWin);
						}
						else // draw inactive window style
						{
							DrawInactiveWindow(buf, sWin);
							RemoveFlash(winList[i].window, true);
						}

						// Create a tooltip...
						if (desktop.tooltips)
						{
							GetWindowText(winList[i].window, toolText, 255);
							SetToolTip(&sWin, toolText);
						}
					}
				}
				else
				{
					if (winList[i].active) // draw active window style
					{
						DrawActiveWindow(buf, win);
						RemoveFlash(winList[i].window, true);
					}
					else if (IsFlashOn(winList[i].window))
					{
						DrawActiveWindow(buf, win);
					}
					else // draw inactive window style
					{
						DrawInactiveWindow(buf, win);
						RemoveFlash(winList[i].window, true);
					}

					// Create a tooltip...
					if (desktop.tooltips)
					{
						GetWindowText(winList[i].window, toolText, 255);
						SetToolTip(&win, toolText);
					}
				}
			}
		}

		if (winMoving)
		{
			RECT win = moveWin.r;
			RECT client;

			GetClientRect(hwndBBPager, &client);
		
			// This is done so that the window only shows within the pager
			if (win.top < client.top) 
				win.top = client.top; // + 1;

			if (win.right > client.right) 
				win.right = client.right; // - 1;

			if (win.bottom > client.bottom) 
				win.bottom = client.bottom; // - 1;

			if (win.left < client.left) 
				win.left = client.left; // + 1;

			if (moveWin.active) // draw active window style
				DrawActiveWindow(buf, win);
			else // draw inactive window style
				DrawInactiveWindow(buf, win);
		}

	}

	ClearToolTips();

	// Finally, copy from the paint buffer to the window...
	BitBlt(hdc, 0, 0, frame.width, frame.height, buf, 0, 0, SRCCOPY);

    //restore the first previous whatever to the dc,
    //get in exchange back our bitmap, and delete it.
    DeleteObject(SelectObject(buf, oldbmp));

    //delete the memory - 'device context'
    DeleteDC(buf);

    //done
    EndPaint(hwnd, &ps);
}

//===========================================================================

void DrawActiveWindow(HDC buf, RECT r)
{
	COLORREF bColor;

	// Checks for windows just showing on the edges of the screen
	if (r.bottom - r.top < 2)
	{
		if (!_stricmp(focusedWindow.styleType, "border"))
			bColor = focusedWindow.borderColor;
		else
			bColor = window.borderColor;

		HPEN borderPen = CreatePen(PS_SOLID, 1, bColor);
		HPEN oldPen = (HPEN) SelectObject(buf, borderPen);

		MoveToEx(buf, r.left, r.top, NULL);
		LineTo(buf, r.right, r.top);

		SelectObject(buf,oldPen);
		DeleteObject(borderPen);

		//MessageBox(0, "Warning sir!\n\nCan't draw this window as a RECT dude!", "DrawActiveWindow", MB_OK | MB_TOPMOST | MB_SETFOREGROUND);

		return;
	}

	if (r.right - r.left < 2)
	{
		if (!_stricmp(focusedWindow.styleType, "border"))
			bColor = focusedWindow.borderColor;
		else
			bColor = window.borderColor;

		HPEN borderPen = CreatePen(PS_SOLID, 1, bColor);
		HPEN oldPen = (HPEN) SelectObject(buf, borderPen);

		MoveToEx(buf, r.left, r.top, NULL);
		LineTo(buf, r.left, r.bottom);

		SelectObject(buf,oldPen);
		DeleteObject(borderPen);

		return;
	}

	if (!_stricmp(focusedWindow.styleType, "texture"))
	{
		MakeStyleGradient(buf, &r, focusedWindow.ownStyle ? focusedWindow.style : &focusedWindow.Style, false);
		CreateBorder(buf, &r, focusedWindow.borderColor, 1);
	}
	else if (!_stricmp(focusedWindow.styleType, "border"))
	{
		MakeStyleGradient(buf, &r, window.ownStyle ? window.style : &window.Style, false);
		CreateBorder(buf, &r, focusedWindow.borderColor, 1);
	}
	else
		MakeStyleGradient(buf, &r, window.style, true);
}

void DrawInactiveWindow(HDC buf, RECT r)
{
	if (r.bottom - r.top < 2)
	{
		HPEN borderPen = CreatePen(PS_SOLID, 1, window.borderColor);
		HPEN oldPen = (HPEN) SelectObject(buf, borderPen);

		MoveToEx(buf, r.left, r.top, NULL);
		LineTo(buf, r.right, r.top);

		SelectObject(buf,oldPen);
		DeleteObject(borderPen);

		return;
	}

	if (r.right - r.left < 2)
	{
		HPEN borderPen = CreatePen(PS_SOLID, 1, window.borderColor);
		HPEN oldPen = (HPEN) SelectObject(buf, borderPen);

		MoveToEx(buf, r.left, r.top, NULL);
		LineTo(buf, r.left, r.bottom);

		SelectObject(buf,oldPen);
		DeleteObject(borderPen);

		return;
	}

	MakeStyleGradient(buf, &r, window.ownStyle ? window.style : &window.Style, false);
		CreateBorder(buf, &r, window.borderColor, 1);
}

//===========================================================================

void DrawActiveDesktop(HDC buf, RECT r, int i)
{
	if (!_stricmp(activeDesktop.styleType, "border3")) 
	{
		r.right = r.right + 2;
		r.bottom = r.bottom + 2;

		if (activeDesktop.Style.parentRelative)
			CreateBorder(buf, &r, activeDesktop.borderColor, 1);
		else
		{
			MakeStyleGradient(buf, &r, activeDesktop.ownStyle ? activeDesktop.style : &activeDesktop.Style, false);
			CreateBorder(buf, &r, activeDesktop.borderColor, 1);
		}
	}
	else if (!_stricmp(activeDesktop.styleType, "border2")) 
	{
		r.left = r.left - 1;
		r.top = r.top - 1;
		r.right = r.right + 1;
		r.bottom = r.bottom + 1;

		if (!activeDesktop.ownStyle && activeDesktop.Style.parentRelative)
			CreateBorder(buf, &r, activeDesktop.borderColor, 1);
		else
		{
			MakeStyleGradient(buf, &r, activeDesktop.ownStyle ? activeDesktop.style : &activeDesktop.Style, false);
			CreateBorder(buf, &r, desktop.Style.borderColor, 1);
		}
	}
	else if (!_stricmp(activeDesktop.styleType, "border")) 
	{
		if (!activeDesktop.ownStyle && activeDesktop.Style.parentRelative)
			CreateBorder(buf, &r, activeDesktop.borderColor, 1);
		else
		{
			MakeStyleGradient(buf, &r, activeDesktop.ownStyle ? activeDesktop.style : &activeDesktop.Style, false);
			CreateBorder(buf, &r, desktop.Style.borderColor, 1);
		}
	}
	else if (!_stricmp(activeDesktop.styleType, "texture")) 
	{
		if (!activeDesktop.ownStyle && !activeDesktop.Style.parentRelative)
			MakeStyleGradient(buf, &r, activeDesktop.ownStyle ? activeDesktop.style : &activeDesktop.Style, false);
		CreateBorder(buf, &r, activeDesktop.borderColor, 1);
	}
	else if (!desktop.Style.parentRelative) // "none"
		MakeStyleGradient(buf, &r, desktop.ownStyle ? desktop.style : &desktop.Style, false);	

	//desktopRect[i] = r;
	desktopRect.insert(desktopRect.begin() + i, r);
}


