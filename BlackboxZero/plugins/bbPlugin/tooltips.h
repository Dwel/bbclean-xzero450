/* tooltips.h */

void ClearToolTips(HWND hwnd);
void SetToolTip (HWND hwnd, RECT *tipRect, const char *tipText);
void SetToolTipW (HWND hwnd, RECT *tipRect, WCHAR const * tipText);
void InitToolTips(HINSTANCE hInstance);
void ExitToolTips();

void make_bb_balloon(plugin_info * PI, systemTray *picon, RECT *r);
void exit_bb_balloon(void);
