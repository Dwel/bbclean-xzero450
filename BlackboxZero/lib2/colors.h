#pragma once
#include "bbapi.h"

BBLIB_EXPORT COLORREF rgb (unsigned r, unsigned g, unsigned b);
BBLIB_EXPORT COLORREF switch_rgb (COLORREF c);
BBLIB_EXPORT COLORREF mixcolors (COLORREF c1, COLORREF c2, int f);
BBLIB_EXPORT COLORREF shadecolor (COLORREF c, int f);
BBLIB_EXPORT unsigned greyvalue (COLORREF c);

/* ------------------------------------------------------------------------- */
/* Function: ParseLiteralColor */
/* Purpose: Parses a given literal color and returns the hex value */
BBLIB_EXPORT COLORREF ParseLiteralColor (LPTSTR color);
/* ------------------------------------------------------------------------- */
/* Function: ReadColorFromString */
/* Purpose: parse a literal or hexadecimal color string */
BBLIB_EXPORT COLORREF ReadColorFromString (TCHAR const * string);

