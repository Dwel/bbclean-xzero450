
// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the FOO_SEEK_REMOTE_EXPORTS
// symbol defined on the command line. this symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// FOO_SEEK_REMOTE_API functions as being imported from a DLL, wheras this DLL sees symbols
// defined with this macro as being exported.
#ifdef FOO_SEEK_REMOTE_EXPORTS
#define FOO_SEEK_REMOTE_API __declspec(dllexport)
#else
#define FOO_SEEK_REMOTE_API __declspec(dllimport)
#endif

#pragma warning( disable: 4786 ) // STL

#include <set>

typedef std::set< HWND > THwndSet ;
