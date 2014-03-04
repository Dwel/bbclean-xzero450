#include "file_info.h"

double file_info_get_length(PFILE_INFO *p_api)
{
	__asm mov ecx, p_api;
	return (*p_api)->get_length();
}
