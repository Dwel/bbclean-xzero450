#include "metadb_handle.h"

void metadb_lock(PMETADB_HANDLE *p_api)
{
	__asm mov ecx, p_api;
	(*p_api)->metadb_lock;
	service_release((PSERVICE_BASE*)p_api);
}

void metadb_unlock(PMETADB_HANDLE *p_api)
{
	__asm mov ecx, p_api;
	(*p_api)->metadb_unlock;
	service_release((PSERVICE_BASE*)p_api);
}

bool format_title(PMETADB_HANDLE *p_api, DWORD *p_hook, PSTRING p_out,
			PTITLEFORMAT_OBJECT **p_script, DWORD *p_filter)
{
	__asm mov ecx, p_api;
    return (*p_api)->format_title((DWORD*)0, p_out, p_script, 0);
}


bool get_info_locked(PMETADB_HANDLE *p_api, PFILE_INFO **p_info)
{
	__asm mov ecx, p_api;
	return (*p_api)->get_info_locked(p_info);
}

double meta_get_length(PMETADB_HANDLE *p_api)
{
	PFILE_INFO *p_info=0;
	double ret;
	service_add_ref((PSERVICE_BASE*)p_api);
	metadb_lock(p_api);
	if(get_info_locked(p_api, &p_info)) ret = file_info_get_length(p_info);
	else ret = P_ERROR;
	service_release((PSERVICE_BASE*)p_api);
	return ret;
}
