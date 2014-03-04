#include "play_callback.h"

void register_callback(PPLAY_CALLBACK_MANAGER *p_api, PPLAY_CALLBACK *p_callback, int flags,
			bool p_forward_status_on_register)
{
	__asm mov ecx, p_api;
	(*p_api)->register_callback(p_callback, flags, p_forward_status_on_register);
}

void unregister_callback(PPLAY_CALLBACK_MANAGER *p_api, PPLAY_CALLBACK *p_callback)
{
	__asm mov ecx, p_api;
	(*p_api)->unregister_callback(p_callback);
}
