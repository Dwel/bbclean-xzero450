/*
	StringUtils.cpp

    Author:     Noccy
    Date:       30-01-07
    Purpose:    Some basic string manipulation functions
*/

#include "stringutils.h"

#include <windows.h>
#include <string>
#include <cctype> // for toupper

using namespace std;

/**
  * Converts string to lower case and returns the result
  * @param String to convert
  */
std::string strlower(std::string str)
{
    for (int i = 0; i < str.length(); i++)
    {
        str[i] = tolower (str[i]);
    }
    return(str);
}

/**
  * Trims the string of leading and trailing spaces
  * @param String to trim
  */
std::string strtrim(std::string str)
{
    string::size_type pos = str.find_last_not_of(' ');
    if (pos != string::npos)
    {
        str.erase(pos + 1);
        pos = str.find_first_not_of(' ');
        if (pos != string::npos) str.erase(0, pos);
    }
    else str.erase(str.begin(), str.end());
    return(str);
}

/**
  * Returns the path part from a string
  * @param String to process
  */
std::string strpath(std::string str)
{
	std::string ret = str.substr( 0, str.find_last_of( '\\' ) );
	return(ret);
}
