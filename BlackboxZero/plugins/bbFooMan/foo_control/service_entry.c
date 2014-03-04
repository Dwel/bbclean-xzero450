#include "service_entry.h"

SERVICE_ENTRY initquit=
{
	(DWORD*)&foobar2000_initquit_i,
	0,
	&initquitguid,
	(DWORD*)&foobar2000_initquit
};
SERVICE_ENTRY compver=
{
	(DWORD*)&foobar2000_compver_i,
	(DWORD*)&initquit,
	&compverguid,
	(DWORD*)&foobar2000_compver
};

FOOBAR2000_CLIENT g_client=
{
	get_version,
	get_service_list,
	config,
	config,
	set_library_path,
	services_init,
	is_debug
};

PFOOBAR2000_CLIENT pg_client=&g_client;
///////////////////////////////// Func's for [foobar2000_client g_client] //////////////////////////////
int get_version()
{
	return FOOBAR2000_CLIENT_VERSION;
}
PSERVICE_ENTRY get_service_list()
{
	return &compver;
}
void config(DWORD *p_stream, DWORD *p_abort) {} //I not used set_ & get_ config

void set_library_path(char *path, char *name)
{
	g_full_path = calloc(strlen(path)+1, 1);
	strcpy(g_full_path, path);
	g_name = calloc(strlen(name)+1, 1);
	strcpy(g_name, name);
}
void services_init(bool val)
{
	/*if (val) g_initialized = true;
	g_services_available = val;*/
}
bool is_debug()
{
#ifdef _DEBUG
	return true;
#else
	return false;
#endif
}
