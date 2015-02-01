/*
 ============================================================================

  This file is part of the bbLeanSkin source code.
  Copyright © 2003-2009 grischka (grischka@users.sourceforge.net)

  bbLeanSkin is a plugin for Blackbox for Windows

  http://bb4win.sourceforge.net/bblean
  http://bb4win.sourceforge.net/


  bbLeanSkin is free software, released under the GNU General Public License
  (GPL version 2) For details see:

    http://www.fsf.org/licenses/gpl.html

  This program is distributed in the hope that it will be useful, but
  WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
  or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License
  for more details.

 ============================================================================
*/
#include "WinInfo.h"

extern HINSTANCE hInstance;
extern unsigned bbSkinMsg;
extern SkinStruct mSkin;

bool GetSkin ();
void subclass_window (HWND hwnd);

LRESULT APIENTRY WindowSubclassProc(HWND, UINT, WPARAM, LPARAM);

