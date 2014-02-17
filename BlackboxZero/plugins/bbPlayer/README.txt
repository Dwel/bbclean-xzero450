================================================================
BBPlayer - Your favorite media player interface as a BB4W plugin
================================================================




Abstract
========

BBPlayer is a plugin for Blackbox for Windows,
which displays the current track played in the player
and provides some bro@ms to control it.




Installation
============

Same as other plugins.

(1) Add the path to BBPlayer.dll in your plugins.rc.

(2) Restart the shell.

If you are using xoblite or bbLean, you can add a plugin via config menu.




Mouse operations
================

Right click
	Show menu.

Ctrl + Left click
	Move BBPlayer.




Settings
========

BBPlayer saves its settings in BBPlayer.rc.
Most of them are self-explanatory, so I won't explain them.


bbplayer.interval
	Interval time to update the information about the current track
	in milliseconds.

bbplayer.style
	Name of the style to use.  Available styles are:

	toolbar, toolbar.button, toolbar.button.pressed,
	toolbar.label, toolbar.windowlabel, toolbar.clock,
	menu.title, menu.frame, menu.hilite.

bbplayer.not_found_text
	The text which is displayed when the player not found.

bbplayer.dtoption
	Text formatting style.

	You must separete each style name by semicolon(;)
	and add extra semicolon at the beginning and the end.

	For example, the following value means
	`left', `noprefix', `singleline' and `vcenter' style.

		;left;noprefix;singleline;vcenter;


	See the following URI about formatting style:

        	http://msdn.microsoft.com/library/en-us/gdi/fontext_0odw.asp

bbplayer.player_type
	Name of the player which you are using.

bbplayer.auto_hide
	Enable or disable the auto hide feature.
	If this feature is enabled,
	BBPlayer will hide automatically when the player is not found,
	and will show automatically when the player is found again.

bbplayer.anti_auto_hide_time
	Time to show auto-hidden BBPlayer in milliseconds.

	If this value is set positive value,
	auto-hidden BBPlayer can be shown by toggling plugins or etc
	until the specified time is elapsed.

	Because BBPlayer cannot be shown by toggling plugins or etc
	if the auto hide feature is enabled.

bbplayer.on_title_change
	The bro@m to be sent
	on changing information of the current playing track.
	BBPlayer will not send if it is null or doesn't start with `@'.

	If the specified bro@m contains `<DEFAULT>',
	it will be replaced as information of the current track.

	For example, you may create a BBInterface label
	which displays as well as BBPlayer:

	@BBInterface Control SetAgent BBPlayer Caption StaticText "<DEFAULT>"




Plugins
=======

BBPlayer uses a plugin to retrieve some information and control the player.
So you have to modify the value of `bbplayer.player_type'
if you want to use another player.

For example, if you want to use a plugin named `for_lilith.dll',
you have to set `lilith' as the value of the above key like the following:

	bbplayer.player_type: lilith

Available plugins are:

	for_lilith.dll		for Lilith
	for_winamp.dll          for Winamp
	for_foobar2000.dll      for foobar2000




Bro@ms
======

Configurations
--------------

@BBShowPlugins
	Show BBPlayer if toggle_with_plugins enabled.

@BBHidePlugins
	Hide BBPlayer if toggle_with_plugins enabled.

@BBPlayer.conf.OPTION VALUE
	Set VALUE as the value of OPTION.
	If OPTION takes boolean value (true or false),
	you can specify `toggle' to toggle OPTION.

	For example:

	@BBPlayer.conf.width 1280
		Set `bbplayer.width' as 1280.

	@BBPlayer.conf.always_on_top toggle
		Toggle `bbplayer.always_on_top'.
		If the feature is enabled, then it will be disabled,
		otherwise it will be enabled.


Controlling Player
------------------

@BBPlayer.ctrl.play
	Start playing.
	If the player is already playing, then do like @BBPlayer.ctrl.pause.

@BBPlayer.ctrl.play N
	Play Nth track.  N must be a positive number.

@BBPlayer.ctrl.pause
	Pause playing.
	If the player is already pausing, then play again.

@BBPlayer.ctrl.stop
	Stop playing.

@BBPlayer.ctrl.next
@BBPlayer.ctrl.prev
	Switch to next/previous track.

The following bro@ms are availble when a proper plugin is loaded.

@BBPlaeyr.for_lilith.dde COMMAND
	Send COMMAND to Lilith via DDEClient.exe.
	See `DDE.txt' (which is included in a Lilith's archive)
	for the details of COMMAND.

@BBPlaeyr.for_winamp.WM_COMMAND N
	Control Winamp.  `N' is an integer.
	For example, `@BBPlayer.for_winamp.WM_COMMAND 40036' will
	toggle the equalizer window.
	See `for_winamp.h' for the details.

@BBPlayer.for_foobar2000.exe ARGS
	Send ARGS to foobar2000.
	For example, `@BBPlayer.for_foobar2000.exe /command:Foobar2000/Close'
	will close foobar2000.




ChangeLog
=========

0.0.4	2005-01-19
	- for_foobar2000: Fixed to work for foobar2000 with Columns UI.

	  Set one of the following values
	  to `bbplayer.for_foobar2000.class' in BBPlayer.rc:

	  Default User Interface
	      {DA7CD0DE-1602-45e6-89A1-C2CA151E008E}

	  Columns UI
	      {E7076D1C-A7BF-4f39-B771-BCBE88F2A2A8}

	  Note that BBPlayer won't work correctly
	  when foobar2000 is minimized and it uses Default User Interface.

0.0.3	2005-01-11
	- Added support for foobar2000 <http://www.foobar2000.org/>.

	- for_winamp: Added bro@m `@BBPlayer.for_winamp.WM_COMMAND N'
	  to control Winamp.

	- for_lilith: Modified to read its settings from `BBPlayer.rc'
	  instead of `for_lilith.rc'.

0.0.2	2004-10-17
	- Added the option `on_title_change' to send a specified bro@m
	  on changing information of the current playing track.

0.0.1	2004-07-23
	- Fixed a bug that BBPlayer appears
	  in applications tab of task manager.

0.0.0	2004-07-17
	- Added new plugin `for_winamp.dll' to support Winamp.

	- Fixed unexpected behavior of auto hide feature.

	- Added slit support.
	  `@BBPlayer.conf.in_slit toggle' will add BBPlayer into the slit
	  or remove from the slit.


0.0.0c	2004-07-14
	- Added anti_auto_hide_time option.

	- Added bro@ms to control the player.


0.0.0b	2004-07-12
	- Added the following options:

	  auto_hide
	  horizontal_padding
	  not_found_text
	  player_type
	  snap_to_edge
	  toggle_with_plugins
	  vertical_padding
	  visibility

	- Added bro@ms to configure BBPlayer.

	- Added menu.

	- Modified to use a plugin to split player-dependent part.


0.0.0a	2004-07-11
	Initial version.




License
=======

BBPlayer is released under the terms of GPL.
See LICENSE for more information.




Misc.
=====

For latest informatoin, see <http://nicht.s8.xrea.com/2004/07/11/BBPlayer>.

Copyright (C) 2004 kana <nicht AT s8 DOT xrea DOT con>




__END__
