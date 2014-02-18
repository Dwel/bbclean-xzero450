/*
	bbDesktop -- Plugin version of bbDesktopIcons

	Adds desktop icons to your blackbox distribution. Point it to a folder and
	it will render whatever it finds there onto your desktop. Icon positions
	will be saved and remembered.

	(c) 2007, Noccy <dev.noccy.com>

	Distributed under GPL2+
*/


#include <windows.h>
#include <string>
#include <iostream>

//
//  FileIcon class, a generic container for icons. The idea is that you pass
//  it whatever you want the icon for, and it will get it for you, either by
//  directly extracting it from the file or by fetching the proper registry
//  entries and grabbing it from the resource pointed to by the shell icon
//  entry.
//
class FileIcon {

// Protected class members
protected:
	// Icon handles for large and small icons
	HICON hIconLarge;
	HICON hIconSmall;

// Public class members
public:

	// Constructor
	FileIcon(std::string FileAndIndex)
	{
	    int iconIndex;
	    std::string iconFile;

		if (FileAndIndex.find(",") != string::npos)
		{
		    int pos = FileAndIndex.find(",");
			iconFile = FileAndIndex.substr(0,pos);
            iconIndex = ConvertToInteger(FileAndIndex.substr(pos+1,FileAndIndex.length()),false);
            // iconIndex = (long)atol(FileAndIndex.substr(pos+2,FileAndIndex.length()).c_str());
		} else {
            iconIndex = 0;
            iconFile = FileAndIndex;
		}

		ExtractIconEx(iconFile.c_str(), iconIndex, &hIconLarge, &hIconSmall, 1);
	}

	// Destructor
	~FileIcon()
	{
		if (hIconLarge) DestroyIcon(hIconLarge);
		if (hIconSmall) DestroyIcon(hIconSmall);
	}

	// Returns a handle to the large icon extracted
	HICON GetLargeIcon()
	{
		return(hIconLarge);
	}

	// Returns a handle to the small icon extracted
	HICON GetSmallIcon()
	{
		return(hIconSmall);
	}

};


