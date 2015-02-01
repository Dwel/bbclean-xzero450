#include "draw_gdi.h"
#include "subclass.h"
#include "utils.h"
#include <plugins/bbPlugin/drawico.h>
#include <blackbox/DrawText.h>

int imax(int a, int b);
int imin(int a, int b);

//-----------------------------------------------------------------
void DeleteBitmaps (WinInfo *WI)
{
    int n = NUMOFGDIOBJS;
    GdiInfo * pGdi = WI->gdiobjs;
    do {
        if (pGdi->hObj)
        {
            DeleteObject(pGdi->hObj);
            pGdi->hObj = NULL;
        }
        pGdi++;
    } while (--n);
}

void PutGradient (WinInfo * WI, HDC hdc, RECT * rc, StyleItem * pG)
{
    if (pG->parentRelative) {
        if (pG->borderWidth)
            CreateBorder(hdc, rc, pG->borderColor, pG->borderWidth);
        return;
    }

    int width = rc->right - rc->left;
    int height = rc->bottom - rc->top;
    int i = pG >= &mSkin.F.Title
        ? pG - &mSkin.F.Title + 6
        : pG - &mSkin.U.Title;

    GdiInfo *pGdi = WI->gdiobjs + i;
    HBITMAP bmp = (HBITMAP)pGdi->hObj;
    HGDIOBJ other;

    if (bmp && width == pGdi->cx && height == pGdi->cy) {
        other = SelectObject(WI->buf, bmp);

    } else {
        RECT r;

        r.left = r.top = 0;
        r.right = width, r.bottom = height;

        if (bmp)
            DeleteObject(bmp);

        pGdi->cx = width;
        pGdi->cx = height;
        pGdi->hObj = bmp = CreateCompatibleBitmap(hdc, width, height);

        if (NULL == bmp)
            return;

        other = SelectObject(WI->buf, bmp);

        StyleItem si;
        si = *pG; // makes a copy of StyleItem into si
        MakeStyleGradient(WI->buf, &r, &si, true);
    }

    BitBlt(hdc, rc->left, rc->top, width, height, WI->buf, 0, 0, SRCCOPY);
    SelectObject(WI->buf, other);
}

//-----------------------------------------------------------------
void DrawButton (WinInfo * WI, HDC hdc, RECT rc, int btn, int state, StyleItem * pG)
{
    int x, y, xa, ya, xe, ye;
    unsigned char *up;

    PutGradient(WI, hdc, &rc, pG);

    COLORREF c = pG->picColor;
    up = mSkin.glyphmap;
    x = up[0];
    y = up[1];
    up = up+2+(btn*2+state)*ONE_GLYPH(x,y);

    xe = (xa = (rc.left + rc.right - x)/2) + x;
    ye = (ya = (rc.top + rc.bottom - y)/2) + y;

    unsigned bits = 0;
    y = ya;
    do {
        x = xa;
        do {
            if (bits < 2)
                bits = 256 | *up++;
            if (bits & 1)
                SetPixel(hdc, x, y, c);
            bits >>= 1;
        } while (++x < xe);
    } while (++y < ye);
}

void draw_line (HDC hDC, int x1, int x2, int y1, int y2, int w)
{
    while (w)
    {
        MoveToEx(hDC, x1, y1, NULL);
        LineTo  (hDC, x2, y2);
        if (x1 == x2)
            x2 = ++x1;
        else
            y2 = ++y1;
        --w;
    }
}

//-----------------------------------------------------------------
#if 1
int get_window_icon (HWND hwnd, HICON * picon)
{
    HICON hIco = NULL;
    SendMessageTimeout(hwnd, WM_GETICON, ICON_SMALL, 0, SMTO_ABORTIFHUNG|SMTO_NORMAL, 1000, (DWORD_PTR*)&hIco);
    if (NULL == hIco)
    {
        SendMessageTimeout(hwnd, WM_GETICON, ICON_BIG, 0, SMTO_ABORTIFHUNG|SMTO_NORMAL, 1000, (DWORD_PTR*)&hIco);
        if (NULL == hIco)
        {
            hIco = (HICON)GetClassLongPtr(hwnd, GCLP_HICONSM);
            if (NULL == hIco)
            {
                hIco = (HICON)GetClassLongPtr(hwnd, GCLP_HICON);
                if (NULL == hIco)
                {
                    return 0;
    }}}}
    *picon = hIco;
    return 1;
}

#else
#define DrawIconSatnHue(hDC, px, py, m_hIcon, sizex, sizey, anistep, hbr, flags, apply, sat, hue)
#define get_ico(hwnd) NULL
#endif

//-----------------------------------------------------------------
void PaintAll (WinInfo * WI)
{
    int const left    = WI->S.HiddenSide;
    int const width   = WI->S.width;
    int const right   = width - WI->S.HiddenSide;

    int const top     = WI->S.HiddenTop;
    int const bottom  = WI->S.height - WI->S.HiddenBottom;
    int const title_height = mSkin.ncTop;
    int const title_bottom = top + title_height;

    HWND focus;
    int active = 0;

    RECT rc;
    HDC hdc, hdc_win;
    HGDIOBJ hbmpOld;
    StyleItem * pG = 0;
    windowGradients * wG = 0;

    //dbg_printf("painting %x", WI->hwnd);

    active =
        (WI->is_active_app
            && (WI->is_active
                || WI->hwnd == (focus = GetFocus())
                || IsChild(WI->hwnd, focus)
                ))
        || (mSkin.bbsm_option & 1);

    hdc_win = GetWindowDC(WI->hwnd);
    hdc = CreateCompatibleDC(hdc_win);
    WI->buf = CreateCompatibleDC(hdc_win);
    hbmpOld = SelectObject(hdc, CreateCompatibleBitmap(hdc_win, width, title_bottom));

    wG = &mSkin.U + active;

    //----------------------------------
    // Titlebar gradient

    rc.top = top;
    rc.left = left;
    rc.right = right;
    rc.bottom = title_bottom;
    pG = &wG->Title;
    PutGradient(WI, hdc, &rc, pG);

    //----------------------------------
    // Titlebar Buttons

    rc.top = top + mSkin.buttonMargin;
    rc.bottom = rc.top + mSkin.buttonSize;

    LONG w_style = WI->style;
    HICON hico = NULL;
    pG = &wG->Button;

    int label_left = left;
    int label_right = right;
    int barrier = mSkin.labelBarrier !=-1 ? 
        ((right - left) * (mSkin.labelBarrier) / 200) : 0;
    int space = mSkin.buttonMargin;
    int d, i;

    for (d = 1, WI->button_count = i = 0; ; i += d)
    {
        bool state;
        int b;
        struct button_set *p;

        b = mSkin.button_string[i] - '0';
        switch (b) {
            case 0 - '0':
                goto _break;
            case '-' - '0':
                if (d < 0)
                    goto _break;
                d = -1;
                i = strlen(mSkin.button_string);
                space = mSkin.buttonMargin;
                continue;
            case btn_Rollup:
                if (0 == (w_style & WS_SIZEBOX))
                    continue;
                state = WI->is_rolled;
                break;
            case btn_Sticky:
                if ((w_style & WS_CHILD) || 0 == WI->has_sticky)
                    continue;
                state = WI->is_sticky;
                break;
            case btn_OnBG:
                if (w_style & WS_CHILD)
                    continue;
                state = WI->is_onbg;
                break;
            case btn_OnTop:
                if (w_style & WS_CHILD)
                    continue;
                state = WI->is_ontop;
                break;
            case btn_Min:
                if (0 == (w_style & WS_MINIMIZEBOX))
                    continue;
                state = WI->is_iconic;
                break;
            case btn_Max:
                if (0 == (w_style & WS_MAXIMIZEBOX))
                    continue;
                state = WI->is_zoomed;
                break;
            case btn_Close:
                state = false;
                break;
            case btn_Icon:
                if (!get_window_icon(WI->hwnd, &hico))
                    continue;
                state = !active;
                break;
            default:
                continue;
        }

        if (d > 0) // left button
        {
            rc.left = label_left + space;
            label_left = rc.right = rc.left + mSkin.buttonSize;
        }
        else // right button
        {
            rc.right = label_right - space;
            label_right = rc.left = rc.right - mSkin.buttonSize;
        }

        p = &WI->button_set[(int)WI->button_count];
        p->set = b;
        p->pos = rc.left;
        space = mSkin.buttonSpace;

        if (btn_Icon == b)
        {
            int s = mSkin.buttonSize;
            int o = (s - 20)/2;
            if (o < 0)
                o = 0;
            else
                s = 20;
            DrawIconSatnHue(hdc, rc.left+o, rc.top+o, hico, s, s, 0, NULL, DI_NORMAL, state, mSkin.iconSat, mSkin.iconHue);
        }
        else
        {
            int const pressed = WI->button_down == b
                || ((2 & mSkin.bbsm_option) && btn_Close == b);
            DrawButton(WI, hdc, rc, b-1, state, pG + pressed);
        }

        if (++WI->button_count == BUTTON_COUNT)
            _break: break;
    }

    //----------------------------------
    // Titlebar Label gradient

    rc.left = label_left + barrier + (left == label_left ? mSkin.labelMargin : mSkin.buttonInnerMargin);
    rc.right = label_right - barrier - (right == label_right ? mSkin.labelMargin : mSkin.buttonInnerMargin);
    rc.top = top + mSkin.labelMargin;
    rc.bottom = title_bottom - mSkin.labelMargin;

    pG = &wG->Label;
    PutGradient(WI, hdc, &rc, pG);

    //----------------------------------
    // Titlebar Text

    rc.left += mSkin.labelIndent;
    rc.right -= mSkin.labelIndent;

    if (NULL == WI->hFont)
        WI->hFont = CreateFontIndirect(&mSkin.Font);
    HGDIOBJ hfontOld = SelectObject(hdc, WI->hFont);
    SetBkMode(hdc, TRANSPARENT);
    SetTextColor(hdc, pG->TextColor);

    int const title_sz = 192;
    WCHAR wTitle[title_sz];
    wTitle[0] = 0;

    if (WI->is_unicode)
    {
        GetWindowTextW(WI->hwnd, wTitle, title_sz);
        BBDrawTextAltW(hdc, wTitle, -1, &rc,
            mSkin.Justify | DT_SINGLELINE | DT_NOPREFIX | DT_END_ELLIPSIS | DT_VCENTER,
            pG);
    }
    else
    {
        GetWindowText(WI->hwnd, (char*)wTitle, title_sz);
        BBDrawTextAlt(hdc, (char*)wTitle, -1, &rc,
            mSkin.Justify | DT_SINGLELINE | DT_NOPREFIX | DT_END_ELLIPSIS | DT_VCENTER,
            pG);
    }

    SelectObject(hdc, hfontOld);

    //----------------------------------
    // Blit the title
    BitBlt(hdc_win, left, top, right-left, title_height, hdc, left, top, SRCCOPY);

    //----------------------------------
    // Frame and Bottom
    if (false == WI->is_iconic
          && (false == WI->is_rolled || (false == mSkin.nixShadeStyle && mSkin.handleHeight)))
    {
        if (int fw = mSkin.frameWidth)
        {
            //----------------------------------
            // Frame left/right(/bottom) border, drawn directly on screen
            rc.top = title_bottom;
            rc.left = left;
            rc.right = right;
            rc.bottom = bottom - WI->S.BottomHeight;
            if (!mSkin.is_style070)
            {
                COLORREF pc = active ? mSkin.F.Title.borderColor : mSkin.U.Title.borderColor;
                HGDIOBJ oldPen = SelectObject(hdc_win, CreatePen(PS_SOLID, 1, pc));
                int const bw = imax(mSkin.F.Title.borderWidth, mSkin.U.Title.borderWidth);
                draw_line(hdc_win, rc.left, rc.left, rc.top, rc.bottom, bw);
                draw_line(hdc_win, rc.right-bw, rc.right-bw, rc.top, rc.bottom, bw);
                DeleteObject(SelectObject(hdc_win, oldPen));
                rc.left += bw;
                rc.right -= bw;
            }

            COLORREF bc = wG->FrameColor;
            HGDIOBJ oldPen = SelectObject(hdc_win, CreatePen(PS_SOLID, 1, bc));
            draw_line(hdc_win, rc.left, rc.left, rc.top, rc.bottom, fw);
            draw_line(hdc_win, rc.right-fw, rc.right-fw, rc.top, rc.bottom, fw);
            if (WI->S.BottomHeight == fw)
                draw_line(hdc_win, rc.left, rc.right, rc.bottom, rc.bottom, fw);
            if (!mSkin.is_style070)
            {
                rc.bottom -= fw;
                draw_line(hdc_win, rc.left, rc.right, rc.top, rc.top, fw);
                draw_line(hdc_win, rc.left, rc.right, rc.bottom, rc.bottom, fw);
            }
            DeleteObject(SelectObject(hdc_win, oldPen));
        }

        if (WI->S.BottomHeight > 0)
        {
            int const gw = mSkin.gripWidth;
            pG = &wG->Handle;
            StyleItem * pG2 = &wG->Grip;
            int const bw = imin(pG->borderWidth, pG2->borderWidth);

            //----------------------------------
            // Bottom Handle gradient
            rc.top = 0;
            rc.bottom = WI->S.BottomHeight;
            rc.left = left;
            rc.right = right;
            if (false == pG2->parentRelative) {
                rc.left += gw - bw;
                rc.right -= gw - bw;
            }
            PutGradient(WI, hdc, &rc, pG);

            //----------------------------------
            // Bottom Grips
            if (false == pG2->parentRelative) {
                rc.right = (rc.left = left) + gw;
                PutGradient(WI, hdc, &rc, pG2);
                rc.left = (rc.right = right) - gw;
                PutGradient(WI, hdc, &rc, pG2);
            }

            //----------------------------------
            // Blit the bottom
            BitBlt(hdc_win, left, bottom - rc.bottom, right-left, rc.bottom, hdc, left, 0, SRCCOPY);

        } // has an handle
    } // not iconic

    DeleteObject(SelectObject(hdc, hbmpOld));
    DeleteDC(hdc);
    DeleteDC(WI->buf);
    ReleaseDC(WI->hwnd, hdc_win);
}


