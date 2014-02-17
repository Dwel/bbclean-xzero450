typedef struct
{
	SERVICE_BASE base;
	unsigned int (*get_command_count)();				//! Retrieves number of implemented commands. Index parameter of other methods must be in 0....command_count-1 range.
	GUID* (*get_command)(GUID *p_out, unsigned int p_index);	//! Retrieves GUID of specified command.
	void (*get_name)(unsigned int p_index, DWORD *p_out);		//! Retrieves name of item, for list of commands to assign keyboard shortcuts to etc.
	bool (*get_description)(unsigned int p_index, DWORD *p_out);	//! Retrieves item's description for statusbar etc.
	GUID* (*get_parent)();						//! Retrieves GUID of owning menu group.
	int (*get_sort_priority)();
	bool (*get_display)(int p_index, DWORD *p_text, int p_flags);
	void (*execute)(unsigned int p_index, DWORD *p_callback);	//! Executes the command. p_callback parameter is reserved for future use and should be ignored / set to null pointer.
} MAINMENU_COMMANDS, *PMAINMENU_COMMANDS;
