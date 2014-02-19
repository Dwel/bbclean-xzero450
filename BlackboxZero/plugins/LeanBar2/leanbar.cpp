/*

    LeanBar2: The bbLeanBar replacement
    (c) 2008, bbClean Developers

    This project is released under the Gnu Public License (GPL) Version 2.

*/

#include <windows.h>
#include "leanbar.h"
#include "../../blackbox/bbapi.h"

LPTSTR szClassName = "LeanBar2"; // Classname for the LeanBar2 window


//////////////////////////////////////////////////////////////////////////////
//
//  LeanBar Class
//

/**
 * Constructor, creates the LeanBar window and gets ready for action
 **/
LeanBar::LeanBar(HINSTANCE hInst) : bbPlugin(hInst) {

    this->RegisterMessage(BB_BROADCAST);
    this->RegisterMessage(BB_TASKSUPDATE);
    this->RegisterMessage(BB_TRAYUPDATE);

    // Save hInstance for later use
    this->hInstance = hInst;

    // Read any settings here
    // this->GetString(..);

    // TODO: Constructor should create the actual window, this is just template code...
    // This should be an instance of the bbFrame class

    this->wnd.style = CS_HREDRAW | CS_VREDRAW; //we will explain this later
    this->wnd.lpfnWndProc = WndProc;
    this->wnd.cbClsExtra = 0;
    this->wnd.cbWndExtra = 0;
    this->wnd.hInstance = this->hInstance;
    this->wnd.hIcon = LoadIcon(NULL, IDI_APPLICATION); //default icon
    this->wnd.hCursor = LoadCursor(NULL, IDC_ARROW);   //default arrow mouse cursor
    this->wnd.hbrBackground = (HBRUSH)(COLOR_BACKGROUND+1);
    this->wnd.lpszMenuName = NULL;                     //no menu
    this->wnd.lpszClassName = szClassName;

    if(!RegisterClass(&this->wnd))                     //register the WNDCLASS
    {
       MessageBox(NULL, "This Program Requires Windows NT",
                        "Error", MB_OK);
       return;
    }

    this->hwnd = CreateWindow(szClassName,
                       "Window Title",
                       WS_OVERLAPPEDWINDOW, //basic window style
                       CW_USEDEFAULT,
                       CW_USEDEFAULT,       //set starting point to default value
                       CW_USEDEFAULT,
                       CW_USEDEFAULT,       //set all the dimensions to default value
                       NULL,                //no parent window
                       NULL,                //no menu
                       this->hInstance,
                       NULL);               //no parameters to pass
    ShowWindow(this->hwnd, SW_SHOWNOACTIVATE); //display the window on the screen
    UpdateWindow(this->hwnd);             //make sure the window is updated correctly

}


/**
 * Destructor, unloads everything and destroys the window and the wndclass
 **/
LeanBar::~LeanBar() {

    // TODO: Destructor code

    UnregisterClass(szClassName,this->hInstance);
    DestroyWindow(this->hwnd);

}


/**
 * Adds an item to the bar
 **/
int LeanBar::AddItem(BarItem* Item) {

    // TODO: Add an item and arrange the items accordingly
    return 0;

}


void LeanBar::ArrangeItems() {

    // TODO: Rearrange the items in the bar to fit properly

}

void LeanBar::OnReconfigure() {

    // TODO: Reload settings etc

}

void LeanBar::OnTrayUpdate(TrayUpdateEnum Event) {

    // TODO: Inform the loaded items of the update

}

void LeanBar::OnTaskUpdate(TaskUpdateEnum Event, HWND hWnd) {

    // TODO: Inform the loaded items of the update

}


/**
 * WndProc for our window
 **/
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {

    // TODO: Handle messages correctly
    return DefWindowProc(hwnd, msg, wParam, lParam);

}



//////////////////////////////////////////////////////////////////////////////
//
//  BarItem Class
//

BarItem::BarItem() { }
BarItem::~BarItem() { }

// Overridable init and terminate functions for bar items
void BarItem::OnInitialize() { }
void BarItem::OnTerminate() { }

