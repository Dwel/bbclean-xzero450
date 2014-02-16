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

Var OptConfigs
Var OptStyles
Var OptCorePlugins
Var OptPlugins
Var OptPlugins2

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
Page InstFiles
Page Custom shellPageEnter shellPageLeave
#UninstPage uninstConfirm
#UninstPage instfiles

LicenseData "GPL.txt" 

Function brandimage
  SetOutPath "$TEMP"
  SetFileAttributes installer.bmp temporary
  File installer.bmp
  SetBrandingImage "$TEMP\installer.bmp" /resizetofit
FunctionEnd

# build selection variables
Var win_xp
Var win_64
var BuildVerDialog
var Group1BuildVerRadioXP
var Group1BuildVerRadioVista
var Group1BuildVer2Radio32
var Group1BuildVer2Radio64
Var GroupBox1
Var GroupBox2
# 'as shell' variables
var as_shell
var ShellDialog
Var GroupBox3
var RadioButtonAsShell
var RadioButtonNoShell
 
# Dummy section visible, RO means read only, user can't
# change this. This should remain empty.
Section "Required Files"
  SectionIn RO
SectionEnd

# Visible options for the user
Section "Configurations" SecConfigs
SectionEnd
Section "Styles" SecStyles
SectionEnd
Section "Core plugins" SecCorePlugins
SectionEnd
Section "Optional plugins" SecPlugins
SectionEnd

# Invisible section
Section "-ReadOptions"
  # This is where we read the optional sections to see if
  # they are selected, and set our variables to reflect this

  SectionGetFlags ${SecConfigs} $0
  IntOp $0 $0 & ${SF_SELECTED}
  IntCmp $0 ${SF_SELECTED} 0 +3 +3
    StrCpy $OptConfigs 1
    GoTo +2
  StrCpy $OptConfigs 0

  SectionGetFlags ${SecStyles} $0
  IntOp $0 $0 & ${SF_SELECTED}
  IntCmp $0 ${SF_SELECTED} 0 +3 +3
    StrCpy $OptStyles 1
    GoTo +2
  StrCpy $OptStyles 0

  SectionGetFlags ${SecCorePlugins} $0
  IntOp $0 $0 & ${SF_SELECTED}
  IntCmp $0 ${SF_SELECTED} 0 +3 +3
    StrCpy $OptCorePlugins 1
    GoTo +2
  StrCpy $OptCorePlugins 0

  SectionGetFlags ${SecPlugins} $0
  IntOp $0 $0 & ${SF_SELECTED}
  IntCmp $0 ${SF_SELECTED} 0 +3 +3
    StrCpy $OptPlugins 1
    GoTo +2
  StrCpy $OptPlugins 0

  SectionGetFlags ${SecPlugins2} $0
  IntOp $0 $0 & ${SF_SELECTED}
  IntCmp $0 ${SF_SELECTED} 0 +3 +3
    StrCpy $OptPlugins2 1
    GoTo +2
  StrCpy $OptPlugins2 0
SectionEnd

################################# XP 32b ######################################
Section /o "-XP_32" Sec_XP_32

  SetOutPath $INSTDIR

  # XP 32 bit files and stuff here
  DetailPrint "XP 32 bit Required Files"

  File "vs_xp_32\bbnote.exe"
  File "vs_xp_32\bbnote-proxy.dll"
  File "vs_xp_32\bbstylemaker.exe"
  File "vs_xp_32\blackbox.exe"
  File "vs_xp_32\bsetbg.exe"
  File "vs_xp_32\bsetroot.exe"
  File "vs_xp_32\bsetshell.exe"
  File "vs_xp_32\deskhook.dll"
  File "vs_xp_32\readme.txt"
  
  # Check for plugins and styles
  StrCmp $OptPlugins 0 SkipXP32_1
  # Plugin files go here
  DetailPrint "XP 32 bit Optional Plugins"
SkipXP32_1:

  StrCmp $OptStyles 0 SkipXP32_2
  # Style files go here
  DetailPrint "XP 32 bit Optional Styles"
SkipXP32_2:
SectionEnd

################################# XP 64b ######################################
Section /o "-XP_64" Sec_XP_64

  SetOutPath $INSTDIR
 
  # XP 64 bit files and stuff here
  DetailPrint "XP 64 bit Required Files"

  File "vs_xp_64\bbnote.exe"
  File "vs_xp_64\bbnote-proxy.dll"
  File "vs_xp_64\bbstylemaker.exe"
  File "vs_xp_64\blackbox.exe"
  File "vs_xp_64\bsetbg.exe"
  File "vs_xp_64\bsetroot.exe"
  File "vs_xp_64\bsetshell.exe"
  File "vs_xp_64\deskhook.dll"
  File "vs_xp_64\readme.txt"
  
  # Check for plugins and styles
  StrCmp $OptPlugins 0 SkipXP64_1
  # Plugin files go here
  DetailPrint "XP 64 bit Optional Plugins"
SkipXP64_1:

  StrCmp $OptStyles 0 SkipXP64_2
  # Style files go here
  DetailPrint "XP 64 bit Optional Styles"
SkipXP64_2:
SectionEnd

################################ Vista 32b ####################################
Section /o "-Vista_32" Sec_Vista_32

  DetailPrint "Vista 32 bit Required Files"

  SetOutPath $INSTDIR
  File "vs_vista_32\bbnote.exe"
  File "vs_vista_32\bbnote-proxy.dll"
  File "vs_vista_32\bbstylemaker.exe"
  File "vs_vista_32\blackbox.exe"
  File "vs_vista_32\bsetbg.exe"
  File "vs_vista_32\bsetroot.exe"
  File "vs_vista_32\bsetshell.exe"
  File "vs_vista_32\deskhook.dll"
  File "vs_vista_32\readme.txt"
 
  StrCmp $OptConfigs 0 SkipVista32_1
  DetailPrint "Vista 32 bit Optional Configs"

  SetOutPath $INSTDIR
  CreateDirectory $INSTDIR
  File "vs_vista_32\blackbox.rc"
  File "vs_vista_32\bsetroot.rc"
  File "vs_vista_32\extensions.rc"
  File "vs_vista_32\menu.rc"
  File "vs_vista_32\plugins.rc"
  File "vs_vista_32\shellfolders.rc"
  File "vs_vista_32\stickywindows.ini"

SkipVista32_1:

  StrCmp $OptStyles 0 SkipVista32_2
  DetailPrint "Vista 32 bit Optional Styles"
 
  SetOutPath $INSTDIR\backgrounds
  File /r "vs_vista_32\backgrounds\"
  SetOutPath $INSTDIR\styles
  File /r "vs_vista_32\styles\"

SkipVista32_2:

  StrCmp $OptCorePlugins 0 SkipVista32_3
  DetailPrint "Vista 32 bit Optional Core Plugins"

  SetOutPath $INSTDIR\plugins\bbAnalog
	File /r "vs_vista_32\plugins\bbAnalog\"
  SetOutPath $INSTDIR\plugins\bbColor3dc
	File /r "vs_vista_32\plugins\bbColor3dc\"
  SetOutPath $INSTDIR\plugins\bbIconBox
	File /r "vs_vista_32\plugins\bbIconBox\"
  SetOutPath $INSTDIR\plugins\bbInterface
	File /r "vs_vista_32\plugins\bbInterface\"
  SetOutPath $INSTDIR\plugins\bbKeys
	File /r "vs_vista_32\plugins\bbKeys\"
  SetOutPath $INSTDIR\plugins\bbLeanBar
	File /r "vs_vista_32\plugins\bbLeanBar\"
  SetOutPath $INSTDIR\plugins\bbLeanSkin
	File /r "vs_vista_32\plugins\bbLeanSkin\"
  SetOutPath $INSTDIR\plugins\bbSlit
	File /r "vs_vista_32\plugins\bbSlit\"

SkipVista32_3:

  StrCmp $OptPlugins 0 SkipVista32_4
  DetailPrint "Vista 32 bit Optional Plugins 1"

  SetOutPath $INSTDIR\plugins\BBAnalogEx
	File /r "vs_vista_32\plugins\BBAnalogEx\"
  SetOutPath $INSTDIR\plugins\bbCalendar
	File /r "vs_vista_32\plugins\bbCalendar\"
  SetOutPath $INSTDIR\plugins\BBDigitalEx
	File /r "vs_vista_32\plugins\BBDigitalEx\"
  SetOutPath $INSTDIR\plugins\bbFoomp
	File /r "vs_vista_32\plugins\bbFoomp\"
  SetOutPath $INSTDIR\plugins\bbLeanBar+
	File /r "vs_vista_32\plugins\bbLeanBar+\"
  SetOutPath $INSTDIR\plugins\BBMagnify
	File /r "vs_vista_32\plugins\BBMagnify\"
  SetOutPath $INSTDIR\plugins\BBPager
	File /r "vs_vista_32\plugins\BBPager\"
  SetOutPath $INSTDIR\plugins\bbRecycleBin
	File /r "vs_vista_32\plugins\bbRecycleBin\"
  SetOutPath $INSTDIR\plugins\BBRSS
	File /r "vs_vista_32\plugins\BBRSS\"
  SetOutPath $INSTDIR\plugins\BBStyle
	File /r "vs_vista_32\plugins\BBStyle\"
  SetOutPath $INSTDIR\plugins\BBSysMeter
	File /r "vs_vista_32\plugins\BBSysMeter\"
  SetOutPath $INSTDIR\plugins\bbWorkspaceWheel
	File /r "vs_vista_32\plugins\bbWorkspaceWheel\"
  SetOutPath $INSTDIR\plugins\SystemBarEx
	File /r "vs_vista_32\plugins\SystemBarEx\"

SkipVista32_4:

  StrCmp $OptPlugins 0 SkipVista32_5
  DetailPrint "Vista 32 bit Optional Plugins 2"

  SetOutPath $INSTDIR\plugins\BB8Ball
	File /r "vs_vista_32\plugins\BB8Ball\"
  SetOutPath $INSTDIR\plugins\bbInterface_iTunes
	File /r "vs_vista_32\plugins\bbInterface_iTunes\"
  SetOutPath $INSTDIR\plugins\BBMessageBox
	File /r "vs_vista_32\plugins\BBMessageBox\"
  SetOutPath $INSTDIR\plugins\BBXO
	File /r "vs_vista_32\plugins\BBXO\"

SkipVista32_5:

SectionEnd

################################ Vista 64b ####################################
Section /o "-Vista_64" Sec_Vista_64

  DetailPrint "Vista 64 bit Required Files"

  SetOutPath $INSTDIR
  File "vs_vista_64\bbnote.exe"
  File "vs_vista_64\bbnote-proxy.dll"
  File "vs_vista_64\bbstylemaker.exe"
  File "vs_vista_64\blackbox.exe"
  File "vs_vista_64\bsetbg.exe"
  File "vs_vista_64\bsetroot.exe"
  File "vs_vista_64\bsetshell.exe"
  File "vs_vista_64\deskhook.dll"
  File "vs_vista_64\readme.txt"
 
  StrCmp $OptConfigs 0 SkipVista64_1
  DetailPrint "Vista 64 bit Optional Configs"

  SetOutPath $INSTDIR
  CreateDirectory $INSTDIR
  File "vs_vista_64\blackbox.rc"
  File "vs_vista_64\bsetroot.rc"
  File "vs_vista_64\extensions.rc"
  File "vs_vista_64\menu.rc"
  File "vs_vista_64\plugins.rc"
  File "vs_vista_64\shellfolders.rc"
  File "vs_vista_64\stickywindows.ini"

SkipVista64_1:

  StrCmp $OptStyles 0 SkipVista64_2
  DetailPrint "Vista 64 bit Optional Styles"
 
  SetOutPath $INSTDIR\backgrounds
  File /r "vs_vista_64\backgrounds\"
  SetOutPath $INSTDIR\styles
  File /r "vs_vista_64\styles\"

SkipVista64_2:

  StrCmp $OptCorePlugins 0 SkipVista64_3
  DetailPrint "Vista 64 bit Optional Core Plugins"

  SetOutPath $INSTDIR\plugins\bbAnalog
	File /r "vs_vista_64\plugins\bbAnalog\"
  SetOutPath $INSTDIR\plugins\bbColor3dc
	File /r "vs_vista_64\plugins\bbColor3dc\"
  SetOutPath $INSTDIR\plugins\bbIconBox
	File /r "vs_vista_64\plugins\bbIconBox\"
  SetOutPath $INSTDIR\plugins\bbInterface
	File /r "vs_vista_64\plugins\bbInterface\"
  SetOutPath $INSTDIR\plugins\bbKeys
	File /r "vs_vista_64\plugins\bbKeys\"
  SetOutPath $INSTDIR\plugins\bbLeanBar
	File /r "vs_vista_64\plugins\bbLeanBar\"
  SetOutPath $INSTDIR\plugins\bbLeanSkin
	File /r "vs_vista_64\plugins\bbLeanSkin\"
  SetOutPath $INSTDIR\plugins\bbSlit
	File /r "vs_vista_64\plugins\bbSlit\"

SkipVista64_3:

  StrCmp $OptPlugins 0 SkipVista64_4
  DetailPrint "Vista 64 bit Optional Plugins 1"

  SetOutPath $INSTDIR\plugins\BBAnalogEx
	File /r "vs_vista_64\plugins\BBAnalogEx\"
  SetOutPath $INSTDIR\plugins\bbCalendar
	File /r "vs_vista_64\plugins\bbCalendar\"
  SetOutPath $INSTDIR\plugins\BBDigitalEx
	File /r "vs_vista_64\plugins\BBDigitalEx\"
  SetOutPath $INSTDIR\plugins\bbFoomp
	File /r "vs_vista_64\plugins\bbFoomp\"
  SetOutPath $INSTDIR\plugins\bbLeanBar+
	File /r "vs_vista_64\plugins\bbLeanBar+\"
  SetOutPath $INSTDIR\plugins\BBMagnify
	File /r "vs_vista_64\plugins\BBMagnify\"
  SetOutPath $INSTDIR\plugins\BBPager
	File /r "vs_vista_64\plugins\BBPager\"
  SetOutPath $INSTDIR\plugins\bbRecycleBin
	File /r "vs_vista_64\plugins\bbRecycleBin\"
  SetOutPath $INSTDIR\plugins\BBRSS
	File /r "vs_vista_64\plugins\BBRSS\"
  SetOutPath $INSTDIR\plugins\BBStyle
	File /r "vs_vista_64\plugins\BBStyle\"
  SetOutPath $INSTDIR\plugins\BBSysMeter
	File /r "vs_vista_64\plugins\BBSysMeter\"
  SetOutPath $INSTDIR\plugins\bbWorkspaceWheel
	File /r "vs_vista_64\plugins\bbWorkspaceWheel\"
  SetOutPath $INSTDIR\plugins\SystemBarEx
	File /r "vs_vista_64\plugins\SystemBarEx\"

SkipVista64_4:

  StrCmp $OptPlugins 0 SkipVista64_5
  DetailPrint "Vista 64 bit Optional Plugins 2"

  SetOutPath $INSTDIR\plugins\BB8Ball
	File /r "vs_vista_64\plugins\BB8Ball\"
  SetOutPath $INSTDIR\plugins\bbInterface_iTunes
	File /r "vs_vista_64\plugins\bbInterface_iTunes\"
  SetOutPath $INSTDIR\plugins\BBMessageBox
	File /r "vs_vista_64\plugins\BBMessageBox\"
  SetOutPath $INSTDIR\plugins\BBXO
	File /r "vs_vista_64\plugins\BBXO\"

SkipVista64_5:

SectionEnd


# windows detection
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
  Pop $BuildVerDialog

  ${NSD_CreateGroupBox} 2% 2% 48% 98% "Windows"
  Pop $GroupBox1

  ${NSD_CreateRadioButton} 5% 33% 40% 6% "XP"
    Pop $Group1BuildVerRadioXP
    ${NSD_AddStyle} $Group1BuildVerRadioXP ${WS_GROUP}
  ${NSD_CreateRadioButton} 5% 66% 40% 6% "Vista, Win7 or Win8"
    Pop $Group1BuildVerRadioVista

  ${NSD_CreateGroupBox} 52% 2% 46% 98% "bits"
  Pop $GroupBox2
 
  ${NSD_CreateRadioButton} 55% 33% 40% 6% "32"
    Pop $Group1BuildVer2Radio32
    ${NSD_AddStyle} $Group1BuildVer2Radio32 ${WS_GROUP}
  ${NSD_CreateRadioButton} 55% 66% 40% 6% "64"
    Pop $Group1BuildVer2Radio64

  ${If} $win_xp == 1
    ${NSD_SetState} $Group1BuildVerRadioXP ${BST_CHECKED}
  ${Else}
    ${NSD_SetState} $Group1BuildVerRadioVista ${BST_CHECKED}
  ${EndIf}

  ${If} $win_64 == 1
    ${NSD_SetState} $Group1BuildVer2Radio64 ${BST_CHECKED}
  ${Else}
    ${NSD_SetState} $Group1BuildVer2Radio32 ${BST_CHECKED}
  ${EndIf}

  nsDialogs::Show
FunctionEnd
 
Var radio_xp
Var radio_64

Function windetectionPageLeave
  ${NSD_GetState} $Group1BuildVerRadioXP $radio_xp
  ${NSD_GetState} $Group1BuildVer2Radio64 $radio_64

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

# as shell dialogue
Function shellPageEnter
  nsDialogs::Create 1018
  Pop $ShellDialog

  ${NSD_CreateGroupBox} 2% 2% 98% 98% "shell"
  Pop $GroupBox3

  ${NSD_CreateRadioButton} 5% 33% 95% 6% "no, do NOT install as shell"
    Pop $RadioButtonNoShell
    ${NSD_AddStyle} $RadioButtonNoShell ${WS_GROUP}
  ${NSD_CreateRadioButton} 5% 66% 95% 6% "yes, install blackbox as default shell"
    Pop $RadioButtonAsShell

  ${NSD_SetState} $RadioButtonNoShell ${BST_CHECKED}
  nsDialogs::Show
FunctionEnd
 
Function shellPageLeave
  ${NSD_GetState} $RadioButtonAsShell $as_shell

  ${If} $as_shell == ${BST_CHECKED}
    StrCpy $as_shell 1
  ${Else}
    StrCpy $as_shell 0
  ${EndIf}
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
