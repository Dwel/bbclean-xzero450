#include "utils.h"
#include "plugin.h"
#include <tchar.h>
/* ------------------------------------------------------------------ */
/* utilities */

void about_box ()
{
    TCHAR szTemp[1000];
    _stprintf(szTemp, TEXT("%s - A bbCmd prototype for Blackbox4Windows.\n© %s %s\n%s"), szVersion, szCopyright, szInfoEmail, szInfoLink);
    MessageBox(NULL, szTemp, TEXT("About"), MB_OK|MB_TOPMOST);
}

/* edit a file with the blackbox editor */
void edit_rc (const char *path)
{
    SendMessage(g_BBhwnd, BB_EDITFILE, (WPARAM)-1, (LPARAM)path);
}

/* helper to copy paintbuffer on screen */
void BitBltRect (HDC hdc_to, HDC hdc_from, RECT *r)
{
    BitBlt(
        hdc_to,
        r->left, r->top, r->right - r->left, r->bottom - r->top,
        hdc_from,
        r->left, r->top,
        SRCCOPY
        );
}



