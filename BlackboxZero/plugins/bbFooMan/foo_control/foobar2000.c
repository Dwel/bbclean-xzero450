#define FOOBAR2000_CLIENT_VERSION 71

#include <windows.h>
#include <stdbool.h>

typedef void* SERVICE_CLASS_REF;
typedef DWORD SERVICE_API;
typedef SERVICE_API *PSERVICE_API;

// Predefinition
//int service_release(PSERVICE_BASE **);
//int service_add_ref(PSERVICE_BASE *);
//int my_service_add_ref(void);

typedef struct
{
	SERVICE_CLASS_REF (*service_enum_find_class)(GUID *p_guid);
	bool (*service_enum_create)(PSERVICE_API **p_out, SERVICE_CLASS_REF p_class, int p_index);
	int (*service_enum_get_count)(SERVICE_CLASS_REF p_class);
	HWND (*get_main_window)();
	bool (*assert_main_thread)();
	bool (*is_main_thread)();
	bool (*is_shutting_down)();
	char* (*get_profile_path)();
	bool (*is_initializing)();
} FOOBAR2000_API, *PFOOBAR2000_API;

typedef struct
{
	void (*instance_create)();
	DWORD *data0;                    // ??? Be zero ???
} INSTANCE;

typedef struct
{
	int (*service_release)();
	int (*service_add_ref)();
	bool (*service_query)(struct SERVICE_BASE ***p_out, GUID *p_guid);
} SERVICE_BASE, *PSERVICE_BASE;
////////////////////////////////////////////////////////////////////////////////

int service_release(PSERVICE_BASE *p_api)
{
	__asm mov ecx, p_api;
	return (*p_api)->service_release();
}

int service_add_ref(PSERVICE_BASE *p_api)
{
	__asm mov ecx, p_api;
	return (*p_api)->service_add_ref();
}

int my_service_add_ref() {return 1;}

DWORD data0=0;
HINSTANCE g_hIns;
char *g_name, *g_full_path;
PFOOBAR2000_API g_api;

#include "foomes.h"
#include "guids.c"
#include "compver.c"
#include "initquit.c"
#include "service_entry.c"
#include "foobar2000_get_interface.c"
#include "api.c"
#include "string_base.c"
#include "titleformat.c"
#include "file_info.c"
#include "metadb_handle.c"
#include "playlist_manager.c"
#include "playback_contol.c"
#include "menu.c"
#include "play_callback.c"
