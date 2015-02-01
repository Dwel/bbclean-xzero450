#include "button_actions.h"
#include "utils.h"

extern HINSTANCE hInstance;
extern unsigned bbSkinMsg;
extern SkinStruct mSkin;

void exec_button_action (WinInfo * WI, int n)
{
    HWND hwnd = WI->hwnd;
    WPARAM SC_xxx;

    switch(n)
    {
        case btn_Close:
            SC_xxx = SC_CLOSE;
        Post_SC:
            {
                POINT pt;
                GetCursorPos(&pt);
                PostMessage(hwnd, WM_SYSCOMMAND, SC_xxx, MAKELPARAM(pt.x, pt.y));
            }
            break;

        case btn_Min:
            if (WI->style & WS_MINIMIZEBOX) {
                if (BBVERSION_LEAN == mSkin.BBVersion) {
                    PostMessage(mSkin.BBhwnd, IsIconic(hwnd) ? BB_WINDOWRESTORE : BB_WINDOWMINIMIZE, 0, (LPARAM)hwnd);
                } else {
                    SC_xxx = IsIconic(hwnd) ? SC_RESTORE : SC_MINIMIZE;
                    goto Post_SC;
                }
            }
            break;

        case btn_Max:
            if (WI->style & WS_MAXIMIZEBOX) {
                if (BBVERSION_LEAN == mSkin.BBVersion) {
                    PostMessage(mSkin.BBhwnd, IsZoomed(hwnd) ? BB_WINDOWRESTORE : BB_WINDOWMAXIMIZE, 0, (LPARAM)hwnd);
                } else {
                    SC_xxx = IsZoomed(hwnd) ? SC_RESTORE : SC_MAXIMIZE;
                    goto Post_SC;
                }
            }
            break;

        case btn_VMax:
            if (WI->style & WS_MAXIMIZEBOX)
                PostMessage(mSkin.BBhwnd, BB_WINDOWGROWHEIGHT, 0, (LPARAM)hwnd);
            break;

        case btn_HMax:
            if (WI->style & WS_MAXIMIZEBOX)
                PostMessage(mSkin.BBhwnd, BB_WINDOWGROWWIDTH, 0, (LPARAM)hwnd);
            break;

        case btn_TMin:
            if (WI->style & WS_MINIMIZEBOX)
                PostMessage(mSkin.BBhwnd, BB_WINDOWMINIMIZETOTRAY, 0, (LPARAM)hwnd);
            break;

        case btn_Lower:
            PostMessage(mSkin.BBhwnd, BB_WINDOWLOWER, 0, (LPARAM)hwnd);
            break;

        case btn_Rollup:
            if (WI->style & WS_SIZEBOX)
                ToggleShadeWindow(hwnd);
            break;

        case btn_Sticky:
            WI->is_sticky = false == WI->is_sticky;
            PostMessage(mSkin.BBhwnd, BB_WORKSPACE,
                WI->is_sticky ? BBWS_MAKESTICKY : BBWS_CLEARSTICKY,
                (LPARAM)hwnd
                );
            break;

        case btn_OnBG:
            WI->is_onbg = false == WI->is_onbg;
            PostMessage(mSkin.BBhwnd, BB_WORKSPACE,
                WI->is_onbg ? BBWS_MAKEONBG : BBWS_CLEARONBG,
                (LPARAM)hwnd
                );
            break;

        case btn_OnTop:
            if (BBVERSION_LEAN == mSkin.BBVersion)
                PostMessage(mSkin.BBhwnd, BB_WORKSPACE, BBWS_TOGGLEONTOP, (LPARAM)hwnd);
            else
                SetWindowPos(hwnd,
                    WI->is_ontop ? HWND_NOTOPMOST : HWND_TOPMOST,
                    0, 0, 0, 0,
                    SWP_NOMOVE|SWP_NOSIZE|SWP_NOACTIVATE);
            break;

        case btn_SysMenu:
        case btn_Icon:
            // PostMessage(hwnd, 0x0313, 0, MAKELPARAM(30, 30));
            if (0 == (WI->style & WS_CHILD))
            {
                COPYDATASTRUCT cds;
                struct sysmenu_info s;
                int b = mSkin.F.Title.borderWidth;

                GetWindowRect(hwnd, &s.rect);
                s.rect.bottom = s.rect.top + WI->S.HiddenTop + mSkin.ncTop - b;
                s.rect.right = s.rect.left + WI->S.width - WI->S.HiddenSide;
                s.rect.top += WI->S.HiddenTop + b;
                s.rect.left += WI->S.HiddenSide;
                if (btn_SysMenu == n)
                    s.rect.right = s.rect.left;
                s.hwnd = hwnd;
#ifndef _WIN64
                s._pad32 = 0;
#endif
                cds.dwData = 202;
                cds.cbData = sizeof s;
                cds.lpData = &s;
                SendMessage(mSkin.loghwnd, WM_COPYDATA, (WPARAM)hwnd, (LPARAM)&cds);
                post_redraw(hwnd);
            }
            break;
    }
}

int get_button (struct WinInfo *WI, int x, int y)
{
    int button_top = WI->S.HiddenTop + mSkin.buttonMargin;
    int margin = WI->S.HiddenSide + mSkin.buttonMargin;
    RECT rc; GetWindowRect(WI->hwnd, &rc);
    y -= rc.top;
    x -= rc.left;
    int n;

    if (y < button_top)
    {
        if (x < mSkin.ncTop)
            n = btn_Topleft;
        else
        if (x >= WI->S.width - mSkin.ncTop)
            n = btn_Topright;
        else
            n = btn_Top;
    }
    else
    if (y >= WI->S.HiddenTop + mSkin.ncTop && false == WI->is_rolled)
        n = btn_None;
    else
    if (y >= button_top + mSkin.buttonSize)
        n = btn_Caption;
    else
    if (x < margin)
        n = btn_Topleft;
    else
    if (x >= WI->S.width - margin)
        n = btn_Topright;
    else
    {
        int i, c = WI->button_count;
        struct button_set *p = WI->button_set;
        for (i = 0; i < c; i++, p++)
            if (x >= p->pos && x < p->pos + mSkin.buttonSize)
                break;
        n = i < c ? p->set : btn_Caption;
    }
    if (n >= btn_Topleft)
    {
        if (WI->is_zoomed)
            n = btn_Caption;
        if (WI->is_iconic)
            n = btn_None;
    }
    return n;
}

LRESULT translate_hittest(WinInfo *WI, int n)
{
    switch(n)
    {
        case btn_Min: return HTMINBUTTON;               //  8 
        case btn_Max: return HTMAXBUTTON;               //  9 
        case btn_Close: return HTCLOSE;                 // 20 
        case btn_Caption: return HTCAPTION;             //  2 
        case btn_Nowhere: return HTNOWHERE;             //  0 
        case btn_Top: n = HTTOP;  goto s1;              // 12
        case btn_Topleft: n = HTTOPLEFT; goto s1;       // 13
        case btn_Topright: n = HTTOPRIGHT;  goto s1;    // 14
        default: break;
      s1:
        if (WI->style & WS_SIZEBOX)
            return n;
        return HTCAPTION;
    }
    return n+100;
}

int translate_button(WPARAM wParam)
{
    if (HTMINBUTTON == wParam)
        return btn_Min;
    if (HTMAXBUTTON == wParam)
        return btn_Max;
    if (HTCLOSE == wParam)
        return btn_Close;
    return wParam - 100;
}

int get_caption_click(WPARAM wParam, char *pCA)
{
    if (HTCAPTION != wParam)
        return btn_None;
    if (GetAsyncKeyState(VK_SHIFT) & 0x8000)
        return pCA[1];
    if (GetAsyncKeyState(VK_CONTROL) & 0x8000)
        return pCA[2];
    if (GetAsyncKeyState(VK_MENU) & 0x8000)
        return pCA[3];
    return pCA[0];
}



