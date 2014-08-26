/* BBPlayer - Your favorite media player interface as a BB4W plugin
   Copyright (C) 2004 kana <nicht AT s8 DOT xrea DOT con>

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*/

#include <limits.h>
#include <stdlib.h>

#include <BBApi.h>

#include "BBPlayerSDK.h"


#define NUMBER_OF(a) (sizeof(a) / sizeof((a)[0]))

#define NOP()




static const char* const BBP_NAME = "BBPlayer";
static const char* const BBP_VERSION = "0.0.4";
static const char* const BBP_AUTHOR = "kana";
static const char* const BBP_RELEASE = "2004-07-11/2005-01-19";
static const char* const BBP_LINK
                             = "http://nicht.s8.xrea.com/2004/07/11/BBPlayer";
static const char* const BBP_EMAIL = "nicht AT s8 DOT xrea DOT con";

static const int BBP_MESSAGES[] = {
	BB_BROADCAST,
	BB_RECONFIGURE,
	0
};


#define TIMER_ID_INTERVAL 1
#define TIMER_ID_UNTI_AUTO_HIDE 2

typedef enum {
	HIDE_BY_AUTO,
	HIDE_BY_USER    /* toggling plugins and etc */
} HideBy;




static struct {
	HWND window;

	char _rc_path[MAX_PATH];
	char* rc_path;
	char text[1024];
	HMODULE plugin;
	BBPlayer_GetMusicInfo_prot pBBPlayer_GetMusicInfo;
	BBPlayer_Control_prot pBBPlayer_Control;
	BBPlayer_InterpretBroam_prot pBBPlayer_InterpretBroam;
	bool anti_auto_hide;
	HideBy hide_by;
	HWND slit;
	bool in_slit;

	int x;
	int y;
	int width;
	int height;
	bool always_on_top;
	int interval;  /* in milli second */
	int style_number;
	char not_found_text[80];
	char dtoption_string[256];
	UINT dtoption_cache;
	int horizontal_padding;
	int vertical_padding;
	bool visibility;
	bool toggle_with_plugins;
	char player_type[16];
	bool auto_hide;
	int snap_to_edge;
	int anti_auto_hide_time;
	char on_title_change[256];
} G;








static void
error(const char* format, ...)
{
	char buf[80*25];
	va_list va;

	va_start(va, format);
	vsnprintf(buf, NUMBER_OF(buf), format, va);
	va_end(va);

	MessageBox( G.window, buf, BBP_NAME,
	            MB_OK | MB_ICONERROR
	              | MB_SETFOREGROUND | MB_TASKMODAL | MB_TOPMOST );
}


static void
SNPRINTF(char* buf, int size, const char* format, ...)
{
	va_list va;

	if (size <= 0) return;

	va_start(va, format);
	vsnprintf(buf, size, format, va);
	va_end(va);

	buf[size - 1] = '\0';
}


int
strrepl(char* buf, int size, const char* pattern, const char* replacement)
{
	char* p;
	int buf_len;
	int pattern_len;
	int replacement_len;

	buf_len = strlen(buf);
	pattern_len = strlen(pattern);
	replacement_len = strlen(replacement);
	if (size < buf_len + replacement_len - pattern_len + 1)
		return -1;

	p = strstr(buf, pattern);
	if (p == NULL)
		return 0;

	memmove( p + replacement_len,
	         p + pattern_len,
	         sizeof(char) * (strlen(p + pattern_len) + 1) );
	memcpy(p, replacement, sizeof(char) * replacement_len);

	return 0;
}


static bool
is_pressed(int key)
{
	return GetKeyState(key) & (1 << (sizeof(SHORT) * CHAR_BIT - 1));
}


static void
slit_add(HWND window)
{
	if (G.slit != NULL) {
		SendMessage(G.slit, SLIT_ADD, 0, (LPARAM)window);
		G.in_slit = true;
	}
}

static void
slit_remove(HWND window)
{
	if (G.slit != NULL) {
		SendMessage(G.slit, SLIT_REMOVE, 0, (LPARAM)window);
		G.in_slit = false;
	}
}

static void
slit_update(void)
{
	if (G.slit != NULL)
		SendMessage(G.slit, SLIT_UPDATE, 0, 0);
}


static void
make_always_on_top(HWND hwnd, bool flag)
{
	SetWindowPos( hwnd,
	              (flag ? HWND_TOPMOST : HWND_NOTOPMOST),
	              0x4C, 0x69, 0x74, 0x79,
	              SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOOWNERZORDER
	                | SWP_NOSIZE );
}


static void
resize(HWND hwnd, int width, int height)
{
	SetWindowPos( hwnd, NULL, 0, 0, width, height,
		      SWP_NOACTIVATE | SWP_NOMOVE
			  | SWP_NOOWNERZORDER | SWP_NOZORDER );

	slit_update();
}


static void
move(HWND hwnd, int x, int y)
{
	SetWindowPos( hwnd, NULL, x, y, 0, 0,
		      SWP_NOACTIVATE | SWP_NOOWNERZORDER
	                  | SWP_NOSIZE | SWP_NOZORDER );
}


static void
set_visibility(HWND hwnd, bool flag)
{
	ShowWindow(hwnd, (flag ? SW_SHOWNA : SW_HIDE));

	slit_update();
}




static void
about_this_plugin(void)
{
	BBPlayer_PluginInfo_prot plugin_info;
	char buf[80*25];

	plugin_info = (BBPlayer_PluginInfo_prot)
	                  GetProcAddress(G.plugin, "BBPlayer_PluginInfo");

	SNPRINTF( buf, NUMBER_OF(buf),
		  "%s %s (%s)\n"
		  "by %s <%s>\n",
		  BBP_NAME, BBP_VERSION, BBP_RELEASE,
		  BBP_AUTHOR, BBP_EMAIL );

	if (plugin_info != NULL) {
		const char* email;
		const char* link;

		email = (*plugin_info)(PLUGIN_EMAIL);
		link = (*plugin_info)(PLUGIN_LINK);

		SNPRINTF( buf + strlen(buf), NUMBER_OF(buf) - strlen(buf),
		          "\n"
		          "%s %s (%s)\n"
		          "by %s %s%s%s\n"
	                  "%s%s",
		          (*plugin_info)(PLUGIN_NAME),
		          (*plugin_info)(PLUGIN_VERSION),
		          (*plugin_info)(PLUGIN_RELEASE),
		          (*plugin_info)(PLUGIN_AUTHOR),
		          (email[0] != '\0' ? "<" : ""),
		          email,
		          (email[0] != '\0' ? ">" : ""),
		          link,
		          (link[0] != '\0' ? "\n" : "")
		);
	}

	SNPRINTF( buf + strlen(buf), NUMBER_OF(buf) - strlen(buf),
	          "\n"
	          "For latest information, see <%s>.\n",
	          BBP_LINK );

	MessageBox( G.window, buf, BBP_NAME,
	            MB_OK | MB_ICONINFORMATION
	              | MB_SETFOREGROUND | MB_TASKMODAL | MB_TOPMOST );
}




static const struct {const char* name; int number;} STYLE_TABLE[] = {
	{"toolbar", SN_TOOLBAR},
	{"toolbar.button", SN_TOOLBARBUTTON},
	{"toolbar.button.pressed", SN_TOOLBARBUTTONP},
	{"toolbar.label", SN_TOOLBARLABEL},
	{"toolbar.windowlabel", SN_TOOLBARWINDOWLABEL},
	{"toolbar.clock", SN_TOOLBARCLOCK},
	{"menu.title", SN_MENUTITLE},
	{"menu.frame", SN_MENUFRAME},
	{"menu.hilite", SN_MENUHILITE}
};

static const char*
style_name_from_number(int n)
{
	int i;

	for (i = 0; i < (int)NUMBER_OF(STYLE_TABLE); i++)
		if (n == STYLE_TABLE[i].number)
			return STYLE_TABLE[i].name;

	return "toolbar";
}

static int
style_number_from_name(const char* s)
{
	int i;

	for (i = 0; i < (int)NUMBER_OF(STYLE_TABLE); i++)
		if (!_stricmp(s, STYLE_TABLE[i].name))
			return STYLE_TABLE[i].number;

	return SN_TOOLBAR;
}


static const struct {const char* name; UINT flag;} DTOPTION_TABLE[] = {
	{";bottom;", DT_BOTTOM},
	{";calcrect;", DT_CALCRECT},
	{";center;", DT_CENTER},
	{";editcontrol;", DT_EDITCONTROL},
	{";end_ellipsis;", DT_END_ELLIPSIS},
	{";expandtabs;", DT_EXPANDTABS},
	{";externalleading;", DT_EXTERNALLEADING},
	/*{";hideprefix;", DT_HIDEPREFIX},*/
	{";internal;", DT_INTERNAL},
	{";left;", DT_LEFT},
	{";modifystring;", DT_MODIFYSTRING},
	{";noclip;", DT_NOCLIP},
	/*{";nofullwidthcharbreak;", DT_NOFULLWIDTHCHARBREAK},*/
	{";noprefix;", DT_NOPREFIX},
	{";path_ellipsis;", DT_PATH_ELLIPSIS},
	/*{";prefixonly;", DT_PREFIXONLY},*/
	{";right;", DT_RIGHT},
	{";rtlreading;", DT_RTLREADING},
	{";singleline;", DT_SINGLELINE},
	{";tabstop;", DT_TABSTOP},
	{";top;", DT_TOP},
	{";vcenter;", DT_VCENTER},
	{";wordbreak;", DT_WORDBREAK},
	{";word_ellipsis;", DT_WORD_ELLIPSIS}
};

static UINT
dtoption_from_string(const char* s)
{
	/* s = ";name1;name2; ... ;nameN;" */

	UINT flag;
	int i;

	flag = 0;
	for (i = 0; i < (int)NUMBER_OF(DTOPTION_TABLE); i++)
		if (strstr(s, DTOPTION_TABLE[i].name))
			flag |= DTOPTION_TABLE[i].flag;

	return flag;
}




#define RC_KEY(name) "bbplayer." name ":"
#define RC_KEY_X RC_KEY("x")
#define RC_KEY_Y RC_KEY("y")
#define RC_KEY_WIDTH RC_KEY("width")
#define RC_KEY_HEIGHT RC_KEY("height")
#define RC_KEY_ALWAYS_ON_TOP RC_KEY("always_on_top")
#define RC_KEY_INTERVAL RC_KEY("interval")
#define RC_KEY_STYLE RC_KEY("style")
#define RC_KEY_NOT_FOUND_TEXT RC_KEY("not_found_text")
#define RC_KEY_DTOPTION RC_KEY("dtoption")
#define RC_KEY_HORIZONTAL_PADDING RC_KEY("horizontal_padding")
#define RC_KEY_VERTICAL_PADDING RC_KEY("vertical_padding")
#define RC_KEY_VISIBILITY RC_KEY("visibility")
#define RC_KEY_TOGGLE_WITH_PLUGINS RC_KEY("toggle_with_plugins")
#define RC_KEY_PLAYER_TYPE RC_KEY("player_type")
#define RC_KEY_AUTO_HIDE RC_KEY("auto_hide")
#define RC_KEY_SNAP_TO_EDGE RC_KEY("snap_to_edge")
#define RC_KEY_UNTI_AUTO_HIDE_TIME RC_KEY("anti_auto_hide_time")
#define RC_KEY_ON_TITLE_CHANGE RC_KEY("on_title_change")


static void
read_settings(const char* rc_path)
{
	const char* s;

	G.x = ReadInt(rc_path, RC_KEY_X, 0);
	G.y = ReadInt(rc_path, RC_KEY_Y, 0);
	G.width = ReadInt(rc_path, RC_KEY_WIDTH, 256);
	G.height = ReadInt(rc_path, RC_KEY_HEIGHT, 20);
	G.always_on_top = ReadBool(rc_path, RC_KEY_ALWAYS_ON_TOP, true);
	G.interval = ReadInt(rc_path, RC_KEY_INTERVAL, 2000);

	s = ReadString(rc_path, RC_KEY_STYLE, "toolbar");
	G.style_number = style_number_from_name(s);

	s = ReadString(rc_path, RC_KEY_NOT_FOUND_TEXT, "Player not found");
	SNPRINTF(G.not_found_text, NUMBER_OF(G.not_found_text), "%s", s);

	s = ReadString( rc_path, RC_KEY_DTOPTION,
	                ";left;noprefix;singleline;vcenter;" );
	SNPRINTF(G.dtoption_string, NUMBER_OF(G.dtoption_string), "%s", s);
	G.dtoption_cache = dtoption_from_string(s);

	G.horizontal_padding = ReadInt(rc_path, RC_KEY_HORIZONTAL_PADDING, 4);
	G.vertical_padding = ReadInt(rc_path, RC_KEY_VERTICAL_PADDING, 0);
	G.visibility = ReadBool(rc_path, RC_KEY_VISIBILITY, true);
	G.toggle_with_plugins = ReadBool( rc_path, RC_KEY_TOGGLE_WITH_PLUGINS,
	                                  true );

	s = ReadString(rc_path, RC_KEY_PLAYER_TYPE, "lilith");
	SNPRINTF(G.player_type, NUMBER_OF(G.player_type), "%s", s);

	G.auto_hide = ReadBool(rc_path, RC_KEY_AUTO_HIDE, false);
	G.snap_to_edge = ReadInt(rc_path, RC_KEY_SNAP_TO_EDGE, 10);
	G.anti_auto_hide_time
		= ReadInt(rc_path, RC_KEY_UNTI_AUTO_HIDE_TIME, 4000);

	s = ReadString(rc_path, RC_KEY_ON_TITLE_CHANGE, "");
	SNPRINTF(G.on_title_change, NUMBER_OF(G.on_title_change), "%s", s);
}


static void
write_settings(const char* rc_path)
{
	WriteInt(rc_path, RC_KEY_X, G.x);
	WriteInt(rc_path, RC_KEY_Y, G.y);
	WriteInt(rc_path, RC_KEY_WIDTH, G.width);
	WriteInt(rc_path, RC_KEY_HEIGHT, G.height);
	WriteBool(rc_path, RC_KEY_ALWAYS_ON_TOP, G.always_on_top);
	WriteInt(rc_path, RC_KEY_INTERVAL, G.interval);
	WriteString( rc_path, RC_KEY_STYLE,
	             style_name_from_number(G.style_number) );
	WriteString(rc_path, RC_KEY_NOT_FOUND_TEXT, G.not_found_text);
	WriteString(rc_path, RC_KEY_DTOPTION, G.dtoption_string);
	WriteInt(rc_path, RC_KEY_HORIZONTAL_PADDING, G.horizontal_padding);
	WriteInt(rc_path, RC_KEY_VERTICAL_PADDING, G.vertical_padding);
	WriteBool(rc_path, RC_KEY_VISIBILITY, G.visibility);
	WriteBool(rc_path, RC_KEY_TOGGLE_WITH_PLUGINS, G.toggle_with_plugins);
	WriteString(rc_path, RC_KEY_PLAYER_TYPE, G.player_type);
	WriteBool(rc_path, RC_KEY_AUTO_HIDE, G.auto_hide);
	WriteInt(rc_path, RC_KEY_SNAP_TO_EDGE, G.snap_to_edge);
	WriteInt(rc_path, RC_KEY_UNTI_AUTO_HIDE_TIME, G.anti_auto_hide_time);
	WriteString(rc_path, RC_KEY_ON_TITLE_CHANGE, G.on_title_change);
}




static void
menu(bool force_show)
{
	Menu* menu;

	menu = MakeNamedMenu(BBP_NAME, "BBPlayer", force_show);


	MakeMenuItem(menu, "Play / Pause", "@BBPlayer.ctrl.play", false);
	MakeMenuItem(menu, "Stop", "@BBPlayer.ctrl.stop", false);
	MakeMenuItem(menu, "Next", "@BBPlayer.ctrl.next", false);
	MakeMenuItem(menu, "Prev", "@BBPlayer.ctrl.prev", false);

	MakeMenuNOP(menu, "");

	if (!G.in_slit) {
		MakeMenuItem( menu, "Always on top",
			      "@BBPlayer.conf.always_on_top toggle",
		              G.always_on_top );
		MakeMenuItem( menu, "Auto hide",
			      "@BBPlayer.conf.auto_hide toggle",
		              G.auto_hide );
		MakeMenuItem( menu, "Toggle with plugins",
			      "@BBPlayer.conf.toggle_with_plugins toggle",
			      G.toggle_with_plugins );
		MakeMenuItem( menu, "Visibility",
			      "@BBPlayer.conf.visibility toggle",
		              G.visibility );
	}

	if (G.slit != NULL) {
		MakeMenuItem( menu, "Use slit",
			      "@BBPlayer.conf.in_slit toggle", G.in_slit );
	}

	MakeMenuNOP(menu, "");

	MakeMenuItem(menu, "About", "@BBPlayer.about", false);


	ShowMenu(menu);
}




static void
update_text(void)
{
	int result;

	result = (*(G.pBBPlayer_GetMusicInfo))( BBPlayer_CURRENT, 0,
	                                        BBPlayer_DEFAULT,
	                                        G.text, NUMBER_OF(G.text) );
	if (result == BBPlayer_SUCCESS) {
		if ( G.auto_hide && !G.visibility && G.hide_by == HIDE_BY_AUTO
		     && !G.in_slit )
		{
			set_visibility(G.window, G.visibility = true);
		}
	} else {
		SNPRINTF(G.text, NUMBER_OF(G.text), "%s", G.not_found_text);

		if ( G.auto_hide && G.visibility && !G.anti_auto_hide
		     && !G.in_slit )
		{
			G.hide_by = HIDE_BY_AUTO;
			set_visibility(G.window, G.visibility = false);
		}
	}

	if (G.on_title_change[0] == '@') {
		char buf[ NUMBER_OF(G.text)
		          + NUMBER_OF(G.on_title_change)
		          + 1 ];

		strcpy(buf, G.on_title_change);
		strrepl(buf, NUMBER_OF(buf), "<DEFAULT>", G.text);

		SendMessage(GetBBWnd(), BB_BROADCAST, 0, (LPARAM)buf);
	}

	InvalidateRect(G.window, NULL, FALSE);
}




static void
set_anti_auto_hide(void)
{
	if (0 < G.anti_auto_hide_time) {
		G.anti_auto_hide = true;
		SetTimer( G.window, TIMER_ID_UNTI_AUTO_HIDE,
			  G.anti_auto_hide_time, NULL );
	}
}


static void
unset_anti_auto_hide(void)
{
	if (0 < G.anti_auto_hide_time) {
		G.anti_auto_hide = false;
		KillTimer(G.window, TIMER_ID_UNTI_AUTO_HIDE);
		update_text();
	}
}




static void
_interpret_broam_bool(bool* var, const char* value)
{
	if (!_stricmp(value, "toggle"))
		*var = !(*var);
	else if (!_stricmp(value, "true"))
		*var = true;
	else if (!_stricmp(value, "false"))
		*var = false;
	else
		NOP();
}


static void
interpret_broam(const char* broam)
{
	(*(G.pBBPlayer_InterpretBroam))(broam);

	if (G.toggle_with_plugins && !_stricmp(broam, "@BBShowPlugins")) {
		interpret_broam("@BBPlayer.conf.visibility true");
		return;
	} else if (G.toggle_with_plugins && !_stricmp(broam,"@BBHidePlugins")) {
		interpret_broam("@BBPlayer.conf.visibility false");
		return;
	}

	if (!!strncmp(broam, "@BBPlayer.", 10)) return;
	broam += 10;

	if (!strncmp(broam, "conf.", 5)) {
		broam += 5;

		if (!strncmp(broam, "x ", 2) && !G.in_slit) {
			G.x = atoi(broam + 2);
			move(G.window, G.x, G.y);
		} else if (!strncmp(broam, "y ", 2) && !G.in_slit) {
			G.y = atoi(broam + 2);
			move(G.window, G.x, G.y);
		} else if (!strncmp(broam, "width ", 6)) {
			G.width = atoi(broam + 6);
			resize(G.window, G.width, G.height);
		} else if (!strncmp(broam, "height ", 7)) {
			G.height = atoi(broam + 7);
			resize(G.window, G.width, G.height);
		} else if (!strncmp(broam, "always_on_top ", 14)) {
			_interpret_broam_bool(&(G.always_on_top), broam + 14);
			make_always_on_top(G.window, G.always_on_top);
		} else if (!strncmp(broam, "interval ", 9)) {
			G.interval = atoi(broam + 9);
			/* KillTimer(G.window, TIMER_ID_INTERVAL); needless */
			SetTimer(G.window,TIMER_ID_INTERVAL, G.interval, NULL);
		} else if (!strncmp(broam, "style ", 6)) {
			G.style_number = style_number_from_name(broam + 6);
		} else if (!strncmp(broam, "dtoption ", 9)) {
			SNPRINTF( G.dtoption_string,
			          NUMBER_OF(G.dtoption_string),
			          "%s",
			          broam + 9 );
			G.dtoption_cache = dtoption_from_string(broam + 9);
		} else if (!strncmp(broam, "horizontal_padding ", 19)) {
			G.horizontal_padding = atoi(broam + 19);
		} else if (!strncmp(broam, "vertical_padding ", 17)) {
			G.vertical_padding = atoi(broam + 17);
		} else if (!strncmp(broam, "not_found_text ", 15)) {
			SNPRINTF( G.not_found_text,
			          NUMBER_OF(G.not_found_text),
			          "%s",
			          broam + 15 );
		} else if (!strncmp(broam, "visibility ", 11) && !G.in_slit) {
			bool t;

			t = G.visibility;
			_interpret_broam_bool(&t, broam + 11);
			if (t) {
				set_anti_auto_hide();
			} else {
				if (!G.visibility && G.hide_by == HIDE_BY_AUTO)
					NOP();
				else
					G.hide_by = HIDE_BY_USER;
			}
			set_visibility(G.window, G.visibility = t);
		} else if (!strncmp(broam, "toggle_with_plugins ", 20)) {
			broam += 20;
			_interpret_broam_bool(&(G.toggle_with_plugins), broam);
		} else if (!strncmp(broam, "auto_hide ", 10)) {
			_interpret_broam_bool(&(G.auto_hide), broam + 10);
		} else if (!strncmp(broam, "snap_to_edge ", 13)) {
			G.snap_to_edge = atoi(broam + 13);
		} else if (!strncmp(broam, "anti_auto_hide_time ", 20)) {
			G.anti_auto_hide_time = atoi(broam + 20);
		} else if (!strncmp(broam, "in_slit ", 8) && G.slit != NULL) {
			_interpret_broam_bool(&(G.in_slit), broam + 8);
			(G.in_slit ? slit_add : slit_remove)(G.window);
		} else if (!strncmp(broam, "on_title_change ", 16)) {
			error( "%s\n"
			       "\n"
			       "This option cannot be modified via bro@m.",
			       RC_KEY_ON_TITLE_CHANGE );
		} else {
			return;
		}

		update_text();
		menu(false);
	} else if (!strncmp(broam, "ctrl.", 5)) {
		broam += 5;

		if (!strncmp(broam, "play", 4)) {
			(*(G.pBBPlayer_Control))(
				BBPlayer_PLAY,
				(broam[4] != '\0' ? atoi(broam + 4) : 0)
			);
		} else if (!strcmp(broam, "stop")) {
			(*(G.pBBPlayer_Control))(BBPlayer_STOP, 0);
		} else if (!strcmp(broam, "pause")) {
			(*(G.pBBPlayer_Control))(BBPlayer_PAUSE, 0);
		} else if (!strcmp(broam, "next")) {
			(*(G.pBBPlayer_Control))(BBPlayer_NEXT, 0);
		} else if (!strcmp(broam, "prev")) {
			(*(G.pBBPlayer_Control))(BBPlayer_PREV, 0);
		} else {
			return;
		}

		update_text();
	} else if (!strcmp(broam, "about")) {
		about_this_plugin();
	} else {
		return;
	}
}








static LRESULT CALLBACK
window_procedure(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp)
{
	switch (msg) {
	case BB_BROADCAST:
		interpret_broam((const char*)lp);
		break;

	case BB_RECONFIGURE:
		read_settings(G.rc_path);
		make_always_on_top(hwnd, G.always_on_top);
		move(hwnd, G.x, G.y);
		resize(hwnd, G.width, G.height);
		update_text();
		break;


	case WM_CREATE:
		make_always_on_top(hwnd, G.always_on_top);

		SendMessage( GetBBWnd(), BB_REGISTERMESSAGE,
		             (WPARAM)hwnd, (LPARAM)BBP_MESSAGES );
		MakeSticky(hwnd);
		SetTimer(hwnd, TIMER_ID_INTERVAL, G.interval, NULL);
		if (G.in_slit) slit_add(hwnd);
		break;

	case WM_DESTROY:
		if (G.in_slit) slit_remove(hwnd);
		/**
		 * Any timer will be killed automatically...
		 * KillTimer(hwnd, TIMER_ID_INTERVAL);
		 */
		RemoveSticky(hwnd);
		SendMessage( GetBBWnd(), BB_UNREGISTERMESSAGE,
		             (WPARAM)hwnd, (LPARAM)BBP_MESSAGES );
		break;

	case WM_RBUTTONUP:
	case WM_NCRBUTTONUP:
		menu(true);
		break;

	case WM_NCHITTEST:
		return (!G.in_slit && is_pressed(VK_CONTROL))
		           ? HTCAPTION : HTCLIENT;

	case WM_TIMER:
		switch (wp) {
		default:
			break;

		case TIMER_ID_INTERVAL:
			update_text();
			break;

		case TIMER_ID_UNTI_AUTO_HIDE:
			unset_anti_auto_hide();
			break;
		}
		return 0;

	case WM_WINDOWPOSCHANGING:
		if (1 <= G.snap_to_edge)
			SnapWindowToEdge((WINDOWPOS*)lp, G.snap_to_edge, true);
		break;


	case WM_PAINT: {
		HDC hdc;
		PAINTSTRUCT ps;
		RECT rect;
		StyleItem* style;
		int bevel_and_border_width;
		HGDIOBJ prev_font;

		GetClientRect(hwnd, &rect);
		style = (StyleItem*)GetSettingPtr(G.style_number);
		bevel_and_border_width = *(int*)GetSettingPtr(SN_BEVELWIDTH)
		                         +*(int*)GetSettingPtr(SN_BORDERWIDTH);

		hdc = BeginPaint(hwnd, &ps);
		MakeStyleGradient(hdc, &rect, style, true);

		SetBkMode(hdc, TRANSPARENT);
		SetTextColor(hdc, style->TextColor);
		rect.left += bevel_and_border_width + G.horizontal_padding;
		rect.top += bevel_and_border_width + G.vertical_padding;
		rect.right -= bevel_and_border_width + G.horizontal_padding;
		rect.bottom -= bevel_and_border_width + G.vertical_padding;
		prev_font = SelectObject(hdc, CreateStyleFont(style));
		DrawText(hdc, G.text, -1, &rect, G.dtoption_cache);
		DeleteObject(SelectObject(hdc, prev_font));
		EndPaint(hwnd, &ps);
		} break;
	}

	return DefWindowProc(hwnd, msg, wp, lp);
}




static int
load_plugin(HINSTANCE bbplayer_instance, const char* path_to_plugin)
{
	FARPROC plugin_begin;
	FARPROC plugin_end;
	FARPROC plugin_info;
	FARPROC get_music_info;
	FARPROC control;
	FARPROC interpret_broam;
	struct {FARPROC* func; const char* name;} table[] = {
		{&plugin_begin, "BBPlayer_PluginBegin"},
		{&plugin_end, "BBPlayer_PluginEnd"},
		{&plugin_info, "BBPlayer_PluginInfo"},
		{&get_music_info, "BBPlayer_GetMusicInfo"},
		{&control, "BBPlayer_Control"},
		{&interpret_broam, "BBPlayer_InterpretBroam"}
	};
	int i;

	G.plugin = LoadLibrary(path_to_plugin);
	if (G.plugin == NULL) {
		error("Plugin `%s' not found", path_to_plugin);
		goto E_LOAD_LIBRARY;
	}

	for (i = 0; i < (int)NUMBER_OF(table); i++) {
		*(table[i].func) = GetProcAddress(G.plugin, table[i].name);
		if (*(table[i].func) == NULL) {
			error( "Plugin `%s' not implement `%s'",
			       path_to_plugin, table[i].name );
			goto E_GET_PROCC_ADDRESS;
		}
	}

	if ( (*(BBPlayer_PluginBegin_prot)plugin_begin)
	         (bbplayer_instance, G.plugin) != BBPlayer_SUCCESS)
	{
		error("Plugin `%s' failed to initialize", path_to_plugin);
		goto E_BEGIN_PLUGIN;
	}

	G.pBBPlayer_GetMusicInfo = (BBPlayer_GetMusicInfo_prot)get_music_info;
	G.pBBPlayer_Control = (BBPlayer_Control_prot)control;
	G.pBBPlayer_InterpretBroam
		= (BBPlayer_InterpretBroam_prot)interpret_broam;

	return 0;


E_BEGIN_PLUGIN:
E_GET_PROCC_ADDRESS:
	FreeLibrary(G.plugin);
E_LOAD_LIBRARY:
	return !0;
}


static void
unload_plugin(HINSTANCE bbplayer_instance)
{
	FARPROC plugin_end;

	plugin_end = GetProcAddress(G.plugin, "BBPlayer_PluginEnd");
	if (plugin_end != NULL)
	  (*(BBPlayer_PluginEnd_prot)plugin_end)(bbplayer_instance, G.plugin);

	FreeLibrary(G.plugin);
}








extern "C" DLL_EXPORT int
beginPlugin(HINSTANCE hPluginInstance)
{
	WNDCLASS wc;
	char* t;
	char dir[MAX_PATH];
	char dll[MAX_PATH];

	if (G.slit == NULL) G.slit = FindWindow("BBSlit", NULL);

	G.anti_auto_hide = false;

	GetModuleFileName(hPluginInstance, dir, NUMBER_OF(dir));
	dir[NUMBER_OF(dir) - 1] = '\0';
	t = strrchr(dir, '\\');
	if (t != NULL) t[1] = '\0';
	SNPRINTF(G._rc_path, NUMBER_OF(G._rc_path), "%s%s.rc", dir, BBP_NAME);
	G.rc_path = G._rc_path;

	read_settings(G.rc_path);

	SNPRINTF(dll, NUMBER_OF(dll), "%sfor_%s.dll", dir, G.player_type);
	if (load_plugin(hPluginInstance, dll) != 0)
		goto E_LOAD_PLUGIN;

	ZeroMemory(&wc, sizeof(wc));
	wc.lpfnWndProc = window_procedure;
	wc.hInstance = hPluginInstance;
	wc.lpszClassName = BBP_NAME;
	if (!RegisterClass(&wc))
		{error("RegisterClass failed");  goto E_REGISTER_CLASS;}

	G.window = CreateWindowEx( WS_EX_TOOLWINDOW,
	                           BBP_NAME,
	                           NULL,  /* BBP_NAME, -- see [2004-07-20] */
	                           WS_POPUP,
	                           G.x, G.y,
	                           G.width, G.height,
	                           NULL,
	                           NULL,
	                           hPluginInstance,
	                           NULL );
	if (G.window == NULL)
		{error("CreateWindowEx failed");  goto E_CREATE_WINDOW_EX;}
	set_visibility(G.window, G.visibility);
	update_text();

	return 0;


E_CREATE_WINDOW_EX:
	UnregisterClass(BBP_NAME, hPluginInstance);
E_REGISTER_CLASS:
E_LOAD_PLUGIN:
	return !0;
}




extern "C" DLL_EXPORT void
endPlugin(HINSTANCE hPluginInstance)
{
	RECT rect;

	GetWindowRect(G.window, &rect);
	G.width = rect.right - rect.left;
	G.height = rect.bottom - rect.top;
	if (!G.in_slit) {
		G.x = rect.left;
		G.y = rect.top;
	}

	DestroyWindow(G.window);  G.window = NULL;
	UnregisterClass(BBP_NAME, hPluginInstance);
	unload_plugin(hPluginInstance);

	write_settings(G.rc_path);
}




extern "C" DLL_EXPORT LPCSTR
pluginInfo(int field)
{
	switch (field) {
	case PLUGIN_NAME:    return BBP_NAME;
	case PLUGIN_VERSION: return BBP_VERSION;
	case PLUGIN_AUTHOR:  return BBP_AUTHOR;
	case PLUGIN_RELEASE: return BBP_RELEASE;
	case PLUGIN_LINK:    return BBP_LINK;
	case PLUGIN_EMAIL:   return BBP_EMAIL;
	default:             return BBP_NAME;
	}
}




extern "C" DLL_EXPORT int
beginSlitPlugin(HINSTANCE plugin_instance, HWND slit_window)
{
	G.slit = slit_window;
	G.in_slit = true;
	return beginPlugin(plugin_instance);
}




/* __END__ */
