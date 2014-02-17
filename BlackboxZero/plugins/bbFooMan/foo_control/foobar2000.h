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
	SERVICE_CLASS_REF __stdcall (*service_enum_find_class)(GUID *p_guid);
	bool __stdcall (*service_enum_create)(PSERVICE_API **p_out, SERVICE_CLASS_REF p_class, int p_index);
	int __stdcall (*service_enum_get_count)(SERVICE_CLASS_REF p_class);
	HWND __stdcall (*get_main_window)();
	bool __stdcall (*assert_main_thread)();
	bool __stdcall (*is_main_thread)();
	bool __stdcall (*is_shutting_down)();
	char* __stdcall (*get_profile_path)();
	bool __stdcall (*is_initializing)();
} FOOBAR2000_API, *PFOOBAR2000_API;

typedef struct
{
	void __stdcall (*instance_create)();
	DWORD *data0;                    // ??? Be zero ???
} INSTANCE;

typedef struct
{
	int __stdcall (*service_release)();
	int __stdcall (*service_add_ref)();
	bool __stdcall (*service_query)(struct SERVICE_BASE ***p_out, GUID *p_guid);
} SERVICE_BASE, *PSERVICE_BASE;
////////////////////////////////////////////////////////////////////////////////
