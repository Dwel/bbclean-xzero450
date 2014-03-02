/*

    Test project for bbPlusPlus

*/

#include "../bbplusplus.h"

// plugin info
LPCSTR szVersion        = "bbPlusPlusTest 1.0";
LPCSTR szAppName        = "bbPlusPlusTest";
LPCSTR szInfoVersion    = "1.0 (BBClean SVN)";
LPCSTR szInfoAuthor     = "Noccy";
LPCSTR szInfoRelDate    = __DATE__;
LPCSTR szInfoLink       = "http://dev.noccy.com";
LPCSTR szInfoEmail      = "lazerfisk@yahoo.com";

// The plugin implementation
class TestPlugin : public bbPlugin {
    public:
    // Constructor
    TestPlugin(HINSTANCE hInst) : bbPlugin(hInst) {
        RegisterMessage(BB_RECONFIGURE);
        RegisterMessage(BB_BROADCAST);
        DebugMsg("TestPlugin is loading...");

        // Get the path of the blackbox.rc file
        std::string rcpath = GetPath(PATH_BLACKBOXRC);
        // And read the setting for toolbar status
        DebugMsg("Toolbar status = " + GetString(rcpath,"session.screen0.toolbar.enabled:","fail"));

    }
    ~TestPlugin() {
        UnregisterMessage(BB_RECONFIGURE);
        UnregisterMessage(BB_BROADCAST);
        DebugMsg("TestPlugin is unloading...");
    }
    void OnReconfigure() {
        DebugMsg("BlackBox is reconfiguring");
    }
    void OnBroadcast(std::string Message) {
        if (Message == "@testplugin.menu") {
            bbMenu MyMenu("MyMenu","Test menu");
            MyMenu.AddItem("Test item","@bbcore.about",false,false);
            MyMenu.Show();
        }
    }
};

// This is our plugin instance
TestPlugin *TestPluginInst;

// And these are the stubs to get it all rolling
int beginPlugin(HINSTANCE hInstance) {
	TestPluginInst = new TestPlugin(hInstance);
    return 0;
}
int beginSlitPlugin(HINSTANCE hInstance, HWND hSlit) {
	TestPluginInst = new TestPlugin(hInstance);
	return 0;
}
int beginPluginEx(HINSTANCE hInstance, HWND slit) {
	TestPluginInst = new TestPlugin(hInstance);
	return 0;
}

// And to unload it again
void endPlugin(HINSTANCE hPluginInstance) {
    delete TestPluginInst;
}

// And for retrieving info on the plugin
LPCSTR pluginInfo(int field) {
	switch (field) 	{
		case PLUGIN_NAME:           return szAppName;       // Plugin name
		case PLUGIN_VERSION:        return szInfoVersion;   // Plugin version
		case PLUGIN_AUTHOR:         return szInfoAuthor;    // Author
		case PLUGIN_RELEASE:        return szInfoRelDate;   // Release date, preferably in yyyy-mm-dd format
		case PLUGIN_LINK:           return szInfoLink;      // Link to author's website
		case PLUGIN_EMAIL:          return szInfoEmail;     // Author's email
		default:                    return szVersion;       // Fallback: Plugin name + version, e.g. "MyPlugin 1.0"
	}
}

