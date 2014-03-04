int get_count(PFOOBAR2000_API *p_api, SERVICE_CLASS_REF p_class)
{
	__asm mov ecx, p_api;
	return (*p_api)->service_enum_get_count(p_class);
}

SERVICE_CLASS_REF find_class(PFOOBAR2000_API *p_api, GUID *serv_guid)
{
	__asm mov ecx, p_api;
	return (*p_api)->service_enum_find_class(serv_guid);
}

bool create(PFOOBAR2000_API *p_api, PSERVICE_API **p_out, SERVICE_CLASS_REF p_class, int p_index)
{
	__asm mov ecx, p_api;
	return (*p_api)->service_enum_create(p_out, p_class, p_index);
}

PSERVICE_API create_api(PFOOBAR2000_API *p_api, GUID *serv_guid)
{
	PSERVICE_API *tmp=0;	// !!!Must be 0!!!
	SERVICE_CLASS_REF api_class = find_class(p_api, serv_guid);
	if(get_count(p_api, api_class)==1)
		if(create(p_api, &tmp, api_class, 0)) return *tmp;
	return 0;
}
