/*

    LeanBar2: The bbLeanBar replacement
    (c) 2008, bbClean Developers

    This project is released under the Gnu Public License (GPL) Version 2.

*/

#include <windows.h>
#include "bbplusplus.h"

// Bar item types
enum BarItemTypes {
    // Displays
    BIT_TASKS = 0,
    BIT_TRAY,
    BIT_CLOCK,
    BIT_WORKSPACELABEL,

    // Extender type
    BIT_EXTENDER = 64,

    // Spacer
    BIT_SPACER = 65,

    // Buttons and stuff
    BIT_CURRENTONLYBUTTON = 128,
    BIT_TASKSTYLEBUTTON,
    BIT_WORKSPACELEFT,
    BIT_WORKSPACERIGHT,
    BIT_WINDOWLEFT,
    BIT_WINDOWRIGHT

};

// Windowproc used by the barItem window
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

//
//  BarItem class; This is something that actually goes into the LeanBar
//  class using the AddItem method. Every element in the bar inherits this
//  class.
//


class BarItem {

    // Protected members
    protected:
        int Width; // Width of item
        int Height; // Height of item
        bool DynamicWidth; // If true the item can shrink/grow as needed.
        RECT ItemRect; // The assigned item rect

    // Public members
    public:
        BarItem(); // Constructor
        virtual ~BarItem(); // Destructor

        virtual void OnInitialize(); // Called as the bar item is initialized
        virtual void OnTerminate(); // Called as the bar item is uninitialized

        virtual void Paint(); // Called when the item should repaint

};

//
//  TrayDisplay class; This is the system tray
//

class TrayDisplay : public BarItem {

    public:
        TrayDisplay();
        virtual ~TrayDisplay();

        virtual void OnInitialize(); // Called as the bar item is initialized
        virtual void OnTerminate(); // Called as the bar item is uninitialized
};


class TrayExtender : public BarItem {

    public:
        TrayExtender();
        virtual ~TrayExtender();

};


//
//  LeanBar class; this is the container that is composed by the actual bar
//  window area. The container host items of the type BarItem.
//

class LeanBar : public bbPlugin {

    // Protected members
    protected:
        HINSTANCE hInstance; // hInstance in use
        HWND hwnd;
        WNDCLASS wnd;
        long barheight; // The height of the bar
        BarItem barItems[32];


    // Public members
    public:
        LeanBar(HINSTANCE hInst); // Constructor
        ~LeanBar(); // Destructor

        int AddItem(BarItem* Item); // Add an item to the bar
        void ArrangeItems(); // Arrange the items

        void OnReconfigure();
        void OnTaskUpdate(TaskUpdateEnum Event, HWND hWnd);
        void OnTrayUpdate(TrayUpdateEnum Event);

};

