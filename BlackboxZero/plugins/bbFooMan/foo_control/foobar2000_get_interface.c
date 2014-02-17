__declspec(dllexport) PFOOBAR2000_CLIENT* _cdecl foobar2000_get_interface(PFOOBAR2000_API *p_api, HINSTANCE hIns)
{
	g_hIns = hIns;
	g_api = *p_api;
	return &pg_client;
}
