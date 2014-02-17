enum t_display_level {
	display_level_none,	//! No playback-related info
	display_level_basic,	//! Static info and is_playing/is_paused stats
	display_level_titles,	//! Display_level_static + dynamic track titles on e.g. live streams
	display_level_all,	//! Display_level_titles + timing + VBR bitrate display etc
};


typedef struct
{
	SERVICE_BASE base;
	int (*get_playlist_count)();									//! Retrieves number of playlists. 
	int (*get_active_playlist)();									//! Retrieves index of active playlist; infinite if no playlist is active.
	void (*set_active_playlist)(int p_index);							//! Sets active playlist (infinite to set no active playlist).
	int (*get_playing_playlist)();									//! Retrieves playlist from which items to be played are taken from.
	void (*set_playing_playlist)(int p_index);							//! Sets playlist from which items to be played are taken from.	
	bool (*remove_playlists)(DWORD *p_mask);							//! Removes playlists according to specified mask. See also: bit_array.
	int (*create_playlist)(char *p_name, int p_name_length, int p_index);				//! Creates a new playlist. @returns Actual index of newly inserted playlist, infinite on failure (call from invalid context).
	bool (*reorder)(int p_order, int p_count);							//! Reorders playlist list according to specified permutation. @returns True on success, false on failure (call from invalid context).
	int (*playlist_get_item_count)(int p_playlist);							//! Retrieves number of items on specified playlist.
	void (*playlist_enum_items)(int p_playlist, DWORD *p_callback, DWORD *p_mask);			//! Enumerates contents of specified playlist.
	int (*playlist_get_focus_item)(int p_playlist);							//! Retrieves index of focus item on specified playlist); returns infinite when no item has focus.
	bool (*playlist_get_name)(int p_playlist, DWORD *p_out);					//! Retrieves name of specified playlist.
	bool (*playlist_reorder_items)(int p_playlist, int *p_order, int p_count);			//! Reorders items in specified playlist according to specified permutation.
	void (*playlist_set_selection)(int p_playlist, DWORD *p_affected, DWORD *p_status);		//! Selects/deselects items on specified playlist.
	bool (*playlist_remove_items)(int p_playlist, DWORD *mask);					//! Removes specified items from specified playlist. Returns true on success or false on failure (playlist locked).
	bool (*playlist_replace_item)(int p_playlist, int p_item, DWORD *p_new_item);			//! Replaces specified item on specified playlist. Returns true on success or false on failure (playlist locked).
	void (*playlist_set_focus_item)(int p_playlist, int p_item);					//! Sets index of focus item on specified playlist); use infinite to set no focus item.
	int (*playlist_insert_items)(int p_playlist, int p_base, DWORD *data, DWORD *p_selection);	//! Inserts new items into specified playlist, at specified position.
	void (*playlist_ensure_visible)(int p_playlist, int p_item);					//! Tells playlist renderers to make sure that specified item is visible.
	bool (*playlist_rename)(int p_index, char *p_name, int p_name_length);				//! Renames specified playlist. @returns True on success, false on failure (playlist locked).
	void (*playlist_undo_backup)(int p_playlist);							//! Creates an undo restore point for specified playlist.
	bool (*playlist_undo_restore)(int p_playlist);							//! Reverts specified playlist to last undo restore point and generates a redo restore point. @returns True on success, false on failure (playlist locked or no restore point available).
	bool (*playlist_redo_restore)(int p_playlist);							//! Reverts specified playlist to next redo restore point and generates an undo restore point. @returns True on success, false on failure (playlist locked or no restore point available).
	bool (*playlist_is_undo_available)(int p_playlist);						//! Returns whether an undo restore point is available for specified playlist.
	bool (*playlist_is_redo_available)(int p_playlist);						//! Returns whether a redo restore point is available for specified playlist.
	void (*playlist_item_format_title)(int p_playlist, int p_item, DWORD *p_hook,			//! Renders information about specified playlist item, using specified titleformatting script parameters.
						PSTRING p_out, PTITLEFORMAT_OBJECT **p_script,
						DWORD *p_filter, int p_playback_info_level);
	bool (*get_playing_item_location)(int *p_playlist, int *p_index);				//! Retrieves playlist position of currently playing item. @returns True on success, false on failure (not playing or currently played item has been removed from the playlist it was on when starting).
	bool (*playlist_sort_by_format)(int p_playlist, char *p_pattern, bool p_sel_only);		//! Sorts specified playlist - entire playlist or selection only - by specified title formatting pattern, or randomizes the order. @returns True on success, false on failure (playlist locked etc).
	void (*on_files_deleted_sorted)(DWORD *p_items);						//! For internal use only); p_items must be sorted by metadb::path_compare); use file_operation_callback static methods instead of calling this directly.
	void (*on_files_moved_sorted)(DWORD *p_from, DWORD *p_to);					//! For internal use only); p_from must be sorted by metadb::path_compare); use file_operation_callback static methods instead of calling this directly.
	bool (*playlist_lock_install)(int *p_playlist, DWORD *p_lock);					//returns false when invalid playlist or already locked
	bool (*playlist_lock_uninstall)(int *p_playlist, DWORD *p_lock);
	bool (*playlist_lock_is_present)(int *p_playlist);
	bool (*playlist_lock_query_name)(int p_playlist, DWORD *p_out);
	bool (*playlist_lock_show_ui)(int p_playlist);
	unsigned int (*playlist_lock_get_filter_mask)(int p_playlist);
	int (*playback_order_get_count)();								//! Retrieves number of available playback order modes.
	char* (*playback_order_get_name)(int p_index);							//! Retrieves name of specified playback order move. @returns Null-terminated UTF-8 encoded string containing name of the playback order mode. Returned pointer points to statically allocated string and can be safely stored without having to free it later.
	GUID* (*playback_order_get_guid)(int p_index);							//! Retrieves GUID of specified playback order mode. Used for managing playback modes without relying on names.
	int (*playback_order_get_active)();								//! Retrieves index of active playback order mode.
	void (*playback_order_set_active)(int p_index);							//! Sets index of active playback order mode.
	void (*queue_remove_mask)(DWORD *p_mask);
	void (*queue_add_item_playlist)(int p_playlist, int p_item);
	void (*queue_add_item)(DWORD *p_item);
	int (*queue_get_count)();
	void (*queue_get_contents)(DWORD *p_out);
	int (*queue_find_index)(DWORD *p_item);								//! Returns index (0-based) on success, infinite on failure (item not in queue).
	void (*register_callback)(DWORD *p_callback, unsigned p_flags);					//! Registers a playlist callback); registered object receives notifications about any modifications of any of loaded playlists.
	void (*register_callback_single)(DWORD *p_callback, unsigned p_flags);				//! Registers a playlist callback); registered object receives notifications about any modifications of active playlist.
	void (*unregister_callback)(DWORD *p_callback);							//! Unregisters a playlist callback (playlist_callback version).
	void (*unregister_callback_single)(DWORD *p_callback);						//! Unregisters a playlist callback (playlist_callback_single version).
	void (*modify_callback)(DWORD *p_callback, unsigned p_flags);					//! Modifies flags indicating which calback methods are requested (playlist_callback version).
	void (*modify_callback_single)(DWORD *p_callback, unsigned p_flags);				//! Modifies flags indicating which calback methods are requested (playlist_callback_single version).
	void (*playlist_execute_default_action)(int p_playlist, int p_item);				//! Executes default doubleclick/enter action for specified item on specified playlist (starts playing the item unless overridden by a lock to do something else).
} PLAYLIST_MANAGER, *PPLAYLIST_MANAGER;
