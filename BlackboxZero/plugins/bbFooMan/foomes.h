#define FOO_REMOTE_WND_CLASS "FOOBAR2k REMOTE CONTROL"
#define P_ERROR -1		// Return if failed

#define WM_FOO_GET_ACTIVE_PLAYLIST_COUNT	WM_USER+1

// Return formating item information
// Before calling you must set foo_control window caption with title script
// (for more information see titleformat_help.html in foobar2000 folder).
// SetWindowText(rhWnd, "$if(%title%,$if2(%album artist%,%artist%) - %title%,%_filename_ext%)");
// Then send WM_FOO_GET_PLAYLIST_ITEM message:
// SendMessage(rhWnd, WM_FOO_GET_PLAYLIST_ITEM, track_numb, 0)
// in wParam you must specify track number or -1 to use current playing
// At last you can use GetWindowText(rhWnd, title, 128) to receive info
#define WM_FOO_GET_PLAYLIST_ITEM		WM_USER+2	

// Return current playing track number
#define WM_FOO_GET_CURRENT_TRACK		WM_USER+3

// Set ordering
// wParam: ordering mode
// Can be others in old or new versions of foobar!!!
// 0 - Default
// 1 - Repeate (playlist)
// 2 - Repeate (track)
// 3 - Shuffle (tracks)
// 4 - Shuffle (albums)
// 5 - Shuffle (directories)
#define WM_FOO_ORDER				WM_USER+4

// Open foobar dialogs
#define WM_FOO_OPEN				WM_USER+5
#define WM_FOO_ADD				WM_USER+6
#define WM_FOO_ADDDIR				WM_USER+7

#define WM_FOO_PLAY				WM_USER+8
#define WM_FOO_STOP				WM_USER+9
#define WM_FOO_PLAY_NEXT			WM_USER+10
#define WM_FOO_PLAY_PREV			WM_USER+11
#define WM_FOO_PLAY_RANDOM			WM_USER+12
#define WM_FOO_PAUSE				WM_USER+13
#define WM_FOO_PLAY_PAUSE			WM_USER+14
#define WM_FOO_VOLUME_UP			WM_USER+15
#define WM_FOO_VOLUME_DOWN			WM_USER+16

// Return volume from -100 (min) to 0 (max)
#define WM_FOO_GET_VOLUME			WM_USER+17
#define WM_FOO_SET_VOLUME			WM_USER+18

#define WM_FOO_MUTE				WM_USER+19	// set/clear mute
#define WM_FOO_IS_PLAYING			WM_USER+20	// bool
#define WM_FOOL_IS_PAUSED			WM_USER+21	// bool
#define WM_FOO_GET_STOP_AFTER_CURRENT		WM_USER+22	// bool
#define WM_FOO_SET_STOP_AFTER_CURRENT		WM_USER+23	// bool

//Returns whether currently played track is seekable.
//If it's not, playback_seek/playback_seek_delta calls will be ignored.
#define WM_FOO_PLAYBACK_CAN_SEEK		WM_USER+24

// 1-...(track length in seconds)
#define WM_FOO_PLAYBACK_SEEK			WM_USER+25
// +/-
#define WM_FOO_PLAYBACK_SEEK_DELTA		WM_USER+26
// in seconds
#define WM_FOO_GET_POSITION			WM_USER+27
#define WM_FOO_GET_LENGTH			WM_USER+28

#define WM_FOO_EXIT				WM_USER+29
#define WM_FOO_ACTIVATE				WM_USER+30
#define WM_FOO_HIDE				WM_USER+31

// Track numb in wParam (start with 0)
#define WM_FOO_PLAY_TRACK			WM_USER+32

//--------------------------------------------------------------------------------------------------------------
// Manipulation with callbacks

#define WM_FOO_REGISTER_CALLBACK		WM_USER+33
#define WM_FOO_CALLBACK				WM_USER+33

// wParam - one of the values that mentioned below:
	#define	PLAYBACK_START			0x1	// t_track_command in lParam

// Before calling you must set foo_control window caption with title script
// (see WM_FOO_GET_PLAYLIST_ITEM message above) and then immediately send
// WM_FOO_REGISTER_CALLBACK with this value. Set "" if not need.
// When this value went to reciever, you can get information of new track
// by GetWindowText with foo_control window handle in first paramets.
// The information which you have requested establishing inquiry will be received.
// !!! Be carefully: when this message went, track is not playing yet
	#define PLAYBACK_NEW_TRACK		0x2


	#define PLAYBACK_STOP			0x4	// t_stop_reason in lParam
	#define PLAYBACK_SEEK			0x8	// user seeked playback; p_time in lParam
	#define PLAYBACK_PAUSE			0x10	// p_state in lParam

// See PLAYBACK_NEW_TRACK
	#define PLAYBACK_EDITED			0x20	// user edited current track


	#define PLAYBACK_DYNAMIC_INFO		0x40	// Dynamic info (VBR bitrate etc) change
	#define PLAYBACK_DYNAMIC_INFO_TRACK	0x80	// Per-track dynamic info (stream track titles etc) change
							// Happens less often than PLAYBACK_DYNAMIC_INFO

	#define PLAYBACK_TIME			0x100	// sends every seconds; p_time in lParam
	#define PLAYBACK_VOLUME_CHANGE		0x200	// p_new_val in lParam
	#define PLAYBACK_ALL			PLAYBACK_START|PLAYBACK_NEW_TRACK| \
						PLAYBACK_STOP|PLAYBACK_SEEK|PLAYBACK_PAUSE| \
						PLAYBACK_EDITED|PLAYBACK_DYNAMIC_INFO| \
						PLAYBACK_DYNAMIC_INFO_TRACK| \
						PLAYBACK_TIME|PLAYBACK_VOLUME_CHANGE
// it is showing what calback set/remove
// you also can combine values: PLAYBACK_STOP|PLAYBACK_START|PLAYBACK_PAUSE
// lParam - handle of receiver window
	
// To remove callback you must send message with the same parameters
// but with WM_FOO_UNREGISTER_CALLBACK message
#define WM_FOO_UNREGISTER_CALLBACK		WM_USER+34
//--------------------------------------------------------------------------------------------------------------

