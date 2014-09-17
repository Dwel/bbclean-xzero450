//
//	ConfigFile.cpp
//
//	Purpose:	Reads and writes blackbox style .rc files
//	Author:		Christopher "Noccy" Vagnetoft <lazerfisk AT yahoo DOT com>
//	Date:		2007-02-15
//

#include "ConfigFile.h"
#include "Convert.hpp"
#include "StringUtils.h"

using namespace std;

//============================================================================
//
//	Constructor and Destructor
//
ConfigFile::ConfigFile(string filename)
{
	// Check if we got a filename passed...
	if (filename != "") {
		// ...and if we did, open it
		(*this).Open(filename);
	}
}

ConfigFile::~ConfigFile()
{
	// Really nothing to do here... Yet...
}

//============================================================================
//
//	File I/O
//
bool ConfigFile::Open(string filename)
{
	// Open the file and read it all
    ifstream rcfile(filename.c_str());
    string line;

    if (rcfile.is_open())
    {
        while (! rcfile.eof() )
        {
            getline (rcfile,line);	// Can this be replaced with line << myfile?
            mConfigEntries.push_back(line);
        }
        mIsFileOpen = true;
    }
    else
	{
		mIsFileOpen = false;
    }
	// we also need to stove away the filename, so it can be used for saving...
	mFileName = filename;
    rcfile.close();
	return(mIsFileOpen);
}

bool ConfigFile::Save(string filename)
{
	// Save the file...
	std::string lFileName = filename;

	// Fetch the stored filename if we didn't get one passed...
	if (lFileName == "") lFileName = mFileName;

	if (lFileName != "")
	{
		ofstream rcfile(lFileName.c_str());
		if (rcfile.is_open())
		{
			for(size_t i = 0; i < mConfigEntries.size(); i++)
			{
				rcfile << mConfigEntries[i] << endl;
			}
		}
		rcfile.close();
		// Save the filename...
		mFileName = lFileName;
		return true;
	}
	return false;
}

bool ConfigFile::Close()
{
	mIsFileOpen = false;
	mFileName = "";
	(*this).Clear();
	return(true);
}

bool ConfigFile::IsOpen()
{
	return(mIsFileOpen);
}

bool ConfigFile::Clear()
{
	// Clear the vector
	 mConfigEntries.clear();
	 return(true);
}


//============================================================================
//
//	Reading of values
//
string ConfigFile::GetString(string key, string defaultvalue)
{
	// Get string value
	string value = (*this).GetItemByKey(key);
	if (value == "") {
		return(defaultvalue);
	} else {
		return(value);
	}
}

bool ConfigFile::GetBool(string key, bool defaultvalue)
{
	// Get boolean value
	string value = (*this).GetItemByKey(key);
	if (value == "") {
		return(defaultvalue);
	} else {
		return((value == "true")?true:false);
	}
}

int ConfigFile::GetInt(string key, int defaultvalue)
{
	// Get integer value
	string value = (*this).GetItemByKey(key);
	if (value != "") {
		return(ConvertToInteger(value,false));
	} else {
		return(defaultvalue);
	}
}


//============================================================================
//
//	Writing of values
//
void ConfigFile::SetString(string key, string value)
{
	// Set string value
	if ((*this).GetItemByKey(key) != "")
	{
		(*this).SetItemByKey(key,value);
	}
	else
	{
		string data = key + ": " + value;
		mConfigEntries.push_back(data);
	}
}

void ConfigFile::SetBool(string key, bool value)
{
	// Set boolean value
	if ((*this).GetItemByKey(key) != "")
	{
		(*this).SetItemByKey(key,(value?"true":"false"));
	}
	else
	{
		string data = key + ": " + (string)(value?"true":"false");
		mConfigEntries.push_back(data);
	}
}

void ConfigFile::SetInt(string key, int value)
{
	// Set int value
	if ((*this).GetItemByKey(key) != "")
	{
		(*this).SetItemByKey(key,ConvertToString(value));
	}
	else
	{
		string data = key + ": " + ConvertToString(value);
		mConfigEntries.push_back(data);
	}

}


//============================================================================
//
//	Used internally -- Returns a string from the collection... Placed in a
//	separate function to make the GetString function look nicer ;)
//
string ConfigFile::GetItemByKey(string key)
{
	string line;
	string linekey;
	string linedata;
    long linepos;
	// Go over every item in the list until one is found
	for(size_t i = 0; i < mConfigEntries.size(); i++)
	{
		line = mConfigEntries[i];
		linepos = line.find(":");
		if (linepos != string::npos)
		{
			linekey = strtrim(strlower(line.substr(0,linepos)));
			linedata = strtrim(line.substr(linepos+2,line.length()));
			if (linekey == key) { return(linedata); }
		}
	}
	return("");
}

//
//	And this one sets a value... interestingly enough :)
//
bool ConfigFile::SetItemByKey(string key, string value)
{
	string line;
	string linekey;
	string linedata;
    long linepos;
	// Go over every item in the list until one is found
	for(size_t i = 0; i < mConfigEntries.size(); i++)
	{
		line = mConfigEntries[i];
		linepos = line.find(":");
		if (linepos != string::npos)
		{
			linekey = strtrim(strlower(line.substr(0,linepos)));
			linedata = strtrim(line.substr(linepos+2,line.length()));
			if (linekey == key) {
				mConfigEntries[i] = value;
				return(true);
			}
		}
	}
	return(false);
}
