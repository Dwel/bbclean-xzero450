/*

    bbPlusPlus: A nice wrapper for the bb4win API

    Part of LeanBar2: The bbLeanBar replacement
    (c) 2008, bbClean Developers

    This project is released under the Gnu Public License (GPL) Version 2.

*/

#include "bbplusplus.h"

//////////////////////////////////////////////////////////////////////////////
//
//  bbPlugin Class
//

/**
 * Constructor, should be chained with the plugins constructor or no events will come through.
 * @param hInst The hInstance passed with beginPlugin.
 **/
bbPlugin::bbPlugin(HINSTANCE hInst) {

    // Save the hInstance for later use
    this->hInstance = hInst;

    // Create the window we'll use for listening to blackbox events
    this->pluginClass.style = CS_HREDRAW | CS_VREDRAW;
    this->pluginClass.lpfnWndProc = bbPlugin::StaticWndProc;
    this->pluginClass.cbClsExtra = 0;
    this->pluginClass.cbWndExtra = 0;
    this->pluginClass.hInstance = this->hInstance;
    this->pluginClass.hIcon = LoadIcon(NULL, IDI_APPLICATION); //default icon
    this->pluginClass.hCursor = LoadCursor(NULL, IDC_ARROW);   //default arrow mouse cursor
    this->pluginClass.hbrBackground = (HBRUSH)(COLOR_BACKGROUND+1);
    this->pluginClass.lpszMenuName = NULL;                     //no menu
    this->pluginClass.lpszClassName = "bbpluspluswnd";

    if(!RegisterClass(&this->pluginClass))                     //register the WNDCLASS
    {
       MessageBox(NULL, "This Program Requires Windows NT",
                        "Error", MB_OK);
       return;
    }

    this->pluginWnd = CreateWindowEx(0,"bbpluspluswnd","bbpluspluswnd",WS_OVERLAPPEDWINDOW,CW_USEDEFAULT,CW_USEDEFAULT,320,200,NULL,NULL,this->hInstance,this);

}

/**
 * Destructor
 **/
bbPlugin::~bbPlugin() {

    DestroyWindow(this->pluginWnd);
    UnregisterClass("bbpluspluswnd",this->hInstance);

}

/**
 * StaticWndProc is the windowproc that's called by windows. It relays the messages to
 * the PluginWndProc.
 **/
LRESULT CALLBACK bbPlugin::StaticWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {

    bbPlugin* pParent;

    // Get pointer to window
    if(uMsg == WM_CREATE) {
        pParent = (bbPlugin*)((LPCREATESTRUCT)lParam)->lpCreateParams;
#ifdef _WIN64
        SetWindowLongPtr(hWnd,GWLP_USERDATA,reinterpret_cast<LONG_PTR>(pParent));
#else
        SetWindowLongPtr(hWnd,GWL_USERDATA,(LONG_PTR)pParent);
#endif
    } else {
#ifdef _WIN64
        pParent = (bbPlugin*)GetWindowLongPtr(hWnd,GWLP_USERDATA);
#else
        pParent = (bbPlugin*)GetWindowLongPtr(hWnd,GWL_USERDATA);
#endif
        if(!pParent) return DefWindowProc(hWnd,uMsg,wParam,lParam);
    }

    // pParent->m_hWnd = hWnd;
    return pParent->PluginWndProc(hWnd,uMsg,wParam,lParam);

}

/**
 * The PluginWndProc is running in the scope of the class instance and can therefore
 * use "this" to call on the appropriate events.
 **/
LRESULT bbPlugin::PluginWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {

    switch(uMsg)
    {
        /* Misc events */
        case BB_RECONFIGURE:
            this->OnReconfigure();
            break;
        case BB_BROADCAST:
        {
            LPCSTR broam = (LPCSTR)lParam;
            std::string message = broam;
            this->OnBroadcast(message);
            break;
        }
        case BB_REDRAWGUI:
            this->OnRedrawGUI();
            break;
        case BB_SHUTDOWN:
            this->OnShutdown(static_cast<ShutdownActionEnum>(wParam));
            break;
        case BB_TOGGLEPLUGINS:
            this->OnTogglePlugins();
            break;
        case BB_SETTOOLBARLABEL:
        {
            LPCSTR broam = (LPCSTR)lParam;
            std::string message = broam;
            this->OnSetToolbarLabel(message);
            break;
        }
        case BB_QUIT:
            this->OnQuit();
            break;
        case BB_RESTART:
            this->OnRestart();
            break;
        case BB_SETSTYLE:
            this->OnSetStyle();
            break;
        /* xob-specific events */
        case BB_TOGGLESLIT:
            this->OnToggleSlit();
            break;
        case BB_TOGGLETOOLBAR:
            this->OnToggleToolbar();
            break;
//@TODO: ifdefed by Mojmir (compile-ability: BB_TOGGLETRAY has same value as BB_TOGGLESYSTEMBAR)
#ifdef NOT_XOBLITE
        case BB_TOGGLETRAY:
            this->OnToggleTray();
            break;
#endif
        case BB_TOGGLESYSTEMBAR:
            this->OnToggleSystembar();
            break;
        /* Tasks and tray */
//@TODO: commented out by Mojmir (compile-ability: BB_TASKUPDATE unknown)
        //case BB_TASKUPDATE:
            // virtual void OnTaskUpdate(TaskUpdateEnum Event, HWND hWnd) {} // BB_TASKSUPDATE
            //break;
        case BB_TRAYUPDATE:
            // virtual void OnTrayUpdate(TrayUpdateEnum Event) {} // BB_TRAYUPDATE
            break;
        /* Workspace */
        case BB_WORKSPACE:
            // TODO: This should only fire OnWorkspareChange when the workspace is *changed*
            // Other events should be put in here to handle f.ex. OnWorkspaceLabelChange() etc.
            this->OnWorkspaceChange();
            break;
    }

    // Call default window proc if we haven't handled the message
    return DefWindowProc(hWnd,uMsg,wParam,lParam);

}


/**
 * GetPluginPath is basically an alias for GetPath(PATH_PLUGIN)
 **/
std::string bbPlugin::GetPluginPath() {

    return this->GetPath(PATH_PLUGIN);

}

/**
 * GetPath returns the path of the item specified from the FilePathEnum
 * @param Item The item to retrieve.
 **/
std::string bbPlugin::GetPath(FilePathEnum Item) {

    std::string Path;
    char szPath[MAX_PATH];

    // Retrieve the path that was requested
    switch(Item) {
        case PATH_PLUGIN:
            GetModuleFileName(this->hInstance, szPath, MAX_PATH);
            Path = szPath;
            break;
        case PATH_BLACKBOX:
            Path = GetBlackboxPath(szPath, MAX_PATH);
            break;
        case PATH_BLACKBOXRC:
            Path = bbrcPath(NULL);
            break;
        case PATH_EXTENSIONSRC:
            Path = extensionsrcPath(NULL);
            break;
        case PATH_MENURC:
            Path = menuPath(NULL);
            break;
        case PATH_PLUGINRC:
            Path = plugrcPath(NULL);
            break;
        case PATH_STYLE:
            Path = stylePath(NULL);
            break;
    }

    // And return it
    return Path;

}

/**
 * Executes a broam
 * @param Broam the bro@m to execute
 **/
void bbPlugin::Execute(std::string Broam) {

    SendMessage(this->BlackBoxWindow(),BB_EXECUTE,(WPARAM)0, (LPARAM)Broam.c_str());

}

/**
 * Outputs a debug message. Use f.ex. DebugView to view it
 * @param Message The message to output
 **/
void bbPlugin::DebugMsg(std::string Message) {

    dbg_printf(Message.c_str());

}

/**
 * Returns the hWnd of the blackbox window
 **/
HWND bbPlugin::BlackBoxWindow() {

    return GetBBWnd();

}

/**
 * Return the version of blackbox the plugin is being loaded under,
 * f.ex. "bbLean 1.16"
 **/
std::string bbPlugin::GetBBVersion() {

    std::string Version;
    Version = ::GetBBVersion();
    return Version;

}

/**
 * Returns info on the operating system the plugin is being loaded under
 **/
std::string bbPlugin::GetOSInfo() {

    std::string OSInfo;
    OSInfo = ::GetOSInfo();
    return OSInfo;

}

/**
 * Returns a matching .rc filename based on the plugin path and filename,
 * for example "plugins\foo\foo.dll" will return "plugins\foo\foo.rc".
 **/
std::string bbPlugin::RCFile() {

    // TODO: bbPlugin::RCFile()
    std::string Value = "";
    return Value;

}

/**
 * Retrieves a boolean value from the specified .rc file
 * @param FileName The filename of the .rc-file
 * @param Key The key to retrieve
 * @param Default The default value
 **/
bool bbPlugin::GetBoolean(std::string FileName, std::string Key, bool Default) {

    bool Value = ReadBool(FileName.c_str(),Key.c_str(),Default);
    return Value;

}

/**
 * Retrieves a Int value from the specified .rc file. Wildcards are allowed.
 * @param FileName The filename of the .rc-file
 * @param Key The key to retrieve
 * @param Default The default value
 **/
int bbPlugin::GetInteger(std::string FileName, std::string Key, int Default) {

    int Value = ReadInt(FileName.c_str(), Key.c_str(), Default);
    return Value;

}

/**
 * Retrieves a string value from the specified .rc file. Wildcards are allowed.
 * @param FileName The filename of the .rc-file
 * @param Key The key to retrieve
 * @param Default The default value
 **/
std::string bbPlugin::GetString(std::string FileName, std::string Key, std::string Default) {

    dbg_printf("Trying to read '%s' from '%s'",Key.c_str(),FileName.c_str());

    std::string Value = ReadString(FileName.c_str(), Key.c_str(), Default.c_str());
    return Value;

}

/**
 * Retrieves a color value from the specified .rc file. Wildcards are allowed.
 * @param FileName The filename of the .rc-file
 * @param Key The key to retrieve
 * @param Default The default value
 **/
COLORREF bbPlugin::GetColor(std::string FileName, std::string Key, std::string Default) {

    COLORREF Value = ReadColor(FileName.c_str(), Key.c_str(), Default.c_str());
    return Value;

}

/**
 * Returns True if the last call to GetColor, GetString, GetInt, or GetBoolean found a match.
 * The BB API differs between no match, exact match, and wildcard match, but here we settle
 * for match or no match.
 **/
bool bbPlugin::FoundValue() {

    return (FoundLastValue != 0);

}

/**
 * Writes a boolean value from the specified .rc file.
 * @param FileName The filename of the .rc-file
 * @param Key The key to write
 * @param Value The value to write
 **/
void bbPlugin::SetBoolean(std::string FileName, std::string Key, bool Value) {

    WriteBool(FileName.c_str(), Key.c_str(), Value);

}

/**
 * Writes a integer value from the specified .rc file.
 * @param FileName The filename of the .rc-file
 * @param Key The key to write
 * @param Value The value to write
 **/
void bbPlugin::SetInteger(std::string FileName, std::string Key, int Value) {

    WriteInt(FileName.c_str(), Key.c_str(), Value);

}

/**
 * Writes a string value from the specified .rc file.
 * @param FileName The filename of the .rc-file
 * @param Key The key to write
 * @param Value The value to write
 **/
void bbPlugin::SetString(std::string FileName, std::string Key, std::string Value) {

    WriteString(FileName.c_str(), Key.c_str(), Value.c_str());

}

/**
 * Writes a color value from the specified .rc file.
 * @param FileName The filename of the .rc-file
 * @param Key The key to write
 * @param Value The value to write
 **/
void bbPlugin::SetColor(std::string FileName, std::string Key, COLORREF Value) {

    WriteColor(FileName.c_str(), Key.c_str(), Value);

}

/**
 * Registers a message to be handled by the plugin
 * @param Message The message to register
 **/
void bbPlugin::RegisterMessage(long Message) {

    int msgs[] = { Message, 0 };
    SendMessage(this->BlackBoxWindow(),BB_REGISTERMESSAGE,(WPARAM)this->pluginWnd, (LPARAM)msgs);

}

/**
 * Unregisters a previously registered message
 * @param Message The message to unregister
 **/
void bbPlugin::UnregisterMessage(long Message) {

    int msgs[] = { Message, 0 };
    SendMessage(this->BlackBoxWindow(),BB_REGISTERMESSAGE,(WPARAM)this->pluginWnd, (LPARAM)msgs);

}

//////////////////////////////////////////////////////////////////////////////
//
//  bbMenu Class
//

/**
 * Create a new menu
 * @param ID The ID of the menu
 * @param Caption The caption of the menu
 **/
bbMenu::bbMenu(std::string ID, std::string Caption) {

    this->menuID = ID;
    this->hMenu = MakeNamedMenu(Caption.c_str(), ID.c_str(), true);

}

bbMenu::~bbMenu() { }

/**
 * Returns the menu handle
 **/
Menu *bbMenu::GetMenu() {

    return this->hMenu;

}

/**
 * Show the menu
 **/
void bbMenu::Show() {

    ShowMenu(this->hMenu);

}

/**
 * Add an item to the menu
 * @param Title The title of the menu item
 * @param Command The command to invoke
 * @param ShowIndicator ?
 * @param Disabled Set to true if the item is to be disabled (grayed out)
 **/
void bbMenu::AddItem(std::string Title, std::string Command, bool ShowIndicator, bool Disabled) {

    // Add the menu item here
    MakeMenuItem(this->hMenu,Title.c_str(),Command.c_str(),ShowIndicator);

    // Disable the item if set to disabled
    //@TODO: commented out by Mojmir (compile-ability: DisableLastItem unknown)
    //if (Disabled) DisableLastItem(this->hMenu);

}

/**
 * Add a NOP item to the menu
 * @param Title The text for the NOP item (optional)
 **/
void bbMenu::AddNOP(std::string Title) {

    MakeMenuNOP(this->hMenu,(Title!="")?Title.c_str():NULL);

}

/**
 * Adds a value item that allows the user to select an integer value
 * @param Title The title of the item
 * @param Command The command to execute for each updated value
 * @param Value The initial value to show
 * @param Minimum The minimum value that's available
 * @param Maximum The maximum value that's available
 * @param Disabled Set to true if the item is to be disabled (grayed out)
 **/
void bbMenu::AddItemInt(std::string Title, std::string Command, int Value, int Minimum, int Maximum, bool Disabled) {

    MakeMenuItemInt(this->hMenu,Title.c_str(),Command.c_str(),Value,Minimum,Maximum);

    // Disable the item if set to disabled
    //@TODO: commented out by Mojmir (compile-ability: DisableLastItem unknown)
    //if (Disabled) DisableLastItem(this->hMenu);

}

/**
 * Adds a value item that allows the user to edit a string
 * @param Title The title of the item
 * @param Command The command to execute when the user presses enter
 * @param Value The default string to display
 * @param Disabled Set to true if the item is to be disabled (grayed out)
 **/
void bbMenu::AddItemString(std::string Title, std::string Command, std::string Value, bool Disabled) {

    MakeMenuItemString(this->hMenu, Title.c_str(), Command.c_str(), Value.c_str());

    // Disable the item if set to disabled
    //@TODO: commented out by Mojmir (compile-ability: DisableLastItem unknown)
    //if (Disabled) DisableLastItem(this->hMenu);

}

/**
 * Adds a path as a submenu to the item. If Command is specified, it will be invoked
 * when an item is selected, "%s" will be replaced with the selected filename.
 * @param Title The title of the item
 * @param Path The path to create the menu from
 * @param Command The command to execute. %s will be replaced with the selected filename
 * @param Disabled Set to true if the item is to be disabled (grayed out)
 **/
void bbMenu::AddItemPath(std::string Title, std::string Path, std::string Command, bool Disabled) {

    //@TODO: commented out by Mojmir (compile-ability: MakePathMenu unknown)
    //MakePathMenu(this->hMenu, Title.c_str(), Path.c_str(), Command.c_str());

    // Disable the item if set to disabled
    //@TODO: commented out by Mojmir (compile-ability: DisableLastItem unknown)
    //if (Disabled) DisableLastItem(this->hMenu);

}

/**
 * Adds a submenu to the menu. The menu must be previously created.
 * @param Title The title of the item
 * @param Menu The menu to add as a child item
 * @param Disabled Set to true if the item is to be disabled (grayed out)
 **/
void bbMenu::AddSubmenu(std::string Title, bbMenu ChildMenu, bool Disabled) {

    MakeSubmenu(this->hMenu, ChildMenu.GetMenu(), Title.c_str());

    // Disable the item if set to disabled
    //@TODO: commented out by Mojmir (compile-ability: DisableLastItem unknown)
    //if (Disabled) DisableLastItem(this->hMenu);
}

/**
 * Check if the menu is still visible
 **/
bool bbMenu::IsVisible() {

    return(MenuExists(this->menuID.c_str()));

}



//////////////////////////////////////////////////////////////////////////////
//
//  bbPlusPlus Namespace
//

/**
 * Paints a gradient into the specified hdc
 * @param hdc The device context to draw into
 * @param gradientType The type of gradient to paint, one of GradientTypeEnum
 * @param colorFrom The first color
 * @param colorTo The second color
 * @param interlaced Wether the gradient should be drawn interlaced
 * @param bevelStyle The style of bevel
 * @param bevelPosition The bevel position
 * @param bevelWidth The width of the bevel
 * @param borderColor The border color
 * @param borderWidth The width of the border
 **/
void bbPlusPlus::PaintGradient(HDC hdc, RECT rect, int gradientType, COLORREF colorFrom, COLORREF colorTo, bool interlaced, int bevelStyle, int bevelPosition, int bevelWidth, COLORREF borderColor, int borderWidth) {

    MakeGradient(hdc,rect,gradientType,colorFrom,colorTo,interlaced,bevelStyle,bevelPosition,bevelWidth,borderColor,borderWidth);

}
