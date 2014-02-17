typedef struct
{
	double (*get_length)();							//! Retrieves length, in seconds.
	void (*set_length)(double p_length);					//! Sets length,  in seconds.
	void (*set_replaygain)(DWORD *p_info);					//! Sets ReplayGain information.
	DWORD* (*get_replaygain)();						//! Retrieves ReplayGain information.
	int (*meta_get_count)();						//! Retrieves count of metadata entries.
	char* (*meta_enum_name)(int p_index);					//! Retrieves the name of metadata entry of specified index. Return value is a null-terminated UTF-8 encoded string.
	int (*meta_enum_value_count)(int p_index);				//! Retrieves count of values in metadata entry of specified index. The value is always equal to or greater than 1.
	char* (*meta_enum_value)(int p_index, int p_value_number);		//! Retrieves specified value from specified metadata entry. Return value is a null-terminated UTF-8 encoded string.
	int (*meta_set_ex)(char *p_name, int p_name_length,
				char *p_value, int p_value_length);		//! Creates a new metadata entry of specified name with specified value. If an entry of same name already exists,  it is erased. Return value is the index of newly created metadata entry.
	void (*meta_insert_value_ex)(int p_index, int p_value_index,
					char *p_value, int p_value_length);	//! Inserts a new value into specified metadata entry.
	void (*meta_remove_mask)(DWORD *p_mask);				//! Removes metadata entries according to specified bit mask.
	void (*meta_reorder)(int *p_order);					//! Reorders metadata entries according to specified permutation.
	void (*meta_remove_values)(int p_index, DWORD *p_mask);			//! Removes values according to specified bit mask from specified metadata entry. If all values are removed,  entire metadata entry is removed as well.
	void (*meta_modify_value_ex)(int p_index, int p_value_index,
					char *p_value, int p_value_length);	//! Alters specified value in specified metadata entry.
	int (*info_get_count)();						//! Retrieves number of technical info entries.
	char* (*info_enum_name)(int p_index);					//! Retrieves the name of specified technical info entry. Return value is a null-terminated UTF-8 encoded string.
	char* (*info_enum_value)(int p_index);					//! Retrieves the value of specified technical info entry. Return value is a null-terminated UTF-8 encoded string.
	int (*info_set_ex)(char *p_name, int p_name_length,
				char *p_value, int p_value_length);		//! Creates a new technical info entry with specified name and specified value. If an entry of the same name already exists,  it is erased. Return value is the index of newly created entry.
	void (*info_remove_mask)(DWORD *p_mask);				//! Removes technical info entries indicated by specified bit mask.
} FILE_INFO, *PFILE_INFO;
