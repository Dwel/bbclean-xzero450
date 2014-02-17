#ifndef _BBPlayerAPI_H
#define _BBPlayerAPI_H

/**
 * This is the header to develop BBPlayer plugins.
 * To create a BBPlayer plugin, you must implement
 * the functions which are declared in the following.
 */

#ifdef __cplusplus
extern "C" {
#endif

#ifndef DLL_EXPORT
#define DLL_EXPORT __declspec(dllexport)
#endif


#define BBPlayer_SUCCESS 0
#define BBPlayer_FAILURE 1




/**
 * Initialize a plugin.
 * Return BBPlayer_SUCCESS for success or BBPlayer_FAILURE for failure.
 *
 * This function is called when BBPlayer loads a plugin.
 */
DLL_EXPORT int BBPlayer_PluginBegin( HINSTANCE bbplayer_instance,
                                     HINSTANCE bbplayer_plugin_instance );

typedef int (*BBPlayer_PluginBegin_prot)(HINSTANCE, HINSTANCE);

/**
 * Finalize a plugin.
 * This function is called when BBPlayer unloads a plugin.
 */
DLL_EXPORT void BBPlayer_PluginEnd( HINSTANCE bbplayer_instance,
                                    HINSTANCE bbplayer_plugin_instance );

typedef void (*BBPlayer_PluginEnd_prot)(HINSTANCE, HINSTANCE);


/**
 * Return the information which is specified by `field'.
 * The meaning of `field' is same as `pluginInfo'.
 */
DLL_EXPORT const char* BBPlayer_PluginInfo(int field);

typedef const char* (*BBPlayer_PluginInfo_prot)(int);




/**
 * Retrieve the specified information and store it into `buf',
 * and return BBPlayer_SUCCESS.
 *
 * `size' is the size of `buf' in sizeof(char).
 * `size' must be positive.
 *
 * If the length of the information exceeds `size',
 * the function succeeds and the string is truncated to `size'
 * characters and be NUL('\0') terminated.
 *
 * If the function fail to retrieve the information,
 * `buf' is never changed and return BBPlayer_FAILURE.
 *
 * `info_type' is used to specify what kind of information to retrieve.
 * BBPlayer_TITLE means to retrieve the title of the specified music,
 * and so on.
 * But BBPlayer_DEFAULT means to retrieve some informations and
 * store in appropriate format.
 *
 * `pos' and `pos_type' are used to specify which music's information
 * to retrieve by the playlist number.
 * `pos_type' can be one of the following value:
 *
 *     BBPlayer_CURRENT   The music which is played now.
 *                        `pos' is ignored.
 *
 *     BBPlayer_ABSOLUTE  The `pos'th music in the playlist.
 *                        `pos' must be positive.
 *
 *     BBPlayer_RELATIVE  The <current no. + `pos'>th music in the playlist.
 *                        `pos' can be any value.
 *
 * If the calculated number is overflow, this function fail.
 * Counting playlist number starts with 1.
 *
 *
 * For example, calling
 *
 *     BBPlayer_GetMusicInfo(BBPlayer_CURRENT, 0, BBPlayer_DEFAULT, buf, size);
 *
 * will retrieve the information about the currently played music
 * and store it in appropriate format.
 *
 * BUGS: BBPlayer calls this function with the following parameters:
 *
 *           pos_type       BBPlayer_CURRENT
 *           pos            0
 *           info_type      BBPlayer_DEFAULT
 *
 *       So you don't have to implement this function for other cases
 *       at this moment (0.0.0c).
 */

typedef enum {
	BBPlayer_CURRENT,
	BBPlayer_ABSOLUTE,
	BBPlayer_RELATIVE
} _BBPlayer_PosType;


typedef enum {
	BBPlayer_DEFAULT,
	BBPlayer_TITLE,
	BBPlayer_ARTIST,
	BBPlayer_ALBUM,
	BBPlayer_DATE,
	BBPlayer_TRACK,
	BBPlayer_GENRE,
	BBPlayer_COMMENT
} _BBPlayer_MusicInfoType;


DLL_EXPORT int
BBPlayer_GetMusicInfo(
	_BBPlayer_PosType pos_type,
	int pos,
	_BBPlayer_MusicInfoType info_type,
	char* buf,
	int size
);

typedef int (*BBPlayer_GetMusicInfo_prot)
                (_BBPlayer_PosType, int, _BBPlayer_MusicInfoType, char*, int);




/**
 * Control player.
 * Return BBPlayer_SUCCESS for success or BBPlayer_FAILURE for failure.
 *
 * `type' can be one of the following value:
 *
 *     BBPlayer_PLAY        Play the current track.
 *                          If `n' is positive, play the `n'th track.
 *                          If `n' is zero or negative
 *                          and player is already playing,
 *                          this command works as BBPlayer_PAUSE.
 *     BBPlayer_STOP        Stop playing.
 *     BBPlayer_PAUSE       Pause playing,
 *                          or resume playing if player is already pausing.
 *     BBPlayer_NEXT        Go to next track.
 *     BBPlayer_PREV        Go to previous track.
 */

typedef enum {
	BBPlayer_PLAY,
	BBPlayer_STOP,
	BBPlayer_PAUSE,
	BBPlayer_NEXT,
	BBPlayer_PREV
} _BBPlayer_ControlType;


DLL_EXPORT int BBPlayer_Control(_BBPlayer_ControlType type, int n);

typedef int (*BBPlayer_Control_prot)(_BBPlayer_ControlType, int);




/**
 * Interpret a given `broam'.
 * This function is called when BBPlayer receives a bro@m.
 */

DLL_EXPORT void BBPlayer_InterpretBroam(const char* broam);

typedef void (*BBPlayer_InterpretBroam_prot)(const char*);




#ifdef __cplusplus
}
#endif

#endif
