# This is install script for NSIS installer for BlackBox 4 Windows

# where to create install.exe
OutFile c:\_builds\install.exe

# where to install program
InstallDir c:\bb_i

# good for debugging
ShowInstDetails Show

!include LogicLib.nsh
!include WinVer.nsh
!include nsDialogs.nsh
!include x64.nsh
!include Sections.nsh

Var win_xp
Var win_64
Var OptStyles
Var OptPlugins

# Set the text to prompt user to enter a directory
DirText "This will install BlackBox 4 Windows program on your computer. Choose a directory"

Name "BlackBox 4 Windows"
RequestExecutionLevel admin
AddBrandingImage left 256

Page Custom brandimage "" ": Brand Image"
Page Custom windetectionPageEnter windetectionPageLeave
Page License
Page Components
Page Directory
Page Custom shellPageEnter shellPageLeave
Page InstFiles
#UninstPage uninstConfirm
#UninstPage instfiles

LicenseData "GPL.txt" 

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
Var grp1
Var grp2
 
; Dummy section visible, RO means read only, user can't
; change this. This should remain empty.
Section "Required Files"
SectionIn RO
SectionEnd

; Visible options for the user
Section "Optional plugins" SecPlugins
SectionEnd

; Visible options for the user
Section "Optional styles" SecStyles
SectionEnd

; Invisible section
Section "-ReadOptions"
  ; This is where we read the optional sections to see if
  ; they are selected, and set our variables to reflect this
  SectionGetFlags ${SecPlugins} $0
  IntOp $0 $0 & ${SF_SELECTED}
  IntCmp $0 ${SF_SELECTED} 0 +3 +3
    StrCpy $OptPlugins 1
    GoTo +2
  StrCpy $OptPlugins 0

  SectionGetFlags ${SecStyles} $0
  IntOp $0 $0 & ${SF_SELECTED}
  IntCmp $0 ${SF_SELECTED} 0 +3 +3
    StrCpy $OptStyles 1
    GoTo +2
  StrCpy $OptStyles 0
SectionEnd

################################# XP 32b ######################################
Section /o "-XP_32" Sec_XP_32

  SetOutPath $INSTDIR

  ; XP 32 bit files and stuff here
  DetailPrint "XP 32 bit Required Files"

  File vs_xp_32\bbnote.exe
  File vs_xp_32\bbnote-proxy.dll
  File vs_xp_32\bbstylemaker.exe
  File vs_xp_32\blackbox.exe
  File vs_xp_32\bsetbg.exe
  File vs_xp_32\bsetroot.exe
  File vs_xp_32\bsetshell.exe
  File vs_xp_32\deskhook.dll
  File vs_xp_32\readme.txt
  
  ; Check for plugins and styles
  StrCmp $OptPlugins 0 SkipXP32_1
  ; Plugin files go here
  DetailPrint "XP 32 bit Optional Plugins"
SkipXP32_1:

  StrCmp $OptStyles 0 SkipXP32_2
  ; Style files go here
  DetailPrint "XP 32 bit Optional Styles"
SkipXP32_2:
SectionEnd

################################# XP 64b ######################################
Section /o "-XP_64" Sec_XP_64

  SetOutPath $INSTDIR
 
  ; XP 64 bit files and stuff here
  DetailPrint "XP 64 bit Required Files"

  File vs_xp_64\bbnote.exe
  File vs_xp_64\bbnote-proxy.dll
  File vs_xp_64\bbstylemaker.exe
  File vs_xp_64\blackbox.exe
  File vs_xp_64\bsetbg.exe
  File vs_xp_64\bsetroot.exe
  File vs_xp_64\bsetshell.exe
  File vs_xp_64\deskhook.dll
  File vs_xp_64\readme.txt
  
  ; Check for plugins and styles
  StrCmp $OptPlugins 0 SkipXP64_1
  ; Plugin files go here
  DetailPrint "XP 64 bit Optional Plugins"
SkipXP64_1:

  StrCmp $OptStyles 0 SkipXP64_2
  ; Style files go here
  DetailPrint "XP 64 bit Optional Styles"
SkipXP64_2:
SectionEnd

################################ Vista 32b ####################################
Section /o "-Vista_32" Sec_Vista_32

  SetOutPath $INSTDIR

  ; Vista 32 bit files and stuff here
  DetailPrint "Vista 32 bit Required Files" ; Here for clarity

  File vs_vista_32\bbnote.exe
  File vs_vista_32\bbnote-proxy.dll
  File vs_vista_32\bbstylemaker.exe
  File vs_vista_32\blackbox.exe
  File vs_vista_32\bsetbg.exe
  File vs_vista_32\bsetroot.exe
  File vs_vista_32\bsetshell.exe
  File vs_vista_32\deskhook.dll
  File vs_vista_32\readme.txt

  ; Check for plugins and styles
  StrCmp $OptPlugins 0 SkipVista32_1
  ; Plugin files go here
  DetailPrint "Vista 32 bit Optional Plugins"
SkipVista32_1:

  StrCmp $OptStyles 0 SkipVista32_2
  ; Style files go here
  DetailPrint "Vista 32 bit Optional Styles"
SkipVista32_2:
SectionEnd

################################ Vista 64b ####################################
Section /o "-Vista_64" Sec_Vista_64

  SetOutPath $INSTDIR
 
  ; Vista 64 bit files and stuff here
  DetailPrint "Vista 64 bit Required Files" ; Here for clarity

  File vs_vista_64\bbnote.exe
  File vs_vista_64\bbnote-proxy.dll
  File vs_vista_64\bbstylemaker.exe
  File vs_vista_64\blackbox.exe
  File vs_vista_64\bsetbg.exe
  File vs_vista_64\bsetroot.exe
  File vs_vista_64\bsetshell.exe
  File vs_vista_64\deskhook.dll
  File vs_vista_64\readme.txt
 
  ; Check for plugins and styles
  StrCmp $OptPlugins 0 SkipVista64_1
  ; Plugin files go here
  DetailPrint "Vista 64 bit Optional Plugins"
SkipVista64_1:

  StrCmp $OptStyles 0 SkipVista64_2
  ; Style files go here
  DetailPrint "Vista 64 bit Optional Styles"
SkipVista64_2:
SectionEnd


Function windetectionPageEnter
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
FunctionEnd

Var radio_xp
Var radio_64

Function windetectionPageLeave
  ${NSD_GetState} $Group1RadioXP $radio_xp
  ${NSD_GetState} $Group2Radio64 $radio_64

  ${If} $radio_xp == ${BST_CHECKED}
    ${If} $radio_64 == ${BST_CHECKED}
        SectionSetFlags ${Sec_XP_64} ${SF_SELECTED}
    ${Else}
        SectionSetFlags ${Sec_XP_32} ${SF_SELECTED}
    ${EndIf}
  ${Else}
    ${If} $radio_64 == ${BST_CHECKED}
        SectionSetFlags ${Sec_Vista_64} ${SF_SELECTED}
    ${Else}
        SectionSetFlags ${Sec_Vista_32} ${SF_SELECTED}
    ${EndIf}
  ${EndIf}
# MessageBox MB_OK "You typed:$\n$\n$0"
FunctionEnd

Function .onInit
FunctionEnd


#Section "BlackBox"
#  SetOutPath $INSTDIR
#	File "bbnote.exe"
#	File "bbnote-proxy.dll"
#	File "bbstylemaker.exe"
#	File "blackbox.exe"
#	File "bsetbg.exe"
#	File "bsetroot.exe"
#	File "bsetshell.exe"
#	File "deskhook.dll"
#	File "readme.txt"
#  #createShortCut "$SMPROGRAMS\BlackBox.lnk" ""
#SectionEnd

#Section /o "BlackBox Styles"
#  SetOutPath $INSTDIR\backgrounds
#  File /r "backgrounds\"
#  SetOutPath $INSTDIR\styles
#  File /r "styles\"
#SectionEnd
#
#Section "BlackBox Configs"
#  SetOutPath $INSTDIR
#  CreateDirectory $INSTDIR
#	File "blackbox.rc"
#	File "bsetroot.rc"
#	File "extensions.rc"
#	File "menu.rc"
#	File "plugins.rc"
#	File "shellfolders.rc"
#	File "stickywindows.ini"
#SectionEnd
#
#Section "BlackBox Essential Plugins"
#  SetOutPath $INSTDIR\plugins\bbAnalog
#	File /r "plugins\bbAnalog\"
#  SetOutPath $INSTDIR\plugins\bbColor3dc
#	File /r "plugins\bbColor3dc\"
#  SetOutPath $INSTDIR\plugins\bbIconBox
#	File /r "plugins\bbIconBox\"
#  SetOutPath $INSTDIR\plugins\bbInterface
#	File /r "plugins\bbInterface\"
#  SetOutPath $INSTDIR\plugins\bbKeys
#	File /r "plugins\bbKeys\"
#  SetOutPath $INSTDIR\plugins\bbLeanBar
#	File /r "plugins\bbLeanBar\"
#  SetOutPath $INSTDIR\plugins\bbLeanSkin
#	File /r "plugins\bbLeanSkin\"
#  SetOutPath $INSTDIR\plugins\bbSlit
#	File /r "plugins\bbSlit\"
#SectionEnd
#
#Section /o "BlackBox Extended Plugin Set I."
#  SetOutPath $INSTDIR\plugins\BBAnalogEx
#	File /r "plugins\BBAnalogEx\"
#  SetOutPath $INSTDIR\plugins\bbCalendar
#	File /r "plugins\bbCalendar\"
#  SetOutPath $INSTDIR\plugins\BBDigitalEx
#	File /r "plugins\BBDigitalEx\"
#  SetOutPath $INSTDIR\plugins\bbFoomp
#	File /r "plugins\bbFoomp\"
#  SetOutPath $INSTDIR\plugins\bbLeanBar+
#	File /r "plugins\bbLeanBar+\"
#  SetOutPath $INSTDIR\plugins\BBMagnify
#	File /r "plugins\BBMagnify\"
#  SetOutPath $INSTDIR\plugins\BBPager
#	File /r "plugins\BBPager\"
#  SetOutPath $INSTDIR\plugins\bbRecycleBin
#	File /r "plugins\bbRecycleBin\"
#  SetOutPath $INSTDIR\plugins\BBRSS
#	File /r "plugins\BBRSS\"
#  SetOutPath $INSTDIR\plugins\BBStyle
#	File /r "plugins\BBStyle\"
#  SetOutPath $INSTDIR\plugins\BBSysMeter
#	File /r "plugins\BBSysMeter\"
#  SetOutPath $INSTDIR\plugins\bbWorkspaceWheel
#	File /r "plugins\bbWorkspaceWheel\"
#  SetOutPath $INSTDIR\plugins\SystemBarEx
#	File /r "plugins\SystemBarEx\"
#SectionEnd
#
#Section /o "BlackBox Extended Plugin Set II."
#  SetOutPath $INSTDIR\plugins\BB8Ball
#	File /r "plugins\BB8Ball\"
#  SetOutPath $INSTDIR\plugins\bbInterface_iTunes
#	File /r "plugins\bbInterface_iTunes\"
#  SetOutPath $INSTDIR\plugins\BBMessageBox
#	File /r "plugins\BBMessageBox\"
#  SetOutPath $INSTDIR\plugins\BBXO
#	File /r "plugins\BBXO\"
#SectionEnd
#
#
#
#
