#pragma once
#include "WinInfo.h"

void exec_button_action (WinInfo * WI, int n);
// get button-id from mousepoint
int get_button (struct WinInfo *WI, int x, int y);
// translate button-id to hittest value
LRESULT translate_hittest(WinInfo *WI, int n);

// translate hittest value to button-id
int translate_button(WPARAM wParam);

// titlebar click actions
int get_caption_click(WPARAM wParam, char *pCA);
