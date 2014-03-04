BBSeekbar is a plugin for Blackbox for Windows.
winamp.dll and foobar2000.dll are BBSeekbar components.

Supported Players:
Winamp
foobar2000 v0.9

Installation:
foo_seek_remote.dll is a foobar2000 component, and is needed for remoting foobar2000.
Copy this file to foobar2000 components' directory(usually C:\Program Files\foobar2000\components\).

Based on code by:
BBInterface v0.9.6 (Copyright 2005 psyci & grischka)
BBmuse v0.7b (Copyright 2004 Kaloth & Arc Angel)
foo_remote (Copyright 2003 steel_space)


To display strings in BBInterface:
¥elapsed time & total time
Set a BBinterface's control named "ElapsedAndTotal".
(BBSeekbar sends the following Bro@m.
"@BBInterface Control SetAgent CurrentAndTotal Caption StaticText %s")
¥elapsed time
Set a BBinterface's control named "ElapsedTime".

PlayerTime.rc is the BBinterface script example.
If you don't understand, take a look at it.


Revision History:
0.0.5b	2006-08-18
 - updated to 0.9 SDK.(foo_seek_remote.dll)

0.0.4b	2005-12-15
 - fixed GDI resource leak on Win9x.
 - fixed the Sticky function.

0.0.3b	2005-10-28
 - fixed a problem with noise when seeking.
 - removed unnecessary code.
 - added display of player's time in BBInterface.

0.0.2b	2005-10-23
 - fixed a bug that caused the plugin to disappear after changing screen resolution.

0.0.1b	2005-10-12
 - initial release
