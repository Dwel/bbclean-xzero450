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

#include "BBApi.h"
#include "win0x500.h"
#include "BImage.h"
#include "hookinfo.h"
#include "subclass.h"
#include "drawico.h"
#include "../../../blackbox/DrawText.cpp"

int imax(int a, int b);
int imin(int a, int b);

#define CALLORIGWINDOWPROC(hwnd, msg, wp, lp) \
    WI->pCallWindowProc(WI->wpOrigWindowProc, hwnd, msg, wp, lp)

#include "BImage.cpp" // @TODO: remove @NOTE: when in CMakeList it is a problem for eclipse to read makefiles

bool Settings_UTF8Encoding = false; // ugh

#include "utils.h"
#include "draw_gdi.h"
#include "button_actions.h"

bool set_region (WinInfo *WI);

//-----------------------------------------------------------------
// This is where it all starts from
void subclass_window(HWND hwnd)
{
    char dllpath[MAX_PATH];
    WinInfo *WI;
    WI = (WinInfo *)GlobalAlloc(GMEM_FIXED|GMEM_ZEROINIT, sizeof *WI);

    // Keep a reference to this piece of code to prevent the
    // engine from being unloaded by chance (i.e. if BB crashes)
    GetModuleFileName(hInstance, dllpath, sizeof dllpath);
    WI->hModule = LoadLibrary(dllpath);

    WI->hwnd = hwnd;
    WI->is_unicode = FALSE != IsWindowUnicode(hwnd);
    WI->pCallWindowProc = WI->is_unicode ? CallWindowProcW : CallWindowProcA;
    WI->is_active_app = true;

    set_WinInfo(hwnd, WI);
    WI->wpOrigWindowProc = (WNDPROC)
        (WI->is_unicode ? SetWindowLongPtrW : SetWindowLongPtrA)
            (hwnd, GWLP_WNDPROC, (LONG_PTR)WindowSubclassProc);

    // some programs dont handle the posted "BBLS_LOAD" msg, so set
    // the region here (is this causing the 'opera does not start' issue?)
    // Also, the console (class=tty) does not handle posted messages at all.
    // Have to use the "hook-early:" option with such windows.
    // With other windows again that would cause troubles, like with tabbed
    // dialogs, which seem to subclass themselves during creation.
    set_region(WI);
}

// This is where it ends then
void detach_skinner (WinInfo *WI)
{
    HWND hwnd = WI->hwnd;
    HMODULE hModule = WI->hModule;
    // clean up
    DeleteBitmaps(WI);
    // the currently set WindowProc
    WNDPROC wpNow = (WNDPROC)(WI->is_unicode ? GetWindowLongPtrW : GetWindowLongPtrA)(hwnd, GWLP_WNDPROC);
    // check, if it's still what we have set
    if (WindowSubclassProc == wpNow) {
        // if so, set back to the original WindowProc
        SetLastError(0);
        (WI->is_unicode ? SetWindowLongPtrW : SetWindowLongPtrA)
            (hwnd, GWLP_WNDPROC, (LONG_PTR)WI->wpOrigWindowProc);
        if (0 == GetLastError()) {
            // remove the property
            del_WinInfo(hwnd);
            // free the WinInfo structure
            GlobalFree (WI);
            // if blackbox is still there, release this dll
            if (hModule && IsWindow(mSkin.loghwnd))
                FreeLibrary(hModule);
            // send a note to the log window
            send_log(hwnd, "Released");
            return;
        }
    }
    // otherwise, the subclassing must not be terminated
    send_log(hwnd, "Subclassed otherwise, cannot release");
}

//-----------------------------------------------------------------
// cut off left/right sizeborder and adjust title height
bool set_region (WinInfo *WI)
{
    HWND hwnd = WI->hwnd;

    WI->style = GetWindowLongPtr(hwnd, GWL_STYLE);
    WI->exstyle = GetWindowLongPtr(hwnd, GWL_EXSTYLE);

    WI->is_ontop = 0 != (WI->exstyle & WS_EX_TOPMOST);
    WI->is_zoomed = FALSE != IsZoomed(hwnd);
    WI->is_iconic = FALSE != IsIconic(hwnd);

    // since the 'SetWindowRgn' throws a WM_WINPOSCHANGED,
    // and WM_WINPOSCHANGED calls set_region, ...
    if (WI->in_set_region)
        return false;

    // check for fullscreen mode
    if (WS_CAPTION != (WI->style & WS_CAPTION))
    {
        if (false == WI->apply_skin)
            return false;

        WI->apply_skin = false;
        SetWindowRgn(hwnd, NULL, TRUE);
        return true;
    }

    SizeInfo S = WI->S;

    RECT rc; GetWindowRect(hwnd, &rc);
    WI->S.width  = rc.right - rc.left;
    WI->S.height = rc.bottom - rc.top;
    GetClientRect(hwnd, &WI->S.rcClient);

    int c = mSkin.cyCaption;
    int b = mSkin.cxSizeFrame;
    int bh = mSkin.ncBottom;

    if (WS_EX_TOOLWINDOW & WI->exstyle)
        c = mSkin.cySmCaption;

    if (0 == (WS_SIZEBOX & WI->style)) {
        b = mSkin.cxFixedFrame;
        bh = mSkin.frameWidth;
    }
    else
    if (WI->is_zoomed && false == WI->is_rolled) {
        bh = mSkin.frameWidth;
    }

    WI->S.HiddenTop = imax(0, b - mSkin.ncTop + c);
    WI->S.HiddenSide = imax(0, b - mSkin.frameWidth);
    WI->S.HiddenBottom = imax(0, b - bh);
    WI->S.BottomAdjust = imax(0, bh - b);
    WI->S.BottomHeight = bh;

    if (WI->is_rolled)
        WI->S.HiddenBottom = 0;

    if (0 == memcmp(&S, &WI->S, sizeof S) && WI->apply_skin)
        return false; // nothing changed

    WI->apply_skin = true;

    HRGN hrgn = CreateRectRgn(
        WI->S.HiddenSide,
        WI->S.HiddenTop,
        WI->S.width - WI->S.HiddenSide,
        WI->S.height - WI->S.HiddenBottom
        );

    WI->in_set_region = true;
    SetWindowRgn(hwnd, hrgn, TRUE);
    WI->in_set_region = false;

    return true;
}

//-----------------------------------------------------------------
//#define LOGMSGS

#ifdef LOGMSGS
#include "../winmsgs.cpp"
#endif

//===========================================================================

LRESULT APIENTRY WindowSubclassProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    WinInfo *WI;
    LRESULT result;
    int n;

    result = 0;
    WI = get_WinInfo(hwnd);

#ifdef LOGMSGS
    dbg_printf("hw %08x  msg %s  wP %08x  lp %08x", hwnd, wm_str(uMsg), wParam, lParam);
#endif

    if (NULL == WI)
        return DefWindowProc(hwnd, uMsg, wParam, lParam);

    if (WI->apply_skin
        || uMsg == WM_NCDESTROY
        || uMsg == WM_STYLECHANGED
        || uMsg == bbSkinMsg)
    switch (uMsg)
    {
    //----------------------------------
    case WM_NCPAINT:
    {
        RECT rc;
        HRGN hrgn;
        bool flag_save;
        BOOL locked;

        if (WI->dont_paint)
            goto leave;

        if (WI->is_rolled)
            goto paint_now;

        if (WI->sync_paint)
            goto paint_after;

        // Okay, so let's create an own region and pass that to
        // the original WndProc instead of (HRGN)wParam
        GetWindowRect(hwnd, &rc);
        hrgn = CreateRectRgn(
            rc.left + WI->S.HiddenSide + mSkin.frameWidth,
            rc.top + WI->S.HiddenTop + mSkin.ncTop,
            rc.right - WI->S.HiddenSide - mSkin.frameWidth,
            rc.bottom - WI->S.HiddenBottom - WI->S.BottomHeight
            );
        result = CALLORIGWINDOWPROC(hwnd, uMsg, (WPARAM)hrgn, lParam);
        DeleteObject(hrgn);

    paint_now:
        PaintAll(WI);
        goto leave;

    paint_after:
        flag_save = WI->dont_paint;
        WI->dont_paint = true;
        result = CALLORIGWINDOWPROC(hwnd, uMsg, wParam, lParam);
        WI->dont_paint = flag_save;
        goto paint_now;

    paint_locked:
        // smooth, but slow and dangerous also
        if (false == mSkin.drawLocked)
            goto paint_after;

        flag_save = WI->dont_paint;
        WI->dont_paint = true;
        locked = LockWindowUpdate(hwnd);
        result = CALLORIGWINDOWPROC(hwnd, uMsg, wParam, lParam);
        if (locked) LockWindowUpdate(NULL);
        WI->dont_paint = flag_save;
        goto paint_now;
    }

    case WM_SYNCPAINT:
        // this is when the user moves other windows over our window. The
        // DefWindowProc will call WM_NCPAINT, WM_ERASEBKGND and WM_PAINT
        WI->sync_paint = true;
        result = CALLORIGWINDOWPROC(hwnd, uMsg, wParam, lParam);
        WI->sync_paint = false;
        goto leave;

    //----------------------------------

    //----------------------------------
    // Windows draws the caption on WM_SETTEXT/WM_SETICON

    case WM_SETICON:
    case WM_SETTEXT:
        if ((WI->exstyle & WS_EX_MDICHILD) && IsZoomed(hwnd))
        {
            post_redraw(GetRootWindow(hwnd));
            break;
        }
        goto paint_locked;

    //----------------------------------
    // Windows draws the caption buttons on WM_SETCURSOR (size arrows),
    // which looks completely unsmooth, have to override this

    case WM_SETCURSOR:
    {
        LPCSTR CU;
        switch (LOWORD(lParam))
        {
            case HTLEFT:
            case HTRIGHT: CU = IDC_SIZEWE;  break;
                
            case HTTOPRIGHT:
            case HTBOTTOMLEFT: CU = IDC_SIZENESW; break;
                
            case HTTOPLEFT:
            case HTGROWBOX:
            case HTBOTTOMRIGHT: CU = IDC_SIZENWSE; break;
                
            case HTTOP:
            case HTBOTTOM: CU = IDC_SIZENS; break;

            default: goto paint_locked;
        }
        SetCursor(LoadCursor(NULL, CU));
        result = 1;
        goto leave;
    }

    //----------------------------------

    case WM_ACTIVATEAPP:
        //dbg_printf("WM_ACTIVATEAPP %d", wParam);
        WI->is_active_app = 0 != wParam;
        post_redraw(hwnd);
        break;

    case WM_NCACTIVATE:
        //dbg_printf("WM_NCACTIVATE %d", wParam);
        WI->is_active = 0 != wParam;
        post_redraw(hwnd);
        goto paint_after;

    //----------------------------------
    case WM_NCHITTEST:
        n = get_button(WI, (short)LOWORD(lParam), (short)HIWORD(lParam));
        if (btn_None == n) {
            result = CALLORIGWINDOWPROC(hwnd, uMsg, wParam, lParam);
        } else {
            result = translate_hittest(WI, n);
        }

        if (WI->is_rolled)
        {
            switch(result)
            {
                case HTBOTTOM: // if this is removed, a rolled window can be opened by dragging
                case HTTOP:
                case HTMENU:
                case HTBOTTOMLEFT:
                case HTBOTTOMRIGHT:
                    result = HTCAPTION;
                    break;

                case HTTOPLEFT:
                    result = HTLEFT;
                    break;

                case HTTOPRIGHT:
                    result = HTRIGHT;
                    break;
            }
        }
        goto leave;

    //----------------------------------
    case WM_MOUSEMOVE:
        if (btn_None == (n = WI->capture_button))
            break;
        {
            POINT pt;
            pt.x = (short)LOWORD(lParam);
            pt.y = (short)HIWORD(lParam);
            ClientToScreen(hwnd, &pt);
            if (get_button(WI, pt.x, pt.y) != n)
                n = btn_None;
        }
        if (WI->button_down != n)
        {
            WI->button_down = (char)n;
            post_redraw(hwnd);
        }
        goto leave;


    //----------------------------------

    set_capture:
        WI->capture_button = WI->button_down = (char)n;
        SetCapture(hwnd);
        post_redraw(hwnd);
        goto leave;

    exec_action:
        WI->capture_button = WI->button_down = btn_None;
        ReleaseCapture();
        exec_button_action(WI, n);
        post_redraw(hwnd);
        goto leave;

    case WM_CAPTURECHANGED:
        if (btn_None == WI->capture_button)
            break;
        if (0 == lParam)
            break;
        WI->capture_button = WI->button_down = btn_None;
        goto leave;

    //----------------------------------
    case WM_LBUTTONUP:
        if (btn_None == WI->capture_button)
            break;
        n = WI->button_down;
        goto exec_action;

    case WM_RBUTTONUP:
        if (btn_None == WI->capture_button)
            break;
        n = WI->button_down;
        if (btn_Max == n) {
            n = (wParam & MK_SHIFT) ? btn_VMax : btn_HMax;
        } else if (btn_Min == n) {
            n = btn_TMin;
        } else {
            n = btn_None;
        }
        goto exec_action;

    case WM_MBUTTONUP:
        if (btn_None == WI->capture_button)
            break;
        n = WI->button_down;
        if (btn_Max == n) {
            n = btn_VMax;
        } else {
            n = btn_None;
        }
        goto exec_action;

    //----------------------------------
    case WM_NCLBUTTONDBLCLK:
        n = get_caption_click(wParam, mSkin.captionClicks.Dbl);
        if (btn_None == n)
            goto case_WM_NCLBUTTONDOWN;
        exec_button_action(WI, n);
        goto leave;

    case_WM_NCLBUTTONDOWN:
    case WM_NCLBUTTONDOWN:
        n = translate_button(wParam);
        if (n > btn_None && n <= btn_Last)
            goto set_capture; // clicked in a button
        if (btn_None == get_caption_click(wParam, mSkin.captionClicks.Left))
            break;
        goto leave;

    case WM_NCLBUTTONUP:
        n = get_caption_click(wParam, mSkin.captionClicks.Left);
        if (btn_None == n)
            break;
        exec_button_action(WI, n);
        goto leave;

    //----------------------------------
    case WM_NCRBUTTONDOWN:
        n = translate_button(wParam);
        if (btn_Max == n || btn_Min == n)
            goto set_capture;
    case WM_NCRBUTTONDBLCLK:
        if (btn_None == get_caption_click(wParam, mSkin.captionClicks.Right))
            break;
        goto leave;

    case WM_NCRBUTTONUP:
        n = get_caption_click(wParam, mSkin.captionClicks.Right);
        if (btn_None == n)
            break;
        exec_button_action(WI, n);
        goto leave;

    //----------------------------------
    case WM_NCMBUTTONDOWN:
        n = translate_button(wParam);
        if (btn_Max == n)
            goto set_capture;
    case WM_NCMBUTTONDBLCLK:
        if (btn_None == get_caption_click(wParam, mSkin.captionClicks.Mid))
            break;
        goto leave;

    case WM_NCMBUTTONUP:
        n = get_caption_click(wParam, mSkin.captionClicks.Mid);
        if (btn_None == n)
            break;
        exec_button_action(WI, n);
        goto leave;

    //----------------------------------
    case WM_SYSCOMMAND:
        // dbg_printf("WM_SYSCOMMAND: %08x %08x", wParam, lParam);
        // ----------
        // these SYSCOMMAND's enter the 'window move/size' modal loop
        if ((wParam >= 0xf001 && wParam <= 0xf008) // size
            || wParam == 0xf012 // move
            )
        {
            // draw the caption before
            PaintAll(WI);
            break;
        }
#if 1
        if (wParam == 0xF100 && lParam == ' ') // SC_KEYMENU + spacebar
        {
            exec_button_action(WI, btn_SysMenu);
            goto leave;
        }
#endif
        // these SYSCOMMAND's draw the caption
        if (wParam == 0xf095 // menu invoked
         || wParam == 0xf100 // sysmenu invoked
         || wParam == 0xf165 // menu closed
            )
        {
            // draw over after
            post_redraw(hwnd);
            break;
        }
        if (wParam == SC_CLOSE && WI->is_rolled)
        {
            // unshade the window on close, just in case it wants
            // to store it's size
            ToggleShadeWindow(hwnd);
            break;
        }

        post_redraw(hwnd);
        break;

    //----------------------------------
    // set flag, whether snapWindows should be applied below
    case WM_ENTERSIZEMOVE:
        WI->is_moving = true;
        break;

    case WM_EXITSIZEMOVE:
        WI->is_moving = false;
        break;

    //----------------------------------
    // If moved, snap to screen edges...
    case WM_WINDOWPOSCHANGING:
    {
        WINDOWPOS *wp = (WINDOWPOS*)lParam;
        if (WI->is_moving
         && mSkin.snapWindows
         && 0 == (WS_CHILD & WI->style)
         && ((wp->flags & SWP_NOSIZE)
             || (WI->S.width == wp->cx 
                && WI->S.height == wp->cy)
            ))
            SnapWindowToEdge(WI, (WINDOWPOS*)lParam, mSkin.snapWindows);

        if (get_rolled(WI)) {
            wp->cy = mSkin.rollupHeight + WI->S.HiddenTop;
            // prevent app from possibly setting a minimum size
            goto leave;
        }
        break;
    }

    //----------------------------------
    case WM_WINDOWPOSCHANGED:
        // we do not let the app know about rolled state
        if (false == WI->is_rolled)
            result = CALLORIGWINDOWPROC(hwnd, uMsg, wParam, lParam);

        // adjust the windowregion
        set_region(WI);

        // MDI childs repaint their buttons at each odd occasion
        if (WS_CHILD & WI->style)
            goto paint_now;

        goto leave;

    //----------------------------------
    case WM_STYLECHANGED:
        set_region(WI);
        break;

    //----------------------------------
    // adjust for the bottom border (handleHeight)
    case WM_NCCALCSIZE:
        if (wParam)
        {
            result = CALLORIGWINDOWPROC(hwnd, uMsg, wParam, lParam);
            ((NCCALCSIZE_PARAMS*)lParam)->rgrc[0].bottom -= WI->S.BottomAdjust;
            goto leave;
        }
        break;

    //----------------------------------
    case WM_GETMINMAXINFO:
    {
        LPMINMAXINFO lpmmi = (LPMINMAXINFO)lParam;
        lpmmi->ptMinTrackSize.x = 12 * mSkin.buttonSize;
        lpmmi->ptMinTrackSize.y = 
            mSkin.ncTop 
            + WI->S.HiddenTop 
            + WI->S.BottomHeight 
            - WI->S.HiddenBottom 
            - mSkin.frameWidth;
        break;
    }

    //----------------------------------
    // Terminate subclassing

    case WM_NCDESTROY:
        WI->apply_skin = false;
        result = CALLORIGWINDOWPROC(hwnd, uMsg, wParam, lParam);
        detach_skinner(WI);
        goto leave;

    //----------------------------------
    default:
        if (uMsg == bbSkinMsg) { // our registered message
            switch (wParam)
            {
                // initialisation message
                case BBLS_LOAD:
                    break;

                // detach the skinner
                case BBLS_UNLOAD:
                    if (WI->is_rolled)
                        ToggleShadeWindow(hwnd);
                    WI->apply_skin = false;
                    SetWindowRgn(hwnd, NULL, TRUE);
                    detach_skinner(WI);
                    break;

                // repaint the caption
                case BBLS_REDRAW:
                    if (WI->apply_skin)
                        goto paint_now;
                    break;

                // changed Skin
                case BBLS_REFRESH:
                    GetSkin();
                    DeleteBitmaps(WI);
                    if (WI->apply_skin && false == set_region(WI))
                        goto paint_now;
                    break;

                // set sticky button state, sent from BB
                case BBLS_SETSTICKY:
                    WI->has_sticky = true;
                    WI->is_sticky = 0 != lParam;
                    if (WI->apply_skin)
                        goto paint_now;
                    break;

                // set onbg button state, sent from BB
                case BBLS_SETONBG:
                    WI->has_onbg = true;
                    WI->is_onbg = 0 != lParam;
                    if (WI->apply_skin)
                        goto paint_now;
                    break;

                case BBLS_GETSHADEHEIGHT:
                    result = mSkin.rollupHeight + WI->S.HiddenTop;
                    break;
            }
            goto leave;
        }

    //----------------------------------
    } //switch

    result = CALLORIGWINDOWPROC(hwnd, uMsg, wParam, lParam);
leave:
    return result;
}

//===========================================================================
