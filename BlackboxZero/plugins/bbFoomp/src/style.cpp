#include "style.h"

// Style items
StyleItem OuterStyle, InnerStyle, ButtonStyle;
COLORREF ShadowColor;

COLORREF GetShadowColor (StyleItem & style)
{
	return (style.validated & VALID_SHADOWCOLOR) ?	style.ShadowColor : MakeShadowColor(style);
}


void GetStyleSettings()
{
	int style_assoc[] =
	{ 
		SN_TOOLBARLABEL,
		SN_TOOLBARWINDOWLABEL,
		SN_TOOLBARCLOCK,
		SN_TOOLBAR,
		SN_TOOLBARBUTTON,
		SN_TOOLBARBUTTONP
	};

	StyleItem * os = (StyleItem *)GetSettingPtr(style_assoc[getSettings().OuterStyleIndex-1]);
	StyleItem * is = (StyleItem *)GetSettingPtr(style_assoc[getSettings().InnerStyleIndex-1]);
	StyleItem * toolbar = (StyleItem *)GetSettingPtr(SN_TOOLBAR);
	StyleItem * toolbarButtonPressed = (StyleItem *)GetSettingPtr(SN_TOOLBARBUTTONP);

	OuterStyle = os->parentRelative ? *toolbar : *os;
	InnerStyle = is->parentRelative ? *toolbar : *is;
	ButtonStyle = *toolbarButtonPressed;



	// Check if the title needs updating, and if so, update it.
	UpdateTitle();

//	SIZE size;
	HDC fonthdc = CreateDC("DISPLAY", NULL, NULL, NULL);
	HFONT font = CreateStyleFont(toolbar);
	HGDIOBJ oldfont = SelectObject(fonthdc, font);
//	GetTextExtentPoint32(fonthdc, foobarWnd, 32, &size);
	getSettings().width = getSettings().FooWidth;
	DeleteObject(SelectObject(fonthdc, oldfont));
	DeleteDC(fonthdc);
}


