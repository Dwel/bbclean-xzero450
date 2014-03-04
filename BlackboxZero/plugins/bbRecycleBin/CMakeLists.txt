set(bbRecycleBin_SOURCES
	bbRecycleBin.cpp
)

include_directories(${CMAKE_SOURCE_DIR}/blackbox)
include_directories(${CMAKE_SOURCE_DIR}/lib)
include_directories(${CMAKE_SOURCE_DIR}/plugins/bbPlugin)

if (MINGW)
	add_definitions(-D_WIN32_IE=0x0601)
endif (MINGW)
add_library(bbRecycleBin SHARED ${bbRecycleBin_SOURCES})
target_link_libraries(bbRecycleBin blackbox bblib bbPlugin)
target_link_libraries(bbRecycleBin version comctl32 shlwapi)
set_property(TARGET bbRecycleBin PROPERTY FOLDER "plugins/bbRecycleBin")
if ( MSVC )
	# weird warning
    set_property( TARGET bbRecycleBin APPEND_STRING PROPERTY COMPILE_FLAGS /wd4996 )
endif ( MSVC )

install(FILES readme.txt DESTINATION plugins/bbRecycleBin)
install(TARGETS bbRecycleBin RUNTIME DESTINATION plugins/bbRecycleBin)

