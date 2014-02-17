typedef struct
{
	SERVICE_BASE base;
	void (*init)();
	void (*quit)();
} INITQUIT;
///////////////////////////////////////////////////////////////////////////////////////////////////////
__declspec(naked) void instance_create_initquit();
bool initquit_service_query(PSERVICE_BASE **p_out, GUID *p_guid);
void init();
void quit();
