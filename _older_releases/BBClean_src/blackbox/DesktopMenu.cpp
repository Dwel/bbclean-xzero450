/*
 ============================================================================

  This file is part of the bbLean source code
  Copyright � 2001-2003 The Blackbox for Windows Development Team
  Copyright � 2004 grischka

  http://bb4win.sourceforge.net/bblean
  http://sourceforge.net/projects/bb4win

 ============================================================================

  bbLean and bb4win are free software, released under the GNU General
  Public License (GPL version 2 or later), with an extension that allows
  linking of proprietary modules under a controlled interface. This means
  that plugins etc. are allowed to be released under any license the author
  wishes. For details see:

  http://www.fsf.org/licenses/gpl.html
  http://www.fsf.org/licenses/gpl-faq.html#LinkingOverControlledInterface

  This program is distributed in the hope that it will be useful, but
  WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
  or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License
  for more details.

 ============================================================================
*/

#include "BB.h"
#include "Workspaces.h"
#include "Menu/MenuMaker.h"
#include "Menu/Menu.h"

//===========================================================================
struct en { Menu *m; int desk; HWND hwndTop; int i; };

static BOOL task_enum_func(struct tasklist *tl, LPARAM lParam)
{
	struct en * en = (struct en *)lParam;
	int k = IsIconic(tl->hwnd);
	if ((k && en->desk < 0) || (0 == k && en->desk == tl->wkspc))
	{
		char buf[80];
		sprintf(buf, "@BBCore.ActivateTask %d", en->i+1);
		MakeMenuItem(en->m, tl->caption, buf, en->desk >= 0 && tl->hwnd == en->hwndTop, IM_TASK, tl);
	}
	en->i++;
	return TRUE;
}

static Menu * build_task_folder(int desk, const char *title, bool popup)
{
	char buf[80];
	sprintf(buf, (-1 == desk)? "IDRoot_icons" : "IDRoot_workspace%d", desk+1);
	Menu *m = MakeNamedMenu(title, buf, popup);
	if (m)
	{
		struct en en = { m, desk, GetTopTask(), 0 };
		EnumTasks(task_enum_func, (LPARAM)&en);
	}
	MakeMenuGrip(m, title);
	return m;
}

Menu * GetTaskFolder(int n, bool popup)
{
	if (n < 0) return NULL;
	DesktopInfo DI; get_desktop_info(&DI, n);
	return build_task_folder(n, DI.name, popup);
}

struct dn { Menu *m; bool popup; };

static BOOL desk_enum_func(struct DesktopInfo *DI, LPARAM lParam)
{
	struct dn *dn = (struct dn *)lParam;
	char buf[80];
	sprintf(buf, "@BBCore.SwitchToWorkspace %d", DI->number+1);
	Menu *s = build_task_folder(DI->number, DI->name, dn->popup);
	MenuItem *fi = MakeSubmenu(dn->m, s, DI->name);
	SetFolderItemCommand(fi, buf);
	CheckMenuItem(fi, DI->isCurrent);
	return TRUE;
}

Menu* MakeIconsMenu(bool popup)
{
	Menu *m = build_task_folder(-1, NLS0("Icons"), popup);
	return m;
}

Menu* MakeDesktopMenu(bool popup) {
	Menu *m = MakeNamedMenu(NLS0("Workspaces"), "IDRoot_workspaces", popup);

	struct dn dn;
	dn.m = m;
	dn.popup = popup;
	EnumDesks(desk_enum_func, (LPARAM)&dn);

	MakeMenuNOP(m);
	MakeSubmenu(m, MakeIconsMenu(popup), NLS0("Icons"));

	const char *CFG = NLS0("New/Remove");
	Menu *s = MakeNamedMenu(CFG, "IDRoot_workspaces_setup", popup);
	MakeSubmenu(m, s, CFG);

	MakeMenuItem(s, NLS0("New Workspace"), "@BBCore.AddWorkspace", false);
	if (Settings_workspaces>1) MakeMenuItem(s, NLS0("Remove Last"), "@BBCore.DelWorkspace", false);
	MakeMenuItem(s, NLS0("Edit Workspace Names"), "@BBCore.EditWorkspaceNames", false);
	MakeMenuGrip(s, "New/Remove");
	MakeMenuGrip(m, "Workspaces");

	return m;
}

//===========================================================================


