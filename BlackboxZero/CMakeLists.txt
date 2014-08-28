project(BlackboxZero)
message("Welcome to BlackBox!")
cmake_minimum_required(VERSION 2.8.9)
set(CMAKE_VERBOSE_MAKEFILE on)

# use this for develoment, if you do not pass it from command line:
# development build means:
#  - logging enabled
#  - overriden install path
#  - not overwriting config .rc files (for most annoying ones so far)
#set(DEVEL_BUILD 1)

if(DEVEL_BUILD EQUAL 1)
  set(CMAKE_INSTALL_PREFIX "c:/bbZero_devel")
  set(USE_LOGGING 1)
else(DEVEL_BUILD EQUAL 1)
  set(DEVEL_BUILD 0)
endif(DEVEL_BUILD EQUAL 1)

# possibly enable c++11 for gcc
#if (MINGW)
#  SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=gnu++0x")
#endif (MINGW)

include_directories (3rd_party/DbgToolkit)
include_directories (3rd_party/logging)
include_directories (.)

if(USE_LOGGING EQUAL 1)
	add_definitions ("-DTRACE_ENABLED")
	add_definitions ("-DTRACE_STATIC")
	add_definitions ("-DTRACE_CONFIG_INCLUDE=\"${CMAKE_CURRENT_SOURCE_DIR}/3rd_party/logging/config.h\"")
	add_definitions ("-DTRACE_LEVELS_INCLUDE=\"${CMAKE_CURRENT_SOURCE_DIR}/3rd_party/logging/levels.h\"")
	add_definitions ("-DTRACE_CONTEXTS_INCLUDE=\"${CMAKE_CURRENT_SOURCE_DIR}/3rd_party/logging/contexts.h\"")
else(USE_LOGGING EQUAL 1)
endif(USE_LOGGING EQUAL 1)

message("bb generator=${CMAKE_GENERATOR}")
message("bb install path=${CMAKE_INSTALL_PREFIX}")

include(build/compiler.cmake)
include(build/install_if_absent.cmake)
include(ExternalProject)

# detect 32 / 64bit environment
if( CMAKE_SIZEOF_VOID_P EQUAL 8 )
	set(buildbits 64)
  message("bb build=64bit")
else( CMAKE_SIZEOF_VOID_P EQUAL 8 )
	set(buildbits 32)
  message("bb build=32bit")
endif( CMAKE_SIZEOF_VOID_P EQUAL 8 )

add_subdirectory(3rd_party)

# font tools and conversion
add_subdirectory(build/bdf2fon)
add_custom_target(fonts ALL)
set(FONTDIR "${CMAKE_SOURCE_DIR}/fonts/artwiz-latin1-1.0-src" CACHE STRING "font directory")
set(FONTOUTDIR "${CMAKE_BINARY_DIR}/converted_fonts" CACHE STRING "font directory")
set(BDF2FNT ${CMAKE_BINARY_DIR}/bdf2fnt.exe CACHE STRING "bdf2fnt path")
set(FNT2FON ${CMAKE_BINARY_DIR}/fnt2fon.exe CACHE STRING "fnt2fon path")
add_custom_command(TARGET fonts COMMAND ${CMAKE_COMMAND}
	 -DSOURCE_DIR=${CMAKE_CURRENT_SOURCE_DIR}
	 -DFONTDIR="${FONTDIR}"
	 -DBDF2FNT="${BDF2FNT}"
	 -DFNT2FON="${FNT2FON}"
	 -DFONTOUTDIR=${FONTOUTDIR}
	 ARGS -P ${CMAKE_SOURCE_DIR}/fonts/fonts.cmake)
install(DIRECTORY ${FONTOUTDIR} 
	DESTINATION .
	FILES_MATCHING PATTERN "*.fon"
	PERMISSIONS OWNER_READ OWNER_WRITE GROUP_READ WORLD_READ)

# core modules
add_subdirectory(lib)
add_subdirectory(blackbox)
add_subdirectory(blackbox/Hooks)

# documentation
add_subdirectory(build/fuzzydoc)
#add_subdirectory(docs)

# plugins
add_subdirectory(plugins/bbKeys)
add_subdirectory(plugins/bbPlugin)
#add_subdirectory(plugins/bbLeanBar)
add_subdirectory(plugins/bbFoomp)
add_subdirectory(plugins/bbLeanSkin)
# 32b for 64b build
if( buildbits EQUAL 64 )
	if (MSVC)
		string(REGEX REPLACE " Win64" "" GENERATOR_FOR_32 ${CMAKE_GENERATOR})
	endif (MSVC)
	if (MINGW)
		# TODO
	endif (MINGW)
	message("bbLeanSkin on 64bit... 32bit generator=${GENERATOR_FOR_32}")

	set(bbLeanSkin_SOURCE_DIR  ${CMAKE_SOURCE_DIR}/plugins/bbLeanSkin)
  set(bbLeanSkinRun32_PATH ${bbLeanSkin_SOURCE_DIR}/bbLeanSkinRun32)
  message("bbLeanSkin on 64bit... path=${bbLeanSkinRun32_PATH}")
	ExternalProject_Add (
		bbLeanSkinRun32
		CMAKE_GENERATOR ${GENERATOR_FOR_32}
		CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX}
		SOURCE_DIR  ${bbLeanSkinRun32_PATH}
		DOWNLOAD_COMMAND ""
	)
	set_property(TARGET bbLeanSkinRun32 PROPERTY FOLDER "plugins/bbLeanSkin")
endif( buildbits EQUAL 64 )

add_subdirectory(plugins/bbSlit)
add_subdirectory(plugins/bbAnalog)
add_subdirectory(plugins/bbAnalogExMod)
add_subdirectory(plugins/BBCalendar)
add_subdirectory(plugins/BB8Ball)
add_subdirectory(plugins/BBDigitalEx)
add_subdirectory(plugins/BBMagnify)
add_subdirectory(plugins/BBMessageBox)
add_subdirectory(plugins/BBRSS)
add_subdirectory(plugins/BBSysMeter)
add_subdirectory(plugins/BBXO)
add_subdirectory(plugins/bbIconBox)
add_subdirectory(plugins/bbColor3dc)
add_subdirectory(plugins/bbWorkspaceWhee)
add_subdirectory(plugins/bbPlugin+)
add_subdirectory(plugins/BlackboxBar)
add_subdirectory(plugins/bbPager)
add_subdirectory(plugins/BBStyle)
add_subdirectory(plugins/bbRecycleBin)
#add_subdirectory(plugins/bbInterface)    # disabled, Pete says bbInterface_iTunes is better
add_subdirectory(plugins/bbMuse)
add_subdirectory(plugins/bbPlayer)
add_subdirectory(plugins/bbSeekBar)
add_subdirectory(plugins/BBColorEx)
#add_subdirectory(plugins/bbLeanBar2)
add_subdirectory(plugins/bbDesktop)
add_subdirectory(plugins/bbIME)
#add_subdirectory(plugins/bbLua)
add_subdirectory(plugins/bbSetCursors)
add_subdirectory(plugins/bbTaskSwitch)

# NOTE: following plugins are not yet mingw compatible
if (NOT MINGW)
  add_subdirectory(plugins/bbFooMan)
	add_subdirectory(plugins/bbInterface_iTunes)
endif (NOT MINGW)

# NOTE: mingw 64 bit does not work yet!
if (MSVC)
	add_subdirectory(plugins/bbSystemBarEx)
endif (MSVC)
if (MINGW)
	if (NOT ${buildbits} MATCHES "64")
		add_subdirectory(plugins/SystemBarEx)
	ENDIF()
endif (MINGW)

# pluginloaders
if (${buildbits} MATCHES "64")
	#add_subdirectory(pluginloaders/wow64adapter)
ENDIF()

# tools
add_subdirectory(tools/bsetshell)
add_subdirectory(tools/bsetroot)
add_subdirectory(tools/bbstylemaker)
add_subdirectory(tools/bbnote)

# installation
set(main_CONFIGS
  config/menu.rc 
  config/blackbox.rc
  config/extensions.rc
  config/plugins.rc
  config/shellfolders.rc
  config/stickywindows.ini
)

if(DEVEL_BUILD EQUAL 0)
  # install config files
  install_if_absent(. ${main_CONFIGS})
  # install styles etc
  install(DIRECTORY styles backgrounds DESTINATION .)
endif(DEVEL_BUILD EQUAL 0)

# not used (yet).. from old makefiles
set (CLEAN_FILES
  bbnote.ini
  docs/bbnote.*
  bbstylemaker.rc
  docs/bbstylemaker.txt
  docs/nls-c.txt
  bsetroot.rc
  docs/bsetroot.*
  docs/*.bmp
  *.exe *.dll
)



###############################################################################
# debug stuff
#message("Debug variable dump:")
#get_cmake_property(_variableNames VARIABLES)
#foreach (_variableName ${_variableNames})
#   message(STATUS "${_variableName}=${${_variableName}}")
#endforeach()
