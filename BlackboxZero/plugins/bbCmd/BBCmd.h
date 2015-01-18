/* ------------------------------------------------------------------ *
  bbCmd derived from bbSDK
 * ------------------------------------------------------------------ */
#pragma once
#include <tchar.h>
#include "Config.h"

/* helper to handle commands from the menu */
struct msg_test {
    const TCHAR *msg;
    const TCHAR *test;
};

class BBCmd
{
    char m_rcpath[MAX_PATH];    /// full path to configuration file
    Config m_config;

public:
    BBCmd () { }
    ~BBCmd () { }

    int Init ();
    void Done ();
    LRESULT WndProc (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

protected:
    void GetStyleSettings ();
    void ReadRCSettings ();
    void WriteRCSettings ();
    void ShowMyMenu (bool popup);
private:
    void invalidate_window ();
    void set_window_modes ();
    void paint_window (HDC hdc_buffer, RECT * p_rect);

    int scan_broam (msg_test * msg_test, TCHAR const * test);
    template <typename T>
    void eval_broam (msg_test * msg_test, int mode, T * pValue);
    enum eval_broam_modes
    {
        M_BOL = 1,
        M_INT = 2,
        M_STR = 3,
    };
};
