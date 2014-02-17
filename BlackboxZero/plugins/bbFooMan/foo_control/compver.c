#include "compver.h"

INSTANCE foobar2000_compver_i=
{
	instance_create_compver,
	&data0			  // ??? Be zero ???
};
COMPONENTVERSION foobar2000_compver=
{
	my_service_add_ref,
	my_service_add_ref,
	compver_service_query,
	get_file_name,
	get_component_name,
	get_component_version,
	get_about_message,
};

/////////////////////////////////// Func's COMPONENTVERSION foobar2000_compver ///////////////////////////////////////
__declspec(naked) void __fastcall instance_create_compver()
{
__asm{
	test ecx, ecx
	push esi
	je lab1
	lea esi, [ecx+0xC]
	jmp lab2
lab1:
	xor esi, esi
lab2:
	push edi
	mov edi, [esp+0xC]
	mov ecx, [edi]
	test ecx, ecx
	je lab3
	mov eax, [ecx]
	call [eax]
lab3:
	test esi, esi
	mov [edi], esi
	pop edi
	je lab4
	mov eax, [esi]
	mov ecx, esi
	call [eax+4]
lab4:
	pop esi
	ret 4
}}

bool compver_service_query(PSERVICE_BASE **p_out, GUID *p_guid)
{
	PSERVICE_BASE *tmp;
	__asm mov tmp, ecx;
	if(IsEqualGUID(&compverguid, p_guid))
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

__declspec(naked) void __fastcall get_file_name()
{
__asm{
	mov ecx, [esp+4]
	mov edx, [ecx]
	push -1
	mov eax, g_name
	push eax
	call [edx+8]
	ret 4
}}

__declspec(naked) void __fastcall get_component_name()
{
__asm{
	mov ecx, [esp+4]
	mov edx, [ecx]
	push -1
	mov eax, comp_name
	push eax
	call [edx+8]
	ret 4
}}

__declspec(naked) void __fastcall get_component_version()
{
__asm{
	mov ecx, [esp+4]
	mov edx, [ecx]
	push -1
	mov eax, comp_ver
	push eax
	call [edx+8]
	ret 4
}}

__declspec(naked) void __fastcall get_about_message()
{
__asm{
	mov ecx, [esp+4]
	mov edx, [ecx]
	push -1
	mov eax, comp_about
	push eax
	call [edx+8]
	ret 4
}}
