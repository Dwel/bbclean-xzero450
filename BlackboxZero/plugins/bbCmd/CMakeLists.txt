set(BBCMD_STATIC true)

#add_definitions(-DUNICODE -D_UNICODE)

set(Boost_DEBUG 0)
set(Boost_USE_STATIC_LIBS 1)
find_package(Boost)
if (Boost_FOUND)
	include_directories(${Boost_INCLUDE_DIR})
	add_definitions( "-DHAS_BOOST" )

  #set(BOOST_LIBS thread date_time system regex serialization)
  set(BOOST_LIBS filesystem)
	find_package(Boost COMPONENTS ${BOOST_LIBS} REQUIRED)
endif()
message("boost: ${Boost_LIBRARIES}")

set(bbCmd_SOURCES
  BBCmd.cpp
  plugin.cpp
  utils.cpp
)

set(bbCmd_HEADERS
	bbCmd.h
)

#set(bbCmd_CONFIGS bbCmd.rc)
#set(bbCmd_RESOURCES resource.rc)

set(CMAKE_RC_COMPILER_INIT windres)
ENABLE_LANGUAGE(RC)

include_directories(${CMAKE_SOURCE_DIR}/blackbox)
include_directories(${CMAKE_SOURCE_DIR}/lib)
include_directories(${CMAKE_SOURCE_DIR}/plugins/bbPlugin)
add_library(bbCmd SHARED ${bbCmd_SOURCES} ${bbCmd_RESOURCES})
target_link_libraries(bbCmd blackbox bblib bbPlugin ${Boost_LIBRARIES})
target_link_libraries(bbCmd version comctl32)

set_property(TARGET bbCmd PROPERTY FOLDER "plugins/bbCmd")

#install(FILES readme.txt DESTINATION plugins/bbCmd)
install(TARGETS bbCmd RUNTIME DESTINATION plugins/bbCmd)
#if(NOT EXISTS "${CMAKE_INSTALL_PREFIX}/plugins/bbCmd/${bbCmd_CONFIGS}")
#	install(FILES ${bbCmd_CONFIGS} DESTINATION plugins/bbCmd)
#endif()

