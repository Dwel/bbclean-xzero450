#pragma once
#include <BBAPI.h>

struct Styles
{
	// Style items
	StyleItem OuterStyle, InnerStyle, ButtonStyle;
	COLORREF ShadowColor;

	void GetStyleSettings ();
};

Styles & getStyles ();
COLORREF GetShadowColor (StyleItem & style);
