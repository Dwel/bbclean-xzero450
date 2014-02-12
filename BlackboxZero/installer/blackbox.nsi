# This is install script for NSIS installer for BlackBox 4 Windows

# where to create install.exe
OutFile c:\_install\install.exe

# where to install program
InstallDir c:\bb_i

!include LogicLib.nsh
!include WinVer.nsh
!include nsDialogs.nsh
!include x64.nsh

Var win_xp
Var win_64

Function .onInit
  ${If} ${AtLeastWinVista}
    ${If} ${RunningX64}
        StrCpy $win_xp 0
        StrCpy $win_64 1
    ${Else}
        StrCpy $win_xp 0
        StrCpy $win_64 0
    ${EndIf}
  ${Else}
    ${If} ${RunningX64}
        StrCpy $win_xp 1
        StrCpy $win_64 1
    ${Else}
        StrCpy $win_xp 1
        StrCpy $win_64 0
    ${EndIf}
  ${EndIf} 
FunctionEnd

# Set the text to prompt user to enter a directory
DirText "This will install BlackBox 4 Windows program on your computer. Choose a directory"

Name "BlackBox 4 Windows"
RequestExecutionLevel admin
AddBrandingImage left 256

Page custom brandimage "" ": Brand Image"
Page custom windetection
Page license
Page components
Page directory
Page instfiles
#UninstPage uninstConfirm
#UninstPage instfiles

#Section "Installer Section"
#SectionEnd
#Section "un.Uninstaller Section"
#SectionEnd

Function brandimage
  SetOutPath "$TEMP"
  SetFileAttributes installer.bmp temporary
  File installer.bmp
  SetBrandingImage "$TEMP\installer.bmp" /resizetofit
FunctionEnd

var dialog
var hwnd
var Group1RadioXP
var Group1RadioVista
var Group2Radio32
var Group2Radio64
var usr_win_xp
var usr_64_bits
Var grp1
Var grp2
 
Function windetection
  nsDialogs::Create 1018
    Pop $dialog

  ${NSD_CreateGroupBox} 2% 2% 48% 98% "Windows"
  Pop $grp1

  ${NSD_CreateRadioButton} 5% 33% 40% 6% "XP"
    Pop $Group1RadioXP
    ${NSD_AddStyle} $Group1RadioXP ${WS_GROUP}
    ${NSD_OnClick} $Group1RadioXP RadioClick
  ${NSD_CreateRadioButton} 5% 66% 40% 6% "Vista, Win7 or Win8"
    Pop $Group1RadioVista
    ${NSD_OnClick} $Group1RadioVista RadioClick

  ${NSD_CreateGroupBox} 52% 2% 46% 98% "bits"
  Pop $grp2
 
  ${NSD_CreateRadioButton} 55% 33% 40% 6% "32"
    Pop $Group2Radio32
    ${NSD_AddStyle} $Group2Radio32 ${WS_GROUP}
    ${NSD_OnClick} $Group2Radio32 RadioClick
  ${NSD_CreateRadioButton} 55% 66% 40% 6% "64"
    Pop $Group2Radio64
    ${NSD_OnClick} $Group2Radio64 RadioClick

  ${If} $win_xp == 1
    ${NSD_SetState} $Group1RadioXP ${BST_CHECKED}
  ${Else}
    ${NSD_SetState} $Group1RadioVista ${BST_CHECKED}
  ${EndIf}

  ${If} $win_64 == 1
    ${NSD_SetState} $Group2Radio64 ${BST_CHECKED}
  ${Else}
    ${NSD_SetState} $Group2Radio32 ${BST_CHECKED}
  ${EndIf}

  nsDialogs::Show
FunctionEnd
 
Function RadioClick
  Pop $hwnd
  ${If} $hwnd == $Group1RadioXP
      StrCpy $usr_win_xp 1
  ${ElseIf} $hwnd == $Group1RadioVista
      StrCpy $usr_win_xp 0
  ${ElseIf} $hwnd == $Group2Radio32
      StrCpy $usr_64_bits 0
  ${ElseIf} $hwnd == $Group2Radio64
      StrCpy $usr_64_bits 1
  ${EndIf}
FunctionEnd

Section "BlackBox"
  SetOutPath $INSTDIR
  CreateDirectory $INSTDIR
	File "bbnote.exe"
	File "bbnote-proxy.dll"
	File "bbstylemaker.exe"
	File "blackbox.exe"
	File "bsetbg.exe"
	File "bsetroot.exe"
	File "bsetshell.exe"
	File "deskhook.dll"
	File "readme.txt"
  #createShortCut "$SMPROGRAMS\BlackBox.lnk" ""
SectionEnd

Section /o "BlackBox Styles"
  SetOutPath $INSTDIR\backgrounds
  File /r "backgrounds\"
  SetOutPath $INSTDIR\styles
  File /r "styles\"
SectionEnd

Section "BlackBox Configs"
  SetOutPath $INSTDIR
  CreateDirectory $INSTDIR
	File "blackbox.rc"
	File "bsetroot.rc"
	File "extensions.rc"
	File "menu.rc"
	File "plugins.rc"
	File "shellfolders.rc"
	File "stickywindows.ini"
SectionEnd

Section "BlackBox Essential Plugins"
  SetOutPath $INSTDIR\plugins\bbAnalog
	File /r "plugins\bbAnalog\"
  SetOutPath $INSTDIR\plugins\bbColor3dc
	File /r "plugins\bbColor3dc\"
  SetOutPath $INSTDIR\plugins\bbIconBox
	File /r "plugins\bbIconBox\"
  SetOutPath $INSTDIR\plugins\bbInterface
	File /r "plugins\bbInterface\"
  SetOutPath $INSTDIR\plugins\bbKeys
	File /r "plugins\bbKeys\"
  SetOutPath $INSTDIR\plugins\bbLeanBar
	File /r "plugins\bbLeanBar\"
  SetOutPath $INSTDIR\plugins\bbLeanSkin
	File /r "plugins\bbLeanSkin\"
  SetOutPath $INSTDIR\plugins\bbSlit
	File /r "plugins\bbSlit\"
SectionEnd

Section /o "BlackBox Extended Plugin Set I."
  SetOutPath $INSTDIR\plugins\BBAnalogEx
	File /r "plugins\BBAnalogEx\"
  SetOutPath $INSTDIR\plugins\bbCalendar
	File /r "plugins\bbCalendar\"
  SetOutPath $INSTDIR\plugins\BBDigitalEx
	File /r "plugins\BBDigitalEx\"
  SetOutPath $INSTDIR\plugins\bbFoomp
	File /r "plugins\bbFoomp\"
  SetOutPath $INSTDIR\plugins\bbLeanBar+
	File /r "plugins\bbLeanBar+\"
  SetOutPath $INSTDIR\plugins\BBMagnify
	File /r "plugins\BBMagnify\"
  SetOutPath $INSTDIR\plugins\BBPager
	File /r "plugins\BBPager\"
  SetOutPath $INSTDIR\plugins\bbRecycleBin
	File /r "plugins\bbRecycleBin\"
  SetOutPath $INSTDIR\plugins\BBRSS
	File /r "plugins\BBRSS\"
  SetOutPath $INSTDIR\plugins\BBStyle
	File /r "plugins\BBStyle\"
  SetOutPath $INSTDIR\plugins\BBSysMeter
	File /r "plugins\BBSysMeter\"
  SetOutPath $INSTDIR\plugins\bbWorkspaceWheel
	File /r "plugins\bbWorkspaceWheel\"
  SetOutPath $INSTDIR\plugins\SystemBarEx
	File /r "plugins\SystemBarEx\"
SectionEnd

Section /o "BlackBox Extended Plugin Set II."
  SetOutPath $INSTDIR\plugins\BB8Ball
	File /r "plugins\BB8Ball\"
  SetOutPath $INSTDIR\plugins\bbInterface_iTunes
	File /r "plugins\bbInterface_iTunes\"
  SetOutPath $INSTDIR\plugins\BBMessageBox
	File /r "plugins\BBMessageBox\"
  SetOutPath $INSTDIR\plugins\BBXO
	File /r "plugins\BBXO\"
SectionEnd




