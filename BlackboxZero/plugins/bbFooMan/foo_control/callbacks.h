// Callbacks -----------------------------------------------------------------------------------------------------------
#define MAX_CALLBACK 128
#define CLEAR_CALLBACK_TIMER 0xBEADBEAD

enum {CSTARTING, CNEW_TRACK, CSTOP, CSEEK, CPAUSE, CEDITED, CDYNAMIC_INFO,
	CDYNAMIC_INFO_TRACK, CTIME, CVOLUME_CHANGE,};

typedef struct
{
	HWND hwnd;
	char *strmem;
} WCALLBACK;

typedef struct
{
	WCALLBACK wcallback[MAX_CALLBACK];
	int count;
} FCALLBACK, *PFCALLBACK;

VOID CALLBACK ClearCallback(HWND hWnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime);
int AddCallback(FCALLBACK callback[], int back_index, HWND newhWnd, char *str);
int RemoveCallback(FCALLBACK callback[], int back_index, HWND delhWnd);
void on_playback_starting(int t_track_command, bool p_paused);
void on_playback_new_track(PMETADB_HANDLE *p_track);
void on_playback_stop(int t_stop_reason);
void on_playback_seek(double p_time);
void on_playback_pause(bool p_state);
void on_playback_edited(PMETADB_HANDLE *p_track);
void on_playback_dynamic_info(PFILE_INFO *p_info);
void on_playback_dynamic_info_track(PFILE_INFO *p_info);
void on_playback_time(double p_time);
void on_volume_change(float p_new_val);
