#pragma once
#include <BBApi.h>

/* ---------------------------------- */
/* Plugin window properties */
struct Config
{
    /* settings */
    int xpos, ypos;
    int width, height;

    bool useSlit;
    bool alwaysOnTop;
    bool snapWindow;
    bool pluginToggle;
    bool drawBorder;
    bool alphaEnabled;
    int  alphaValue;
    TCHAR windowText[128];

    /* our plugin window */
    HWND hwnd;

    /* current state variables */
    bool is_ontop;
    bool is_moving;
    bool is_sizing;
    bool is_hidden;
    bool is_inslit;

    /* the Style */
    StyleItem Frame;

    /* GDI objects */
    HBITMAP bufbmp;
    HFONT hFont;
};


