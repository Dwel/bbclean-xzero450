typedef struct
{
	DWORD *current_service_entry_i;
	DWORD *next_service_entry;	//PSERVICE_ENTRY
	GUID *service_guid;
	DWORD *current_service_entry;
} SERVICE_ENTRY, *PSERVICE_ENTRY;

typedef struct
{
	int (*get_version)();
	PSERVICE_ENTRY (*get_service_list)();
	void (*get_config)(DWORD *p_stream, DWORD *p_abort);
	void (*set_config)(DWORD *p_stream, DWORD *p_abort);
	void (*set_library_path)(char *path, char *name);
	void (*services_init)(bool val);
	bool (*is_debug)();
} FOOBAR2000_CLIENT, *PFOOBAR2000_CLIENT;
/////////////////////////////////////////////////////////////////////////////////////////////////////////
// Predefinition
int get_version();
PSERVICE_ENTRY get_service_list();
void config(DWORD *p_stream, DWORD *p_abort);
void set_library_path(char *path, char *name);
void services_init(bool val);
bool is_debug();
