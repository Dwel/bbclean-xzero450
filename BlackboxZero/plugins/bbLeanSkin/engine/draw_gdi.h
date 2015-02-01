#pragma once
#include "WinInfo.h"

void DeleteBitmaps (WinInfo *WI);
void PutGradient (WinInfo *WI, HDC hdc, RECT *rc, StyleItem *pG);
void DrawButton (WinInfo *WI, HDC hdc, RECT rc, int btn, int state, StyleItem *pG);
void draw_line (HDC hDC, int x1, int x2, int y1, int y2, int w);
int get_window_icon(HWND hwnd, HICON *picon);
void PaintAll (WinInfo * WI);
