// ----- BBMuse v0.97b ----- \\

Author  : Kaloth
Website : http://www.geocities.com/madant8
email   : mulletwarriorextreem@hotmail.com

Licence : GNU General Public License (GPL version 2 or later)

// ----- What is it? ----- \\

BBmuse displays the title and track number of the current song playing in winamp, it also monitors the playback state of winamp and changes it's display accordingly ;) BBmuse also acts as a bridge between the world of blackbox bro@ms and winamp. Various things are possible including a track progress slider with the help of BBinterface.

When winamp is not running it can be configured to display a clock or a custom message.

It's name is a tribute to my favourite band, and because BBAmptitle was already taken :P

NOTE: This is a beta release, it works on my comp but is far from
being certified stable, use at you own risk!


// ----- How do I use it? ----- \\

Simply install in the normal way, and then run winamp to see what happens. All major features are accessable from the lovely menu :D

For those of us new to blackbox here is a patronising step by step installation guide :)


1) Simply extract all the files in this zip into your blackbox plugins directory...

Eg: C:\Blackbox\Plugins\

2) Then add the following line to Plugins.rc (In your blackbox home directory).

plugins\bbmuse\bbmuse.dll

// ----- Using BBmuse with BBInterface ----- \\

Using BBmuse to add some nice extra features to BBInterface is realy easy! Things that can be done include (but are not limited to)...

 * A fully working progress slider
 * A track label
 * Mini media player frame

Basically it's up to you to experiment, the sky's the limit ;) I have providied an example of how to make a media player bbinterface frame with progress slider in the file named 'BBmusePlayer.rc' which is a bbinterface script. To make your own you will need to know about bro@ms (see the blackbox website), bbinterface and the 'WINAMP_MESSAGES.txt' file that includes a description of all the messages you can send to winamp. Also take a read through the 'Bro@ms' section of this readme for details on the specific message formats used...

Note: To get the slider working make sure you have selected 'BBInterface Extensions' in the BBmuse config menu.

// ----- Bro@ms ----- \\

@BBmuse SetCaption Hello to the World!

  The above changes the message displayed by BBmuse, in this case it would change to "Hello to the World!". You can replace this with any string you want.

@BBmuse SetCaptionNow Hello to the World!

  Same as SetCaption but forces the caption to be changed even when Winamp is running.

@BBmuse GetWinampInfo

  Triggers BBmuse to look for Winamp and display track information.


BBmuse also lets you send bro@ms to winamp :) see the WINAMP_MESSAGES.txt for values you can use with the following bro@ms...

@BBmuse_toWinamp <type> <id> <data>

Where...
    type = the type of message (WM_USER or WM_COMMAND or BBI_SPECIAL).
    id   = the message id (see WINAMP_MESSAGES.txt).
    data = the message data field (see WINAMP_MESSAGES.txt, this is only relevent for WM_USER messages).

Results from the messages you sent are returned in the following format...

@BBmuse_fromWinamp_Result <id> <data>

Where...
    id = the id from the message you sent
    data = the resulting data as an integer.

	BBmuse also sends out some usefull bro@ms to tell you what Winamp is doing, this is most usefull to devs and BBinterface peeps...

@BBmuse_fromWinamp_Paused      - Playback is paused.
@BBmuse_fromWinamp_Playing     - A track is playing.
@BBmuse_fromWinamp_Stopped     - Playback is stopped OR winamp is closed OR winamp encountered an error.
@BBmuse_fromWinamp_Opened      - Winamp has just been opened.
@BBmuse_fromWinamp_Closed      - Winamp has just been closed.
@BBmuse_fromWinamp_TitleChange - Winamp's window title has changed.

	BBinterface users will also be pleased to know that BBmuse sends out track progress information every second in the following message format...

@BBInterface Control SetControlProperty winampslider Value %d

	And the current string displayed in BBmuse as...

@BBInterface Control SetAgent winamplabel Caption StaticText %s

	What this means is you can make a BBi slider called "winampslider" and it will follow the track progress of winamp, and a label called "winamplabel" will have the track title in it ;) To get the slider to control the track position you must add the following line to you bbinterface script...

@BBInterface Control SetAgent winampslider Value Bro@m "@BBmuse_toWinamp BBI_SPECIAL 106 %d"

	Take a look at the BBinterface script "BBmusePlayer.rc" for an example of using the winampslider to achieve a track progress control.


Here is an example of a simple winamp menu for BB...

   [submenu]            (Winamp Control)
      [exec]            (Close)            {@BBmuse_toWinamp WM_COMMAND 40001}
      [exec]            (Play)             {@BBmuse_toWinamp WM_COMMAND 40045}
      [exec]            (Pause/Unpause)    {@BBmuse_toWinamp WM_COMMAND 40046}
      [exec]            (Stop)             {@BBmuse_toWinamp WM_COMMAND 40047}
      [exec]            (Next)             {@BBmuse_toWinamp WM_COMMAND 40048}
      [exec]            (Previous)         {@BBmuse_toWinamp WM_COMMAND 40044}
      [exec]            (Open File)        {@BBmuse_toWinamp WM_COMMAND 40029}
   [end]

Another cool thing you can do is use Kana's BBMediator to pass info to BBInterface objects, you can get some realy cool stuff going like frames that are only visible when winamp is open. Thanks to Kana for his awesome plugin :P


// ----- RC Settings ----- \\

An example rc file is included and should be fairly self explanitory, all major options are configurable from the menu.



// ----- Change History ----- \\

-- v0.96b

    Kaloth

        Fixed stalling when more than one winamp bro@m is sent at the same time.

-- v0.96b

    Kaloth

        Fixed a bug that crashed blackbox when winamp was playing very long tracks (eg: longer than 2 hours..)

        Fixed the track name resolve when "Scroll title in taskbar" is set in winamp (Thanks to EasyWinampControl by Yann Hamon and Malnilion).

-- v0.95b

    Kaloth

        Fixed a multi-monitor issue (negative positioning)

-- v0.94b
    Kaloth

        Changed minimum size to 5x5 pixels.

-- v0.93b
    Kaloth

        Added a multi-thread action queue for sending messages to winamp. This should hopefuly fix the problem that some people have been experiancing with the seekbar functionality as it also destroys the oldest thread if more than 10 are running (to catch ones that don't return properly) and destroys all the threads when winamp closes.

        Added the option to dissable the bbinterface extensions (fixes menu under xoblite)

-- v0.91b

    Kaloth

        Fixed a little bug in the bbinterface slider communication that caused a short tracks to restart over and over.

-- v0.9b

    Kaloth

        Fixed compatability with musikCube

-- v0.8b

    Kaloth

        Re-center track title on pause (for readability purposes).

        2 way bbinterface track progress control (slider).

-- v0.7b

    Kaloth

        Fixed error in winamp closed message invocation.

        Changed format of result messages to include the function id.

        Added WinampControlProc to stop BB hanging when winamp does not return immediately.

-- v0.61b

    Arc Angel

        Removed the SM_CXSCREEN spos lines to enable multi monitor support.

        Time display now uses strftime instead of the asctime so users could modify what to display.

    Kaloth

        Fixed bug in idle message menu.

-- v0.6b

    Added full Winamp bro@m support.

    Added ability to report track progress among other things.

    Re-aranged a few things and added custom message support for idle state.

    Added customizable font settings

    Fixed pink font outline problem

    Added customisable idle message mode

-- v0.5b

    Added clock

    Fixed caption clipping

    Hopefully fixed win98 support.

    Improved resize method, now much smoother.

-- v0.4b

    Major re-coding of Winamp change detection - Now uses WinEventHooks.

    Improved Scrolling - Pauses at ends, fixed some context errors.

    Now supports Windows 98+

    Added border toggle.

-- v0.3b

    Now only scrolls title when requested and title doesn't fit in the window.

    Improved scrolling

-- v0.2b

    Complete change to the way it grabs track information (parse m3u ...became... getWindowText()).

-- v0.1b

    Initial Code Version.