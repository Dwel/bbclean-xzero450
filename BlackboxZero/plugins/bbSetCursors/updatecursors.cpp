/*
    UpdateCursors.cpp

    Copyright:  (c) 2007, Noccy
    Author:     Noccy
    Date:       29-01-07 03:14
	Purpose:    Sets and updates the cursor theme according to a .ct file
*/

#include <windows.h>
#include <direct.h>
#include "Registry.h"
#include "stringutils.h"
#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>
#include "UpdateCursors.h"
#include "ConfigFile.h"

using namespace std;

// Not present in SDK, so we define it here (if not already defined)
#ifndef SPI_SETCURSORS
#define SPI_SETCURSORS              0x0057
#endif

enum CURSORTYPES {
    CUR_DEFAULT,
    CUR_ARROWWAIT,
    CUR_HOURGLASS,
    CUR_SIZENS,
    CUR_SIZEWE,
    CUR_SIZED1,
    CUR_SIZED2,
    CUR_SIZEALL,
    CUR_TEXT,
    CUR_PRECISION,
    CUR_HAND,
    CUR_STOP,
    CUR_OPTION
};

char* RegistryKey[] = {
    "Arrow",
    "AppStarting",
    "Wait",
    "SizeNS",
    "SizeWE",
    "SizeNWSE",
    "SizeNESW",
    "SizeAll",
    "IBeam",
    "Crosshair",
    "Hand",
    "No",
    "UpArrow"
};

char* FileValues[] = {
	"default",
	"arrowwait",
	"hourglass",
	"sizens",
	"sizewe",
	"sizenwse",
	"sizenesw",
	"sizeall",
	"text",
	"precision",
	"hand",
	"stop",
	"option"
};

#define CursorCount 12
std::string l_cursors[CursorCount];




/*
    Writes cursor info to the registry and signals a refresh of the active cursors
*/
bool applyCursors()
{
	// Go over our collection of cursrors, and see which ones are present. These
	// will be written to the registry using our registry class
	registry reg;
	cout << "Applying cursors..." << endl;
	if (reg.openKey(HKEY_CURRENT_USER,"Control Panel\\Cursors"))
	{
		for (int n = 0; n < CursorCount; n++)
		{
			// Check that we actually have a *value* here...
			if (l_cursors[n] != "")
			{
				if (!reg.set_value(RegistryKey[n],l_cursors[n]))
					cout << "Couldnt write registry key " << RegistryKey[n] << " (" << l_cursors[n] << endl;
			}
		}
		reg.closeKey();
		SystemParametersInfo(SPI_SETCURSORS, 0, 0, 0);
		return(0);
	}
	else
	{
		cout << "Failed to open registry key!" << endl;
	}
}

/*
   Reads file information from the specified file. Returns true on success.
*/
bool readFileInfo(std::string filename)
{
    string curmatch;
    string lookfor;
    string cursor;

    // Open the file
    ConfigFile stylefile(filename.c_str());

    // If the file could be opened, read the information and signal the user
    if (stylefile.IsOpen())
    {
        cout << "Reading cursor theme..." << endl;
        // Read the file line by line
		for (int n = 0; n < CursorCount; n++) {
			curmatch = FileValues[n];
			lookfor = "cursor." + curmatch;
			cursor = stylefile.GetString(lookfor);
			if (cursor != "")
			{
				// set the cursor
				l_cursors[n] = CompletePath(cursor);
				cout << " - " << FileValues[n] << ": " << cursor << endl;
				if (!file_exists(l_cursors[n]))
				{
					cout << "Error: Resource could not be found!" << endl;
					return(false);
				}
			}
		}
        return(true); // return true to signal success
    }
    else
    {
        // File couldnt be opened. Tell the user, and return false.
        cout << "Unable to open file: " << filename << endl;
        return(false);
    }
}


std::string GetCurrentDir(void)
{
	char buff[MAX_PATH];
	std::string retval;

	#ifdef __PLUGIN
	if GetModuleFileName(hInst, buff, sizeof(buff))
	{
		retval = strpath(buff);
		// retval = retval + "\\"; // append a backslash to the end...
	}
	#else
	if (_getcwd(buff, MAX_PATH))
	{
		retval = buff;
		retval = retval + "\\";	// append a backslash to the end...
	}
	#endif

	return(retval);
}

std::string CompletePath(std::string path)
{
	std::string retstr;
	std::string curdir = GetCurrentDir();

	// Here we first check if it is indeed a relative path
	if (IsRelativePath(path))
	{
		// Aye, so we just push the current dir onto the path
		retstr = curdir + path;
	} else {
		// No its not. Is the first character a backslash? In that case we just
		// append the current drive letter
		if (path[0] == '\\')
		{
			retstr = curdir.substr(0,2) + path;
		}
		else
		{
			retstr = path;
		}
	}
	return(retstr);
}

bool IsRelativePath(std::string path)
{
	// Return true if this is a relative path... If the first character is a backslash
	// or the path contains a colon, lets assume it is NOT.
	return((path[1] != ':' && path[0] != '\\'));
	//if ((path.find_first_not_of(':') != string::npos) || (path.substr(0,1) == "\\"))
	//	return(false);
	//return(true);
}

bool file_exists( std :: string& file )
{
	bool ret = false;
	WIN32_FIND_DATA lpFindFileData;
	HANDLE h = FindFirstFile( file.c_str(), &lpFindFileData );
	if ( h != INVALID_HANDLE_VALUE )
		ret = true;
	FindClose( h );
	return ret;
}
