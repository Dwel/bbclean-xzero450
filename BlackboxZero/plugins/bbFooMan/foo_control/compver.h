typedef struct
{
	SERVICE_BASE base;
	void (*get_file_name)();
	void (*get_component_name)();
	void (*get_component_version)();
	void (*get_about_message)();
} COMPONENTVERSION;

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Predefinition
__declspec(naked) void instance_create_compver();
bool compver_service_query(PSERVICE_BASE **p_out, GUID *p_guid);
__declspec(naked) void get_file_name();
__declspec(naked) void get_about_message();
__declspec(naked) void get_component_version();
__declspec(naked) void get_file_name();
__declspec(naked) void get_component_name();
