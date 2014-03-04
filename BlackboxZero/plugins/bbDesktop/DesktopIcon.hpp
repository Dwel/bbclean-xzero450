/*
	bbDesktop -- Plugin version of bbDesktopIcons

	Adds desktop icons to your blackbox distribution. Point it to a folder and
	it will render whatever it finds there onto your desktop. Icon positions
	will be saved and remembered.

	(c) 2007, Noccy <dev.noccy.com>

	Distributed under GPL2+
*/


#include <windows.h>
#include <wingdi.h>
#include <string>
#include <iostream>
#include "IconManager.hpp"

//
//  The DesktopIcon class is responsible for rendering an icon on the desktop.
//  It should fetch the icon and the title in a proper way and from that point
//  on take care of drawing and management.
//
class DesktopIcon {

// Protected class members
protected:

	// Our icons current placement
	int x;
	int y;
	// Title of the icon
	std::string title;
	// The icon that's in use (instance of FileIcon)
	FileIcon* icon;
	// Font handle
	HFONT hIconFont;

    // Stuff we need to execute the program or item.
	std::string command;
	std::string parameters;
	std::string workdir;

	bool selected;

// Public class members
public:

	// Constructor, takes the arguments needed to create the icon.
	DesktopIcon(std::string IconTitle, std::string command, std::string parameters, std::string workdir, std::string IconFile, int x, int y)
	{

		// Assign intial parameters
		this->icon = new FileIcon(IconFile);
		this->hIconFont = CreateFont(12,0,0,0,0,0,0,0,0,0,0,0,0,"Tahoma");
		//this->hIconFont = (HFONT)GetStockObject(SYSTEM_FONT);
		this->title = IconTitle; // "Dolly is a white sheep";
		this->command = command;
		this->parameters = parameters;
		this->workdir = workdir;
		this->x = x;
		this->y = y;

		/*HDC hdc = GetDC(0);
		hIconFont = (HFONT)GetCurrentObject(hdc, OBJ_FONT);
		ReleaseDC(0, hdc);*/

	}

	// Destructor, destroys the icons and
	~DesktopIcon()
	{

		delete(icon);

	}

	// This function creates a simple window with alpha opacity set (if this
	// is supported by the current OS. This highlight is destroyed by MoveTo()
	// or DestroyDragHighlight()
	void CreateDragHighlight()
	{
	}

	// This moves the highlight to the proper location
	void MoveHighlightTo(int x, int y)
	{
	}

	// This destroys the highlight again
	void DestroyDragHighlight()
	{
	}

	// Moves the icon definitely to a new location
	void MoveTo(int x, int y)
	{
	}

	// Returns true if mouse click is in icon rectangle
	bool IsMouseIn(int x, int y)
	{
        // Is the click within the icon container?
        if ((x >= this->x) && (x <= this->x + 32) && (y >= this->y) && (y <= this->y + 32))
        {
            return(true);
        }

		return(false);
	}

	void Select(bool state)
	{
		selected = state;
		this->Paint(GetBlackboxDC());
	}

	// Paints the icon and the caption
	void Paint(HDC hDesktop)
	{

		// Now fetch the device context to paint to and draw the icon
		DrawIcon(hDesktop, x, y, icon->GetLargeIcon());

		// Create the rect for our text
		RECT drect;
		drect.left = x + ( 32 / 2 ) - (Settings.MaxIconLabelWidth / 2);
		drect.top = y + 35;
		drect.right = drect.left + (Settings.MaxIconLabelWidth);
		drect.bottom = drect.top + (Settings.MaxIconLabelWidth);

		// Set color and background mode
		/*if (!selected) {*/
			SetBkMode(hDesktop, TRANSPARENT);
		/*} else {
			SetBkMode(hDesktop, OPAQUE);
			SetBkColor(hDesktop, 0x800000);
		}*/

		// Output the text
		HFONT hOldFont = (HFONT)SelectObject(hDesktop, hIconFont);

        // Draw the effect first
        if (Settings.Effect == EF_SHADOW)
        {
            SetTextColor(hDesktop, Settings.BackColor);
            drect.left++; drect.top++; drect.right++; drect.bottom++;
            DrawTextEx(hDesktop, (CHAR*)title.c_str(), title.length(), &drect, DT_CENTER | DT_NOCLIP | DT_WORDBREAK, NULL);
            drect.left--; drect.top--; drect.right--; drect.bottom--;
        }
        else if (Settings.Effect == EF_OUTLINE)
        {
            SetTextColor(hDesktop, Settings.BackColor);
            int xpan, ypan;
            RECT outline;
            for (xpan = -1; xpan <= 1; xpan++) {
                for (ypan = -1; ypan <= 1; ypan++) {
                    outline.left = drect.left + xpan;
                    outline.top = drect.top + ypan;
                    outline.right = drect.right + xpan;
                    outline.bottom = drect.bottom + ypan;
                    DrawTextEx(hDesktop, (CHAR*)title.c_str(), title.length(), &outline, DT_CENTER | DT_NOCLIP | DT_WORDBREAK, NULL);
                }
            }
        }

        // Then drop the text
		SetTextColor(hDesktop, Settings.ForeColor);
		DrawTextEx(hDesktop, (CHAR*)title.c_str(), title.length(), &drect, DT_CENTER | DT_NOCLIP | DT_WORDBREAK, NULL);

		SelectObject(hDesktop, hOldFont);


	}

	// Invokes the icon command
	void Invoke()
	{

        ShellExecute(NULL,"open",this->command.c_str(),this->parameters.c_str(),this->workdir.c_str(),SW_SHOW);
		dbg_printf("Icon %s invoked!",this->title.c_str());

	}

};
