#include "menu.h"

unsigned int get_command_count(PMAINMENU_COMMANDS *p_api)
{
	__asm mov ecx, p_api;
	return (*p_api)->get_command_count();
}

GUID* get_command(PMAINMENU_COMMANDS *p_api, GUID *p_out, unsigned int p_index)
{
	__asm mov ecx, p_api;
	return (*p_api)->get_command(p_out, p_index);
}

void execute(PMAINMENU_COMMANDS *p_api, unsigned int p_index, DWORD *p_callback)
{
	__asm mov ecx, p_api;
	(*p_api)->execute(p_index, p_callback/*not used, may be 0*/);
	__asm mov ecx, p_api;
	(*p_api)->get_sort_priority();
}

bool run_main(PFOOBAR2000_API *p_api, GUID *p_guid)
{
	PMAINMENU_COMMANDS *api=0;
	int index=0, count, i;
	GUID m_guid;
	bool ret;
	SERVICE_CLASS_REF api_class = find_class(p_api, &mainmenu_commands);
	while(create(p_api, (PSERVICE_API **)&api, api_class, index))
	{
		for(i=0, count = get_command_count(api); i<count; i++)
			if(IsEqualGUID(p_guid, get_command(api, &m_guid, i)))
				{execute(api, i, 0), ret=true; goto ex;}
		index++;
	}
	ret=false;
	ex:
	__asm mov ecx, api;
	(*api)->base.service_release();
	return ret;
}

bool main_add_directory(PFOOBAR2000_API *p_api) {return run_main(p_api, &guid_main_add_directory);}
bool main_add_files(PFOOBAR2000_API *p_api) {return run_main(p_api, &guid_main_add_files);}
bool main_open(PFOOBAR2000_API *p_api) {return run_main(p_api, &guid_main_open);}
bool main_exit(PFOOBAR2000_API *p_api) {return run_main(p_api, &guid_main_exit);}
bool main_activate(PFOOBAR2000_API *p_api) {return run_main(p_api, &guid_main_activate);}
bool main_hide(PFOOBAR2000_API *p_api) {return run_main(p_api, &guid_main_hide);}
