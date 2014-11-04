#include "SearchItem.h"
#include "../BB.h"
#include "../Settings.h"
#include "Menu.h"
#include "RecentItem.h"

MenuItem * MakeMenuItemSearch(Menu * PluginMenu, const char * Title, const char * Cmd, const char * init_string)
{
  return helper_menu(PluginMenu, Title, MENU_ID_STRING, new SearchItem(Cmd, init_string));
}

SearchItem::SearchItem (const char* pszCommand, const char *init_string)
    : StringItem(pszCommand, init_string)
    , m_hText(0)
    , m_wpEditProc(0)
    , m_textrect()
{
}

SearchItem::~SearchItem ()
{
    if (m_hText)
    {
        DestroyWindow(m_hText);
        m_hText = NULL;
    }
}

void SearchItem::Paint (HDC hDC)
{
    RECT r;
    HFONT hFont;
    int x, y, w, h, padd;
    if (Settings_menu.showBroams)
    {
        if (m_hText)
        {
            DestroyWindow(m_hText);
            m_hText = NULL;
        }
        m_Justify = MENUITEM_STANDARD_JUSTIFY;
        MenuItem::Paint(hDC);
        return;
    }

    m_Justify = MENUITEM_CUSTOMTEXT;
    MenuItem::Paint(hDC);

    GetTextRect(&r);
    if (EqualRect(&m_textrect, &r))
        return;

    m_textrect = r;

    if (NULL == m_hText)
    {
        m_hText = CreateWindow(
            //TEXT("EDIT"),
            TEXT("COMBOBOX"),
            m_pszTitle,
            WS_CHILD
            | WS_VISIBLE
            | ES_AUTOHSCROLL
            | ES_MULTILINE,
            0, 0, 0, 0,
            m_pMenu->m_hwnd,
            (HMENU)1234,
            hMainInstance,
            NULL
            );

        SetWindowLongPtr(m_hText, GWLP_USERDATA, (LONG_PTR)this);
        m_wpEditProc = (WNDPROC)SetWindowLongPtr(m_hText, GWLP_WNDPROC, (LONG_PTR)EditProc);
#if 0
        int n = GetWindowTextLength(m_hText);
        SendMessage(m_hText, EM_SETSEL, 0, n);
        SendMessage(m_hText, EM_SCROLLCARET, 0, 0);
#endif
        m_pMenu->m_hwndChild = m_hText;
        if (GetFocus() == m_pMenu->m_hwnd)
            SetFocus(m_hText);
    }

    hFont = MenuInfo.hFrameFont;
    SendMessage(m_hText, WM_SETFONT, (WPARAM)hFont, 0);

    x = r.left - 1;
    y = r.top + 2;
    h = r.bottom - r.top - 4;
    w = r.right - r.left + 2;

    SetWindowPos(m_hText, NULL, x, y, w, h, SWP_NOZORDER);

    padd = imax(0, (h - get_fontheight(hFont)) / 2);
    r.left  = padd+2;
    r.right = w - (padd+2);
    r.top   = padd;
    r.bottom = h - padd;
    SendMessage(m_hText, EM_SETRECT, 0, (LPARAM)&r);
}

void SearchItem::Measure (HDC hDC, SIZE * size, StyleItem * pSI)
{
    StringItem::Measure(hDC, size, pSI);
}

void SearchItem::Invoke (int button)
{
	if (button == INVOKE_RET)
		OnInput();
}

Menu * MakeResultMenu(Menu * parent, std::vector<std::string> const & res)
{
	char broam[1024];
	char text[1024];
	for (size_t i = 0, ie = res.size(); i < ie; ++i)
	{
		_snprintf_s(broam, 1024, "@BBCore.Exec %s", res[i].c_str());
		_snprintf_s(text, 1024, "%s", res[i].c_str());
		MenuItem * mi = MakeMenuItem(parent, text, broam, false);
		MenuItemOption(mi, BBMENUITEM_JUSTIFY, DT_RIGHT);
	}
	return parent;
}	

void SearchItem::OnInput ()
{
	int const len = SendMessage(m_hText, WM_GETTEXTLENGTH, 0, 0);
	char * buffer = static_cast<char *>(alloca(sizeof(char) * (len + 1)));
	SendMessage(m_hText, WM_GETTEXT, (WPARAM)len + 1, (LPARAM)buffer);

    // if old != new
    m_results.clear();
    m_results.push_back(TEXT("ONE"));
    m_results.push_back(TEXT("2"));
    m_results.push_back(TEXT("3"));

	Menu * menu = MakeNamedMenu(NLS0("Search results"), "Search_results", true);
	MakeResultMenu(menu, m_results);
	MenuOption(menu, BBMENU_MAXWIDTH | BBMENU_CENTER | BBMENU_PINNED | BBMENU_ONTOP, 512);
	ShowMenu(menu);

	//    if (pRect)
	//MenuOption(m, BBMENU_RECT | BBMENU_NOTITLE | BBMENU_SYSMENU, pRect);
}

//===========================================================================
LRESULT CALLBACK SearchItem::EditProc(HWND hText, UINT msg, WPARAM wParam, LPARAM lParam)
{
    SearchItem * pItem = reinterpret_cast<SearchItem *>(GetWindowLongPtr(hText, GWLP_USERDATA));
   LRESULT r = 0;
    Menu * pMenu = pItem->m_pMenu;

    pMenu->incref();
    switch(msg)
    {
        // Send Result
        case WM_MOUSEMOVE:
            PostMessage(pMenu->m_hwnd, WM_MOUSEMOVE, wParam, MAKELPARAM(10, pItem->m_nTop+2));
            break;

        // Key Intercept
        case WM_KEYDOWN:
        {
            switch (wParam)
            {
                case VK_DOWN:
                case VK_UP:
                case VK_TAB:
                    pItem->Invoke(0);
                    pItem->next_item(wParam);
                    goto leave;

                case VK_RETURN:
                    pItem->Invoke(INVOKE_RET);
                    pItem->next_item(0);
                    goto leave;

                case VK_ESCAPE:
                    SetWindowText(hText, pItem->m_pszTitle);
                    pItem->next_item(0);
                    goto leave;
            }
            //pItem->OnInput();
            break;
        }
        case WM_CHAR:
            switch (wParam)
            {
                case 'A' - 0x40: // ctrl-A: select all
                    SendMessage(hText, EM_SETSEL, 0, GetWindowTextLength(hText));
                case 13:
                case 27:
                    goto leave;
            }
            break;

        // --------------------------------------------------------
        // Paint

        case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc;
            RECT r;
            StyleItem *pSI;

            hdc = BeginPaint(hText, &ps);
            GetClientRect(hText, &r);
            pSI = &mStyle.MenuFrame;
            MakeGradientEx(hdc, r, pSI->type, pSI->Color, pSI->ColorTo, pSI->ColorSplitTo, pSI->ColorToSplitTo,
                pSI->interlaced, BEVEL_SUNKEN, BEVEL1, 0, 0, 0);
            CallWindowProc(pItem->m_wpEditProc, hText, msg, (WPARAM)hdc, lParam);
            EndPaint(hText, &ps);
            goto leave;
        }

        case WM_ERASEBKGND:
            r = TRUE;
            goto leave;

        case WM_DESTROY:
            pItem->hText = NULL;
            pMenu->m_hwndChild = NULL;
            break;

        case WM_SETFOCUS:
            break;

        case WM_KILLFOCUS:
            pItem->Invoke(0);
            break;
    }
    r = CallWindowProc(pItem->m_wpEditProc, hText, msg, wParam, lParam);
leave:
    pMenu->decref();
    return r;
}


