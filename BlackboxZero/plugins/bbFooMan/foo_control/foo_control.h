char *comp_name="Remote Control";
char *comp_ver="1.1";
char *comp_about="Foobar2k Remote Control v1.1\n"
		 "This plugin allows to control Foobar2000 by sending commands\n"
		 "to plugin window. For more information see <foomes.h>.\n"
		 "I hate fat & slow code, so I rewrote a some part of SDK\n"
		 "for using in C. See the size of plugin!\n\n"
		 "Copyright (c) Nail, 2007\n"
		 "Nail.mailme [at] gmail.com";

#include "foobar2000.c"

// Func predefinition
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
