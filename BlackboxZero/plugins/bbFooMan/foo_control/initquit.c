#include "initquit.h"

INSTANCE foobar2000_initquit_i=
{
	instance_create_initquit,
	&data0 			 // ??? Be zero ???
};
INITQUIT foobar2000_initquit=
{
	my_service_add_ref,
	my_service_add_ref,
	initquit_service_query,
	init,
	quit,
};

/////////////////////////////////// INITQUIT foobar2000_initquit ///////////////////////////////////////
	__declspec(naked) void __fastcall instance_create_initquit()
{
__asm{
	push esi
	push edi
	mov edi, [esp+0xC]
	lea esi, [ecx+0xC]
	mov ecx, [edi]
	test ecx, ecx
	je lab1
	mov eax, [ecx]
	call [eax]
lab1:
	test esi, esi
	mov [edi], esi
	je lab2
	mov eax, [esi]
	mov ecx, esi
	call [eax+4]
lab2:
	pop edi
	pop esi
	ret 4
}}

bool initquit_service_query(PSERVICE_BASE **p_out, GUID *p_guid)
{
	PSERVICE_BASE *tmp;
	__asm mov tmp, ecx;
	if(IsEqualGUID(&initquitguid, p_guid))
	{
		if(*p_out!=0)
		{
			__asm mov ecx, p_out;
			__asm mov ecx, dword [ecx];
			(**p_out)->service_release();
		}
		(*p_out)=tmp;
		if(tmp)
		{
			__asm mov ecx, tmp;
			(*tmp)->service_add_ref();
		}
		return true;
	}
	return false;
}
