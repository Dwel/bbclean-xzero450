#include "utils.h"
#include "hookinfo.h"
#include "WinInfo.h"

extern HINSTANCE hInstance;
extern unsigned bbSkinMsg;
extern SkinStruct mSkin;

HWND GetRootWindow (HWND hwnd)
{
    HWND pw, dw;
    dw = GetDesktopWindow();
    while (NULL != (pw = GetParent(hwnd)) && dw != pw)
        hwnd = pw;
    return hwnd;
}

int get_module (HWND hwnd, char * buffer, int buffsize)
{
    char sFileName[MAX_PATH]; HINSTANCE hi; int i, r;
    hi = (HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE);
    r = GetModuleFileName(hi, sFileName, MAX_PATH);
    if (0 == r)
        r = GetModuleFileName(NULL, sFileName, MAX_PATH);
    for (i = r; i && sFileName[i-1] != '\\'; i--);
    r -= i;
    if (r >= buffsize)
        r = buffsize-1;
    memcpy(buffer, sFileName + i, r);
    buffer[r] = 0;
    return r;
}

char * sprint_window (char *buffer, size_t max_ln, HWND hwnd, const char *msg)
{
    char sClassName[200]; sClassName[0] = 0;
    GetClassName(hwnd, sClassName, sizeof sClassName);

    char sFileName[200]; sFileName[0] = 0;
    get_module(hwnd, sFileName, sizeof sFileName);

    char caption[128]; caption[0] = 0;
    GetWindowText(hwnd, caption, sizeof caption);

    sprintf_s(buffer, max_ln,

#ifdef BBLEANSKIN_ENG32
        "%s window/32 with title \"%s\"\r\n\t%s:%s"
#else
        "%s window with title \"%s\"\r\n\t%s:%s"
#endif
        //" - %08x %08x"
        , msg, caption, sFileName, sClassName
        //, GetWindowLongPtr(hwnd, GWL_STYLE), GetWindowLongPtr(hwnd, GWL_EXSTYLE),
        );
    return buffer;
}

bool get_rolled (WinInfo * WI)
{
    DWORD prop = (DWORD)(DWORD_PTR)GetProp(WI->hwnd, BBSHADE_PROP);
    bool const rolled = 0 != (IsZoomed(WI->hwnd)
        ? HIWORD(prop)
        : LOWORD(prop));
    return WI->is_rolled = rolled;
}

void window_set_pos (HWND hwnd, RECT rc)
{
    int const width = rc.right - rc.left;
    int const height = rc.bottom - rc.top;
    SetWindowPos(hwnd, NULL,
        rc.left, rc.top, width, height,
        SWP_NOZORDER|SWP_NOACTIVATE);
}

int get_shade_height (HWND hwnd)
{
    return SendMessage(hwnd, bbSkinMsg, BBLS_GETSHADEHEIGHT, 0);
}

void ShadeWindow (HWND hwnd)
{
    RECT rc;
    get_rect(hwnd, &rc);
    int height = rc.bottom - rc.top;
    LPARAM prop = (LPARAM)GetProp(hwnd, BBSHADE_PROP);

    int h1 = LOWORD(prop);
    int h2 = HIWORD(prop);
    if (IsZoomed(hwnd))
    {
        if (h2) height = h2, h2 = 0;
        else h2 = height, height = get_shade_height(hwnd);
    }
    else
    {
        if (h1) height = h1, h1 = 0;
        else h1 = height, height = get_shade_height(hwnd);
        h2 = 0;
    }

    prop = MAKELPARAM(h1, h2);
    if (0 == prop)
        RemoveProp(hwnd, BBSHADE_PROP);
    else
        SetProp(hwnd, BBSHADE_PROP, (PVOID)prop);

    rc.bottom = rc.top + height;
    window_set_pos(hwnd, rc);
}

void ToggleShadeWindow (HWND hwnd)
{
    if (BBVERSION_LEAN == mSkin.BBVersion)
        SendMessage(mSkin.BBhwnd, BB_WINDOWSHADE, 0, (LPARAM)hwnd);
    else
        ShadeWindow(hwnd);
}

void post_redraw (HWND hwnd)
{
    PostMessage(hwnd, bbSkinMsg, BBLS_REDRAW, 0);
}

void get_workarea (HWND hwnd, RECT * w, RECT * s)
{
    static HMONITOR (WINAPI *pMonitorFromWindow)(HWND hwnd, DWORD dwFlags);
    static BOOL     (WINAPI *pGetMonitorInfoA)(HMONITOR hMonitor, LPMONITORINFO lpmi);

    if (NULL == pMonitorFromWindow)
    {
        HMODULE hUserDll = GetModuleHandle("USER32.DLL");
        *(FARPROC*)&pMonitorFromWindow = GetProcAddress(hUserDll, "MonitorFromWindow" );
        *(FARPROC*)&pGetMonitorInfoA = GetProcAddress(hUserDll, "GetMonitorInfoA"   );
        if (NULL == pMonitorFromWindow) *(DWORD*)&pMonitorFromWindow = 1;
    }

    if (*(DWORD*)&pMonitorFromWindow > 1)
    {
        MONITORINFO mi; mi.cbSize = sizeof(mi);
        HMONITOR hMon = pMonitorFromWindow(hwnd, MONITOR_DEFAULTTONEAREST);
        if (hMon && pGetMonitorInfoA(hMon, &mi))
        {
            if (w) *w = mi.rcWork;
            if (s) *s = mi.rcMonitor;
            return;
        }
    }

    if (w)
    {
        SystemParametersInfo(SPI_GETWORKAREA, 0, w, 0);
    }

    if (s)
    {
        s->left = s->top = 0;
        s->right = GetSystemMetrics(SM_CXSCREEN);
        s->bottom = GetSystemMetrics(SM_CYSCREEN);
    }
}

void SnapWindowToEdge (WinInfo * WI, WINDOWPOS * pwPos, int nDist)
{
    RECT workArea, scrnArea; int x; int y; int z; int dx, dy, dz;

    get_workarea(pwPos->hwnd, &workArea, &scrnArea);

    int fx = WI->S.HiddenSide;
    int fy = WI->S.HiddenTop;
    int bo = WI->S.HiddenBottom;

    //if (workArea.bottom < scrnArea.bottom) bo += 4;
    //if (workArea.top > scrnArea.top) fy += 4;

    // top/bottom edge
    dy = y = pwPos->y + fy - workArea.top;
    dz = z = pwPos->y + pwPos->cy - bo - workArea.bottom;
    if (dy<0) dy=-dy;
    if (dz<0) dz=-dz;
    if (dz < dy) y = z, dy = dz;

    // left/right edge
    dx = x = pwPos->x + fx - workArea.left;
    dz = z = pwPos->x - fx + pwPos->cx - workArea.right;
    if (dx<0) dx=-dx;
    if (dz<0) dz=-dz;
    if (dz < dx) x = z, dx = dz;

    if(dy < nDist) pwPos->y -= y;
    if(dx < nDist) pwPos->x -= x;
}

void get_rect (HWND hwnd, RECT *rp)
{
    GetWindowRect(hwnd, rp);
    if (WS_CHILD & GetWindowLongPtr(hwnd, GWL_STYLE))
    {
        HWND pw = GetParent(hwnd);
        ScreenToClient(pw, (LPPOINT)&rp->left);
        ScreenToClient(pw, (LPPOINT)&rp->right);
    }
}


//===========================================================================

/*int BBDrawTextAltW(HDC hDC, LPCWSTR lpString, RECT *lpRect, unsigned uFormat, StyleItem* pG){

    if (pG->ShadowXY){ // draw shadow
        RECT rcShadow;
        int x = pG->ShadowX;
        int y = pG->ShadowY;
        SetTextColor(hDC, pG->ShadowColor);
        if (pG->FontShadow){ // draw shadow with outline
            for (int i = 0; i <= 2; i++){
                for (int j = 0; j <= 2; j++){
                    if (!((i|j)&0x2)) continue;
                    _CopyOffsetRect(&rcShadow, lpRect, i, j);
                    DrawTextW(hDC, lpString, -1, &rcShadow, uFormat);
                }
            }
        }
        else{
            _CopyOffsetRect(&rcShadow, lpRect, x, y);
            DrawTextW(hDC, lpString, -1, &rcShadow, uFormat);
        }
    }
    if (pG->FontShadow){ // draw outline
        RECT rcOutline;
        SetTextColor(hDC, pG->OutlineColor);
        for (int i = -1; i <= 1; i++){
            for (int j = -1; j <= 1; j++){
                if (!(i|j)) continue; 
                _CopyOffsetRect(&rcOutline, lpRect, i, j);
                DrawTextW(hDC, lpString, -1, &rcOutline, uFormat);
            }
        }
    }
    // draw text
    SetTextColor(hDC, pG->TextColor);
    return DrawTextW(hDC, lpString, -1, lpRect, uFormat);
}*/

/*int BBDrawTextAlt(HDC hDC, const char *lpString, RECT *lpRect, unsigned uFormat, StyleItem * pG){
    int i, j;

    if (pG->ShadowXY){ // draw shadow
        RECT rcShadow;
        int x = pG->ShadowX;
        int y = pG->ShadowY;
        SetTextColor(hDC, pG->ShadowColor);
        if (pG->FontShadow){ // draw shadow with outline
            for (i = 0; i < 3; i++){
                for (j = 0; j < 3; j++){
                    if (!((i|j)&0x2)) continue; 
                    _CopyOffsetRect(&rcShadow, lpRect, i-x, j-y);
                    DrawText(hDC, lpString, -1, &rcShadow, uFormat);
                }
            }
        }
        else{
            _CopyOffsetRect(&rcShadow, lpRect, x, y);
            DrawText(hDC, lpString, -1, &rcShadow, uFormat);
        }
    }
   if (pG->FontShadow){ // draw outline
        RECT rcOutline;
        SetTextColor(hDC, pG->OutlineColor);
        for (i = -1; i < 2; i++){
            for (j = -1; j < 2; j++){
                if (!(i|j)) continue;
                _CopyOffsetRect(&rcOutline, lpRect, i, j);
                DrawText(hDC, lpString, -1, &rcOutline, uFormat);
            }
        }
    }
    // draw text
    SetTextColor(hDC, pG->TextColor);
    return DrawText(hDC, lpString, -1, lpRect, uFormat);
}*/



