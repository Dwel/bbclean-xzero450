/*

    bbPlusPlus: A nice wrapper for the bb4win API

    Part of LeanBar2: The bbLeanBar replacement
    (c) 2008, bbClean Developers

    This project is released under the Gnu Public License (GPL) Version 2.

*/

#ifndef __BBPLUSPLUS_H
#define __BBPLUSPLUS_H

#include "..\..\blackbox\bbapi.h"
#include <string>

enum FilePathEnum {
    PATH_PLUGIN = 0,
    PATH_BLACKBOX,
    PATH_BLACKBOXRC,
    PATH_EXTENSIONSRC,
    PATH_MENURC,
    PATH_PLUGINRC,
    PATH_STYLE
};

enum TaskUpdateEnum {
    TASK_ADDED = 0,
    TASK_MODIFIED,
    TASK_ACTIVATED,
    TASK_REMOVED,
    TASK_REFRESH,
    TASK_FLASHED
};

enum TrayUpdateEnum {
    TRAY_ADDED = 0,
    TRAY_MODIFIED,
    TRAY_REMOVED
};

enum ShutdownActionEnum {
    SHUT_SHUTDOWN = 0,
    SHUT_REBOOT,
    SHUT_LOGOFF,
    SHUT_HIBERNATE,
    SHUT_SUSPEND,
    SHUT_LOCKWORKSTATION
};



class bbPlugin {

    protected:
        HINSTANCE hInstance;
        WNDCLASS pluginClass;
        HWND pluginWnd;
        bbPlugin(HINSTANCE hInst);
        virtual ~bbPlugin();

    // Templates for events; these are called on various events caused by blackbox
    // These has to be registered with RegisterMessage(..)
        virtual void OnReconfigure() {} // BB_RECONFIGURE
        virtual void OnQuit() {} // BB_QUIT
        virtual void OnRestart() {} // BB_RESTART
        virtual void OnSetStyle() {} // BB_SETSTYLE
        virtual void OnTogglePlugins() {} // BB_TOGGLEPLUGINS
        virtual void OnToggleSlit() {} // BB_TOGGLESLIT
        virtual void OnToggleToolbar() {} // BB_TOGGLETOOLBAR
        virtual void OnToggleTray() {} // BB_TOGGLETRAY
        virtual void OnToggleSystembar() {} // BB_TOGGLESYSTEMBAR
        virtual void OnTaskUpdate(TaskUpdateEnum Event, HWND hWnd) {} // BB_TASKSUPDATE
        virtual void OnTrayUpdate(TrayUpdateEnum Event) {} // BB_TRAYUPDATE
        virtual void OnBroadcast(std::string Message) {} // BB_BROADCAST
        virtual void OnWorkspaceChange() {} // BB_WORKSPACE
        virtual void OnRedrawGUI() {} // BB_REDRAWGUI
        virtual void OnShutdown(ShutdownActionEnum Action) {} // BB_SHUTDOWN
        virtual void OnSetToolbarLabel(std::string Text) {} // BB_SETTOOLBARLABEL

    // Misc calls
        void Execute(std::string Broam);

    // Set stuff
        void SetStyle(std::string NewStyle);
        void SetWorkspace(int Index);

    // Get stuff
        std::string GetBBVersion();
        std::string GetOSInfo();
        int GetWorkspace();
        std::string GetWorkspaceName();

    // Message handling
        HWND BlackBoxWindow();
        void RegisterMessage(long Message);
        void UnregisterMessage(long Message);

    // Misc utility functions
        std::string GetPluginPath();
        std::string GetPath(FilePathEnum Item);

        void DebugMsg(std::string Message);

    // Settings management
        std::string RCFile();

        bool GetBoolean(std::string FileName, std::string Key, bool Default);
        int GetInteger(std::string FileName, std::string Key, int Default);
        std::string GetString(std::string FileName, std::string Key, std::string Default);
        COLORREF GetColor(std::string FileName, std::string Key, std::string Default);

        bool FoundValue();

        void SetBoolean(std::string FileName, std::string Key, bool Value);
        void SetInteger(std::string FileName, std::string Key, int Value);
        void SetString(std::string FileName, std::string Key, std::string Value);
        void SetColor(std::string FileName, std::string Key, COLORREF Value);

    // For the wndproc
        static LRESULT CALLBACK StaticWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
        LRESULT PluginWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

};

class bbMenu {

    private:
        Menu *hMenu; // Handle to our menu
        std::string menuID;

    protected:
        Menu *GetMenu(); // Returns the menu (for the API calls)

    public:
        bbMenu(std::string ID = "", std::string Caption = "");
        ~bbMenu();

        void Show();
        void AddItem(std::string Title, std::string Command, bool ShowIndicator, bool Disabled);
        void AddNOP(std::string Title = "");
        void AddItemInt(std::string Title, std::string Command, int Value, int Minimum, int Maximum, bool Disabled);
        void AddItemString(std::string Title, std::string Command, std::string Value, bool Disabled);
        void AddItemPath(std::string Title, std::string Path, std::string Command, bool Disabled);
        void AddSubmenu(std::string Title, bbMenu ChildMenu, bool Disabled);
        bool IsVisible();

};

/*
    This is just me thinking out loud. It might get implemented something like this tho
*/
class bbFrame {

    protected:
        // Constructor and destructor
        bbFrame() {}
        ~bbFrame() {}

        // Events for the window
        virtual void OnCreate() {}
        virtual void OnDestroy() {}
        virtual void OnPaint() {}

        // Drawing aids
        void PutText(int x, int y, std::string text, COLORREF color) {}
        void PutLine(int x1, int y1, int x2, int y2, COLORREF color) {}
        void PutRect(int x1, int y1, int x2, int y2, COLORREF color) {}
        void PutFilledRect(int x1, int y1, int x2, int y2, COLORREF color) {}
        void PutImage(int x, int y, HBITMAP hBitmap) {}

    public:
        void Show() {}
        void Hide() {}
        void MoveTo(int x, int y) {}
        void SizeTo(int width, int height) {}

}

namespace bbPlusPlus {

    enum GradientTypeEnum {
        GRAD_HORIZONTAL = B_HORIZONTAL,
        GRAD_VERTICAL = B_VERTICAL,
        GRAD_DIAGONAL = B_DIAGONAL,
        GRAD_CROSSDIAGONAL = B_CROSSDIAGONAL,
        GRAD_PIPECROSS = B_PIPECROSS,
        GRAD_ELLIPTIC = B_ELLIPTIC,
        GRAD_RECTANGLE = B_RECTANGLE,
        GRAD_PYRAMID = B_PYRAMID,
        GRAD_SOLID = B_SOLID,
        GRAD_SPLITVERTICAL = B_SPLITVERTICAL
    };

    void PaintGradient(HDC hdc, RECT rect, int gradientType, COLORREF colorFrom, COLORREF colorTo, bool interlaced, int bevelStyle, int bevelPosition, int bevelWidth, COLORREF borderColor, int borderWidth);

/*
	Generic Gradient Function
	API_EXPORT void MakeGradient(HDC hdc, RECT rect, int gradientType, COLORREF colorFrom, COLORREF colorTo, bool interlaced, int bevelStyle, int bevelPosition, int bevelWidth, COLORREF borderColour, int borderWidth);

	Draw a Gradient Rectangle from StyleItem, optional using the style border.
	API_EXPORT void MakeStyleGradient(HDC hDC, RECT* p_rect, StyleItem * m_si, bool withBorder);

	Draw a Border
	API_EXPORT void CreateBorder(HDC hdc, RECT* p_rect, COLORREF borderColour, int borderWidth);

	Create a font handle from styleitem, with parsing and substitution.
	API_EXPORT HFONT CreateStyleFont(StyleItem * si);

	Draw text with shadow, etc.
	API_EXPORT int BBDrawText(HDC hDC, LPCTSTR lpString, int nCount, LPRECT lpRect, UINT uFormat, StyleItem * si);
*/


};

#endif
