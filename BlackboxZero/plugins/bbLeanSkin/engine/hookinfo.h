/*
 ============================================================================

  This file is part of the bbLeanSkin source code.
  This file is part of the bbLeanSkin+ source code.
  Copyright © 2003-2009 grischka (grischka@users.sourceforge.net)
  Copyright © 2008-2009 The Blackbox for Windows Development Team

  bbLeanSkin is a plugin for Blackbox for Windows
  bbLeanSkin+ is a plugin for Blackbox for Windows

  http://bb4win.sourceforge.net/bblean
  http://bb4win.sourceforge.net/


  bbLeanSkin is free software, released under the GNU General Public License
  bbLeanSkin+ is free software, released under the GNU General Public License
  (GPL version 2) For details see:

    http://www.fsf.org/licenses/gpl.html

  This program is distributed in the hope that it will be useful, but
  WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
  or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License
  for more details.

============================================================================
*/

#include "BBApi.h" // we require the StyleItem definition..

// This file contains information common to both the loader and the skinner

// strings
#define BBLEANSKIN_ENGINEDLL    "bbLeanSkinEng.dll"
#define BBLEANSKIN_INFOPROP     "BBLEANSKIN_INFOPROP"
#define BBLEANSKIN_MSG          "BBLEANSKIN_MSG"
#define BBLEANSKIN_SHMEMID      "BBLEANSKIN_SHMEMID"
#define BBLEANSKIN_RUN32EVENT   "BBLEANSKIN_RUN32EVENT"

// branch versions
#define BBVERSION_LEAN 2
#define BBVERSION_XOB 1
#define BBVERSION_09X 0

#ifndef DLL_EXPORT
#define DLL_EXPORT
#endif

extern "C" DLL_EXPORT int EntryFunc(int option);
// options for 'int EntryFunc(int option);'
#define ENGINE_SETHOOKS 0
#define ENGINE_UNSETHOOKS 1
#define ENGINE_SKINWINDOW 2
#define ENGINE_GETVERSION 3

/* increment this if you make changes to the data or structures below.
   This makes shure the user loads the correct bbLeanSkinEng.dll */
#define ENGINE_THISVERSION 1170

// wParams for the registered 'BBLEANSKIN_MSG'
#define BBLS_REDRAW            1
#define BBLS_SETSTICKY         2
#define BBLS_GETSHADEHEIGHT    3
#define BBLS_REFRESH           4
#define BBLS_LOAD              5
#define BBLS_UNLOAD            6
#define BBLS_SETONBG           7

#pragma pack(push,4)

// ---------------------------------------------
// To pass information when showing the window
// menu

struct sysmenu_info {
    HWND hwnd;
#ifndef _WIN64
    int _pad32;
#endif
    RECT rect;
};

// ---------------------------------------------
struct exclusion_item
{
    unsigned char flen, clen, option;
    char buff[2];
};

struct exclusion_info
{
    int size;
    int count;
    struct exclusion_item ei[1];
};

#define GLYPH_MAX_SIZE 17
#define ONE_GLYPH(sx,sy) ((sx*sy+7)/8)
#define BUTTON_COUNT 7

// ---------------------------------------------
struct windowGradients
{
    StyleItem Title;
    StyleItem Label;
    StyleItem Handle;
    StyleItem Grip;
    StyleItem Button;
    StyleItem ButtonP;
    COLORREF FrameColor;
};

struct SkinStruct
{
    struct windowGradients U;
    struct windowGradients F;

    LOGFONT Font;
    int Justify; // DT_LEFT/CENTER/RIGHT

    int frameWidth;
    int handleHeight;

    int ncTop;
    int ncBottom;
    int rollupHeight;

    int gripWidth;
    int labelHeight;
    int labelMargin;
    int labelIndent;
	int labelBarrier;

    int buttonSize;
    int buttonSpace;
    int buttonMargin;
    int buttonInnerMargin;

    unsigned char glyphmap[2+BUTTON_COUNT*2*ONE_GLYPH(GLYPH_MAX_SIZE,GLYPH_MAX_SIZE)];
    char button_string[32];

    struct {
        char Dbl[4];
        char Right[4];
        char Mid[4];
        char Left[4];
    } captionClicks;

    int snapWindows;
    bool enableLog;
    bool nixShadeStyle;
    int imageDither;
    bool drawLocked;
    bool is_style070;
    char bbsm_option;

    int iconSat;
    int iconHue;

    int cxSizeFrame;
    int cxFixedFrame;
    int cyCaption;
    int cySmCaption;

    int BBVersion;

    HWND BBhwnd;
#ifndef _WIN64
    int _pad32_1;
#endif

    HWND loghwnd;
#ifndef _WIN64
    int _pad32_2;
#endif

    struct exclusion_info exInfo;
};

#define offset_exInfo ((size_t)&((SkinStruct*)NULL)->exInfo)

#pragma pack(pop)

// ---------------------------------------------

