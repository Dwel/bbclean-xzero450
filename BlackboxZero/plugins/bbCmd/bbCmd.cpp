#include "BBCmd.h"
#include "utils.h"
#include "plugin.h"
#include <lib/bblib.h>

LRESULT CALLBACK WndProc (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

int BBCmd::Init ()
{
    /* --------------------------------------------------- */
    /* Zero out variables, read configuration and style */
    memset(&m_config, 0, sizeof(Config));
    ReadRCSettings();
    GetStyleSettings();

    /* --------------------------------------------------- */
    /* create the window */

    m_config.hwnd = CreateWindowEx(
        WS_EX_TOOLWINDOW,   /* window ex-style */
        szAppName,          /* window class name */
        NULL,               /* window caption text */
        WS_POPUP | WS_CLIPCHILDREN | WS_CLIPSIBLINGS, /* window style */
        0,                  /* x position */
        0,                  /* y position */
        0,                  /* window width */
        0,                  /* window height */
        NULL,               /* parent window */
        NULL,               /* window menu */
        g_hInstance,        /* hInstance of .dll */
        NULL                /* creation data */
        );

    /* set window location and properties */
    set_window_modes();

    /* show window (without stealing focus) */
    ShowWindow(m_config.hwnd, SW_SHOWNA);
    return 0; /* 0 = success */
}

void BBCmd::Done ()
{
    /* Get out of the Slit, in case we are... */
    if (m_config.is_inslit)
        SendMessage(g_hSlit, SLIT_REMOVE, 0, (LPARAM)m_config.hwnd);

    /* Destroy the window... */
    DestroyWindow(m_config.hwnd);

    /* clean up HBITMAP object */
    if (m_config.bufbmp)
        DeleteObject(m_config.bufbmp);

    /* clean up HFONT object */
    if (m_config.hFont)
        DeleteObject(m_config.hFont);
}

/* this invalidates the window, and resets the bitmap at the same time. */
void BBCmd::invalidate_window ()
{
    if (m_config.bufbmp)
    {
        // delete the double buffer bitmap (if we have one), so it will be drawn again next time with WM_PAINT
        DeleteObject(m_config.bufbmp);
        m_config.bufbmp = NULL;
    }
    // notify the OS that the window needs painting
    InvalidateRect(m_config.hwnd, NULL, FALSE);
}

LRESULT BBCmd::WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    static int msgs[] = { BB_RECONFIGURE, BB_BROADCAST, 0};

    switch (message)
    {
        case WM_CREATE:
            /* Register to reveive these message */
            SendMessage(g_BBhwnd, BB_REGISTERMESSAGE, (WPARAM)hwnd, (LPARAM)msgs);
            /* Make the window appear on all workspaces */
            MakeSticky(hwnd);
            break;

        case WM_DESTROY:
            /* as above, in reverse */
            RemoveSticky(hwnd);
            SendMessage(g_BBhwnd, BB_UNREGISTERMESSAGE, (WPARAM)hwnd, (LPARAM)msgs);
            break;

        /* ---------------------------------------------------------- */
        /* Blackbox sends a "BB_RECONFIGURE" message on style changes etc. */

        case BB_RECONFIGURE:
            ReadRCSettings();
            GetStyleSettings();
            set_window_modes();
            break;

        /* ---------------------------------------------------------- */
        /* Painting directly on screen. Good enough for static plugins. */
#if 1
        case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc;
            RECT r;

            /* get screen DC */
            hdc = BeginPaint(hwnd, &ps);

            /* Setup the rectangle */
            r.left = r.top = 0;
            r.right = m_config.width;
            r.bottom =  m_config.height;

            /* and paint everything on it*/
            paint_window(hdc, &r);

            /* Done */
            EndPaint(hwnd, &ps);
            break;
        }

        /* ---------------------------------------------------------- */
        /* Painting with a cached double-buffer. If your plugin updates
           frequently, this avoids flicker */
#else
        case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc, hdc_buffer;
            HGDIOBJ otherbmp;
            RECT r;

            /* get screen DC */
            hdc = BeginPaint(hwnd, &ps);

            /* create a DC for the buffer */
            hdc_buffer = CreateCompatibleDC(hdc);

            if (NULL == m_config.bufbmp) /* No bitmap yet? */
            {
                /* Make a bitmap ... */
                m_config.bufbmp = CreateCompatibleBitmap(hdc, m_config.width, m_config.height);

                /* ... and select it into the DC, saving the previous default. */
                otherbmp = SelectObject(hdc_buffer, m_config.bufbmp);

                /* Setup the rectangle */
                r.left = r.top = 0;
                r.right = m_config.width;
                r.bottom =  m_config.height;

                /* and paint everything on it*/
                paint_window(hdc_buffer, &r);
            }
            else
            {
                /* Otherwise it has been painted already,
                   so just select it into the DC */
                otherbmp = SelectObject(hdc_buffer, m_config.bufbmp);
            }

            /* Copy the buffer on the screen, within the invalid rectangle: */
            BitBltRect(hdc, hdc_buffer, &ps.rcPaint);

            /* Put back the previous default bitmap */
            SelectObject(hdc_buffer, otherbmp);
            /* clean up */
            DeleteDC(hdc_buffer);

            /* Done. */
            EndPaint(hwnd, &ps);
            break;
        }
#endif
        /* ---------------------------------------------------------- */
        /* Manually moving/sizing has been started */

        case WM_ENTERSIZEMOVE:
            m_config.is_moving = true;
            break;

        case WM_EXITSIZEMOVE:
            if (m_config.is_moving)
            {
                if (m_config.is_inslit)
                {
                    /* moving in the slit is not really supported but who
                       knows ... */
                    SendMessage(g_hSlit, SLIT_UPDATE, 0, (LPARAM)hwnd);
                }
                else
                {
                    /* if not in slit, record new position */
                    WriteInt(m_rcpath, RC_KEY("xpos"), m_config.xpos);
                    WriteInt(m_rcpath, RC_KEY("ypos"), m_config.ypos);
                }

                if (m_config.is_sizing)
                {
                    /* record new size */
                    WriteInt(m_rcpath, RC_KEY("width"), m_config.width);
                    WriteInt(m_rcpath, RC_KEY("height"), m_config.height);
                }
            }
            m_config.is_moving = m_config.is_sizing = false;
            set_window_modes();
            break;

        /* --------------------------------------------------- */
        /* snap to edges on moving */

        case WM_WINDOWPOSCHANGING:
            if (m_config.is_moving)
            {
                WINDOWPOS* wp = (WINDOWPOS*)lParam;
                if (m_config.snapWindow && false == m_config.is_sizing)
                    SnapWindowToEdge(wp, 10, SNAP_FULLSCREEN);

                /* set a minimum size */
                if (wp->cx < 40)
                    wp->cx = 40;

                if (wp->cy < 20)
                    wp->cy = 20;
            }
            break;

        /* --------------------------------------------------- */
        /* record new position or size */

        case WM_WINDOWPOSCHANGED:
            if (m_config.is_moving)
            {
                WINDOWPOS* wp = (WINDOWPOS*)lParam;
                if (m_config.is_sizing)
                {
                    /* record sizes */
                    m_config.width = wp->cx;
                    m_config.height = wp->cy;

                    /* redraw window */
                    invalidate_window();
                }

                if (false == m_config.is_inslit)
                {
                    /* record position, if not in slit */
                    m_config.xpos = wp->x;
                    m_config.ypos = wp->y;
                }
            }
            break;

        /* ---------------------------------------------------------- */
        /* start moving or sizing accordingly to keys held down */

        case WM_LBUTTONDOWN:
            UpdateWindow(hwnd);
            if (GetAsyncKeyState(VK_MENU) & 0x8000)
            {
                /* start sizing, when alt-key is held down */
                PostMessage(hwnd, WM_SYSCOMMAND, 0xf008, 0);
                m_config.is_sizing = true;
            }
            else
            if (GetAsyncKeyState(VK_CONTROL) & 0x8000)
            {
                /* start moving, when control-key is held down */
                PostMessage(hwnd, WM_SYSCOMMAND, 0xf012, 0);
            }
            break;

        /* ---------------------------------------------------------- */
        /* normal mouse clicks */

        case WM_LBUTTONUP:
            /* code goes here ... */
            break;

        case WM_RBUTTONUP:
            /* Show the user menu on right-click (might test for control-key
               held down if wanted */
            /* if (wParam & MK_CONTROL) */
            ShowMyMenu(true);
            break;

        case WM_LBUTTONDBLCLK:
            /* Do something here ... */
            about_box();
            break;

        /* ---------------------------------------------------------- */
        /* Blackbox sends Broams to all windows... */

        case BB_BROADCAST:
        {
            TCHAR const * msg = (LPTSTR)lParam;
            msg_test msg_test;

            /* check general broams */
            if (!_tcsicmp(msg, TEXT("@BBShowPlugins")))
            {
                if (m_config.is_hidden)
                {
                    m_config.is_hidden = false;
                    ShowWindow(hwnd, SW_SHOWNA);
                }
                break;
            }

			if (!_tcsicmp(msg, TEXT("@BBHidePlugins")))
            {
                if (m_config.pluginToggle && false == m_config.is_inslit)
                {
                    m_config.is_hidden = true;
                    ShowWindow(hwnd, SW_HIDE);
                }
                break;
            }

            /* if the broam is not for us, return now */
            if (0 != memicmp(msg, BROAM_PREFIX, sizeof BROAM_PREFIX - 1)) //@FIXME
                break;

            msg_test.msg = msg + sizeof BROAM_PREFIX - 1;

			if (scan_broam(&msg_test, TEXT("useSlit")))
            {
                eval_broam(&msg_test, M_BOL, &m_config.useSlit);
                break;
            }

			if (scan_broam(&msg_test, TEXT("alwaysOnTop")))
            {
                eval_broam(&msg_test, M_BOL, &m_config.alwaysOnTop);
                break;
            }

			if (scan_broam(&msg_test, TEXT("drawBorder")))
            {
                eval_broam(&msg_test, M_BOL, &m_config.drawBorder);
                break;
            }

			if (scan_broam(&msg_test, TEXT("snapWindow")))
            {
                eval_broam(&msg_test, M_BOL, &m_config.snapWindow);
                break;
            }

			if (scan_broam(&msg_test, TEXT("pluginToggle")))
            {
                eval_broam(&msg_test, M_BOL, &m_config.pluginToggle);
                break;
            }

			if (scan_broam(&msg_test, TEXT("alphaEnabled")))
            {
                eval_broam(&msg_test, M_BOL, &m_config.alphaEnabled);
                break;
            }

			if (scan_broam(&msg_test, TEXT("alphaValue")))
            {
                eval_broam(&msg_test, M_INT, &m_config.alphaValue);
                break;
            }

			if (scan_broam(&msg_test, TEXT("windowText")))
            {
                eval_broam(&msg_test, M_STR, &m_config.windowText);
                break;
            }

			if (scan_broam(&msg_test, TEXT("editRC")))
            {
                edit_rc(m_rcpath);
                break;
            }

			if (scan_broam(&msg_test, TEXT("About")))
            {
                about_box();
                break;
            }

            break;
        }

        /* ---------------------------------------------------------- */
        /* prevent the user from closing the plugin with alt-F4 */

        case WM_CLOSE:
            break;

        /* ---------------------------------------------------------- */
        /* let windows handle any other message */
        default:
            return DefWindowProc(hwnd,message,wParam,lParam);
    }
    return 0;
}

/* ------------------------------------------------------------------ */
/* paint the window into the buffer HDC */
void BBCmd::paint_window (HDC hdc_buffer, RECT * p_rect)
{
    /* and draw the frame */
    MakeStyleGradient(hdc_buffer, p_rect, &m_config.Frame, m_config.drawBorder);

    if (m_config.windowText[0])
    {
        HGDIOBJ otherfont;
        int margin;
        RECT text_rect;

        /* Set the font, storing the default.. */
        otherfont = SelectObject(hdc_buffer, m_config.hFont);

        /* adjust the rectangle */
        margin = m_config.Frame.marginWidth + m_config.Frame.bevelposition;
        if (m_config.drawBorder)
            margin += m_config.Frame.borderWidth;

        text_rect.left  = p_rect->left + margin;
        text_rect.top   = p_rect->top + margin;
        text_rect.right = p_rect->right - margin;
        text_rect.bottom = p_rect->bottom - margin;

        /* draw the text */
        SetTextColor(hdc_buffer, m_config.Frame.TextColor);
        SetBkMode(hdc_buffer, TRANSPARENT);
        DrawText(hdc_buffer, m_config.windowText, -1, &text_rect,
            m_config.Frame.Justify|DT_VCENTER|DT_SINGLELINE|DT_END_ELLIPSIS);

        /* Put back the previous default font. */
        SelectObject(hdc_buffer, otherfont);
    }
}

/* ------------------------------------------------------------------ */
/* Update position and size, as well as onTop, transparency and
   inSlit states. */
void BBCmd::set_window_modes ()
{
    HWND hwnd = m_config.hwnd;

    /* do we want to use the slit and is there a slit at all?  */
    if (m_config.useSlit && g_hSlit)
    {
        /* if in slit, dont move... */
        SetWindowPos(hwnd, NULL,
            0, 0, m_config.width, m_config.height,
            SWP_NOACTIVATE|SWP_NOSENDCHANGING|SWP_NOZORDER|SWP_NOMOVE
            );

        if (m_config.is_inslit)
        {
            /* we are already in the slit, so send update */
            SendMessage(g_hSlit, SLIT_UPDATE, 0, (LPARAM)hwnd);
        }
        else
        {
            /* transpareny must be off in slit */
            SetTransparency(hwnd, 255);
            /* enter slit now */
            m_config.is_inslit = true;
            SendMessage(g_hSlit, SLIT_ADD, 0, (LPARAM)hwnd);
        }
    }
    else
    {
        HWND hwnd_after = NULL;
        UINT flags = SWP_NOACTIVATE|SWP_NOSENDCHANGING|SWP_NOZORDER;
        RECT screen_rect;

        if (m_config.is_inslit)
        {
            /* leave it */
            SendMessage(g_hSlit, SLIT_REMOVE, 0, (LPARAM)hwnd);
            m_config.is_inslit = false;
        }

        if (m_config.is_ontop != m_config.alwaysOnTop)
        {
            m_config.is_ontop = m_config.alwaysOnTop;
            hwnd_after = m_config.is_ontop ? HWND_TOPMOST : HWND_NOTOPMOST;
            flags = SWP_NOACTIVATE|SWP_NOSENDCHANGING;
        }

        // make shure the plugin is on the screen:
        GetWindowRect(GetDesktopWindow(), &screen_rect);
        m_config.xpos = iminmax(m_config.xpos, screen_rect.left, screen_rect.right - m_config.width);
        m_config.ypos = iminmax(m_config.ypos, screen_rect.top, screen_rect.bottom - m_config.height);

        SetWindowPos(hwnd, hwnd_after, m_config.xpos, m_config.ypos, m_config.width, m_config.height, flags);
        SetTransparency(hwnd, (BYTE)(m_config.alphaEnabled ? m_config.alphaValue : 255));
    }

    /* window needs drawing */
    invalidate_window();
}

#if 0

/* ------------------------------------------------------------------ */
/* Locate the configuration file */

/* this shows how to 'delay-load' an API that is in one branch but maybe
   not in another */

bool FindRCFile (char* pszOut, const char* rcfile, HINSTANCE plugin_instance)
{
    bool (*pFindRCFile)(LPSTR rcpath, LPCSTR rcfile, HINSTANCE plugin_instance);

    /* try to grab the function from blackbox.exe */
    *(FARPROC*)&pFindRCFile = GetProcAddress(GetModuleHandle(NULL), "FindRCFile");
    if (pFindRCFile) {
        /* use if present */
        return pFindRCFile(rcpath, rcfile, plugin_instance);

    } else {
       /* otherwise do something similar */
       int len = GetModuleFileName(plugin_instance, pszOut, MAX_PATH);
       while (len && pszOut[len-1] != '\\')
           --len;
       strcpy(pszOut + len, rcfile);
       return FileExists(pszOut);
    }
}
#endif
/* ------------------------------------------------------------------ */
/* Read the configuration file */

void BBCmd::ReadRCSettings ()
{
    /* Locate configuration file */
    FindRCFile(m_rcpath, RC_FILE, g_hInstance);

    /* Read our settings. (If the config file does not exist,
       the Read... functions give us just the defaults.) */

    m_config.xpos   = ReadInt(m_rcpath, RC_KEY("xpos"), 10);
    m_config.ypos   = ReadInt(m_rcpath, RC_KEY("ypos"), 10);
    m_config.width  = ReadInt(m_rcpath, RC_KEY("width"), 80);
    m_config.height = ReadInt(m_rcpath, RC_KEY("height"), 40);

    m_config.alphaEnabled   = ReadBool(m_rcpath, RC_KEY("alphaEnabled"), false);
    m_config.alphaValue     = ReadInt(m_rcpath,  RC_KEY("alphaValue"), 192);
    m_config.alwaysOnTop    = ReadBool(m_rcpath, RC_KEY("alwaysOntop"), true);
    m_config.drawBorder     = ReadBool(m_rcpath, RC_KEY("drawBorder"), true);
    m_config.snapWindow     = ReadBool(m_rcpath, RC_KEY("snapWindow"), true);
    m_config.pluginToggle   = ReadBool(m_rcpath, RC_KEY("pluginToggle"), true);
    m_config.useSlit        = ReadBool(m_rcpath, RC_KEY("useSlit"), true);

    _tcscpy(m_config.windowText, ReadString(m_rcpath, RC_KEY("windowText"), szVersion));
}

/* ------------------------------------------------------------------ */
/* Get some blackbox style */

void BBCmd::GetStyleSettings (void)
{
    m_config.Frame = *(StyleItem *)GetSettingPtr(SN_TOOLBAR);
    if (m_config.hFont)
        DeleteObject(m_config.hFont);
    m_config.hFont = CreateStyleFont(&m_config.Frame);
}

/* ------------------------------------------------------------------ */
/* Show or update configuration menu */

void BBCmd::ShowMyMenu (bool popup)
{
    Menu *pMenu, *pSub;

    /* Create the main menu, with a title and an unique IDString */
    pMenu = MakeNamedMenu(szAppName, MENU_ID("Main"), popup);

    /* Create a submenu, also with title and unique IDString */
    pSub = MakeNamedMenu("Configuration", MENU_ID("Config"), popup);

    /* Insert first Item */
    MakeMenuItem(pSub, "Draw Border", BROAM("drawBorder"), m_config.drawBorder);

    if (g_hSlit)
        MakeMenuItem(pSub, "Use Slit", BROAM("useSlit"), m_config.useSlit);

    if (false == m_config.is_inslit)
    {
        /* these are only available if outside the slit */
        MakeMenuItem(pSub, "Always On Top", BROAM("alwaysOnTop"), m_config.alwaysOnTop);
        MakeMenuItem(pSub, "Snap To Edges", BROAM("snapWindow"), m_config.snapWindow);
        MakeMenuItem(pSub, "Toggle With Plugins", BROAM("pluginToggle"), m_config.pluginToggle);
        MakeMenuItem(pSub, "Transparency", BROAM("alphaEnabled"), m_config.alphaEnabled);
        MakeMenuItemInt(pSub, "Alpha Value", BROAM("alphaValue"), m_config.alphaValue, 0, 255);
    }

    /* Insert the submenu into the main menu */
    MakeSubmenu(pMenu, pSub, "Configuration");

    /* The configurable text string */
    MakeMenuItemString(pMenu, "Display Text", BROAM("windowText"), m_config.windowText);

    /* ---------------------------------- */
    /* add an empty line */
    MakeMenuNOP(pMenu, NULL);

    /* add an entry to let the user edit the setting file */
    MakeMenuItem(pMenu, "Edit Settings", BROAM("editRC"), false);

    /* and an about box */
    MakeMenuItem(pMenu, "About", BROAM("About"), false);

    /* ---------------------------------- */
    /* Finally, show the menu... */
    ShowMenu(pMenu);
}

/* ------------------------------------------------------------------ */
/* helper to handle commands from the menu */

int BBCmd::scan_broam (msg_test * msg_test, const TCHAR * test)
{
    size_t const len = _tcslen(test);
    TCHAR const * msg = msg_test->msg;

    if (_tcsnicmp(msg, test, len) != 0)
        return 0;

    msg += len;
    if (*msg != 0 && *msg != ' ')
        return 0;

    /* store for function below */
    msg_test->msg = msg;
    msg_test->test = test;
    return 1;
}

template <typename T>
void BBCmd::eval_broam (msg_test * msg_test, int mode, T * pValue)
{
/*
    TCHAR rc_key[256];

    // Build the full rc_key. i.e. "@bbCmd.xxx:"
	_stprintf(rc_key, TEXT("%s%s:"), RC_PREFIX, msg_test->test);

    TCHAR const * msg = msg_test->msg;
    // skip possible whitespace after broam
    while (*msg == ' ')
        ++msg;

    switch (mode)
    {
        // --- set boolean variable ---------------- 
        case M_BOL:
            if (0 == _tcsicmp(msg, TEXT("true")))
                *(bool*)pValue = true;
            else
            if (0 == _tcsicmp(msg, TEXT("false")))
                *(bool*)pValue = false;
            else
                // just toggle
                *(bool*)pValue = false == *(bool*)pValue;

            // write the new setting to the rc - file
            WriteBool(m_rcpath, rc_key, *(bool*)pValue);
            break;

        // --- set integer variable -------------------
        case M_INT:
            *(int*)pValue = atoi(msg);

            // write the new setting to the rc - file
            WriteInt(m_rcpath, rc_key, *(int*)pValue);
            break;

        // --- set string variable ------------------- 
        case M_STR:
            _tcscpy((TCHAR*)pValue, msg);

            // write the new setting to the rc - file
            WriteString(m_rcpath, rc_key, pValue);
            break;
    }

    // Apply new settings 
    set_window_modes();

    // Update the menu checkmarks
    ShowMyMenu(false);
	*/
}

