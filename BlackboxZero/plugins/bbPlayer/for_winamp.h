/* Misc definitions / declarations for Winamp */

#ifndef __WINAMP_H__
#define __WINAMP_H__

#ifdef __cplusplus
extern "C" {
#endif




/* result = SendMessage(winamp, WM_WA_IPC, parameter, IPC_XXX); */
#define WM_WA_IPC WM_USER




#define IPC_STARTPLAY 102
#define IPC_ISPLAYING 104 /* 0:not playing,  1:playing,  3:paused */

/**
 * wp == 0:  playing time in milliseconds of the cuurent track
 * wp == 1:  the current track length in seconds
 *
 * -1 for not playing or error
 */
#define IPC_GETOUTPUTTIME 105

/* wp == jump_dest_time_in_milliseconds (0:success,  1:EOF,  2:not playing) */
#define IPC_JUMPTOTIME 106

#define IPC_SETPLAYLISTPOS 121 /* wp == new_playlist_position */
#define IPC_SETVOLUME 122 /* wp == volume_from_0_to_255 */
#define IPC_GETLISTLENGTH 124 /* return the length of the current playlist */
#define IPC_GETLISTPOS 125 /* return the playlist position */
#define IPC_GET_SHUFFLE 250 /* return the shuffle option status */
#define IPC_GET_REPEAT 251 /* return the repeat option status */
#define IPC_SET_SHUFFLE 252 /* wp == shuffle_option_status */
#define IPC_SET_REPEAT 253 /* wp == repeat_option_status */





/* SendMessage(winamp, WM_COMMAND, command_name, 0); */

#define WINAMP_OPTIONS_EQ 40036 /* toggles the EQ window */
#define WINAMP_OPTIONS_PLEDIT 40040 /* toggles the playlist window */
#define WINAMP_VOLUMEUP 40058 /* turns the volume up a little */
#define WINAMP_VOLUMEDOWN 40059 /* turns the volume down a little */
#define WINAMP_FFWD5S 40060 /* fast forwards 5 seconds */
#define WINAMP_REW5S 40061 /* rewinds 5 seconds */

#define WINAMP_MAINMENU 40043

#define WINAMP_REWIND 40044
#define WINAMP_PLAY 40045
#define WINAMP_PAUSE 40046
#define WINAMP_STOP 40047
#define WINAMP_NEXTSONG 40048
#define WINAMP_PREVSONG 40198

#define WINAMP_FILE_PLAY 40029 /* pops up the load file(s) box */
#define WINAMP_FILE_DIR 40187 /* pops up the load directory box */
#define WINAMP_OPTIONS_PREFS 40012 /* pops up the preferences */
#define WINAMP_OPTIONS_AOT 40019 /* toggles always on top */
#define WINAMP_HELP_ABOUT 40041 /* pops up the about box  */

/**
 * - IDs 42000 to 45000 are reserved for gen_ff
 * - IDs from 45000 to 57000 are reserved for library
 */




#ifdef __cplusplus
}
#endif

#endif
