enum t_track_command {
	track_command_default = 0,
	track_command_play,
	track_command_next,
	track_command_prev,
	track_command_settrack,
	track_command_rand,
	track_command_resume,
};

enum t_stop_reason {
        stop_reason_user = 0,
        stop_reason_eof,
        stop_reason_starting_another,
        stop_reason_shutting_down,
};

typedef struct
{
	SERVICE_BASE base;
	bool (*get_now_playing)(PMETADB_HANDLE **p_out);//! Retrieves now playing item handle. @returns true on success, false on failure (not playing).
	void (*start)(int p_command, bool p_paused);	//! Starts playback. If playback is already active, existing process is stopped first.
	void (*stop)();					//! Stops playback.
	bool (*is_playing)();				//! Returns whether playback is active.
	bool (*is_paused)();				//! Returns whether playback is active and in paused state.
	void (*pause)(bool p_state);			//! Toggles pause state if playback is active.
	bool (*get_stop_after_current)();		//! Retrieves stop-after-current-track option state.
	void (*set_stop_after_current)(bool p_state);	//! Alters stop-after-current-track option state.
	void (*set_volume)(float p_value);		//! Alters playback volume level.
	float (*get_volume)();				//! Retrieves playback volume level.
	void (*volume_up)();				//! Alters playback volume level one step up.
	void (*volume_down)();				//! Alters playback volume level one step down.
	void (*volume_mute_toggle)();			//! Toggles playback mute state.
	void (*playback_seek)(double p_value);		//! Seeks in currenly played track to specified time.
	void (*playback_seek_delta)(double p_value);	//! Seeks in currently played track by specified time forward or back.
	bool (*playback_can_seek)();			//! Returns whether currently played track is seekable. If it's not, playback_seek/playback_seek_delta calls will be ignored.
	double (*playback_get_position)();		//! Returns current playback position within currently played track, in seconds.
	bool (*playback_format_title)(DWORD *p_hook,	//! Renders information about currently playing item. @returns true on success, false when no item is currently being played.
		DWORD *p_out, DWORD *p_script, DWORD *p_filter, int p_level);
} PLAYBACK_CONTROL, *PPLAYBACK_CONTROL;
