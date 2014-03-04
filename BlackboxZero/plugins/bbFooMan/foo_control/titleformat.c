#include "titleformat.h"

bool compile(PTITLEFORMAT_COMPILER *p_api, PTITLEFORMAT_OBJECT **p_out, char *p_spec)
{
	__asm mov ecx, p_api;
	return (*p_api)->compile(p_out, p_spec);
}
