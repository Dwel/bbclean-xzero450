enum {
        flag_on_playback_starting            = 1 << 0,
        flag_on_playback_new_track            = 1 << 1,
        flag_on_playback_stop                = 1 << 2,
        flag_on_playback_seek                = 1 << 3,
        flag_on_playback_pause                = 1 << 4,
        flag_on_playback_edited                = 1 << 5,
        flag_on_playback_dynamic_info        = 1 << 6,
        flag_on_playback_dynamic_info_track    = 1 << 7,
        flag_on_playback_time                = 1 << 8,
        flag_on_volume_change                = 1 << 9,

        flag_on_playback_all = flag_on_playback_starting | flag_on_playback_new_track |
            flag_on_playback_stop | flag_on_playback_seek |
            flag_on_playback_pause | flag_on_playback_edited |
            flag_on_playback_dynamic_info | flag_on_playback_dynamic_info_track | flag_on_playback_time | flag_on_volume_change,
};


typedef struct
{
    //! Playback process is being initialized. on_playback_new_track() should be called soon after this when first file is successfully opened for decoding.
	void (*on_playback_starting)(int t_track_command, bool p_paused);
    //! Playback advanced to new track.
	void (*on_playback_new_track)(PMETADB_HANDLE *p_track);
    //! Playback stopped.
	void (*on_playback_stop)(int t_stop_reason);
    //! User has seeked to specific time.
	void (*on_playback_seek)(double p_time);
    //! Called on pause/unpause.
	void (*on_playback_pause)(bool p_state);
    //! Called when currently played file gets edited.
	void (*on_playback_edited)(PMETADB_HANDLE *p_track);
    //! Dynamic info (VBR bitrate etc) change.
	void (*on_playback_dynamic_info)(PFILE_INFO *p_info);
    //! Per-track dynamic info (stream track titles etc) change. Happens less often than (*on_playback_dynamic_info().
	void (*on_playback_dynamic_info_track)(PFILE_INFO *p_info);
    //! Called every second, for time display
	void (*on_playback_time)(double p_time);
    //! User changed volume settings. Possibly called when not playing.
    //! @param p_new_val new volume level in dB; 0 for full volume.
	void (*on_volume_change)(float p_new_val);
} PLAY_CALLBACK, *PPLAY_CALLBACK;


typedef struct
{
	SERVICE_BASE base;
	//! Registers a play_callback object.
	void (*register_callback)(PPLAY_CALLBACK *p_callback, int p_flags,
					bool p_forward_status_on_register);
	//! Unregisters a play_callback object.
	void (*unregister_callback)(PPLAY_CALLBACK *p_callback);
} PLAY_CALLBACK_MANAGER, *PPLAY_CALLBACK_MANAGER;
