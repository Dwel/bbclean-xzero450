typedef struct
{
	SERVICE_BASE base;
	DWORD* (*get_location)();								//! Retrieves location represented by this metadb_handle object. Returned reference is valid until calling context releases metadb_handle that returned it (metadb_handle_ptr is deallocated etc).
	bool (*format_title)(DWORD *p_hook, PSTRING p_out, PTITLEFORMAT_OBJECT **p_script,
				DWORD *p_filter);						//! Renders information about item referenced by this metadb_handle object. @returns true on success, false when dummy file_info instance was used because actual info is was not (yet) known.
	void (*metadb_lock)();									//! Locks metadb to prevent other threads from modifying it while you're working with some of its contents. Some functions (metadb_handle::get_info_locked(), metadb_handle::get_info_async_locked()) can be called only from inside metadb lock section.
	void (*metadb_unlock)();								//! Unlocks metadb after metadb_lock(). Some functions (metadb_handle::get_info_locked(), metadb_handle::get_info_async_locked()) can be called only from inside metadb lock section.
	DWORD* (*get_filestats)();								//! Returns last seen file stats, filestats_invalid if unknown.
	bool (*is_info_loaded)();								//! Queries whether cached info about item referenced by this metadb_handle object is already available.
	bool (*get_info)(PFILE_INFO p_info);							//! Queries cached info about item referenced by this metadb_handle object. Returns true on success, false when info is not yet known.
	bool (*get_info_locked)(PFILE_INFO **p_info);						//! Queries cached info about item referenced by this metadb_handle object. Returns true on success, false when info is not yet known. This is more efficient than get_info() since no data is copied.
	bool (*is_info_loaded_async)();								//! Queries whether cached info about item referenced by this metadb_handle object is already available.
	bool (*get_info_async)(PFILE_INFO p_info);						//! Queries cached info about item referenced by this metadb_handle object. Returns true on success, false when info is not yet known.
	bool (*get_info_async_locked)(PFILE_INFO **p_info);					//! Queries cached info about item referenced by this metadb_handle object. Returns true on success, false when info is not yet known. This is more efficient than get_info() since no data is copied.
	void (*format_title_from_external_info)(PFILE_INFO p_info, DWORD *p_hook, DWORD *p_out,	//! Renders information about item referenced by this metadb_handle object, using external file_info data.
						DWORD *p_script, DWORD *p_filter);
} METADB_HANDLE, *PMETADB_HANDLE;
