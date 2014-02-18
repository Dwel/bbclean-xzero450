//
//	ConfigFile.h
//
//	Purpose:	Reads and writes blackbox style .rc files
//	Author:		Christopher Vagnetoft <lazerfisk AT yahoo DOT com>
//	Date:		2007-02-15
//

#ifndef CONFIGFILE_H
#define CONFIGFILE_H

#include <string>
#include <iostream>
#include <fstream>
#include <vector>

using namespace std;

class ConfigFile
{
	public:
		// Constructor and destructor
		ConfigFile(string filename = "");
		~ConfigFile();
		// Public methods
		bool Open(string filename);
		bool Save(string filename = "");
		bool Close();
		bool Clear();
		bool IsOpen();
		// Reading
		string GetString(string key, string defaultvalue = "");
		bool GetBool(string key, bool defaultvalue = false);
		int GetInt(string key, int defaultvalue = 0);
		// Writing
		void SetString(string key, string value);
		void SetBool(string key, bool value);
		void SetInt(string key, int value);
	private:
		// A few variables we need
		string mFileName;
		bool mIsFileOpen;
		ifstream mFileHandle;
		// Constant...
		static const int cMaxConfigEntries = 2048;
		vector<string> mConfigEntries;
		// Behind the scenes...
		string GetItemByKey(string key);
		bool SetItemByKey(string key, string value);
};

#endif // CONFIGFILE_H
