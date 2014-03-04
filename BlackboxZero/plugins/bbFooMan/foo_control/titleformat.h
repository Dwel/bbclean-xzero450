typedef struct
{
	SERVICE_BASE base;
	void (*run)(DWORD *p_source, DWORD *p_out, DWORD *p_filter);
} TITLEFORMAT_OBJECT, *PTITLEFORMAT_OBJECT;

typedef struct
{
	SERVICE_BASE base;
	bool (*compile)(PTITLEFORMAT_OBJECT **p_out, char *p_spec);	//! Returns false in case of a compilation error.
} TITLEFORMAT_COMPILER, *PTITLEFORMAT_COMPILER;
