/* C++ API adaptor to call from C */

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <BBApi.h>

#include "cpp_adaptor.h"




extern "C" {


DLL_EXPORT void
Cpp_MakeStyleGradient(HDC hdc, RECT* rect, void* style, BOOL border)
{
	MakeStyleGradient(hdc, rect, (StyleItem*)style, border);
}


DLL_EXPORT COLORREF
Cpp_StyleItem_TextColor(void* style)
{
	return ((StyleItem*)style)->TextColor;
}


DLL_EXPORT HFONT
Cpp_CreateStyleFont(void* style)
{
	return CreateStyleFont((StyleItem*)style);
}


}




/* __END__ */
