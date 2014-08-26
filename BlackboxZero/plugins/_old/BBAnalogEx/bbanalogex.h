/*         
 ============================================================================
 Blackbox for Windows: Plugin BBAnalogEx 1.0 by Miroslav Petrasko [Theo] 
 ============================================================================
 Copyright (c) 2001-2004 The Blackbox for Windows Development Team
 http://desktopian.org/bb/ - #bb4win on irc.freenode.net
 ============================================================================
 Cleaningwoman (c) 2014-20?? Mojmir
 http://blackbox4windows.com
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
  For additional license information, please read the included license
 ============================================================================
*/
#pragma once
#include "BBApi.h"

// Plugin functions
void GetStyleSettings ();
void ReadRCSettings ();
void WriteRCSettings ();
void InitBBAnalogEx ();
void mySetTimer ();
void setStatus ();
void createMenu ();
void getCurrentDate ();
void executeAlarm ();
void createAlarmFile ();

extern "C"
{
	__declspec(dllexport) int beginPlugin (HINSTANCE hMainInstance);
	__declspec(dllexport) void endPlugin (HINSTANCE hMainInstance);
	__declspec(dllexport) LPCSTR pluginInfo (int field);
	// This is the function BBSlit uses to load your plugin into the Slit
	__declspec(dllexport) int beginSlitPlugin (HINSTANCE hMainInstance, HWND hBBSlit);
}

